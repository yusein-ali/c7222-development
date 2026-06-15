/*
 * Board configuration for RPi_BD Hub MCU.
 *
 * Derived from Outputs.PDF sheet "RPI_BD_Hub_MCU.SchDoc".
 */

// -----------------------------------------------------
// NOTE: THIS HEADER IS ALSO INCLUDED BY ASSEMBLER SO
//       SHOULD ONLY CONSIST OF PREPROCESSOR DIRECTIVES
// -----------------------------------------------------

#ifndef _BOARDS_RPI_BD_HUB_MCU_H
#define _BOARDS_RPI_BD_HUB_MCU_H

// For board detection
#define RPI_BD_HUB_MCU

// --- RP2354A variant ---
#define PICO_RP2350A 1

// --- UART ---
// UART0 is routed to the MCU debug connector.
#ifndef PICO_DEFAULT_UART
#define PICO_DEFAULT_UART 0
#endif
#ifndef PICO_DEFAULT_UART_TX_PIN
#define PICO_DEFAULT_UART_TX_PIN 0
#endif
#ifndef PICO_DEFAULT_UART_RX_PIN
#define PICO_DEFAULT_UART_RX_PIN 1
#endif
#ifndef RPI_BD_BLE_UART
#define RPI_BD_BLE_UART 1
#endif
#ifndef RPI_BD_BLE_UART_TX_PIN
#define RPI_BD_BLE_UART_TX_PIN 4
#endif
#ifndef RPI_BD_BLE_UART_RX_PIN
#define RPI_BD_BLE_UART_RX_PIN 5
#endif
#ifndef RPI_BD_BLE_UART_CTS_PIN
#define RPI_BD_BLE_UART_CTS_PIN 6
#endif
#ifndef RPI_BD_BLE_UART_RTS_PIN
#define RPI_BD_BLE_UART_RTS_PIN 7
#endif
#ifndef RPI_BD_BLE_UART_BAUD
#define RPI_BD_BLE_UART_BAUD 921600U
#endif

// --- I2C ---
// Shared system I2C bus: charger, fuel gauge, and LED controller.
#ifndef PICO_DEFAULT_I2C
#define PICO_DEFAULT_I2C 0
#endif
#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 16
#endif
#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 17
#endif

#ifndef RPI_BD_LED_CONTROLLER_I2C_ADDR
#define RPI_BD_LED_CONTROLLER_I2C_ADDR 0x30
#endif
#ifndef RPI_BD_FUEL_GAUGE_I2C_ADDR
#define RPI_BD_FUEL_GAUGE_I2C_ADDR 0x55
#endif
#ifndef RPI_BD_CHARGER_I2C_ADDR
#define RPI_BD_CHARGER_I2C_ADDR 0x6a
#endif

// --- SPI ---
// System SPI bus for the external EEPROM/SPI memory.
#ifndef PICO_DEFAULT_SPI
#define PICO_DEFAULT_SPI 0
#endif
#ifndef PICO_DEFAULT_SPI_RX_PIN
#define PICO_DEFAULT_SPI_RX_PIN 20
#endif
#ifndef PICO_DEFAULT_SPI_CSN_PIN
#define PICO_DEFAULT_SPI_CSN_PIN 21
#endif
#ifndef PICO_DEFAULT_SPI_SCK_PIN
#define PICO_DEFAULT_SPI_SCK_PIN 22
#endif
#ifndef PICO_DEFAULT_SPI_TX_PIN
#define PICO_DEFAULT_SPI_TX_PIN 23
#endif

// --- USB ---
#ifndef PICO_VBUS_PIN
#define PICO_VBUS_PIN 25
#endif

// --- Control / interrupt GPIO ---
#define RPI_BD_BLE_WAKEUP_PIN 2
#define RPI_BD_HOST_WAKEUP_PIN 3
#define RPI_BD_POWER_KILL_PIN 13
#define RPI_BD_CHARGER_IRQ_PIN 14
#define RPI_BD_FUEL_GAUGE_IRQ_PIN 15
#define RPI_BD_5V_ENABLE_PIN 24
#define RPI_BD_BD_MOD_CTRL0_PIN 26
#define RPI_BD_BD_MOD_CTRL1_PIN 28

// --- FLASH ---
// RP2354A includes stacked 2 MB QSPI flash on the primary QSPI chip select.
#ifndef PICO_FLASH_SIZE_BYTES
#define PICO_FLASH_SIZE_BYTES (2 * 1024 * 1024)
#endif

#ifndef PICO_FLASH_SPI_CLKDIV
#define PICO_FLASH_SPI_CLKDIV 2
#endif

#ifndef PICO_RP2350_A2_SUPPORTED
#define PICO_RP2350_A2_SUPPORTED 1
#endif

#endif
