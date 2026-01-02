/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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

// Perform initialisation
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

// Turn the led on or off
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
	// Just set the GPIO on or off
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	// Ask the wifi "driver" to set the GPIO on or off
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

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

static void log_task(void* argument) {
	(void) argument;
	for(;;) {
		printf("Low priority logger tick\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

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
