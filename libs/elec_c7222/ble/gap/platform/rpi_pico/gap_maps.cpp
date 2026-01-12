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
	{Gap::EventId::kSecurityLevel, GAP_EVENT_SECURITY_LEVEL, kNoSubevent},
	{Gap::EventId::kDedicatedBondingCompleted, GAP_EVENT_DEDICATED_BONDING_COMPLETED, kNoSubevent},
	{Gap::EventId::kAdvertisingReport, GAP_EVENT_ADVERTISING_REPORT, kNoSubevent},
	{Gap::EventId::kExtendedAdvertisingReport, GAP_EVENT_EXTENDED_ADVERTISING_REPORT, kNoSubevent},
	{Gap::EventId::kInquiryResult, GAP_EVENT_INQUIRY_RESULT, kNoSubevent},
	{Gap::EventId::kInquiryComplete, GAP_EVENT_INQUIRY_COMPLETE, kNoSubevent},
	{Gap::EventId::kRssiMeasurement, GAP_EVENT_RSSI_MEASUREMENT, kNoSubevent},
	{Gap::EventId::kLocalOobData, GAP_EVENT_LOCAL_OOB_DATA, kNoSubevent},
	{Gap::EventId::kPairingStarted, GAP_EVENT_PAIRING_STARTED, kNoSubevent},
	{Gap::EventId::kPairingComplete, GAP_EVENT_PAIRING_COMPLETE, kNoSubevent},
	{Gap::EventId::kDisconnectionComplete, HCI_EVENT_DISCONNECTION_COMPLETE, kNoSubevent},
	{Gap::EventId::kCommandComplete, HCI_EVENT_COMMAND_COMPLETE, kNoSubevent},
	{Gap::EventId::kLeScanRequestReceived, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_SCAN_REQUEST_RECEIVED},
	{Gap::EventId::kLeScanTimeout, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_SCAN_TIMEOUT},
	{Gap::EventId::kLePeriodicAdvertisingSyncEstablished, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHMENT},
	{Gap::EventId::kLePeriodicAdvertisingReport, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_REPORT},
	{Gap::EventId::kLePeriodicAdvertisingSyncLost, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_PERIODIC_ADVERTISING_SYNC_LOST},
	{Gap::EventId::kLeConnectionComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_CONNECTION_COMPLETE},
	{Gap::EventId::kLeEnhancedConnectionComplete, HCI_EVENT_LE_META,
#ifdef HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE
	 HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE},
#else
	 HCI_SUBEVENT_LE_ENHANCED_CONNECTION_COMPLETE_V1},
#endif
	{Gap::EventId::kLeRemoteConnectionParameterRequest, HCI_EVENT_LE_META,
	 HCI_SUBEVENT_LE_REMOTE_CONNECTION_PARAMETER_REQUEST},
	{Gap::EventId::kLeConnectionUpdateComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE},
	{Gap::EventId::kLePhyUpdateComplete, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE},
	{Gap::EventId::kLeDataLengthChange, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE},
	{Gap::EventId::kLeAdvertisingSetTerminated, HCI_EVENT_LE_META, HCI_SUBEVENT_LE_ADVERTISING_SET_TERMINATED},
	{Gap::EventId::kL2capConnectionParameterUpdateRequest, L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_REQUEST,
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
	{static_cast<uint8_t>(Gap::AdvertisingType::kAdvInd), 0x00},
	{static_cast<uint8_t>(Gap::AdvertisingType::kAdvDirectInd), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingType::kAdvScanInd), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingType::kAdvNonConnInd), 0x03},
};

constexpr EnumMap8 kDirectAddressTypeMap[] = {
	{static_cast<uint8_t>(Gap::DirectAddressType::kPublic), BD_ADDR_TYPE_LE_PUBLIC},
	{static_cast<uint8_t>(Gap::DirectAddressType::kRandom), BD_ADDR_TYPE_LE_RANDOM},
};

constexpr EnumMap8 kAdvertisingFilterPolicyMap[] = {
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::kScanAnyConnectAny), 0x00},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::kScanWhitelistConnectAny), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::kScanAnyConnectWhitelist), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingFilterPolicy::kScanWhitelistConnectWhitelist), 0x03},
};

