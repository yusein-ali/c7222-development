/**
 * @file hal_uart_dma.c
 * @brief BTstack `hal_uart_dma` implementation for the RPi_BD custom RP2350 board.
 *
 * BTstack's embedded UART block driver (`btstack_uart_block_embedded`) requires
 * a board-level HAL that provides non-blocking, block-oriented TX and RX over
 * the UART connected to the BLE controller. This file implements that HAL for
 * UART1 on the RPi_BD hub MCU, using RP2350 DMA for both directions so the CPU
 * is not stalled byte-by-byte during HCI packet transfers.
 *
 * ## Hardware mapping
 *
 * All pin and peripheral assignments come from `rpi_bd_hub_mcu.h`:
 *
 * | Signal | GPIO | RP2350 function |
 * |--------|------|-----------------|
 * | TX     | GP4  | UART1_TX        |
 * | RX     | GP5  | UART1_RX        |
 * | CTS    | GP6  | UART1_CTS       |
 * | RTS    | GP7  | UART1_RTS       |
 *
 * Baud rate: 921600 bps, 8N1, hardware CTS/RTS flow control enabled.
 *
 * ## DMA strategy
 *
 * TX and RX each claim a dedicated DMA channel at init time. Both channels use
 * `DMA_IRQ_0` with a shared ISR (`dma_irq0_handler`) that checks which channel
 * fired and calls the corresponding BTstack completion callback. BTstack provides
 * one block at a time for each direction; the callbacks signal that the transfer
 * is complete and BTstack may queue the next block.
 *
 * ## Functions
 *
 * - `hal_uart_dma_init()` — configure GPIO, UART1, and DMA channels.
 * - `hal_uart_dma_set_block_received()` / `hal_uart_dma_set_block_sent()` —
 *   register BTstack's TX and RX completion callbacks.
 * - `hal_uart_dma_set_baud()` — change the UART baud rate at runtime (used by
 *   BTstack during controller initialization if baud-rate switching is needed).
 * - `hal_uart_dma_set_flowcontrol()` — enable or disable CTS/RTS at runtime.
 * - `hal_uart_dma_send_block()` — start a DMA TX transfer for one HCI packet.
 * - `hal_uart_dma_receive_block()` — arm a DMA RX transfer for one HCI packet.
 * - `hal_uart_dma_set_csr_irq_handler()` — no-op; CSR wakeup IRQ is not wired
 *   on this board.
 * - `hal_uart_dma_set_sleep()` — no-op; UART clock gating is not implemented.
 * - `rpi_bd_ble_uart_debug_snapshot()` — read out DMA transfer counters for
 *   debugging HCI transport issues without a logic analyzer.
 */

#include "hal_uart_dma.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/uart.h"

#include "rpi_bd_hub_mcu.h"

#define BLE_UART      uart1
#define BLE_UART_BAUD RPI_BD_BLE_UART_BAUD

static int tx_dma_ch = -1;
static int rx_dma_ch = -1;

static void (*s_block_received)(void);
static void (*s_block_sent)(void);

static volatile uint32_t dbg_tx_blocks;
static volatile uint32_t dbg_rx_blocks;
static volatile uint32_t dbg_tx_irqs;
static volatile uint32_t dbg_rx_irqs;
static volatile uint32_t dbg_tx_bytes;
static volatile uint32_t dbg_rx_bytes;

/**
 * @brief Shared DMA IRQ handler for TX and RX completion.
 *
 * Fires on `DMA_IRQ_0` whenever either the TX or RX DMA channel completes a
 * transfer. Clears the interrupt flag for the firing channel and calls the
 * registered BTstack callback so BTstack can queue the next block.
 */
static void dma_irq0_handler(void) {
    uint32_t ints = dma_hw->ints0;

    if (ints & (1u << (uint32_t)rx_dma_ch)) {
        dbg_rx_irqs++;
        dma_hw->ints0 = 1u << (uint32_t)rx_dma_ch;
        if (s_block_received) s_block_received();
    }
    if (ints & (1u << (uint32_t)tx_dma_ch)) {
        dbg_tx_irqs++;
        dma_hw->ints0 = 1u << (uint32_t)tx_dma_ch;
        if (s_block_sent) s_block_sent();
    }
}

