/**
 * @file main_freertos_device.cpp
 * @brief FreeRTOS C++ device example demonstrating buttons, LEDs, PWM, and synchronization.
 *
 * Purpose of this example:
 * - Show how to use the C++ device wrappers (GPIO/LED/Button/PWM) with FreeRTOS-CPP11
 *   on the Pico W platform.
 * - Demonstrate safe cross-task access to a shared LED using a mutex/condition_variable
 *   via the SafeLed helper.
 * - Demonstrate event delivery from a GPIO ISR to normal task context using a FreeRTOS
 *   software timer and the ButtonEvent helper class.
 *
 * Devices and objects used:
 * - Board LEDs (LED1_GREEN, LED2_RED, LED2_GREEN) via `Led` and `SafeLed`.
 * - Board buttons (BUTTON_B1, BUTTON_B2) via `Button`/GPIO IRQ and polling helpers.
 * - PWM output for LED3_RED via `PwmOut` to demonstrate duty-cycle control.
 * - FreeRTOS wrapper classes (`FreeRtosTimer`, `FreeRtosTask`) to defer ISR work
 *   to task context and use wrapper delay/scheduler helpers.
 *
 * Concurrency and synchronization:
 * - `std::thread` is used to create FreeRTOS tasks through FreeRTOS-CPP11 integration.
 * - `ButtonEvent` uses `std::mutex` + `std::condition_variable` to provide a
 *   wait-with-timeout event mailbox for button IRQ events.
 * - `SafeLed` uses `std::mutex` + `std::condition_variable` to serialize access
 *   to a shared LED across multiple tasks.
 *
 * Helper classes:
 * - `ButtonEvent` collects IRQ events and provides a blocking `GetEvents()` API.
 * - `SafeLed` ensures exclusive LED control between tasks, avoiding concurrent writes.
 *
 * ISR dispatching model:
 * - The GPIO ISR for Button 1 records the event and starts a one-shot `FreeRtosTimer`,
 *   passing the event bitmask as a `void*` argument.
 * - The timer callback runs in the FreeRTOS timer service task (normal execution
 *   context) and forwards the event to `ButtonEvent`.
 *
 * ButtonEvent dispatch via one-shot timer (detailed flow):
 * 1. A GPIO interrupt fires for BUTTON_B1 and `button1_irq_handler` runs in IRQ context.
 * 2. The ISR does minimal work: it starts a one-shot `FreeRtosTimer` and passes the
 *    event bitmask as the timer's `callback_arg` (no heavy logic in IRQ).
 * 3. When the timer expires, `button1_irq_dispatcher` runs in the FreeRTOS timer
 *    service task (normal task context).
 * 4. The dispatcher converts the `void*` argument back to the event bitmask and
 *    calls `button1_event.SetEvents(events)`, which notifies the waiting task.
 * 5. The `button1_monitor` task unblocks in `GetEvents()` and processes the event.
 *
 * System LED behavior:
 * - The system LED (LED1_GREEN) is shared by `button2_monitor` and `system_monitor`.
 * - When Button 2 is pressed, `button2_monitor` tries to acquire the LED; if it
 *   succeeds, the LED stays **on** while the button remains pressed, then turns
 *   off on release.
 * - When Button 2 is not holding the LED, `system_monitor` blinks it by acquiring
 *   the LED for 500 ms, turning it on, then off and releasing it.
 */
#include <thread>
#include <chrono>

#include <cstdio>

#include "freertos_task.hpp"
#include "freertos_timer.hpp"
#include "gpio.hpp"
#include "platform.hpp"

#include "button_event.hpp"
#include "pwm.hpp"
#include "safe_led.hpp"

/**
 * @brief One-shot timer used to defer GPIO IRQ handling to the timer task.
 *
 * The ISR records the event and starts this timer. When it fires, it runs in
 * the FreeRTOS timer service task and signals the ButtonEvent object.
 */
c7222::FreeRtosTimer* dispatcher_timer = nullptr;

/**
 * @brief Thread-safe event mailbox for Button 1 IRQ events.
 *
 * Producers: the timer callback (not ISR).
 * Consumers: the Button 1 monitor task.
 */
c7222::ButtonEvent button1_event;
/**
 * @brief Shared LED protected by SafeLed for multi-task coordination.
 */
c7222::SafeLed* system_led = nullptr;
/**
 * @brief PWM-controlled LED instance (LED3_RED).
 */
std::unique_ptr<c7222::PwmOut> pwm_led3_red = nullptr;
/**
 * @brief Current PWM duty cycle used to dim LED3_RED.
 */
float duty_cycle = 1.0f;

/**
 * @brief Timer callback that publishes Button 1 events to the task.
 *
 * This runs in the FreeRTOS timer service task context, not in interrupt
 * context. It pushes the event bitmask into ButtonEvent.
 */
void button1_irq_dispatcher(void* arg){
	const auto events = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
	printf("[TIMER] Dispatching button 1 event: %lu\n", events);
	button1_event.SetEvents(events);
}

/**
 * @brief GPIO IRQ handler for Button 1.
 *
 * Runs in IRQ context. It must be minimal, so it schedules the dispatcher
 * timer and passes the event bitmask as the timer callback argument.
 */
