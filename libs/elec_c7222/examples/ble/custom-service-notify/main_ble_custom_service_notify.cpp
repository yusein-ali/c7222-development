/**
 * @file main_ble_custom_service_notify.cpp
 * @brief BLE example showing a minimal custom service with NOTIFY.
 *
 * This example demonstrates:
 * - defining a NOTIFY characteristic in a `.gatt` profile
 * - locating the service/characteristic by UUID via `AttributeServer`
 * - periodically updating a characteristic value and sending notifications
 *
 * ---
 * ### GATT layout
 * Service UUID: 0xFFD0
 * - NotifyValue (UUID 0xFFD1): READ | NOTIFY (dynamic)
 *
 * ---
 * ### How to test (nRF Connect / LightBlue)
 * 1) Connect to "c7222-notify-demo"
 * 2) Open "NotifyValue"
 * 3) Enable notifications (CCCD)
 * 4) Observe periodic updates (counter increments)
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
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

constexpr uint16_t kServiceUuid = 0xFFD0;
constexpr uint16_t kNotifyCharUuid = 0xFFD1;

static c7222::AttributeServer* g_att_server = nullptr;
static c7222::Characteristic* g_notify_value_ch = nullptr;

static GapEventHandler g_gap_event_handler;
static CharacteristicEventHandler g_notify_logger;

static void on_ble_stack_on() {
    std::printf("Bluetooth stack turned ON\n");

    auto* ble = c7222::Ble::GetInstance();
    auto* gap = ble->GetGap();
    auto& adv_builder = gap->GetAdvertisementDataBuilder();

    gap->AddEventHandler(g_gap_event_handler);

    ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
                               c7222::AdvertisementData::Flags::kBrEdrNotSupported);
    ble->SetDeviceName("c7222-notify-demo");

    const uint32_t manufacturer_value = 0xC7222005;
    adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
                                            reinterpret_cast<const uint8_t*>(&manufacturer_value),
                                            sizeof(manufacturer_value)));

    c7222::Gap::AdvertisementParameters adv_params;
    adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
    adv_params.min_interval = 320;
    adv_params.max_interval = 400;
    gap->SetAdvertisingParameters(adv_params);

    gap->StartAdvertising();

    std::printf("Advertising started as 'c7222-notify-demo'\n");
    std::printf("Enable notifications on NotifyValue (0xFFD1) to receive updates.\n");
}

static std::vector<uint8_t> u32_to_ascii(uint32_t v) {
    // Small, dependency-free conversion.
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%lu", static_cast<unsigned long>(v));
    const size_t n = std::strlen(buf);
    return std::vector<uint8_t>(buf, buf + n);
}

[[noreturn]] void ble_custom_service_notify_task(void* /*params*/) {
    auto* ble = c7222::Ble::GetInstance(false);

    // Enable ATT server from generated GATT DB
    g_att_server = ble->EnableAttributeServer(profile_data);
    g_gap_event_handler.SetAttributeServer(g_att_server);

    // Locate service/characteristic by UUID
    auto* service = g_att_server->FindServiceByUuid(c7222::Uuid(kServiceUuid));
    assert(service != nullptr);

    g_notify_value_ch = service->FindCharacteristicByUuid(c7222::Uuid(kNotifyCharUuid));
    assert(g_notify_value_ch != nullptr);

    // User-friendly name (requires CHARACTERISTIC_USER_DESCRIPTION in .gatt)
    if(g_notify_value_ch->HasUserDescription()) {
        g_notify_value_ch->SetUserDescription("NotifyValue");
    }

    // Attach a logger so CCCD writes / reads are visible on UART (same style as other examples)
    g_notify_logger.SetCharacteristicName("NotifyValue");
    g_notify_value_ch->AddEventHandler(g_notify_logger);

    // Initial value
    (void)g_notify_value_ch->SetValue(std::vector<uint8_t>{'0'});

    // Start BLE stack + advertising
    ble->SetOnBleStackOnCallback(on_ble_stack_on);
    ble->TurnOn();

    std::printf("Custom service NOTIFY example started.\n");

    // Periodic updates + notifications
    uint32_t counter = 0;
    while(true) {
        counter++;

        // Update characteristic value
        // This call also trigegrs notification (if not subscribed, the stack/handler will typically ignore/fail silently)
        // The underlying library handles CCCD subscription state.
        (void)g_notify_value_ch->SetValue(u32_to_ascii(counter));

        c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(1000));
    }
}

}  // namespace

/**
 * @brief Program entry point for the custom service NOTIFY example.
 */
[[noreturn]] int main() {
    auto* platform = c7222::Platform::GetInstance();
    platform->Initialize();

    std::printf("Starting FreeRTOS BLE Custom Service (NOTIFY) example...\n");

    static c7222::FreeRtosTask ble_task;
    (void)ble_task.Initialize("BLE_App",
                              1024,
                              c7222::FreeRtosTask::IdlePriority() + 1,
                              ble_custom_service_notify_task,
                              nullptr);

    c7222::FreeRtosTask::StartScheduler();
    while(1) {}
}
