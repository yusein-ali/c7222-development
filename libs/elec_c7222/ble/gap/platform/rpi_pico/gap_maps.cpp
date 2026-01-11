#include "gap_maps.hpp"

#include <btstack.h>
#include <btstack_defines.h>

#include <cstddef>

namespace c7222::btstack_map {
namespace {

struct EventMapEntry {
	Gap::EventId id;
	uint8_t event_code;
	uint8_t subevent_code;
};

constexpr uint8_t kNoSubevent = 0x00;

constexpr EventMapEntry kEventMap[] = {
	{Gap::EventId::SecurityLevel, GAP_EVENT_SECURITY_LEVEL, kNoSubevent},
	{Gap::EventId::DedicatedBondingCompleted, GAP_EVENT_DEDICATED_BONDING_COMPLETED, kNoSubevent},
	{Gap::EventId::AdvertisingReport, GAP_EVENT_ADVERTISING_REPORT, kNoSubevent},
	{Gap::EventId::ExtendedAdvertisingReport, GAP_EVENT_EXTENDED_ADVERTISING_REPORT, kNoSubevent},
	{Gap::EventId::InquiryResult, GAP_EVENT_INQUIRY_RESULT, kNoSubevent},
	{Gap::EventId::InquiryComplete, GAP_EVENT_INQUIRY_COMPLETE, kNoSubevent},
	{Gap::EventId::RssiMeasurement, GAP_EVENT_RSSI_MEASUREMENT, kNoSubevent},
	{Gap::EventId::LocalOobData, GAP_EVENT_LOCAL_OOB_DATA, kNoSubevent},
	{Gap::EventId::PairingStarted, GAP_EVENT_PAIRING_STARTED, kNoSubevent},
	{Gap::EventId::PairingComplete, GAP_EVENT_PAIRING_COMPLETE, kNoSubevent},
	{Gap::EventId::DisconnectionComplete, HCI_EVENT_DISCONNECTION_COMPLETE, kNoSubevent},
	{Gap::EventId::CommandComplete, HCI_EVENT_COMMAND_COMPLETE, kNoSubevent},
	{Gap::EventId::LeScanRequestReceived, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED},
	{Gap::EventId::LeScanTimeout, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_SCAN_TIMEOUT},
	{Gap::EventId::LePeriodicAdvertisingSyncEstablished, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHMENT},
	{Gap::EventId::LePeriodicAdvertisingReport, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT},
	{Gap::EventId::LePeriodicAdvertisingSyncLost, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST},
	{Gap::EventId::LeConnectionComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_CONNECTION_COMPLETE},
	{Gap::EventId::LeEnhancedConnectionComplete, HCI_EVENT_LE_META,
#ifdef HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE
	 HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE},
#else
	 HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V1},
#endif
	{Gap::EventId::LeRemoteConnectionParameterRequest, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST},
	{Gap::EventId::LeConnectionUpdateComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE},
	{Gap::EventId::LePhyUpdateComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE},
	{Gap::EventId::LeDataLengthChange, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE},
	{Gap::EventId::LeAdvertisingSetTerminated, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED},
	{Gap::EventId::L2capConnectionParameterUpdateRequest, L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST,
	 kNoSubevent},
};

struct EnumMap8 {
	uint8_t value;
	uint8_t btstack;
};

struct EnumMap16 {
	uint16_t value;
	uint16_t btstack;
};

constexpr EnumMap8 kAdvertisingTypeMap[] = {
	{static_cast<uint8_t>(Gap::AdvertisingType::AdvInd), 0x00},
	{static_cast<uint8_t>(Gap::AdvertisingType::AdvDirectInd), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingType::AdvScanInd), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingType::AdvNonConnInd), 0x03},
};

constexpr EnumMap8 kDirectAddressTypeMap[] = {
	{static_cast<uint8_t>(Gap::DirectAddressType::Public), BD_ADDR_TYPE_LE_PUBLIC},
	{static_cast<uint8_t>(Gap::DirectAddressType::Random), BD_ADDR_TYPE_LE_RANDOM},
};

constexpr EnumMap8 kAdvertisingFilterPolicyMap[] = {
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::ScanAnyConnectAny), 0x00},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::ScanWhitelistConnectAny), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::ScanAnyConnectWhitelist), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::ScanWhitelistConnectWhitelist), 0x03},
};

constexpr EnumMap8 kPhyMap[] = {
	{static_cast<uint8_t>(Gap::Phy::None), 0x00},
	{static_cast<uint8_t>(Gap::Phy::Le1M), 0x01},
	{static_cast<uint8_t>(Gap::Phy::Le2M), 0x02},
	{static_cast<uint8_t>(Gap::Phy::LeCoded), 0x03},
};

