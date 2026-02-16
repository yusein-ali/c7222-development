/**
 * @file main_freertos.c
 * @brief FreeRTOS-based LED test application for the Pico 2 W board.
 *
 * This C source is intended for testing a Pico 2 W board by blinking the
 * onboard LED using the Pico C SDK and FreeRTOS. It configures LED control
 * for either:
 * - boards with a direct LED GPIO (`PICO_DEFAULT_LED_PIN`), or
 * - Pico W style boards where the LED is controlled through the CYW43 Wi-Fi
 *   chip (`CYW43_WL_GPIO_LED_PIN`).
 *
 * Two FreeRTOS tasks are created:
 * - `led_task()`: toggles the LED periodically.
 * - `log_task()`: prints a periodic low-priority status message.
 */

#include "pico/stdlib.h"
#include <assert.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 100
#endif

/**
 * @brief Initialize the board-specific LED backend.
 *
 * Usage:
 * - Call once during startup before any call to `pico_set_led()`.
 * - In `main()`, the return value is checked with `hard_assert()` to ensure
 *   initialization succeeded.
 *
 * @return `PICO_OK` on success for GPIO-based boards, or the result of
 *         `cyw43_arch_init()` for Pico W style boards.
 */
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
	// A device like Pico that uses a GPIO for the LED will define
	// PICO_DEFAULT_LED_PIN so we can use normal GPIO functionality to turn the
	// led on and off
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
	// For Pico W devices we need to initialise the driver etc
	return cyw43_arch_init();
#endif
}

/**
 * @brief Set the onboard LED state.
 *
 * This function abstracts the hardware difference between direct GPIO LED
 * control and CYW43-controlled LED GPIO on Pico W style boards.
 *
 * Usage:
 * - Call after `pico_led_init()` succeeds.
 * - Pass `true` to turn the LED on, `false` to turn it off.
 *
 * @param led_on Desired LED state (`true` = on, `false` = off).
 */
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
	// Just set the GPIO on or off
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	// Ask the wifi "driver" to set the GPIO on or off
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

/**
 * @brief FreeRTOS task that blinks the onboard LED at `LED_DELAY_MS`.
 *
 * Usage:
 * - Created from `main()` using `xTaskCreate()`.
 * - Runs forever and alternates LED ON/OFF with `vTaskDelay()`.
 *
 * @param argument Unused task argument.
 */
static void led_task(void* argument) {
	(void) argument;
	const TickType_t led_delay_ticks = pdMS_TO_TICKS(LED_DELAY_MS);
	for(;;) {
		pico_set_led(true);
		printf("LED ON (normal)\n");
		vTaskDelay(led_delay_ticks);
		pico_set_led(false);
		printf("LED OFF (normal)\n");
		vTaskDelay(led_delay_ticks);
	}
}

/**
 * @brief Low-priority FreeRTOS task for periodic logging.
 *
 * Usage:
 * - Created from `main()` with lower priority than `led_task()`.
 * - Runs forever and prints a heartbeat log every 1 second.
 *
 * @param argument Unused task argument.
 */
static void log_task(void* argument) {
	(void) argument;
	for(;;) {
		printf("Low priority logger tick\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/**
 * @brief Application entry point.
 *
 * Startup sequence:
 * 1. Initialize stdio (`stdio_init_all()`).
 * 2. Initialize LED backend (`pico_led_init()`).
 * 3. Create FreeRTOS tasks (`led_task()` and `log_task()`).
 * 4. Start scheduler (`vTaskStartScheduler()`).
 *
 * Usage:
 * - Build this target with Pico SDK + FreeRTOS support.
 * - Flash to supported hardware (including Pico 2 W) to validate LED and
 *   scheduler/task behavior.
 *
 * @return Never returns under normal operation.
 */
int main() {
	bool bRet = stdio_init_all();
	assert(bRet != false);
	int rc = pico_led_init();
	hard_assert(rc == PICO_OK);

	// Create LED task at normal priority (tskIDLE_PRIORITY + 1)
	BaseType_t led_created = xTaskCreate(led_task, "led", 256, NULL,
										 tskIDLE_PRIORITY + 1, NULL);

	// Create logger task at below normal priority (tskIDLE_PRIORITY)
	BaseType_t log_created = xTaskCreate(log_task, "logger", 256, NULL,
										 tskIDLE_PRIORITY, NULL);

	hard_assert(led_created == pdPASS && log_created == pdPASS);

	// Start FreeRTOS scheduler
	vTaskStartScheduler();

	// Should never reach here
	while(true) {
		tight_loop_contents();
	}
}
