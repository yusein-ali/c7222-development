/**
 * @file c7222_pico_w_board.h
 * @brief Pico W board-level GPIO layout and helpers (C API).
 *
 * This header defines the GPIO layout for LEDs and buttons on the C7222 Pico W
 * board and provides C-style helper functions for initializing and accessing
 * those pins, plus a thin C wrapper for the Pico W on-board LED. The
 * implementation is in
 * `libs/elec_c7222/devices/platform/rpi_pico/c7222_pico_w_board.c`.
 *
 * Note: This header is intended for **C** code. For C++ projects, prefer the
 * `c7222_pico_w_board.hpp` API and its associated classes (`PicoWBoard`,
 * `Led`, and `Button`).
 *
 * ## Usage assumptions (from the implementation)
 * - Call @ref c7222_pico_w_board_init_gpio() once before using any of the LED or
 *   button helpers. All other functions assert if the board has not been
 *   initialized.
 * - Buttons are configured as inputs with pull-ups enabled.
 * - LEDs are configured as outputs. Note: the current implementation drives
 *   LEDs as **active-low** (see `c7222_pico_w_board_led_on/off/read`).
 */
#ifndef ELEC_C7222_DEVICES_C7222_PICO_W_BOARD_H_
#define ELEC_C7222_DEVICES_C7222_PICO_W_BOARD_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logical LED identifiers mapped to GPIO pins.
 */
enum c7222_led_type {
	C7222_PICO_W_LED3_GREEN = 16,
	C7222_PICO_W_LED3_RED   = 17,
	C7222_PICO_W_LED2_GREEN = 18,
	C7222_PICO_W_LED2_RED   = 19,
	C7222_PICO_W_LED1_GREEN = 20,
	C7222_PICO_W_LED1_RED   = 21
};

/**
 * @brief Logical button identifiers mapped to GPIO pins.
 */
enum c7222_button_type {
	C7222_PICO_W_BUTTON_B1 = 2,
	C7222_PICO_W_BUTTON_B2 = 3,
	C7222_PICO_W_BUTTON_B3 = 4,
	C7222_PICO_W_BUTTON_B4 = 5
};

/**
 * @brief GPIO IRQ event bits compatible with Pico SDK GPIO IRQ flags.
 *
 * These values match the Pico SDK bit assignments so they can be passed
 * directly to `gpio_set_irq_enabled*` after mapping.
 */
enum c7222_gpio_event_type {
	C7222_GPIO_EVENT_LEVEL_LOW = 0x1u,
	C7222_GPIO_EVENT_LEVEL_HIGH = 0x2u,
	C7222_GPIO_EVENT_FALL = 0x4u,
	C7222_GPIO_EVENT_RISE = 0x8u
};



/**
 * @brief Initialize all LED and button GPIOs for the board.
 *
 * Implementation assumptions:
 * - Configures all LED pins as outputs and drives them to the default state.
 * - Configures all button pins as inputs with pull-ups.
 * - Sets an internal "initialized" flag used by other functions.
 */
void c7222_pico_w_board_init_gpio(void);
/**
 * @brief Initialize a single LED GPIO.
 *
 * @param led LED identifier to initialize.
 * @note Requires @ref c7222_pico_w_board_init_gpio() to have been called.
 */
void c7222_pico_w_board_init_led(enum c7222_led_type led);

/**
 * @brief Initialize a button GPIO and optionally attach an IRQ handler.
 *
 * @param button Button identifier.
 * @param handler IRQ callback (may be NULL).
 * @param events Bitmask of @ref c7222_gpio_event_type.
 * @param enabled Enable or disable the IRQ for this button.
 *
 * Implementation assumptions:
 * - Configures the button as input with pull-up.
 * - Uses `gpio_set_irq_enabled_with_callback` if a handler is provided.
 * - Does not attach a global callback if handler is NULL (IRQ disabled or enabled without callback).
 */
void c7222_pico_w_board_button_init(enum c7222_button_type button,
											   void (*handler)(uint32_t, uint32_t),
											   uint32_t events,
											   bool enabled);

/**
 * @brief Read the current button GPIO level.
 *
 * @return true if the pin reads high, false otherwise.
 * @note Requires @ref c7222_pico_w_board_init_gpio() to have been called.
 */
bool c7222_pico_w_board_button_read(enum c7222_button_type button);
/**
 * @brief Read the current LED state.
 *
 * @return true if LED is ON (active-low), false otherwise.
 * @note Implementation reads the GPIO and inverts it.
 */
bool c7222_pico_w_board_led_read(enum c7222_led_type led);
/**
 * @brief Turn an LED ON (active-low).
 */
void c7222_pico_w_board_led_on(enum c7222_led_type led);
/**
 * @brief Turn an LED OFF (active-low).
 */
void c7222_pico_w_board_led_off(enum c7222_led_type led);
/**
 * @brief Toggle an LED output.
 */
void c7222_pico_w_board_led_toggle(enum c7222_led_type led);

/**
 * @brief Initialize the Pico W on-board LED.
 *
 * Uses either `PICO_DEFAULT_LED_PIN` or `CYW43_WL_GPIO_LED_PIN` depending on
 * build configuration. Sets an internal initialization flag.
 *
 * @return true on success, false if no on-board LED is available.
 */
bool c7222_pico_w_onboard_led_init(void);
/**
 * @brief Set the on-board LED state.
 *
 * @param on true for ON, false for OFF.
 * @note Requires @ref c7222_pico_w_onboard_led_init().
 */
void c7222_pico_w_onboard_led_set(bool on);
/** @brief Turn the on-board LED ON. */
void c7222_pico_w_onboard_led_on(void);
/** @brief Turn the on-board LED OFF. */
void c7222_pico_w_onboard_led_off(void);
/** @brief Toggle the on-board LED. */
void c7222_pico_w_onboard_led_toggle(void);
/**
 * @brief Read the on-board LED state.
 *
 * @return true if the GPIO reads high. For CYW43 LED, readback is not supported,
 *         so this returns false.
 */
bool c7222_pico_w_onboard_led_read(void);

#ifdef __cplusplus
}  // extern "C"
#endif


#endif // ELEC_C7222_DEVICES_C7222_PICO_W_BOARD_H_