constexpr EnumMap8 kAddressTypeMap[] = {
	{static_cast<uint8_t>(BleAddress::AddressType::LePublic), BD_ADDR_TYPE_LE_PUBLIC},
	{static_cast<uint8_t>(BleAddress::AddressType::LeRandom), BD_ADDR_TYPE_LE_RANDOM},
	{static_cast<uint8_t>(BleAddress::AddressType::LePublicIdentity), BD_ADDR_TYPE_LE_PUBLIC_IDENTITY},
	{static_cast<uint8_t>(BleAddress::AddressType::LeRandomIdentity), BD_ADDR_TYPE_LE_RANDOM_IDENTITY},
	{static_cast<uint8_t>(BleAddress::AddressType::Sco), BD_ADDR_TYPE_SCO},
	{static_cast<uint8_t>(BleAddress::AddressType::Acl), BD_ADDR_TYPE_ACL},
	{static_cast<uint8_t>(BleAddress::AddressType::Unknown), BD_ADDR_TYPE_UNKNOWN},
};

constexpr EnumMap16 kAdvertisingEventTypeMap[] = {
	{static_cast<uint16_t>(Gap::AdvertisingEventType::Connectable), LE_ADVERTISING_PROPERTIES_CONNECTABLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::Scannable), LE_ADVERTISING_PROPERTIES_SCANNABLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::Directed), LE_ADVERTISING_PROPERTIES_DIRECTED},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::HighDutyCycle), LE_ADVERTISING_PROPERTIES_HIGH_DUTY_CYCLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::Legacy), LE_ADVERTISING_PROPERTIES_LEGACY},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::Anonymous), LE_ADVERTISING_PROPERTIES_ANONYMOUS},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::IncludeTxPower),
	 LE_ADVERTISING_PROPERTIES_INCLUDE_TX_POWER},
};

constexpr EnumMap8 kAdvertisingChannelMap[] = {
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::Channel37), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::Channel38), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::Channel39), 0x04},
};

template <typename Map>
bool map_enum_value(uint8_t value, const Map* table, size_t table_size, uint8_t& out) {
	for (size_t i = 0; i < table_size; ++i) {
		if (table[i].value == value) {
			out = table[i].btstack;
			return true;
		}
	}
	return false;
}

template <typename Map>
bool map_btstack_value(uint8_t value, const Map* table, size_t table_size, uint8_t& out) {
	for (size_t i = 0; i < table_size; ++i) {
		if (table[i].btstack == value) {
			out = table[i].value;
			return true;
		}
	}
	return false;
}

} // namespace

bool to_btstack_event(Gap::EventId id, uint8_t& event_code, uint8_t& subevent_code) {
	for (const auto& entry : kEventMap) {
		if (entry.id == id) {
			event_code = entry.event_code;
			subevent_code = entry.subevent_code;
			return true;
		}
	}
	return false;
}

bool from_btstack_event(uint8_t event_code, uint8_t subevent_code, Gap::EventId& id) {
	for (const auto& entry : kEventMap) {
		if (entry.event_code == event_code && entry.subevent_code == subevent_code) {
			id = entry.id;
			return true;
		}
	}
	return false;
}

uint16_t to_btstack(Gap::AdvertisingEventType type) {
	const uint16_t bits = static_cast<uint16_t>(type);
	uint16_t result = 0;
	for (const auto& entry : kAdvertisingEventTypeMap) {
		if (bits & entry.value) {
			result |= entry.btstack;
		}
	}
	return result;
}

Gap::AdvertisingEventType from_btstack_advertising_event_type(uint16_t bits) {
	uint16_t result = 0;
	for (const auto& entry : kAdvertisingEventTypeMap) {
		if (bits & entry.btstack) {
			result |= entry.value;
		}
	}
	return static_cast<Gap::AdvertisingEventType>(result);
}

uint8_t to_btstack(Gap::AdvertisingType type) {
	uint8_t out = 0;
	if (map_enum_value(static_cast<uint8_t>(type),
					   kAdvertisingTypeMap,
					   sizeof(kAdvertisingTypeMap) / sizeof(kAdvertisingTypeMap[0]),
					   out)) {
		return out;
	}
	return 0;
}

bool from_btstack_advertising_type(uint8_t value, Gap::AdvertisingType& out) {
	uint8_t mapped = 0;
	if (!map_btstack_value(value,
						   kAdvertisingTypeMap,
						   sizeof(kAdvertisingTypeMap) / sizeof(kAdvertisingTypeMap[0]),
						   mapped)) {
		return false;
	}
	out = static_cast<Gap::AdvertisingType>(mapped);
	return true;
}

uint8_t to_btstack(Gap::DirectAddressType type) {
	uint8_t out = 0;
	if (map_enum_value(static_cast<uint8_t>(type),
					   kDirectAddressTypeMap,
					   sizeof(kDirectAddressTypeMap) / sizeof(kDirectAddressTypeMap[0]),
					   out)) {
		return out;
	}
	return 0;
}

bool from_btstack_direct_address_type(uint8_t value, Gap::DirectAddressType& out) {
	uint8_t mapped = 0;
	if (!map_btstack_value(value,
						   kDirectAddressTypeMap,
						   sizeof(kDirectAddressTypeMap) / sizeof(kDirectAddressTypeMap[0]),
						   mapped)) {
		return false;
	}
	out = static_cast<Gap::DirectAddressType>(mapped);
	return true;
}

