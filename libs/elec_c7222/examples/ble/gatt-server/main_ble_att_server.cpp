/**
 * @file main_ble_att_server.cpp
 * @brief BLE GATT server example with AttributeServer and SecurityManager.
 *
 * Demonstrates how to:
 * - Enable the AttributeServer with a GATT profile database.
 * - Register GAP and Security Manager event handlers.
 * - Locate services/characteristics and attach event handlers.
 * - Periodically update a characteristic value (temperature) using a timer.
 *
 * Dependencies:
 * - `GapEventHandler` logs GAP events and restarts advertising on disconnect.
 * - `SecurityEventHandler` logs pairing/authorization and performs minimal policy.
 * - `BleOnchipTemperature` binds `CharacteristicEventHandler` instances to
 *   temperature/configuration characteristics for event logging.
 */
#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "advertisement_data.hpp"
#include "gap_event_handler.hpp"
#include "ble_onchip_temperature.hpp"
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


/// On-board LED used as a heartbeat while advertising.
static c7222::OnBoardLED* onboard_led = nullptr;
/// Temperature sensor wrapper used to read on-chip temperature.
static c7222::OnChipTemperatureSensor* temp_sensor = nullptr;
/// Periodic timer used to update the temperature characteristic.
static c7222::FreeRtosTimer app_timer;
/// Temperature characteristic handle resolved from the ATT database.
static c7222::Characteristic* temperature_characteristic = nullptr;
/// Configuration characteristic handle resolved from the ATT database.
static c7222::Characteristic* configuration_characteristic = nullptr;
/// Platform abstraction (initializes CYW43/BTstack).
static c7222::Platform* platform = nullptr;
/// SecurityManager instance for pairing/authorization.
static c7222::SecurityManager* security_manager = nullptr;
/// AttributeServer instance providing GATT database access.
static c7222::AttributeServer* att_server = nullptr;

/// Helper that binds characteristic event handlers for logging.
static BleOnchipTemperature* ble_temperature_manager = nullptr;
/// Security event handler (minimal example policy).
static SecurityEventHandler security_event_handler;
/// GAP event handler (logging + restart advertising).
static GapEventHandler gap_event_handler;

/**
 * @brief Periodic timer callback to update temperature value.
 *
 * Reads the temperature sensor and writes a fixed-point value (C * 100) into
 * the temperature characteristic when a connection is active.
 */
static void timer_callback() {
	assert(onboard_led != nullptr && "OnBoardLED instance is null in timer callback!");
	assert(temp_sensor != nullptr && "OnChipTemperatureSensor instance is null in timer callback!");

	auto temperature_c = temp_sensor->GetCelsius();

	onboard_led->Toggle();

	// Update the temperature characteristic if present.
	if(temperature_characteristic != nullptr) {
		auto temp_fixed_point = static_cast<int16_t>(temperature_c * 100);
		// SetValue writes the value and triggers notify/indicate if enabled.
		if(att_server->IsConnected()) {
			temperature_characteristic->SetValue(temp_fixed_point);
		}
	} else {
		printf("Timer Callback: T = %.2f C\n", temperature_c);
	}
}

// -------------------------------------------------------------------------
// Packet Handler: Receive events from the BLE Stack
// -------------------------------------------------------------------------
/**
 * @brief Callback executed when the BLE stack is fully initialized.
 *
 * Configures advertising data/parameters and starts advertising.
 */
static void on_turn_on() {
	printf("Bluetooth Turned On\n");
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();

	// Register GAP event handler for logging.
	gap->AddEventHandler(gap_event_handler);
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	// Generate the packet using the advertisement data class.
	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
							   c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("Pico2_BLE++");

	uint32_t value = 0x12345678;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											 (uint8_t*) &value,
											 sizeof(value)));

	// Configure advertising parameters (200ms to 250ms interval).
	{
		c7222::Gap::AdvertisementParameters adv_params;
		adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
		adv_params.min_interval = 320;
		adv_params.max_interval = 400;
		gap->SetAdvertisingParameters(adv_params);
	}
	// Start advertising.
	gap->StartAdvertising();
	printf("Advertising started as 'Pico2_BLE'...\n");
}

// -------------------------------------------------------------------------
// BLE Application Task
// -------------------------------------------------------------------------
/**
 * @brief FreeRTOS task that owns BLE initialization and GATT server setup.
 *
 * Initializes platform, security manager, attribute server, resolves
 * characteristics, and starts advertising. A timer periodically updates
 * the temperature characteristic when connected.
 */
