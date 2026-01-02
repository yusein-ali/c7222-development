/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <assert.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <new>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 100
#endif

using namespace std::chrono_literals;

int pico_led_init()
{
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

void pico_set_led(bool led_on)
{
#if defined(PICO_DEFAULT_LED_PIN)
  gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

std::thread led_thread;
std::thread log_thread;

static void led_task()
{
  vTaskPrioritySet(nullptr, tskIDLE_PRIORITY + 1);
  
  const auto led_delay = std::chrono::milliseconds(LED_DELAY_MS);
  for (;;)
  {
    pico_set_led(true);
    printf("LED ON (std::thread)\n");
    std::this_thread::sleep_for(led_delay);

    pico_set_led(false);
    printf("LED OFF (std::thread)\n");
    std::this_thread::sleep_for(led_delay);
  }
}

static void log_task()
{
  vTaskPrioritySet(nullptr, tskIDLE_PRIORITY);
  for (;;)
  {
    printf("Low priority logger tick\n");
    std::this_thread::sleep_for(1s);
  }
}



// 1. The Startup Logic
void startup_wrapper(void* param) {
	// A. Now the scheduler IS running.

	// B. Create your C++ threads here
	led_thread = std::thread{led_task};
	led_thread.detach();
	log_thread = std::thread{log_task};
	log_thread.detach();
	// The taks can actually start, run, and signal "I'm alive",
	// so wait_for_start() will succeed.

	// D. Delete this startup task (cleanup)
	vTaskDelete(NULL);
}

int main()
{
  bool bRet = stdio_init_all();
  assert(bRet != false);
  int rc = pico_led_init();
  hard_assert(rc == PICO_OK);


  // Create the ONE raw task manually
  // led_thread = new std::thread{led_task};
  // log_thread = new std::thread{log_task};
  TaskHandle_t xHandle = nullptr;
  xTaskCreate(startup_wrapper, "Startup", 2048, NULL, 1, &xHandle);
  hard_assert(xHandle != nullptr);
  // vTaskCoreAffinitySet(xHandle, 1 << 0);
  // C. Now detach works!
  // Start Scheduler
  vTaskStartScheduler();

  while (true)
  {
    tight_loop_contents();
  }
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	// Breakpoint here!
	// If you hit this, you know 100% that 'pcTaskName' (likely "Startup") ran out of stack.
	while(1){

  }
}


#include <stdlib.h>

extern "C" void* __wrap_malloc(size_t size) {
	return pvPortMalloc(size);
}
extern "C" void __wrap_free(void* ptr) {
	vPortFree(ptr);
}
extern "C" void* __wrap_calloc(size_t num, size_t size) {
	size_t total = num * size;
	void* ptr = pvPortMalloc(total);
	if(ptr)
		memset(ptr, 0, total);
	return ptr;
}
// Note: realloc is complex in Heap 4, avoid if possible or implement strictly.