/**
 * @brief Initialize UART1, GPIO pins, DMA channels, and the DMA IRQ handler.
 *
 * Must be called once before any other HAL function. Configures GPIO alternate
 * functions for TX/RX/CTS/RTS, initializes UART1 at `RPI_BD_BLE_UART_BAUD`
 * with 8N1 framing and hardware flow control, claims two DMA channels (one for
 * TX, one for RX), and registers the shared DMA completion IRQ handler on
 * `DMA_IRQ_0`.
 *
 * Called by BTstack's embedded UART block driver during `hci_transport_h4`
 * initialization, before the first HCI command is sent.
 */
void hal_uart_dma_init(void) {
    gpio_set_function(RPI_BD_BLE_UART_TX_PIN,  GPIO_FUNC_UART);
    gpio_set_function(RPI_BD_BLE_UART_RX_PIN,  GPIO_FUNC_UART);
    gpio_set_function(RPI_BD_BLE_UART_RTS_PIN, GPIO_FUNC_UART);
    gpio_set_function(RPI_BD_BLE_UART_CTS_PIN, GPIO_FUNC_UART);

    uart_init(BLE_UART, BLE_UART_BAUD);
    uart_set_format(BLE_UART, 8, 1, UART_PARITY_NONE);
    uart_set_hw_flow(BLE_UART, true, true);
    uart_set_fifo_enabled(BLE_UART, true);

    tx_dma_ch = dma_claim_unused_channel(true);
    rx_dma_ch = dma_claim_unused_channel(true);

    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

/**
 * @brief Register the callback invoked when an RX block transfer completes.
 *
 * BTstack calls this once during transport initialization to supply its internal
 * RX-complete handler. The callback is called from the `DMA_IRQ_0` ISR and must
 * be ISR-safe.
 *
 * @param callback Function pointer called by the DMA IRQ when an RX block
 *                 transfer finishes. Pass NULL to disable the callback.
 */
void hal_uart_dma_set_block_received(void (*callback)(void)) {
    s_block_received = callback;
}

/**
 * @brief Register the callback invoked when a TX block transfer completes.
 *
 * BTstack calls this once during transport initialization to supply its internal
 * TX-complete handler. The callback is called from the `DMA_IRQ_0` ISR and must
 * be ISR-safe.
 *
 * @param callback Function pointer called by the DMA IRQ when a TX block
 *                 transfer finishes. Pass NULL to disable the callback.
 */
void hal_uart_dma_set_block_sent(void (*callback)(void)) {
    s_block_sent = callback;
}

/**
 * @brief Change the UART baud rate at runtime.
 *
 * BTstack may call this during controller initialization if the port
 * configuration requests a different baud rate after the initial HCI reset
 * sequence. On this board, init and runtime baud rates are both 921600 so
 * this function is called but causes no effective change.
 *
 * @param baud New baud rate in bits per second.
 * @return Always 0 (success).
 */
int hal_uart_dma_set_baud(uint32_t baud) {
    uart_set_baudrate(BLE_UART, baud);
    return 0;
}

/**
 * @brief Enable or disable CTS/RTS hardware flow control at runtime.
 *
 * BTstack calls this with `flowcontrol=1` during transport initialization when
 * the port configuration enables flow control (`hci_transport_config_uart_t`
 * `flowcontrol` field). On this board, flow control is always enabled after
 * init; this function allows BTstack to change it if needed.
 *
 * @param flowcontrol Non-zero to enable CTS/RTS; zero to disable.
 * @return Always 0 (success).
 */
int hal_uart_dma_set_flowcontrol(int flowcontrol) {
    bool fc = (flowcontrol != 0);
    uart_set_hw_flow(BLE_UART, fc, fc);
    return 0;
}

/**
 * @brief Start a DMA TX transfer for one HCI packet block.
 *
 * Configures the TX DMA channel to transfer `length` bytes from `buffer` to the
 * UART1 data register, paced by the UART TX DREQ so the DMA rate matches the
 * UART baud rate. Enables the `DMA_IRQ_0` interrupt for the TX channel so
 * `dma_irq0_handler` fires when the transfer completes and can call
 * `s_block_sent`.
 *
 * @param buffer Pointer to the HCI packet data to transmit. Must remain valid
 *               until the TX-complete callback fires.
 * @param length Number of bytes to transfer.
 */
void hal_uart_dma_send_block(const uint8_t *buffer, uint16_t length) {
    dbg_tx_blocks++;
    dbg_tx_bytes += length;

    dma_channel_config c = dma_channel_get_default_config(tx_dma_ch);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, uart_get_dreq(BLE_UART, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    dma_channel_set_irq0_enabled(tx_dma_ch, true);
    dma_channel_configure(tx_dma_ch, &c,
                          &uart_get_hw(BLE_UART)->dr,
                          buffer, length, true);
}

/**
 * @brief Arm a DMA RX transfer to receive one HCI packet block.
 *
 * Configures the RX DMA channel to transfer `len` bytes from the UART1 data
 * register into `buffer`, paced by the UART RX DREQ. Enables the `DMA_IRQ_0`
 * interrupt for the RX channel so `dma_irq0_handler` fires when the expected
 * number of bytes has been received and can call `s_block_received`.
 *
 * BTstack calls this repeatedly: first to receive the HCI packet type byte,
 * then to receive the header, then to receive the payload, driving a small
 * state machine inside the H4 transport layer.
 *
 * @param buffer Destination buffer for the incoming bytes. Must remain valid
 *               until the RX-complete callback fires.
 * @param len    Number of bytes to receive.
 */
void hal_uart_dma_receive_block(uint8_t *buffer, uint16_t len) {
    dbg_rx_blocks++;
    dbg_rx_bytes += len;

    dma_channel_config c = dma_channel_get_default_config(rx_dma_ch);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, uart_get_dreq(BLE_UART, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_set_irq0_enabled(rx_dma_ch, true);
    dma_channel_configure(rx_dma_ch, &c,
                          buffer,
                          &uart_get_hw(BLE_UART)->dr,
                          len, true);
}

/**
 * @brief No-op: CSR controller wakeup IRQ handler registration.
 *
 * BTstack calls this when a CSR-family controller is in use and a host-wakeup
 * pin is available. The RPi_BD board does not route a CSR wakeup line to the
 * RP2350; HCI traffic pacing is handled entirely by CTS/RTS flow control.
 *
 * @param csr_irq_handler Ignored.
 */
void hal_uart_dma_set_csr_irq_handler(void (*csr_irq_handler)(void)) {
    (void)csr_irq_handler;
}

/**
 * @brief No-op: UART power management / sleep control.
 *
 * BTstack calls this to gate the UART clock when entering a low-power state.
 * UART1 clock gating is not implemented on this board; the UART stays active.
 *
 * @param sleep Non-zero to request sleep; zero to request wake. Ignored.
 */
void hal_uart_dma_set_sleep(uint8_t sleep) {
    (void)sleep;
}

/**
 * @brief Capture a snapshot of the DMA transfer debug counters.
 *
 * Provides visibility into HCI transport activity without requiring a logic
 * analyzer. All counters are cumulative since `hal_uart_dma_init()` and are
 * updated from the `DMA_IRQ_0` ISR or from the DMA send/receive entry points.
 * Read them atomically by calling this function from a single task context.
 *
 * @param[out] tx_blocks  Number of TX DMA blocks submitted via
 *                        `hal_uart_dma_send_block()`.
 * @param[out] rx_blocks  Number of RX DMA blocks armed via
 *                        `hal_uart_dma_receive_block()`.
 * @param[out] tx_irqs    Number of TX DMA completion interrupts fired.
 * @param[out] rx_irqs    Number of RX DMA completion interrupts fired.
 * @param[out] tx_bytes   Total bytes submitted for TX.
 * @param[out] rx_bytes   Total bytes submitted for RX.
 * @param[out] uart_fr    Current value of the UART1 flag register (hardware `fr` field).
 *                        Useful for detecting TX/RX FIFO full/empty conditions.
 */
void rpi_bd_ble_uart_debug_snapshot(uint32_t *tx_blocks,
                                    uint32_t *rx_blocks,
                                    uint32_t *tx_irqs,
                                    uint32_t *rx_irqs,
                                    uint32_t *tx_bytes,
                                    uint32_t *rx_bytes,
                                    uint32_t *uart_fr) {
    *tx_blocks = dbg_tx_blocks;
    *rx_blocks = dbg_rx_blocks;
    *tx_irqs = dbg_tx_irqs;
    *rx_irqs = dbg_rx_irqs;
    *tx_bytes = dbg_tx_bytes;
    *rx_bytes = dbg_rx_bytes;
    *uart_fr = uart_get_hw(BLE_UART)->fr;
}
