/**
 * @file main_ble_custom_service_rw.cpp
 * @brief BLE example showing a minimal custom service with READ + WRITE characteristics.
 *
 * This example demonstrates how to implement a custom service using:
 * - a `.gatt` profile (compiled into `app_profile.h`)
 * - `AttributeServer` to locate services/characteristics by UUID
 * - `Characteristic::EventHandler` to react to reads and writes
 *
 * ---
 * ### GATT layout
 * Service UUID: 0xFFE0
 * - PublicValue (UUID 0xFFE1): READ (dynamic)
 * - WriteValue  (UUID 0xFFE2): WRITE / WRITE_WITHOUT_RESPONSE (dynamic)
 *
 * ---
 * ### How to test (nRF Connect / LightBlue)
 * 1) Connect to "c7222-rw-demo"
 * 2) Read "PublicValue" -> returns the current value
 * 3) Write any bytes to "WriteValue"
 * 4) Read "PublicValue" again -> it becomes whatever you wrote
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../common/gap_event_handler.hpp"
#include "../common/characteristic_event_handler.hpp"

#include "advertisement_data.hpp"
#include "attribute_server.hpp"
#include "ble.hpp"
#include "characteristic.hpp"
#include "freertos_task.hpp"
#include "gap.hpp"
#include "platform.hpp"

#include "app_profile.h"

namespace {

constexpr uint16_t kServiceUuid = 0xFFE0;
constexpr uint16_t kPublicCharUuid = 0xFFE1;
constexpr uint16_t kWriteCharUuid = 0xFFE2;

static c7222::AttributeServer* g_att_server = nullptr;

static c7222::Characteristic* g_public_value_ch = nullptr;
static c7222::Characteristic* g_write_value_ch = nullptr;

static GapEventHandler g_gap_event_handler;

// Simple logger handlers (same style as other examples)
static CharacteristicEventHandler g_public_logger;
static CharacteristicEventHandler g_write_logger;

/**
 * @brief Write handler that mirrors written bytes into the public (read) characteristic.
 *
 * This is the "custom service logic" part: we react to writes and update another
 * characteristic value in response.
 */
class MirrorWriteHandler : public c7222::Characteristic::EventHandler {
public:
	explicit MirrorWriteHandler(c7222::Characteristic* public_value)
		: public_value_(public_value) {}

	void OnWrite(const std::vector<uint8_t>& data) override {
		if(public_value_ == nullptr) {
			return;
		}
		// Mirror the raw bytes into the readable characteristic.
		(void)public_value_->SetValue(std::vector<uint8_t>(data.begin(), data.end()));
		std::printf("[RW] Mirrored %zu bytes into PublicValue\n", data.size());
	}

private:
	c7222::Characteristic* public_value_;
};

static MirrorWriteHandler* g_mirror_handler = nullptr;

/**
 * @brief Called once the BLE stack is fully initialized.
 *
 * Starts advertising using a minimal payload.
 */
static void on_ble_stack_on() {
	std::printf("Bluetooth stack turned ON\n");

	auto* ble = c7222::Ble::GetInstance();
	auto* gap = ble->GetGap();
	auto& adv_builder = gap->GetAdvertisementDataBuilder();

	gap->AddEventHandler(g_gap_event_handler);

	ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
							   c7222::AdvertisementData::Flags::kBrEdrNotSupported);
	ble->SetDeviceName("c7222-rw-demo");

	const uint32_t manufacturer_value = 0xC7222004;
	adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
											reinterpret_cast<const uint8_t*>(&manufacturer_value),
											sizeof(manufacturer_value)));

	c7222::Gap::AdvertisementParameters adv_params;
	adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
	adv_params.min_interval = 320;
	adv_params.max_interval = 400;
	gap->SetAdvertisingParameters(adv_params);

	gap->StartAdvertising();

	std::printf("Advertising started as 'c7222-rw-demo'\n");
	std::printf("Try writing to WriteValue (0xFFE2) and then reading PublicValue (0xFFE1).\n");
}

[[noreturn]] void ble_custom_service_rw_task(void* /*params*/) {
	// Platform init (CYW43/BTstack) — same pattern as other BLE examples
	auto* platform = c7222::Platform::GetInstance();
	platform->Initialize();

	auto* ble = c7222::Ble::GetInstance(false);

	// Enable ATT server from generated GATT DB
	g_att_server = ble->EnableAttributeServer(profile_data);
	g_gap_event_handler.SetAttributeServer(g_att_server);

	// Locate service/characteristics by UUID
	auto* service = g_att_server->FindServiceByUuid(c7222::Uuid(kServiceUuid));
	assert(service != nullptr);

	g_public_value_ch = service->FindCharacteristicByUuid(c7222::Uuid(kPublicCharUuid));
	g_write_value_ch = service->FindCharacteristicByUuid(c7222::Uuid(kWriteCharUuid));
	assert(g_public_value_ch != nullptr);
	assert(g_write_value_ch != nullptr);

	// User-friendly names (requires CHARACTERISTIC_USER_DESCRIPTION in .gatt)
	if(g_public_value_ch->HasUserDescription()) {
		g_public_value_ch->SetUserDescription("PublicValue");
	}
	if(g_write_value_ch->HasUserDescription()) {
		g_write_value_ch->SetUserDescription("WriteValue");
	}

	// Attach log handlers (so reads/writes show up in UART)
	g_public_logger.SetCharacteristicName("PublicValue");
	g_write_logger.SetCharacteristicName("WriteValue");
	g_public_value_ch->AddEventHandler(g_public_logger);
	g_write_value_ch->AddEventHandler(g_write_logger);

	// Attach custom logic handler: mirror writes into the readable value
	static MirrorWriteHandler mirror(g_public_value_ch);
	g_mirror_handler = &mirror;
	g_write_value_ch->AddEventHandler(*g_mirror_handler);

	// Initialize readable value (dynamic characteristic)
	(void)g_public_value_ch->SetValue(std::vector<uint8_t>{'h','e','l','l','o'});

	// Start BLE stack + advertising
	ble->SetOnBleStackOnCallback(on_ble_stack_on);
	ble->TurnOn();

	std::printf("Custom service RW example started.\n");

	while(true) {
		c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(250));
	}
}

}  // namespace

/**
 * @brief Program entry point for the custom service RW example.
 */
[[noreturn]] int main() {
	auto* platform = c7222::Platform::GetInstance();
	platform->Initialize();
	
	std::printf("Starting FreeRTOS BLE Custom Service (RW) example...\n");

	static c7222::FreeRtosTask ble_task;
	(void)ble_task.Initialize("BLE_App",
							  1024,
							  c7222::FreeRtosTask::IdlePriority() + 1,
							  ble_custom_service_rw_task,
							  nullptr);

	c7222::FreeRtosTask::StartScheduler();
	while(1) {}
}
