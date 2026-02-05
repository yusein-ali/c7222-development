#include <cstdint>
#include <cstdio>
#include <cassert>
#include <array>
#include <string>
#include <iostream>

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

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdio_uart.h"

static c7222::OnBoardLED* onboard_led = nullptr;
static c7222::OnChipTemperatureSensor* temp_sensor = nullptr;
static c7222::FreeRtosTimer app_timer;
static c7222::Characteristic* temperature_characteristic = nullptr;
static c7222::Characteristic* led_characteristic = nullptr;
static c7222::Platform* platform = nullptr;
static c7222::SecurityManager* security_manager = nullptr;
static c7222::AttributeServer* att_server = nullptr;

static SecurityEventHandler security_event_handler;
static GapEventHandler gap_event_handler;

// -------------------- Course board (Aalto ELEC C7222) pins --------------------
#define UART_ID     uart0
#define UART_BAUD   115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// From the course-board silkscreen
#define LED1_PIN 21
#define LED2_PIN 20
#define LED3_PIN 19

static void board_init() {
	// Route printf() to UART0 (GP0/GP1) so output shows on the UART header
	uart_init(UART_ID, UART_BAUD);
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
	stdio_uart_init_full(UART_ID, UART_BAUD, UART_TX_PIN, UART_RX_PIN);

	// Initialize LEDs and force them OFF
	gpio_init(LED1_PIN);
	gpio_init(LED2_PIN);
	gpio_init(LED3_PIN);

	gpio_set_dir(LED1_PIN, GPIO_OUT);
	gpio_set_dir(LED2_PIN, GPIO_OUT);
	gpio_set_dir(LED3_PIN, GPIO_OUT);

	gpio_put(LED1_PIN, 0);
	gpio_put(LED2_PIN, 0);
	gpio_put(LED3_PIN, 0);
}

static inline void leds_all(int on) {
	gpio_put(LED1_PIN, on);
	gpio_put(LED2_PIN, on);
	gpio_put(LED3_PIN, on);
}

static inline void led_write(int idx, int on) {
	const uint pin = (idx == 1) ? LED1_PIN : (idx == 2) ? LED2_PIN : LED3_PIN;
	gpio_put(pin, on);
}

static inline void led_toggle(int idx) {
	const uint pin = (idx == 1) ? LED1_PIN : (idx == 2) ? LED2_PIN : LED3_PIN;
	gpio_put(pin, !gpio_get(pin));
}

static void handle_led_cmd(std::string s) {
	for (auto& c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));

	const auto has = [&](const char* w) { return s.find(w) != std::string::npos; };

	if (has("ALL")) {
		if (has("ON"))  { leds_all(1); return; }
		if (has("OFF")) { leds_all(0); return; }
	}

	if (has("LED1")) {
		if (has("TOGGLE")) { led_toggle(1); return; }
		if (has("ON"))     { led_write(1, 1); return; }
		if (has("OFF"))    { led_write(1, 0); return; }
	}

	if (has("LED2")) {
		if (has("TOGGLE")) { led_toggle(2); return; }
		if (has("ON"))     { led_write(2, 1); return; }
		if (has("OFF"))    { led_write(2, 0); return; }
	}

	if (has("LED3")) {
		if (has("TOGGLE")) { led_toggle(3); return; }
		if (has("ON"))     { led_write(3, 1); return; }
		if (has("OFF"))    { led_write(3, 0); return; }
	}
}

static void timer_callback() {
	assert(onboard_led != nullptr && "OnBoardLED instance is null in timer callback!");
	assert(temp_sensor != nullptr && "OnChipTemperatureSensor instance is null in timer callback!");

	auto temperature_c = temp_sensor->GetCelsius();
	onboard_led->Toggle();

	if (temperature_characteristic != nullptr) {
		auto temp_fixed_point = static_cast<int16_t>(temperature_c * 100);
		if (att_server->IsConnected()) {
			temperature_characteristic->SetValue(temp_fixed_point);
		}
	} else {
		printf("Timer Callback: T = %.2f C\n", temperature_c);
	}
}

static void on_turn_on() {
	printf("Bluetooth Turned On\n");
	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adb = ble->GetAdvertisementDataBuilder();

	gap->AddEventHandler(gap_event_handler);
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
							   c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("Pico2_BLE++");

	uint32_t value = 0x12345678;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											(uint8_t*)&value,
											sizeof(value)));

	{
		c7222::Gap::AdvertisementParameters adv_params;
		adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
		adv_params.min_interval = 320;
		adv_params.max_interval = 400;
		gap->SetAdvertisingParameters(adv_params);
	}

	gap->StartAdvertising();
	printf("Advertising started as 'Pico2_BLE++'...\n");
}

