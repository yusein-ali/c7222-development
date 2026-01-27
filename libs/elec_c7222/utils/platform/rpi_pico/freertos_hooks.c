#include <FreeRTOS.h>
#include <task.h>

#include <stdint.h>
#include <stdio.h>

#include <pico/time.h>

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	// Breakpoint here!
	// If you hit this, you know 100% that 'pcTaskName' (likely "Startup") ran out of stack.
	printf("Stack of Task \"%s\" with handle 0x%08lx OVERFLOWED\r\n", pcTaskName, (uint32_t)xTask);

	while(1) {}
}

#if (configGENERATE_RUN_TIME_STATS == 1)
void vConfigureTimerForRunTimeStats(void) {
	// Pico SDK time base already running; nothing to configure.
}

unsigned long ulGetRunTimeCounterValue(void) {
	return (unsigned long)time_us_64();
}

#endif