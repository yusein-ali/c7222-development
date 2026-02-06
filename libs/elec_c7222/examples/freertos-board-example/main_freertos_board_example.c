/**
 * @file main_freertos_board_example.c
 * @brief FreeRTOS + Pico W board validation example.
 *
 * This example is intended for students who are testing the Pico W board for
 * the first time. It validates the `c7222_pico_w_board` API and demonstrates
 * foundational FreeRTOS patterns commonly used on microcontrollers.
 *
 * ## Learning goals and intent
 *
 * - Verify that **GPIO mapping** for LEDs and buttons matches the board layout.
 * - Show how to **combine interrupts with tasks** for responsive input handling.
 * - Compare **interrupt-driven** inputs (B1/B2) against **polled** inputs (B3/B4).
 * - Practice using **FreeRTOS synchronization primitives** and task scheduling.
 *
 * ## What this example demonstrates
 *
 * - **Board GPIO initialization** using `c7222_pico_w_board_init_gpio()`.
 * - **Active-low LEDs**: LEDs are driven low to turn ON and high to turn OFF.
 * - **Button input with pull-ups**: buttons are configured as inputs with
 *   internal pull-ups, so a pressed button reads low.
 * - **FreeRTOS tasks**: five tasks run concurrently:
 *   - One task for each button (B1–B4).
 *   - One manager task that prints a heartbeat.
 * - **Binary semaphore**: B1 uses a binary semaphore signaled from an IRQ.
 * - **Event group**: B2 uses an event flag bit signaled from an IRQ.
 * - **IRQ handling**: B1 and B2 use falling-edge interrupts to wake tasks.
 * - **Polling**: B3 and B4 are polled to compare interrupt vs polling behavior.
 *
 * ## FreeRTOS features shown
 *
 * - **xTaskCreate**: create multiple concurrent tasks.
 * - **Binary semaphore**: a lightweight signal from ISR to task (B1).
 * - **Event group**: flag-based wakeup for a task (B2).
 * - **ISR safe APIs**: `xSemaphoreGiveFromISR` and `xEventGroupSetBitsFromISR`.
 * - **vTaskDelay**: basic cooperative timing and debouncing for polling tasks.
 *
 * ## Expected behavior
 *
 * - Press **B1**: the B1 task prints a message and toggles LED1_GREEN.
 * - Press **B2**: the B2 task prints a message and toggles LED2_GREEN.
 * - Press **B3**: the polling task prints a message and toggles LED3_GREEN.
 * - Press **B4**: the polling task prints a message and toggles LED3_RED.
 * - The manager task prints a heartbeat message once per second.
 * - The IRQ handler lights LED1_RED for B1 and LED2_RED for B2 while pressed.
 *
 * ## Notes for first-time testing
 *
 * - Make sure the board is powered and connected.
 * - LEDs are active-low on this board; ON means GPIO is 0.
 * - Button presses generate **falling-edge** interrupts because the inputs are
 *   pulled up and go low when pressed.
 * - If nothing prints, confirm USB serial is enabled and the baud rate matches.
 */

#include <stdio.h>
#include "pico/stdlib.h"


#include "FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"
#include "task.h"

#include "c7222_pico_w_board.h"

/// Binary semaphore used to signal B1 press events from ISR to task.
static SemaphoreHandle_t b1_semaphore;
/// Event group used to signal B2 press events from ISR to task.
static EventGroupHandle_t b2_event_group;
static const EventBits_t kB2PressedBit = (1u << 0);

/**
 * @brief Shared GPIO IRQ handler for B1 and B2.
 *
 * Uses ISR-safe FreeRTOS APIs to notify the corresponding task. Also updates
 * indicator LEDs so students can see edge detection immediately.
 */
static void button_irq_handler(uint32_t gpio, uint32_t events) {
	BaseType_t higher_priority_woken = pdFALSE;

	if((events & (C7222_GPIO_EVENT_FALL | C7222_GPIO_EVENT_RISE)) == 0) {
		// Ignore unrelated GPIO events.
		return;
	}
	if((events & C7222_GPIO_EVENT_FALL) != 0) {
		if(gpio == C7222_PICO_W_BUTTON_B1) {
			// Wake B1 task on press.
			xSemaphoreGiveFromISR(b1_semaphore, &higher_priority_woken);
			// Visual feedback: turn on RED LED while pressed.
			c7222_pico_w_board_led_on(C7222_PICO_W_LED1_RED);
		} else if(gpio == C7222_PICO_W_BUTTON_B2) {
			// Wake B2 task on press.
			xEventGroupSetBitsFromISR(b2_event_group, kB2PressedBit, &higher_priority_woken);
			// Visual feedback: turn on RED LED while pressed.
			c7222_pico_w_board_led_on(C7222_PICO_W_LED2_RED);
		}
	} else {
		if(gpio == C7222_PICO_W_BUTTON_B1) {
			// Button released: clear feedback LED.
			c7222_pico_w_board_led_off(C7222_PICO_W_LED1_RED);
		} else if(gpio == C7222_PICO_W_BUTTON_B2) {
			// Button released: clear feedback LED.
			c7222_pico_w_board_led_off(C7222_PICO_W_LED2_RED);
		}
	}

	portYIELD_FROM_ISR(higher_priority_woken);
}