void button1_irq_handler(uint32_t events){
	assert(dispatcher_timer != nullptr && "Dispatcher timer not initialized");
	assert(dispatcher_timer->IsValid() && "Dispatcher timer is not valid");
	dispatcher_timer->StartFromISR(reinterpret_cast<void*>(static_cast<uintptr_t>(events)));
}

/**
 * @brief Button 1 monitoring task.
 *
 * - Arms the IRQ and the dispatcher timer.
 * - Waits for events using ButtonEvent (condition-variable backed).
 * - Toggles LED2_GREEN on timeout (heartbeat).
 * - On press, turns LED2_RED on and decreases LED3_RED PWM duty cycle, which decreases brightness.
 * - On release, turns LED2_RED off.
 */
[[noreturn]] void button1_monitor(void* param){
	(void)param;
	auto platform = c7222::Platform::GetInstance();
	dispatcher_timer = new c7222::FreeRtosTimer("Button1Dispatcher",
												 c7222::FreeRtosTask::MsToTicks(10),
												 c7222::FreeRtosTimer::Type::kOneShot,
												 button1_irq_dispatcher);
	
	platform->EnableButtonIrq(c7222::PicoWBoard::ButtonId::BUTTON_B1, c7222::GpioInputEvent::BothEdges, button1_irq_handler);	
	
	auto& led1 = platform->GetLed(c7222::PicoWBoard::LedId::LED2_RED);
	auto& led2 = platform->GetLed(c7222::PicoWBoard::LedId::LED2_GREEN);
	printf("[BUT1]: Started!\r\n");
	for(;;) {
		uint32_t events = 0;
		if(!button1_event.GetEvents(1000, events)) {
			led2.Toggle();
			continue; // timeout
		}

		if(events & static_cast<uint32_t>(c7222::GpioInputEvent::FallingEdge)) {
			printf("Button 1 Pressed\n");
			led1.On();
			duty_cycle -= 0.1f;
			if(duty_cycle < 0.0f) {
				duty_cycle = 1.0f;
			}
			pwm_led3_red->SetDutyCycle(duty_cycle);
			printf("Set LED3_RED duty cycle to %.0f%%\n", duty_cycle * 100.0f);
			
		} else if(events & static_cast<uint32_t>(c7222::GpioInputEvent::RisingEdge)) {
			printf("Button 1 Released\n");
			led1.Off();
		} else {
			printf("Button1 Unknown event: %lu\n", events);
		}
	}
}

/**
 * @brief Button 2 monitoring task.
 *
 * Uses polling (every 100 ms) and coordinates access to the shared system LED
 * via SafeLed. This demonstrates cross-task protection of a shared LED.
 */
[[noreturn]] void button2_monitor(void* param){
	(void)param;
	assert(system_led != nullptr && "System LED not initialized");
	bool led_acquired = false;
	bool button_pressed = false;
	printf("[BUT2]: Started!\r\n");
	for(;;) {
		bool b2 = c7222::Platform::GetInstance()->IsButtonPressed(c7222::PicoWBoard::ButtonId::BUTTON_B2);
		if(b2 != button_pressed){
			if(b2) {
				printf("Button 2 Pressed. Acquiring system led: ");
				led_acquired = system_led->AcquireFor(10);
				if(led_acquired) {
					printf("OK\n");
					system_led->On();
				} else {
					printf("FAILED\n");
				}
			} else {
				printf("Button 2 Released. Releasing system led!\n");
				if(led_acquired) {
					system_led->Off();
					system_led->Release();
					led_acquired = false;
				}
			}
			button_pressed = b2;
		}
		c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(100));
	}
}

/**
 * @brief Periodic system task that blinks the shared system LED.
 *
 * It attempts to acquire the SafeLed with a timeout to avoid blocking
 * indefinitely when another task holds the LED.
 */
[[noreturn]] void system_monitor(void* param){
	(void)param;
	bool led_acquired;
	printf("[SYS]: Started!\r\n");
	for(;;) {
		c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(500));
		led_acquired = system_led->AcquireFor(500);
		if(led_acquired){
			system_led->On();
		} else {
			printf("[SYS] Cannot acquire the led!\r\n");
		}
		c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(500));
		if(led_acquired){
			system_led->Off();
			system_led->Release();
		}
	}
}

/**
 * @brief Program entry point.
 */
[[noreturn]] int main() {
	// Create the platform singleton and initialize architecture (CYW43, etc).
	auto* platform = c7222::Platform::GetInstance();
	platform->Initialize();

	std::printf("Starting FreeRTOS C++ devices examples...\n");
	// Initialize the on-board LED and wrap it in a SafeLed for thread-safe access.
	system_led = new c7222::SafeLed(c7222::PicoWBoard::LedId::LED1_GREEN);

	pwm_led3_red = platform->CreateLedPwm(c7222::PicoWBoard::LedId::LED3_RED, 255);
	pwm_led3_red->Enable(true);

	// Each std::thread maps to a FreeRTOS task via FreeRTOS-CPP11.
	std::thread button1_monitor_thread(button1_monitor, nullptr);
	std::thread button2_monitor_thread(button2_monitor, nullptr);
	std::thread system_monitor_thread(system_monitor, nullptr);

	// button1_monitor_thread.detach();
	// button2_monitor_thread.detach();
	// system_monitor_thread.detach();

	c7222::FreeRtosTask::StartScheduler();

	while(1) {}
}
