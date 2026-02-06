/**
 * @file main_ble_gap.cpp
 * @brief Minimal BLE GAP example (FreeRTOS).
 *
 * This example focuses purely on GAP functionality: setting up advertising,
 * registering a GAP event handler, and periodically updating manufacturer data.
 *
 * Dependencies:
 * - `GapEventHandler` from `examples/ble/common/gap_event_handler.hpp` is used
 *   to log GAP events (advertising, scanning, connections). It provides a
 *   lightweight, reusable handler for examples.
 * - `c7222::Ble` and `c7222::Gap` encapsulate BTstack and provide the APIs for
 *   advertising and data updates.
 * - `c7222::OnBoardLED` provides a visible heartbeat while advertising.
 */
#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "advertisement_data.hpp"
#include "../common/gap_event_handler.hpp"
#include "ble.hpp"
#include "gap.hpp"
#include "onboard_led.hpp"
#include "pico/stdlib.h"
#include "platform.hpp"
#include "task.h"

/// On-board LED used as a heartbeat while advertising.
static c7222::OnBoardLED* onboard_led = nullptr;
/// Platform abstraction (initializes CYW43/BTstack).
static c7222::Platform* platform = nullptr;
/// Common GAP event handler used for logging.
static GapEventHandler gap_event_handler;

// ------------------------------------------------------------
// GAP demo: configure advertising and start it when BLE turns on.
// ------------------------------------------------------------
/**
 * @brief Callback executed when the BLE stack is fully initialized.
 *
 * Sets up advertising parameters and data, then starts advertising.
 */
static void on_turn_on() {
	std::printf("Bluetooth Turned On\n");
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	// Register the common GAP event handler for logging.
	gap->AddEventHandler(gap_event_handler);

	// Flags + device name for a discoverable GAP demo.
	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
							   c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("Pico2_BLE++");

	// Manufacturer specific payload to show how data is injected.
	uint32_t value = 0x12345678;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											 (uint8_t*)&value,
											 sizeof(value)));

	// Basic advertising parameters (200ms to 250ms interval).
	{
		c7222::Gap::AdvertisementParameters adv_params;
		adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
		adv_params.min_interval = 320;
		adv_params.max_interval = 400;
		gap->SetAdvertisingParameters(adv_params);
	}

	gap->StartAdvertising();
	std::printf("Advertising started as 'Pico2_BLE++'...\n");
}

// ------------------------------------------------------------
// BLE Application Task
// ------------------------------------------------------------
/**
 * @brief FreeRTOS task that owns BLE initialization and periodic updates.
 *
 * Initializes the platform and BLE stack, then periodically updates
 * manufacturer data while advertising is active.
 */
[[noreturn]] void ble_app_task(void* params) {
	(void)params;
	static uint32_t seconds = 0;

	// Initialize platform (CYW43 + BTstack).
	platform = c7222::Platform::GetInstance();
	platform->Initialize();

	// LED used for a visible heartbeat.
	onboard_led = c7222::OnBoardLED::GetInstance();
	auto* ble = c7222::Ble::GetInstance(false);
	auto* gap = ble->GetGap();
	auto& adb = ble->GetAdvertisementDataBuilder();

	// Register the stack-on callback and power up the BLE stack.
	ble->SetOnBleStackOnCallback(on_turn_on);
	ble->TurnOn();

	std::printf("BLE Stack is ON!\n");

	while(true) {
		seconds = xTaskGetTickCount() / 1000;
		vTaskDelay(pdMS_TO_TICKS(100));

		if(gap->IsAdvertisingEnabled()) {
			// Update manufacturer data by replacing the last element in the builder:
			// Pop removes the most recently added advertising element, and Push appends
			// the new element. After the update, SetAdvertisingData() rebuilds and applies
			// the full advertising payload to the controller.
			auto ad = c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											   (uint8_t*)&seconds,
											   sizeof(seconds));
			adb.Pop();
			adb.Push(ad);
			ble->SetAdvertisingData();
			onboard_led->Toggle();
		}
	}
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
/**
 * @brief Program entry point.
 */
[[noreturn]] int main() {
	stdio_init_all();
	std::printf("Starting FreeRTOS BLE GAP Example...\n");

	// Launch the BLE GAP task.
	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while(1) {}
}
