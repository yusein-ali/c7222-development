/**
 * @file main_freertos.cpp
 * @brief C++ FreeRTOS LED test for Pico 2 W using `std::thread`.
 *
 * This source tests a Pico 2 W board by blinking the onboard LED while running
 * a low-priority logger task. It uses the Pico C SDK for board/LED access and
 * FreeRTOS as the scheduler, but the application task model is C++-oriented.
 *
 * Difference from `main_freertos.c`:
 * - `main_freertos.c` creates tasks directly with `xTaskCreate()` and uses
 *   `vTaskDelay()`.
 * - `main_freertos.cpp` creates `std::thread` tasks and uses
 *   `std::this_thread::sleep_for()`, with priorities assigned from inside the
 *   running task via `vTaskPrioritySet()`.
 *
 * Dependency on FreeRTOS-cpp11:
 * - The `std::thread`/chrono behavior in this file depends on the
 *   FreeRTOS-cpp11 integration (`FREERTOS_CPP11_SOURCES` and
 *   `FREERTOS_CPP11_INCLUDE_DIRS` in CMake).
 * - FreeRTOS-cpp11 provides the libstdc++ threading glue for FreeRTOS so C++
 *   threading APIs map to FreeRTOS primitives.
 */

#include <assert.h>
#include <chrono>
#include <new>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <functional>


#include "pico/stdlib.h"
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 100
#endif

using namespace std::chrono_literals;

int pico_led_init() {
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
	return cyw43_arch_init();
#else
	return PICO_ERROR_GENERIC;
#endif
}

/**
 * @brief Set the onboard LED state.
 *
 * Usage:
 * - Call after `pico_led_init()` returns `PICO_OK`.
 * - Pass `true` to switch LED on, `false` to switch LED off.
 *
 * @param led_on Desired LED state.
 */
void pico_set_led(bool led_on) {
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

/**
 * @brief Global LED worker thread handle.
 *
 * Created in `BaseClass::startup()` and detached immediately.
 */
std::thread led_thread;
/**
 * @brief Global logger worker thread handle.
 *
 * Created in `BaseClass::startup()` and detached immediately.
 */
std::thread log_thread;

/**
 * @brief LED blink worker executed by `std::thread`.
 *
 * Behavior:
 * - Raises its own FreeRTOS priority to `tskIDLE_PRIORITY + 1`.
 * - Toggles LED ON/OFF forever with `LED_DELAY_MS` period.
 *
 * Usage:
 * - Spawn via `std::thread{led_task}` after FreeRTOS scheduler is running.
 */
static void led_task() {
	vTaskPrioritySet(nullptr, tskIDLE_PRIORITY + 1);

	const auto led_delay = std::chrono::milliseconds(LED_DELAY_MS);
	for(;;) {
		pico_set_led(true);
		printf("LED ON (std::thread)\n");
		std::this_thread::sleep_for(led_delay);

		pico_set_led(false);
		printf("LED OFF (std::thread)\n");
		std::this_thread::sleep_for(led_delay);
	}
}

/**
 * @brief Low-priority periodic logger executed by `std::thread`.
 *
 * Behavior:
 * - Sets its FreeRTOS priority to `tskIDLE_PRIORITY`.
 * - Prints a heartbeat log once per second forever.
 *
 * Usage:
 * - Spawn via `std::thread{log_task}` after scheduler start.
 */
static void log_task() {
	vTaskPrioritySet(nullptr, tskIDLE_PRIORITY);
	for(;;) {
		printf("Low priority logger tick\n");
		std::this_thread::sleep_for(1s);
	}
}

/**
 * @brief Singleton startup coordinator.
 *
 * This class provides a controlled one-time startup point that runs as an
 * initial FreeRTOS task. It then creates and detaches C++ worker threads.
 */
class BaseClass{
public:
    /**
     * @brief Get singleton instance.
     *
     * @return Reference to the unique `BaseClass` object.
     */
    static BaseClass& instance() {
        static BaseClass instance;
        return instance;
    }

    BaseClass(const BaseClass&) = delete;
    BaseClass& operator=(const BaseClass&) = delete;
    BaseClass(BaseClass&&) = delete;
    BaseClass& operator=(BaseClass&&) = delete;

    /**
     * @brief Destructor for diagnostics.
     */
    virtual ~BaseClass() {
        std::cout << "BaseClass destructor\n";
    }

    /**
     * @brief Startup routine run as a one-shot FreeRTOS task.
     *
     * Sequence:
     * 1. Wait briefly to allow scheduler/runtime stabilization.
     * 2. Create and detach LED and logger `std::thread` workers.
     * 3. Delete the current startup task with `vTaskDelete(NULL)`.
     *
     * Usage:
     * - Invoked via a bootstrap `xTaskCreate()` in `main()`.
     * - Requires FreeRTOS scheduler to be active.
     */
    void startup() {
      // A. Now the scheduler IS running.
      std::this_thread::sleep_for(100ms); // Give time for scheduler to stabilise
      std::cout << "BaseClass startup\n";
      // B. Create your C++ threads here
      led_thread = std::thread{led_task};
      led_thread.detach();
      log_thread = std::thread{log_task};
      log_thread.detach();
      // The taks can actually start, run, and signal "I'm alive",
      // so wait_for_start() will succeed.

      // D. Delete this startup task (cleanup)
      std::cout << "BaseClass startup ending, deleting task\n";
      vTaskDelete(NULL);
	}

private:
    /**
     * @brief Private constructor to enforce singleton usage.
     */
    BaseClass() {
        std::cout << "BaseClass constructor\n";
    }
};

/**
 * @brief Application entry point.
 *
 * Startup sequence:
 * 1. Initialize stdio and LED backend.
 * 2. Create a single bootstrap FreeRTOS task.
 * 3. Bootstrap task (`BaseClass::startup()`) creates detached `std::thread`
 *    workers.
 * 4. Start scheduler with `vTaskStartScheduler()`.
 *
 * How this is used:
 * - Build and link with Pico SDK, FreeRTOS kernel, and FreeRTOS-cpp11.
 * - Flash to Pico 2 W to validate C++ thread mapping onto FreeRTOS and LED
 *   operation.
 *
 * @return Never returns under normal conditions.
 */
int main() {
	bool bRet = stdio_init_all();
	assert(bRet != false);
	int rc = pico_led_init();
	hard_assert(rc == PICO_OK);
	std::cout << "Pico LED initialized\n";
	auto func = std::bind(&BaseClass::startup, &BaseClass::instance());
	// Create the ONE raw task manually
	// led_thread = new std::thread{led_task};
	// log_thread = new std::thread{log_task};
	// TaskHandle_t xHandle = nullptr;
	xTaskCreate([](void*){ BaseClass::instance().startup(); }, "Startup", 2048, NULL, 1, nullptr);
	// hard_assert(xHandle != nullptr);
	// C. Now detach works!
	// Start Scheduler
	vTaskStartScheduler();

	while(true) {
		tight_loop_contents();
	}
}

