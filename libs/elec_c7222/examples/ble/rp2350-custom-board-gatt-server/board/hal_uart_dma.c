/*
 * BTstack hal_uart_dma implementation for RP2350.
 *
 * Drives UART1 at 921600 8N1 with hardware RTS/CTS flow control.
 * Both TX and RX use DMA so the CPU is not involved byte-by-byte.
 *
 * Pin assignments come from the board header (rpi_bd_hub_mcu.h):
 *   TX  GP4  (UART1_TX on RP2350 silicon)
 *   RX  GP5  (UART1_RX on RP2350 silicon)
 *   CTS GP6  (UART1_CTS on RP2350 silicon)
 *   RTS GP7  (UART1_RTS on RP2350 silicon)
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

void hal_uart_dma_set_block_received(void (*callback)(void)) {
    s_block_received = callback;
}

void hal_uart_dma_set_block_sent(void (*callback)(void)) {
    s_block_sent = callback;
}

int hal_uart_dma_set_baud(uint32_t baud) {
    uart_set_baudrate(BLE_UART, baud);
    return 0;
}

int hal_uart_dma_set_flowcontrol(int flowcontrol) {
    bool fc = (flowcontrol != 0);
    uart_set_hw_flow(BLE_UART, fc, fc);
    return 0;
}

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

void hal_uart_dma_set_csr_irq_handler(void (*csr_irq_handler)(void)) {
    (void)csr_irq_handler;
}

void hal_uart_dma_set_sleep(uint8_t sleep) {
    (void)sleep;
}

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
