#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "advertisement_data.hpp"
#include "../common/app_gap.hpp"
#include "ble.hpp"
#include "characteristic.hpp"
#include "freertos_timer.hpp"
#include "gap.hpp"
#include "onboard_led.hpp"
#include "onchip_temperature_sensor.hpp"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "platform.hpp"
#include "security_event_handler.hpp"
#include "security_manager.hpp"
#include "task.h"
#include "app_profile.h"


static c7222::OnBoardLED* onboard_led = nullptr;
static c7222::OnChipTemperatureSensor* temp_sensor = nullptr;
static c7222::FreeRtosTimer app_timer;
static c7222::Characteristic* temperature_characteristic = nullptr;
static c7222::Platform* platform = nullptr;
static c7222::SecurityManager* security_manager = nullptr;
static c7222::AttributeServer* att_server = nullptr;

static SecurityEventHandler security_event_handler;
static GapEventHandler gap_event_handler;

static void timer_callback() {
	assert(onboard_led != nullptr && "OnBoardLED instance is null in timer callback!");
	assert(temp_sensor != nullptr && "OnChipTemperatureSensor instance is null in timer callback!");

	auto temperature_c = temp_sensor->GetCelsius();

	onboard_led->Toggle();

	// set the value of the temperature characteristic
	if(temperature_characteristic != nullptr) {
		auto temp_fixed_point = static_cast<int16_t>(temperature_c * 100);
		// this must call SetValue with the fixed-point representation
		// but also notify or indicate if the client has enabled them.
		if(att_server->IsConnected()) {
		// 	printf("Timer Callback: T = %.2f C Value written 0x%04x\n",
		// 		   temperature_c,
		// 		   temp_fixed_point);
			temperature_characteristic->SetValue(temp_fixed_point);
		}
	} else {
		printf("Timer Callback: T = %.2f C\n", temperature_c);
	}
}

// -------------------------------------------------------------------------
// Packet Handler: Receive events from the BLE Stack
// -------------------------------------------------------------------------
static void on_turn_on() {
	printf("Bluetooth Turned On\n");
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adb = ble->GetAdvertisementDataBuilder();

	gap->AddEventHandler(gap_event_handler);
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	// Generate the packet using the advertisement data class
	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
							   c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("Pico2_BLE++");

	uint32_t value = 0x12345678;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											 (uint8_t*) &value,
											 sizeof(value)));

	// ------------------------------------------------
	// generate the advertisement parameters
	// ------------------------------------------------
	// note that the default parameters are fine for most use cases
	{
		c7222::Gap::AdvertisementParameters adv_params;
		// Set a custom interval: 200ms to 250ms
		// Interval is in units of 0.625 ms, so 320 * 0.625 = 200ms, 400 * 0.625 = 250ms
		adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
		adv_params.min_interval = 320;
		adv_params.max_interval = 400;
		// 2. Set Advertisement Parameters (Interval: 100ms approx)
		gap->SetAdvertisingParameters(adv_params);
	}
	// Start Advertising
	gap->StartAdvertising();
	printf("Advertising started as 'Pico2_BLE'...\n");
}

// -------------------------------------------------------------------------
// BLE Application Task
// -------------------------------------------------------------------------
[[noreturn]] void ble_app_task(void* params) {
	(void) params;
	
	static uint32_t seconds = 0;
	// Initialize CYW43 Architecture platform (Starts the SDK background worker)
	platform = c7222::Platform::GetInstance();
	platform->Initialize();

	onboard_led = c7222::OnBoardLED::GetInstance();
	temp_sensor = c7222::OnChipTemperatureSensor::GetInstance();

	app_timer.Initialize("AppTimer",
						 pdMS_TO_TICKS(2000),
						 c7222::FreeRtosTimer::Type::kPeriodic,
						 std::bind(&timer_callback));

	auto* ble = c7222::Ble::GetInstance(false);
	auto* gap = ble->GetGap();
	// security_manager = ble->GetSecurityManager();
	{
		c7222::SecurityManager::SecurityParameters sm_params;
		// Configure Security Manager parameters
		sm_params.authentication =
			c7222::SecurityManager::AuthenticationRequirement::kMitmProtection;

		sm_params.io_capability = c7222::SecurityManager::IoCapability::kDisplayOnly;
		sm_params.gatt_client_required_security_level =
			c7222::SecurityManager::GattClientSecurityLevel::kLevel2;  // Authenticated + encrypted
		security_manager = ble->EnableSecurityManager(sm_params);
		security_event_handler.SetSecurityManager(security_manager);
		ble->AddSecurityEventHandler(&security_event_handler);
	}

	att_server = ble->EnableAttributeServer(profile_data);
	gap_event_handler.SetAttributeServer(att_server);
	auto& adb = ble->GetAdvertisementDataBuilder();

	ble->DumpAttributeServerContext();
	std::cout << "Attribute server initialized." << std::endl
			  << "Printing Attribute Server" << std::endl;
	std::cout << *att_server << std::endl;

	printf("CYW43 init complete. Setting up BTstack... here!\n");

	// now, let us look for the Temperature Service and its characteristic
	auto* temp_service = att_server->FindServiceByUuid(
		c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING)));

	if(temp_service != nullptr) {
		std::cout << "Found Temperature Service!" << std::endl;
		temperature_characteristic = temp_service->FindCharacteristicByUuid(
			c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE)));
	} else {
		std::cout << "Temperature Service not found!" << std::endl;
		std::cout << "Not setting up temperature updates." << std::endl;
		temperature_characteristic = nullptr;
	}

	ble->SetOnBleStackOnCallback(on_turn_on);
	ble->TurnOn();

	std::cout << "BLE Stack is ON!" << std::endl;
	const auto ret = app_timer.Start(100);
	if(!ret) {
		std::cout << "Failed to start timer!" << std::endl;
	} else {
		std::cout << "Timer started and will fire in 100 ticks!" << std::endl;
	}
	// Enter infinite loop to keep task alive (or perform other app logic)
	while(true) {
		seconds = xTaskGetTickCount() / 1000;
		// Blink LED to show system is alive
		// onboard_led->Toggle();
		// vTaskDelay(pdMS_TO_TICKS(500));
		// onboard_led->Toggle();
		vTaskDelay(pdMS_TO_TICKS(100));
		// printf("BLE App Task is running...%lu\n", seconds);
		if(gap->IsAdvertisingEnabled()) {
			// printf("Updating the manuf specific data to %lu\n", seconds);
			auto ad = c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											   (uint8_t*) &seconds,
											   sizeof(seconds));
			adb.Pop();
			adb.Push(ad);
			ble->SetAdvertisingData();
			onboard_led->Toggle();
		} else {
			// printf("Not advertising.\n");
		}
	}
}

// -------------------------------------------------------------------------
// Main
// -------------------------------------------------------------------------
[[noreturn]] int main() {
	stdio_init_all();
	printf("Starting FreeRTOS BLE Example...\n");

	// Create the BLE application task
	// Stack size 1024 words (4096 bytes) is usually sufficient for basic advertising
	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the Scheduler
	vTaskStartScheduler();

	// Should never reach here
	while(1) {}

	// return 0;
}


