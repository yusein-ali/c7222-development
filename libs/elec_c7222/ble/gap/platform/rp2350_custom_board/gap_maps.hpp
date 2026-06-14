#ifndef ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_H_
#define ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_H_

#include <btstack.h>
#include <cstdint>

#include "gap.hpp"
namespace c7222::btstack_map {

BleError map_btstack_status(int status);
BleAddress::AddressType map_address_type(uint8_t btstack_type);
BleAddress make_address(uint8_t btstack_type, const bd_addr_t addr);
BleAddress make_unknown_address(const bd_addr_t addr);
Gap::Phy map_phy(uint8_t btstack_phy);
Gap::AdvertisingEventType map_legacy_advertising_event_type(uint8_t adv_type);

bool to_btstack_event(Gap::EventId id, uint8_t& event_code, uint8_t& subevent_code);
bool from_btstack_event(uint8_t event_code, uint8_t subevent_code, Gap::EventId& id);

uint16_t ToBtStack(Gap::AdvertisingEventType type);
Gap::AdvertisingEventType from_btstack_advertising_event_type(uint16_t bits);

uint8_t ToBtStack(Gap::AdvertisingType type);
bool from_btstack_advertising_type(uint8_t value, Gap::AdvertisingType& out);

uint8_t ToBtStack(Gap::DirectAddressType type);
bool from_btstack_direct_address_type(uint8_t value, Gap::DirectAddressType& out);

uint8_t ToBtStack(Gap::AdvertisingFilterPolicy policy);
bool from_btstack_advertising_filter_policy(uint8_t value, Gap::AdvertisingFilterPolicy& out);

uint8_t ToBtStack(Gap::Phy phy);
bool from_btstack_phy(uint8_t value, Gap::Phy& out);

uint8_t ToBtStack(BleAddress::AddressType type);
bool from_btstack_address_type(uint8_t value, BleAddress::AddressType& out);

uint8_t to_btstack_advertising_channel_map(uint8_t map);
uint8_t from_btstack_advertising_channel_map(uint8_t map);

} // namespace c7222::btstack_map

#endif // ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_H_