[[noreturn]] void ble_app_task(void* params) {
	(void) params;
	
	static uint32_t seconds = 0;
	// Initialize CYW43 Architecture platform (starts the SDK background worker).
	platform = c7222::Platform::GetInstance();
	platform->Initialize();

	onboard_led = c7222::OnBoardLED::GetInstance();
	temp_sensor = c7222::OnChipTemperatureSensor::GetInstance();

	// Timer used for periodic temperature updates.
	app_timer.Initialize("AppTimer",
						 pdMS_TO_TICKS(2000),
						 c7222::FreeRtosTimer::Type::kPeriodic,
						 std::bind(&timer_callback));

	auto* ble = c7222::Ble::GetInstance(false);
	auto* gap = ble->GetGap();
	// Configure and enable Security Manager.
	{
		c7222::SecurityManager::SecurityParameters sm_params;
		sm_params.authentication =
			c7222::SecurityManager::AuthenticationRequirement::kMitmProtection;

		sm_params.io_capability = c7222::SecurityManager::IoCapability::kDisplayOnly;
		sm_params.gatt_client_required_security_level =
			c7222::SecurityManager::GattClientSecurityLevel::kLevel2;  // Authenticated + encrypted
		security_manager = ble->EnableSecurityManager(sm_params);
		security_event_handler.SetSecurityManager(security_manager);
		ble->AddSecurityEventHandler(&security_event_handler);
	}
	// Enable AttributeServer with the generated GATT database.
	att_server = ble->EnableAttributeServer(profile_data);
	gap_event_handler.SetAttributeServer(att_server);
	auto& adb = ble->GetAdvertisementDataBuilder();
	std::cout << "Attribute server initialized." << std::endl;

	// Verify the Environmental Sensing Service exists in the DB.
	auto* service = att_server->FindServiceByUuid(
		c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING)));
	if(service != nullptr) {
		std::cout << "Environmental Sensing Service found in ATT DB." << std::endl << *service
				  << std::endl;
	} else {
		std::cout << "Environmental Sensing Service NOT found in ATT DB!" << std::endl;
		assert(false);
	}

	// Resolve configuration characteristic by handle and initialize it.
	configuration_characteristic = att_server->FindCharacteristicByHandle(
		ATT_CHARACTERISTIC_fc930f88_1a30_45d7_8c17_604c1a036b9f_01_USER_DESCRIPTION_HANDLE);

	if(configuration_characteristic != nullptr) {
		configuration_characteristic->SetValue((uint16_t) 0x0000);
		std::cout << "Configuration Characteristic found in ATT DB." << std::endl
				  << *configuration_characteristic << std::endl;
	} else {
		std::cout << "Configuration Characteristic NOT found in ATT DB!" << std::endl;
		assert(false);
	}

	// Ensure user description descriptor exists and set a friendly label.
	if(!configuration_characteristic->HasUserDescription()){
		std::cout << "Characteristic does not have user description!" << std::endl;
		assert(false);
	}
	configuration_characteristic->SetUserDescription("Configuration");

	// Resolve the temperature characteristic.
	auto* temp_service = att_server->FindServiceByUuid(
		c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING)));

	if(temp_service != nullptr) {
		std::cout << "Found Temperature Service!" << std::endl;
		temperature_characteristic = temp_service->FindCharacteristicByUuid(
			c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE)));
		temperature_characteristic->SetUserDescription("Temperature");
	} else {
		std::cout << "Temperature Service not found!" << std::endl;
		std::cout << "Not setting up temperature updates." << std::endl;
		temperature_characteristic = nullptr;
	}

	// Attach event handlers for the temperature and configuration characteristics.
	std::cout << "Initializing BleOnchipTemperature manager with characteristics..." << std::endl;
	ble_temperature_manager = BleOnchipTemperature::GetInstance(temperature_characteristic,
														   configuration_characteristic);
	

	std::cout << "Printing Attribute Server" << std::endl;
	std::cout << *att_server << std::endl;

	printf("CYW43 init complete. Powering up BTstack... here!\n");
	// Start BLE stack; on_turn_on() will begin advertising.
	ble->SetOnBleStackOnCallback(on_turn_on);
	ble->TurnOn();

	std::cout << "BLE Stack is ON!" << std::endl;
	const auto ret = app_timer.Start(100);
	if(!ret) {
		std::cout << "Failed to start timer!" << std::endl;
	} else {
		std::cout << "Timer started and will fire in 100 ticks!" << std::endl;
	}
	// Enter infinite loop to keep task alive (and update advertising data).
	while(true) {
		seconds = xTaskGetTickCount() / 1000;
		vTaskDelay(pdMS_TO_TICKS(100));
		if(gap->IsAdvertisingEnabled()) {
			// Replace the last advertising element with updated manufacturer data.
			auto ad = c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											   (uint8_t*) &seconds,
											   sizeof(seconds));
			adb.Pop();
			adb.Push(ad);
			ble->SetAdvertisingData();
			onboard_led->Toggle();
		}
	}
}

// -------------------------------------------------------------------------
// Main
// -------------------------------------------------------------------------
/**
 * @brief Program entry point.
 */
[[noreturn]] int main() {
	stdio_init_all();
	printf("Starting FreeRTOS BLE Example...\n");

	// Create the BLE application task.
	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);

	// Start the scheduler.
	vTaskStartScheduler();

	// Should never reach here.
	while(1) {}
}


