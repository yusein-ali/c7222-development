#include <FreeRTOS.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	// Breakpoint here!
	// If you hit this, you know 100% that 'pcTaskName' (likely "Startup") ran out of stack.
	printf("Stack of Task \"%s\" with handle 0x%08x OVERFLOWED\r\n", pcTaskName, (uint32_t)xTask);

	while(1) {}
}

#if (configGENERATE_RUN_TIME_STATS == 1)
void vConfigureTimerForRunTimeStats(void) {
	// Use CLOCK_MONOTONIC; no explicit configuration is required on Linux.
}

unsigned long ulGetRunTimeCounterValue(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	const unsigned long seconds_us = (unsigned long)ts.tv_sec * 1000000ul;
	const unsigned long nanos_us = (unsigned long)(ts.tv_nsec / 1000ul);
	return seconds_us + nanos_us;
}

#endif
