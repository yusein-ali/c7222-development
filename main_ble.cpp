#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include <advertisement_data.hpp>

// Define a simple advertisement payload
const uint8_t adv_data[] = {
	// Flags: General Discoverable Mode, BR/EDR Not Supported
	0x02,
	0x01,
	0x06,
	// Name: "Pico2_BLE"
	0x0A,
	0x09,
	'P',
	'i',
	'c',
	'o',
	'2',
	'_',
	'B',
	'L',
	'E'};

static btstack_packet_callback_registration_t hci_event_callback_registration;
// Replace this line:
// gap_advertisements_set_params(160, 160);

// With this corrected version:
uint8_t adv_type = 0; // 0 = ADV_IND (Connectable undirected advertising)
uint8_t direct_address_type = 0;
bd_addr_t null_addr = {0};
uint8_t channel_map = 0x07; // Use all 3 advertising channels (37, 38, 39)
uint8_t filter_policy = 0;	// 0 = Allow scan/connect from any

// -------------------------------------------------------------------------
// Packet Handler: Receive events from the BLE Stack
// -------------------------------------------------------------------------
static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
	UNUSED(channel);
	UNUSED(size);

	if(packet_type != HCI_EVENT_PACKET)
		return;

	switch(hci_event_packet_get_type(packet)) {

	// This event fires when the stack state changes (e.g. OFF -> ON)
	case BTSTACK_EVENT_STATE:
		if(btstack_event_state_get_state(packet) != HCI_STATE_WORKING)
			return;

		printf("BTstack is up and running.\n");

		// 1. Set Advertisement Data
		gap_advertisements_set_data(sizeof(adv_data), (uint8_t*) adv_data);

		// 2. Set Advertisement Parameters (Interval: 100ms approx)
		// min_interval * 0.625ms, max_interval * 0.625ms
		gap_advertisements_set_params(160,				   // Min Interval (160 * 0.625 = 100ms)
									  160,				   // Max Interval
									  adv_type,			   // Advertising Type
									  direct_address_type, // Direct Address Type
									  null_addr,		   // Direct Address (not used for ADV_IND)
									  channel_map,		   // Channel Map
									  filter_policy		   // Filter Policy
		);

		// 3. Enable Advertising
		gap_advertisements_enable(1);
		printf("Advertising started as 'Pico2_BLE'...\n");
		break;

	default:
		break;
	}
}

// -------------------------------------------------------------------------
// BLE Application Task
// -------------------------------------------------------------------------
void ble_app_task(void* params) {
	(void) params;

	// Generate the packet using the advertisement data class
	c7222::AdvertisementData adv;

	
	// Initialize CYW43 Architecture (Starts the SDK background worker)
	if(cyw43_arch_init()) {
		printf("CYW43 init failed\n");
		vTaskDelete(NULL);
	}

	printf("CYW43 init complete. Setting up BTstack...\n");

	// Standard BTstack initialization
	l2cap_init();
	sm_init();

	// Register our packet handler to receive system events
	hci_event_callback_registration.callback = &packet_handler;
	hci_add_event_handler(&hci_event_callback_registration);

	// Turn on the Bluetooth hardware
	hci_power_control(HCI_POWER_ON);

	// Enter infinite loop to keep task alive (or perform other app logic)
	while(true) {
		// Blink LED to show system is alive
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
		vTaskDelay(pdMS_TO_TICKS(500));
		cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
		vTaskDelay(pdMS_TO_TICKS(500));
		printf("BLE App Task is running...%lu\n", xTaskGetTickCount()/1000);
	}
}

// -------------------------------------------------------------------------
// Main
// -------------------------------------------------------------------------
int main() {
	stdio_init_all();
	sleep_ms(2000); // Give serial monitor time to connect
	printf("Starting FreeRTOS BLE Example...\n");

	// Create the BLE application task
	// Stack size 1024 words (4096 bytes) is usually sufficient for basic advertising
	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the Scheduler
	vTaskStartScheduler();

	// Should never reach here
	while(1){

	}

	return 0;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
	// Breakpoint here!
	// If you hit this, you know 100% that 'pcTaskName' (likely "Startup") ran out of stack.
	while(1) {}
}