[[noreturn]] void ble_app_task(void* params) {
	(void)params;

	static uint32_t seconds = 0;

	platform = c7222::Platform::GetInstance();
	platform->Initialize();

	board_init();
	printf("Board init complete (UART + LEDs off)\n");

	onboard_led = c7222::OnBoardLED::GetInstance();
	temp_sensor = c7222::OnChipTemperatureSensor::GetInstance();

	app_timer.Initialize("AppTimer",
						 pdMS_TO_TICKS(2000),
						 c7222::FreeRtosTimer::Type::kPeriodic,
						 std::bind(&timer_callback));

	auto* ble = c7222::Ble::GetInstance(false);
	auto* gap = ble->GetGap();

	{
		c7222::SecurityManager::SecurityParameters sm_params;
		sm_params.authentication =
			c7222::SecurityManager::AuthenticationRequirement::kMitmProtection;
		sm_params.io_capability = c7222::SecurityManager::IoCapability::kDisplayOnly;
		sm_params.gatt_client_required_security_level =
			c7222::SecurityManager::GattClientSecurityLevel::kLevel2;
		security_manager = ble->EnableSecurityManager(sm_params);
		security_event_handler.SetSecurityManager(security_manager);
		ble->AddSecurityEventHandler(&security_event_handler);
	}

	att_server = ble->EnableAttributeServer(profile_data);
	gap_event_handler.SetAttributeServer(att_server);
	auto& adb = ble->GetAdvertisementDataBuilder();

	ble->DumpAttributeServerContext();
	std::cout << "Attribute server initialized.\n"
			  << "Printing Attribute Server\n";
	std::cout << *att_server << std::endl;

	printf("CYW43 init complete. Setting up BTstack...\n");

	// Temperature characteristic (Environmental Sensing)
	auto* temp_service = att_server->FindServiceByUuid(
		c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING)));

	if (temp_service != nullptr) {
		std::cout << "Found Temperature Service!\n";
		temperature_characteristic = temp_service->FindCharacteristicByUuid(
			c7222::Uuid(static_cast<uint16_t>(ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE)));
	} else {
		std::cout << "Temperature Service not found!\n";
		temperature_characteristic = nullptr;
	}

	// LED command characteristic (custom UUIDs in app_profile.gatt)
	// Service UUID: 0000FFF0-0000-1000-8000-00805F9B34FB
	// Char UUID:    0000FFF1-0000-1000-8000-00805F9B34FB
	static constexpr std::array<uint8_t, 16> kLedServiceUuid = {
		0x00,0x00,0xFF,0xF0, 0x00,0x00,0x10,0x00, 0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB
	};
	static constexpr std::array<uint8_t, 16> kLedCharUuid = {
		0x00,0x00,0xFF,0xF1, 0x00,0x00,0x10,0x00, 0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB
	};

	auto* led_service = att_server->FindServiceByUuid(c7222::Uuid(kLedServiceUuid));
	if (led_service != nullptr) {
		led_characteristic = led_service->FindCharacteristicByUuid(c7222::Uuid(kLedCharUuid));
	}

	if (led_characteristic != nullptr) {
		led_characteristic->GetValueAttribute().SetWriteCallback(
			[](uint16_t /*offset*/, const uint8_t* data, uint16_t size) -> c7222::BleError {
				std::string cmd(reinterpret_cast<const char*>(data),
				                reinterpret_cast<const char*>(data) + size);
				printf("LED cmd: %s\n", cmd.c_str());
				handle_led_cmd(cmd);
				return c7222::BleError::kSuccess;
			});
		printf("LED characteristic ready (e.g., 'LED1 ON', 'LED2 OFF', 'ALL OFF')\n");
	} else {
		printf("LED characteristic not found (update app_profile.gatt + rebuild)\n");
	}

	ble->SetOnBleStackOnCallback(on_turn_on);
	ble->TurnOn();

	std::cout << "BLE Stack is ON!\n";
	const auto ret = app_timer.Start(100);
	if (!ret) {
		std::cout << "Failed to start timer!\n";
	} else {
		std::cout << "Timer started.\n";
	}

	while (true) {
		seconds = xTaskGetTickCount() / 1000;
		vTaskDelay(pdMS_TO_TICKS(100));

		if (gap->IsAdvertisingEnabled()) {
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

[[noreturn]] int main() {
	stdio_init_all();
	printf("Starting FreeRTOS BLE Example...\n");

	xTaskCreate(ble_app_task, "BLE_App", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while (1) {}
}
