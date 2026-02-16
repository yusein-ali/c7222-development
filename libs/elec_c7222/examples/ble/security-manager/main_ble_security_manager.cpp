/**
 * @file main_ble_security_manager.cpp
 * @brief BLE Security Manager example demonstrating encrypted GATT access.
 *
 * This example exposes a custom vendor service with two characteristics:
 *
 * - PublicValue (READ): readable without pairing.
 * - SecureValue (READ_ENCRYPTED): read requires an encrypted link.
 *
 * The goal is to demonstrate how the Security Manager integrates with the
 * Attribute Server and how GATT security flags behave in practice.
 *
 * ---
 * ### How to test (nRF Connect / LightBlue)
 *
 * 1. Connect to the device ("c7222-sec-demo")
 * 2. Read "PublicValue" -> succeeds immediately
 * 3. Read "SecureValue" -> fails before pairing and/or triggers pairing
 * 4. Pair (Just Works) and read again -> succeeds
 *
 * ---
 * ### Notes
 *
 * - The secure behavior is enforced by the characteristic flags in the `.gatt`
 *   file (READ_ENCRYPTED).
 * - This example uses "Just Works" pairing (`NoInputNoOutput`) for simplicity.
 */

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "../common/gap_event_handler.hpp"
#include "../common/security_event_handler.hpp"

#include "advertisement_data.hpp"
#include "attribute_server.hpp"
#include "ble.hpp"
#include "freertos_task.hpp"
#include "gap.hpp"
#include "platform.hpp"
#include "security_manager.hpp"

#include "app_profile.h"

namespace {

constexpr uint16_t kServiceUuid = 0xFFF0;
constexpr uint16_t kPublicCharUuid = 0xFFF1;
constexpr uint16_t kSecureCharUuid = 0xFFF2;

static c7222::AttributeServer* g_att_server = nullptr;
static c7222::SecurityManager* g_security_manager = nullptr;

static c7222::Characteristic* g_public_value_ch = nullptr;
static c7222::Characteristic* g_secure_value_ch = nullptr;

static GapEventHandler g_gap_event_handler;
static SecurityEventHandler g_security_event_handler;

static bool set_string_value(c7222::Characteristic* ch, const char* s) {
    if(ch == nullptr || s == nullptr) {
        return false;
    }

    const size_t n = std::strlen(s);
    std::vector<uint8_t> bytes(s, s + n);
    return ch->SetValue(std::move(bytes));
}

static void on_ble_stack_on() {
    std::printf("Bluetooth stack turned ON\n");

    auto* ble = c7222::Ble::GetInstance();
    auto* gap = ble->GetGap();
    auto& adv_builder = gap->GetAdvertisementDataBuilder();

    gap->AddEventHandler(g_gap_event_handler);

    ble->SetAdvertisementFlags(c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
                               c7222::AdvertisementData::Flags::kBrEdrNotSupported);
    ble->SetDeviceName("c7222-sec-demo");

    // Minimal manufacturer payload (not required, but useful for sanity-checking).
    const uint32_t manufacturer_value = 0xC7222003;
    adv_builder.Add(c7222::AdvertisementData(c7222::AdvertisementDataType::kManufacturerSpecific,
                                            reinterpret_cast<const uint8_t*>(&manufacturer_value),
                                            sizeof(manufacturer_value)));

    c7222::Gap::AdvertisementParameters adv_params;
    adv_params.advertising_type = c7222::Gap::AdvertisingType::kAdvInd;
    adv_params.min_interval = 320;
    adv_params.max_interval = 400;
    gap->SetAdvertisingParameters(adv_params);

    gap->StartAdvertising();

    std::printf("Advertising started as 'c7222-sec-demo'\n");
    std::printf("Expected behavior:\n");
    std::printf(" - Read PublicValue: works without pairing\n");
    std::printf(" - Read SecureValue: fails before pairing, works after pairing\n");
}

[[noreturn]] void ble_security_manager_task(void* /*params*/) {
    auto* ble = c7222::Ble::GetInstance(false);

    // --- Enable Security Manager (pairing + link security) ---
    {
        c7222::SecurityManager::SecurityParameters sm_params;

        // Simple "Just Works" pairing for the example.
        sm_params.io_capability = c7222::SecurityManager::IoCapability::kNoInputNoOutput;

        // No MITM required for the demo.
        sm_params.authentication =
            c7222::SecurityManager::AuthenticationRequirement::kNoMitmProtection;

        // Require at least an encrypted link for the GATT client.
        sm_params.gatt_client_required_security_level =
            c7222::SecurityManager::GattClientSecurityLevel::kLevel1;

        g_security_manager = ble->EnableSecurityManager(sm_params);

        g_security_event_handler.SetSecurityManager(g_security_manager);
        ble->AddSecurityEventHandler(&g_security_event_handler);
    }

    // --- Enable ATT server from generated GATT DB ---
    g_att_server = ble->EnableAttributeServer(profile_data);
    g_gap_event_handler.SetAttributeServer(g_att_server);

    // --- Find our demo service + characteristics ---
    auto* service = g_att_server->FindServiceByUuid(c7222::Uuid(kServiceUuid));
    assert(service != nullptr);

    g_public_value_ch = service->FindCharacteristicByUuid(c7222::Uuid(kPublicCharUuid));
    g_secure_value_ch = service->FindCharacteristicByUuid(c7222::Uuid(kSecureCharUuid));

    assert(g_public_value_ch != nullptr);
    assert(g_secure_value_ch != nullptr);

    // Optional: user descriptions (requires CHARACTERISTIC_USER_DESCRIPTION in the .gatt).
    if(g_public_value_ch->HasUserDescription()) {
        g_public_value_ch->SetUserDescription("PublicValue");
    }
    if(g_secure_value_ch->HasUserDescription()) {
        g_secure_value_ch->SetUserDescription("SecureValue (READ_ENCRYPTED)");
    }

    // Initialize characteristic values (requires DYNAMIC in the .gatt).
    (void)set_string_value(g_public_value_ch, "hello");
    (void)set_string_value(g_secure_value_ch, "top-secret");

    // --- Turn on BLE stack ---
    ble->SetOnBleStackOnCallback(on_ble_stack_on);
    ble->TurnOn();

    std::printf("BLE Security Manager example started.\n");

    while(true) {
        c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(250));
    }
}

}  // namespace

/**
 * @brief Main entry point for the BLE Security Manager example.
 */
[[noreturn]] int main() {
    auto* platform = c7222::Platform::GetInstance();
    platform->Initialize();

    std::printf("Starting FreeRTOS BLE Security Manager example...\n");

    static c7222::FreeRtosTask ble_task;
    (void)ble_task.Initialize("BLE_App",
                              1024,
                              c7222::FreeRtosTask::IdlePriority() + 1,
                              ble_security_manager_task,
                              nullptr);

    c7222::FreeRtosTask::StartScheduler();
    while(1) {}
}