/**
 * @brief Task for button B1 (interrupt-driven).
 *
 * Waits on a binary semaphore signaled by the GPIO ISR and toggles LED1_GREEN.
 */
static void task_button_b1(void* ctx) {
	(void)ctx;
	for(;;) {
		if(xSemaphoreTake(b1_semaphore, portMAX_DELAY) == pdTRUE) {
			// Act on the press event signaled by the ISR.
			printf("[B1] Falling edge detected\n");
			// Toggle a GREEN LED to show task-level handling.
			c7222_pico_w_board_led_toggle(C7222_PICO_W_LED1_GREEN);
		}
	}
}

/**
 * @brief Task for button B2 (interrupt-driven).
 *
 * Waits on an event-group bit signaled by the GPIO ISR and toggles LED2_GREEN.
 */
static void task_button_b2(void* ctx) {
	(void) ctx;
	for(;;) {
		EventBits_t bits = xEventGroupWaitBits(b2_event_group,
											   kB2PressedBit,
											   pdTRUE,
											   pdFALSE,
											   portMAX_DELAY);
		if((bits & kB2PressedBit) != 0) {
			// Act on the press event signaled by the ISR.
			printf("[B2] Falling edge detected\n");
			// Toggle a GREEN LED to show task-level handling.
			c7222_pico_w_board_led_toggle(C7222_PICO_W_LED2_GREEN);
		}
	}
}

/**
 * @brief Task for button B3 (polled).
 *
 * Periodically reads the input and toggles LED3_GREEN on press.
 */
static void task_button_b3(void* ctx) {
	(void) ctx;
	for(;;) {
		bool b3 = c7222_pico_w_board_button_read(C7222_PICO_W_BUTTON_B3);
		if(!b3) {
			// Active-low input: 0 means pressed.
			printf("[B3] Pressed (polled)\n");
			// Toggle LED on press.
			c7222_pico_w_board_led_toggle(C7222_PICO_W_LED3_GREEN);
			// Basic debounce delay for the polled button.
			vTaskDelay(pdMS_TO_TICKS(200));
		}
		// Polling interval.
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/**
 * @brief Task for button B4 (polled).
 *
 * Periodically reads the input and toggles LED3_RED on press.
 */
static void task_button_b4(void* ctx) {
	(void) ctx;
	for(;;) {
		bool b4 = c7222_pico_w_board_button_read(C7222_PICO_W_BUTTON_B4);
		if(!b4) {
			// Active-low input: 0 means pressed.
			printf("[B4] Pressed (polled)\n");
			// Toggle LED on press.
			c7222_pico_w_board_led_toggle(C7222_PICO_W_LED3_RED);
			// Basic debounce delay for the polled button.
			vTaskDelay(pdMS_TO_TICKS(200));
		}
		// Polling interval.
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}

/**
 * @brief Management/heartbeat task.
 *
 * Prints a periodic heartbeat so students can verify the scheduler is running.
 */
static void task_manager(void* ctx) {
	(void) ctx;
	for(;;) {
		// Scheduler heartbeat to confirm RTOS is running.
		printf("[MGR] Heartbeat\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/**
 * @brief Entry point for the FreeRTOS board validation example.
 */
int main(void) {
	stdio_init_all();

	/// Board GPIO initialization (LEDs + buttons).
	c7222_pico_w_board_init_gpio();

	/// IPC primitives (created before enabling IRQs).
	b1_semaphore = xSemaphoreCreateBinary();
	b2_event_group = xEventGroupCreate();

	/// Attach IRQs for falling edges on B1 and B2.
	c7222_pico_w_board_button_init(C7222_PICO_W_BUTTON_B1,
								   button_irq_handler,
								   C7222_GPIO_EVENT_FALL|C7222_GPIO_EVENT_RISE,
								   true);
	c7222_pico_w_board_button_init(C7222_PICO_W_BUTTON_B2,
								   button_irq_handler,
								   C7222_GPIO_EVENT_FALL|C7222_GPIO_EVENT_RISE,
								   true);

	/// Create tasks: one per button + manager.
	xTaskCreate(task_button_b1, "btn_b1", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(task_button_b2, "btn_b2", 512, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(task_button_b3, "btn_b3", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(task_button_b4, "btn_b4", 512, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(task_manager, "manager", 512, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the scheduler (never returns).
	vTaskStartScheduler();
	for(;;) {
	}
}
