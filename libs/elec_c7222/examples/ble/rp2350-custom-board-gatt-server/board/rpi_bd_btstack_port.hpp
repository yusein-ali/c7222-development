/**
 * @file rpi_bd_btstack_port.hpp
 * @brief BtstackPort implementation for the RPi_BD custom RP2350 board.
 *
 * This file provides a concrete `c7222::BtstackPort` subclass that wires the
 * BTstack HCI layer to the external BLE controller on the RPi_BD hub MCU board.
 * It is intentionally board-specific: all pin assignments and baud-rate constants
 * come from `rpi_bd_hub_mcu.h` and must not be used on any other hardware.
 *
 * The port selects the following BTstack tables:
 *
 * - **Run loop:** BTstack FreeRTOS run loop. `Ble` creates a dedicated task to
 *   execute it; no application code needs to call `btstack_run_loop_execute()`
 *   directly.
 * - **HCI transport:** H4 over UART1, driven by a DMA-backed HAL in
 *   `hal_uart_dma.c` at 921600 bps with CTS/RTS hardware flow control.
 * - **Chipset driver:** none. The external controller speaks standard H4 and
 *   does not require vendor initialization commands or baud-rate switching.
 * - **TLV storage:** none (`btstack_tlv_none`). Bonding keys are not persisted
 *   across resets. Replace with a flash-backed TLV implementation if
 *   reconnection without re-pairing is required.
 */
#ifndef RPI_BD_BTSTACK_PORT_HPP
#define RPI_BD_BTSTACK_PORT_HPP

#include "btstack_port.hpp"

/**
 * @class RpiBdBtstackPort
 * @brief Concrete BTstack port for the RPi_BD custom RP2350 board.
 *
 * Derives from `c7222::BtstackPort` and configures BTstack for HCI H4 over
 * UART1 with DMA TX/RX and hardware flow control. The BTstack FreeRTOS run
 * loop is selected so `Ble` runs it in a dedicated task rather than blocking
 * the application.
 *
 * ---
 * ### Ownership and Lifetime
 *
 * The port object must outlive the `c7222::Ble` singleton. In practice, declare
 * it as a static or file-scope variable in `main` before passing its address to
 * `Ble::GetInstance()`.
 *
 * ---
 * ### Usage
 *
 * ```cpp
 * static RpiBdBtstackPort btstack_port;
 * auto* ble = c7222::Ble::GetInstance(false, &btstack_port);
 * att_server = ble->EnableAttributeServer(profile_data); // triggers Init + Apply
 * ```
 *
 * `Init()` and `Apply()` are called internally by `Ble` before the BTstack run
 * loop task starts. Application code does not call them directly.
 */
class RpiBdBtstackPort final : public c7222::BtstackPort {
  public:
    /**
     * @brief Construct the port and select all BTstack tables.
     *
     * Fills the `BtstackPortTables` bundle with:
     * - BTstack FreeRTOS run loop
     * - H4 UART transport backed by `btstack_uart_block_embedded`
     * - UART configuration for UART1 at 921600 bps with CTS/RTS flow control
     * - No chipset driver
     * - No-op TLV storage (no bonding persistence)
     *
     * No hardware is touched here. Hardware initialization happens in `Init()`.
     */
    RpiBdBtstackPort();

    /**
     * @brief Initialize BTstack memory pools.
     *
     * Calls `btstack_memory_init()` to prepare BTstack's internal memory pools
     * before `Apply()` writes the tables into the BTstack runtime. Must complete
     * before `Ble` calls `Apply()` and the run loop task starts.
     *
     * Idempotent: returns true immediately if already initialized.
     *
     * @return true on success; false if initialization failed.
     */
    bool Init() override;

    /**
     * @brief Release BTstack memory pools.
     *
     * Calls `btstack_memory_deinit()` and clears the initialized flag. Safe to
     * call even if `Init()` was never called or was called and failed.
     */
    void Deinit() override;
};

#endif // RPI_BD_BTSTACK_PORT_HPP