constexpr EnumMap8 kPhyMap[] = {
	{static_cast<uint8_t>(Gap::Phy::kNone), 0x00},
	{static_cast<uint8_t>(Gap::Phy::kLe1M), 0x01},
	{static_cast<uint8_t>(Gap::Phy::kLe2M), 0x02},
	{static_cast<uint8_t>(Gap::Phy::kLeCoded), 0x03},
};

constexpr EnumMap8 kAddressTypeMap[] = {
	{static_cast<uint8_t>(BleAddress::AddressType::kLePublic), BD_ADDR_TYPE_LE_PUBLIC},
	{static_cast<uint8_t>(BleAddress::AddressType::kLeRandom), BD_ADDR_TYPE_LE_RANDOM},
	{static_cast<uint8_t>(BleAddress::AddressType::kLePublicIdentity), BD_ADDR_TYPE_LE_PUBLIC_IDENTITY},
	{static_cast<uint8_t>(BleAddress::AddressType::kLeRandomIdentity), BD_ADDR_TYPE_LE_RANDOM_IDENTITY},
	{static_cast<uint8_t>(BleAddress::AddressType::kSco), BD_ADDR_TYPE_SCO},
	{static_cast<uint8_t>(BleAddress::AddressType::kAcl), BD_ADDR_TYPE_ACL},
	{static_cast<uint8_t>(BleAddress::AddressType::kUnknown), BD_ADDR_TYPE_UNKNOWN},
};

constexpr EnumMap16 kAdvertisingEventTypeMap[] = {
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kConnectable), LE_ADVERTISING_PROPERTIES_CONNECTABLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kScannable), LE_ADVERTISING_PROPERTIES_SCANNABLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kDirected), LE_ADVERTISING_PROPERTIES_DIRECTED},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kHighDutyCycle), LE_ADVERTISING_PROPERTIES_HIGH_DUTY_CYCLE},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kLegacy), LE_ADVERTISING_PROPERTIES_LEGACY},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kAnonymous), LE_ADVERTISING_PROPERTIES_ANONYMOUS},
	{static_cast<uint16_t>(Gap::AdvertisingEventType::kIncludeTxPower),
	 LE_ADVERTISING_PROPERTIES_INCLUDE_TX_POWER},
};

constexpr EnumMap8 kAdvertisingChannelMap[] = {
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::kChannel37), 0x01},
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::kChannel38), 0x02},
	{static_cast<uint8_t>(Gap::AdvertisingChannelMap::kChannel39), 0x04},
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
		return BleError::kSuccess;
	}
	if(status > 0) {
		BleError mapped;
		if(btstack_map::from_btstack_error(static_cast<uint8_t>(status), mapped)) {
			return mapped;
		}
	}
	return BleError::kUnspecifiedError;
}

BleAddress::AddressType map_address_type(uint8_t btstack_type) {
	BleAddress::AddressType type = BleAddress::AddressType::kUnknown;
	btstack_map::from_btstack_address_type(btstack_type, type);
	return type;
}

BleAddress make_address(uint8_t btstack_type, const bd_addr_t addr) {
	BleAddress::AddressType type = map_address_type(btstack_type);
	return BleAddress(type, addr);
}

BleAddress make_unknown_address(const bd_addr_t addr) {
	return BleAddress(BleAddress::AddressType::kUnknown, addr);
}

Gap::Phy map_phy(uint8_t btstack_phy) {
	Gap::Phy phy = Gap::Phy::kNone;
	btstack_map::from_btstack_phy(btstack_phy, phy);
	return phy;
}

Gap::AdvertisingEventType map_legacy_advertising_event_type(uint8_t adv_type) {
	using EventType = Gap::AdvertisingEventType;
	uint16_t bits = static_cast<uint16_t>(EventType::kLegacy);
	switch(adv_type) {
	case 0x00: // ADV_IND
		bits |= static_cast<uint16_t>(EventType::kConnectable);
		bits |= static_cast<uint16_t>(EventType::kScannable);
		break;
	case 0x01: // ADV_DIRECT_IND
		bits |= static_cast<uint16_t>(EventType::kConnectable);
		bits |= static_cast<uint16_t>(EventType::kDirected);
		break;
	case 0x02: // ADV_SCAN_IND
		bits |= static_cast<uint16_t>(EventType::kScannable);
		break;
	case 0x03: // ADV_NONCONN_IND
		break;
	case 0x04: // SCAN_RSP
		bits |= static_cast<uint16_t>(EventType::kScannable);
		break;
	default:
		break;
	}
	return static_cast<EventType>(bits);
}

} // namespace c7222::btstack_map
