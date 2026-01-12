#include "gap.hpp"

#include <cassert>
#include <ostream>

namespace c7222 {

std::ostream& operator<<(std::ostream& os, const AdvertisementDataBuilder& adb){
    os << "AdvertisementDataBuilder{\n";
    auto ad_list = adb.to_advertisement_data_list();
    for (const auto& ad : ad_list) {
        os << ad << "\n";
    }
    os << "}";
    return os;
}
std::ostream& operator<<(std::ostream& os, const AdvertisementData& ad){
    const auto& data = ad.data();
    if (data.empty()) {
        return os;
    }

    size_t length = data[0];
    auto type = static_cast<AdvertisementDataType>(data[1]);

    os << "  AD{len=" << length << ", type=";

    switch(type) {
        case AdvertisementDataType::kFlags: os << "Flags"; break;
        case AdvertisementDataType::kIncompleteList16BitUuid: os << "IncompleteList16BitUuid"; break;
        case AdvertisementDataType::kCompleteList16BitUuid: os << "CompleteList16BitUuid"; break;
        case AdvertisementDataType::kShortenedLocalName: os << "ShortenedLocalName"; break;
        case AdvertisementDataType::kCompleteLocalName: os << "CompleteLocalName"; break;
        case AdvertisementDataType::kTxPowerLevel: os << "TxPowerLevel"; break;
        case AdvertisementDataType::kSlaveConnectionIntervalRange: os << "SlaveConnectionIntervalRange"; break;
        case AdvertisementDataType::kServiceData16BitUuid: os << "ServiceData16BitUuid"; break;
        case AdvertisementDataType::kManufacturerSpecific: os << "ManufacturerSpecific"; break;
        default: os << "Unknown(" << static_cast<int>(type) << ")"; break;
    }

    os << ", data=[";
    for (size_t i = 2; i < data.size(); ++i) {
        os << static_cast<int>(data[i]);
        if (i < data.size() - 1) {
            os << ", ";
        }
    }
    os << "]}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AdvertisementData::Flags& flag){
	switch(flag){
		case AdvertisementData::Flags::kLeLimitedDiscoverableMode:
			os << "LeLimitedDiscoverableMode";
			break;
		case AdvertisementData::Flags::kLeGeneralDiscoverableMode:
			os << "LeGeneralDiscoverableMode";
			break;
		case AdvertisementData::Flags::kBrEdrNotSupported:
			os << "BrEdrNotSupported";
			break;
		case AdvertisementData::Flags::kSimultaneousLeAndBrEdrController:
			os << "SimultaneousLeAndBrEdrController";
			break;
		case AdvertisementData::Flags::kSimultaneousLeAndBrEdrHost:
			os << "SimultaneousLeAndBrEdrHost";
			break;
		case AdvertisementData::Flags::kAll:
			os << "All";
			break;
		default:
			os << "Unknown Flag";
			break;
	}
	return os;
}

} // namespace c7222

	std::ostream& operator<<(std::ostream& os, const c7222::Gap::AdvertisingReport& ar) {
	os << "AdvertisingReport{ event_type: " << static_cast<int>(ar.advertising_event_type)
	   << ", address: " << ar.address << ", rssi: " << static_cast<int>(ar.rssi)
	   << ", data_length: " << static_cast<int>(ar.data_length) << " }";
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingEventType type) {
	const uint16_t bits = static_cast<uint16_t>(type);
	if(bits == 0) {
		return os << "None";
	}
	bool first = true;
	const auto append = [&](const char* name) {
		if(!first) {
			os << "|";
		}
		os << name;
		first = false;
	};
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kConnectable)) {
		append("Connectable");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kScannable)) {
		append("Scannable");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kDirected)) {
		append("Directed");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kHighDutyCycle)) {
		append("HighDutyCycle");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kLegacy)) {
		append("Legacy");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kAnonymous)) {
		append("Anonymous");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::kIncludeTxPower)) {
		append("IncludeTxPower");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::Phy phy) {
	switch(phy) {
	case c7222::Gap::Phy::kNone:
		return os << "None";
	case c7222::Gap::Phy::kLe1M:
		return os << "Le1M";
	case c7222::Gap::Phy::kLe2M:
		return os << "Le2M";
	case c7222::Gap::Phy::kLeCoded:
		return os << "LeCoded";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingType type) {
	switch(type) {
	case c7222::Gap::AdvertisingType::kAdvInd:
		return os << "AdvInd";
	case c7222::Gap::AdvertisingType::kAdvDirectInd:
		return os << "AdvDirectInd";
	case c7222::Gap::AdvertisingType::kAdvScanInd:
		return os << "AdvScanInd";
	case c7222::Gap::AdvertisingType::kAdvNonConnInd:
		return os << "AdvNonConnInd";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::DirectAddressType type) {
	switch(type) {
	case c7222::Gap::DirectAddressType::kPublic:
		return os << "Public";
	case c7222::Gap::DirectAddressType::kRandom:
		return os << "Random";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingChannelMap map) {
	const uint8_t bits = static_cast<uint8_t>(map);
	if(bits == 0) {
		return os << "None";
	}
	if(bits == static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::kAll)) {
		return os << "All";
	}
	bool first = true;
	const auto append = [&](const char* name) {
		if(!first) {
			os << "|";
		}
		os << name;
		first = false;
	};
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::kChannel37)) {
		append("Channel37");
	}
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::kChannel38)) {
		append("Channel38");
	}
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::kChannel39)) {
		append("Channel39");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingFilterPolicy policy) {
	switch(policy) {
	case c7222::Gap::AdvertisingFilterPolicy::kScanAnyConnectAny:
		return os << "ScanAnyConnectAny";
	case c7222::Gap::AdvertisingFilterPolicy::kScanWhitelistConnectAny:
		return os << "ScanWhitelistConnectAny";
	case c7222::Gap::AdvertisingFilterPolicy::kScanAnyConnectWhitelist:
		return os << "ScanAnyConnectWhitelist";
	case c7222::Gap::AdvertisingFilterPolicy::kScanWhitelistConnectWhitelist:
		return os << "ScanWhitelistConnectWhitelist";
	}
	return os << "Unknown";
}