uint8_t to_btstack(Gap::AdvertisingFilterPolicy policy) {
	uint8_t out = 0;
	if (map_enum_value(static_cast<uint8_t>(policy),
					   kAdvertisingFilterPolicyMap,
					   sizeof(kAdvertisingFilterPolicyMap) / sizeof(kAdvertisingFilterPolicyMap[0]),
					   out)) {
		return out;
	}
	return 0;
}

bool from_btstack_advertising_filter_policy(uint8_t value, Gap::AdvertisingFilterPolicy& out) {
	uint8_t mapped = 0;
	if (!map_btstack_value(value,
						   kAdvertisingFilterPolicyMap,
						   sizeof(kAdvertisingFilterPolicyMap) / sizeof(kAdvertisingFilterPolicyMap[0]),
						   mapped)) {
		return false;
	}
	out = static_cast<Gap::AdvertisingFilterPolicy>(mapped);
	return true;
}

uint8_t to_btstack(Gap::Phy phy) {
	uint8_t out = 0;
	if (map_enum_value(static_cast<uint8_t>(phy),
					   kPhyMap,
					   sizeof(kPhyMap) / sizeof(kPhyMap[0]),
					   out)) {
		return out;
	}
	return 0;
}

bool from_btstack_phy(uint8_t value, Gap::Phy& out) {
	uint8_t mapped = 0;
	if (!map_btstack_value(value,
						   kPhyMap,
						   sizeof(kPhyMap) / sizeof(kPhyMap[0]),
						   mapped)) {
		return false;
	}
	out = static_cast<Gap::Phy>(mapped);
	return true;
}

uint8_t to_btstack(BleAddress::AddressType type) {
	uint8_t out = 0;
	if (map_enum_value(static_cast<uint8_t>(type),
					   kAddressTypeMap,
					   sizeof(kAddressTypeMap) / sizeof(kAddressTypeMap[0]),
					   out)) {
		return out;
	}
	return BD_ADDR_TYPE_UNKNOWN;
}

bool from_btstack_address_type(uint8_t value, BleAddress::AddressType& out) {
	uint8_t mapped = 0;
	if (!map_btstack_value(value,
						   kAddressTypeMap,
						   sizeof(kAddressTypeMap) / sizeof(kAddressTypeMap[0]),
						   mapped)) {
		return false;
	}
	out = static_cast<BleAddress::AddressType>(mapped);
	return true;
}

uint8_t to_btstack_advertising_channel_map(uint8_t map) {
	uint8_t result = 0;
	for (const auto& entry : kAdvertisingChannelMap) {
		if (map & entry.value) {
			result |= entry.btstack;
		}
	}
	return result;
}

uint8_t from_btstack_advertising_channel_map(uint8_t map) {
	uint8_t result = 0;
	for (const auto& entry : kAdvertisingChannelMap) {
		if (map & entry.btstack) {
			result |= entry.value;
		}
	}
	return result;
}

BleError map_btstack_status(int status) {
	if(status == 0) {
		return BleError::Success;
	}
	if(status > 0) {
		BleError mapped;
		if(btstack_map::from_btstack_error(static_cast<uint8_t>(status), mapped)) {
			return mapped;
		}
	}
	return BleError::UnspecifiedError;
}

BleAddress::AddressType map_address_type(uint8_t btstack_type) {
	BleAddress::AddressType type = BleAddress::AddressType::Unknown;
	btstack_map::from_btstack_address_type(btstack_type, type);
	return type;
}

BleAddress make_address(uint8_t btstack_type, const bd_addr_t addr) {
	BleAddress::AddressType type = map_address_type(btstack_type);
	return BleAddress(type, addr);
}

BleAddress make_unknown_address(const bd_addr_t addr) {
	return BleAddress(BleAddress::AddressType::Unknown, addr);
}

Gap::Phy map_phy(uint8_t btstack_phy) {
	Gap::Phy phy = Gap::Phy::None;
	btstack_map::from_btstack_phy(btstack_phy, phy);
	return phy;
}

Gap::AdvertisingEventType map_legacy_advertising_event_type(uint8_t adv_type) {
	using EventType = Gap::AdvertisingEventType;
	uint16_t bits = static_cast<uint16_t>(EventType::Legacy);
	switch(adv_type) {
	case 0x00: // ADV_IND
		bits |= static_cast<uint16_t>(EventType::Connectable);
		bits |= static_cast<uint16_t>(EventType::Scannable);
		break;
	case 0x01: // ADV_DIRECT_IND
		bits |= static_cast<uint16_t>(EventType::Connectable);
		bits |= static_cast<uint16_t>(EventType::Directed);
		break;
	case 0x02: // ADV_SCAN_IND
		bits |= static_cast<uint16_t>(EventType::Scannable);
		break;
	case 0x03: // ADV_NONCONN_IND
		break;
	case 0x04: // SCAN_RSP
		bits |= static_cast<uint16_t>(EventType::Scannable);
		break;
	default:
		break;
	}
	return static_cast<EventType>(bits);
}

} // namespace c7222::btstack_map
