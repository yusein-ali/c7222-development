#ifndef ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_HPP
#define ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_HPP

#include <cstdint>

#include "gap.hpp"

namespace c7222::btstack_map {

bool to_btstack_event(Gap::EventId id, uint8_t& event_code, uint8_t& subevent_code);
bool from_btstack_event(uint8_t event_code, uint8_t subevent_code, Gap::EventId& id);

uint16_t to_btstack(Gap::AdvertisingEventType type);
Gap::AdvertisingEventType from_btstack_advertising_event_type(uint16_t bits);

uint8_t to_btstack(Gap::AdvertisingType type);
bool from_btstack_advertising_type(uint8_t value, Gap::AdvertisingType& out);

uint8_t to_btstack(Gap::DirectAddressType type);
bool from_btstack_direct_address_type(uint8_t value, Gap::DirectAddressType& out);

uint8_t to_btstack(Gap::AdvertisingFilterPolicy policy);
bool from_btstack_advertising_filter_policy(uint8_t value, Gap::AdvertisingFilterPolicy& out);

uint8_t to_btstack(Gap::Phy phy);
bool from_btstack_phy(uint8_t value, Gap::Phy& out);

uint8_t to_btstack(BleAddress::AddressType type);
bool from_btstack_address_type(uint8_t value, BleAddress::AddressType& out);

uint8_t to_btstack_advertising_channel_map(uint8_t map);
uint8_t from_btstack_advertising_channel_map(uint8_t map);

} // namespace c7222::btstack_map

#endif // ELEC_C7222_BLE_GAP_PLATFORM_RPI_PICO_GAP_MAPS_HPP
