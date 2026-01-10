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
        case AdvertisementDataType::Flags: os << "Flags"; break;
        case AdvertisementDataType::IncompleteList16BitUUID: os << "IncompleteList16BitUUID"; break;
        case AdvertisementDataType::CompleteList16BitUUID: os << "CompleteList16BitUUID"; break;
        case AdvertisementDataType::ShortenedLocalName: os << "ShortenedLocalName"; break;
        case AdvertisementDataType::CompleteLocalName: os << "CompleteLocalName"; break;
        case AdvertisementDataType::TxPowerLevel: os << "TxPowerLevel"; break;
        case AdvertisementDataType::SlaveConnectionIntervalRange: os << "SlaveConnectionIntervalRange"; break;
        case AdvertisementDataType::ServiceData16BitUUID: os << "ServiceData16BitUUID"; break;
        case AdvertisementDataType::ManufacturerSpecific: os << "ManufacturerSpecific"; break;
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
		case AdvertisementData::Flags::LE_Limited_Discoverable_Mode:
			os << "LE_Limited_Discoverable_Mode";
			break;
		case AdvertisementData::Flags::LE_General_Discoverable_Mode:
			os << "LE_General_Discoverable_Mode";
			break;
		case AdvertisementData::Flags::BR_EDR_Not_Supported:
			os << "BR_EDR_Not_Supported";
			break;
		case AdvertisementData::Flags::Simultaneous_LE_and_BR_EDR_Controller:
			os << "Simultaneous_LE_and_BR_EDR_Controller";
			break;
		case AdvertisementData::Flags::Simultaneous_LE_and_BR_EDR_Host:
			os << "Simultaneous_LE_and_BR_EDR_Host";
			break;
		case AdvertisementData::Flags::All:
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
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Connectable)) {
		append("Connectable");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Scannable)) {
		append("Scannable");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Directed)) {
		append("Directed");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::HighDutyCycle)) {
		append("HighDutyCycle");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Legacy)) {
		append("Legacy");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Anonymous)) {
		append("Anonymous");
	}
	if(bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::IncludeTxPower)) {
		append("IncludeTxPower");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::Phy phy) {
	switch(phy) {
	case c7222::Gap::Phy::None:
		return os << "None";
	case c7222::Gap::Phy::Le1M:
		return os << "Le1M";
	case c7222::Gap::Phy::Le2M:
		return os << "Le2M";
	case c7222::Gap::Phy::LeCoded:
		return os << "LeCoded";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingType type) {
	switch(type) {
	case c7222::Gap::AdvertisingType::AdvInd:
		return os << "AdvInd";
	case c7222::Gap::AdvertisingType::AdvDirectInd:
		return os << "AdvDirectInd";
	case c7222::Gap::AdvertisingType::AdvScanInd:
		return os << "AdvScanInd";
	case c7222::Gap::AdvertisingType::AdvNonConnInd:
		return os << "AdvNonConnInd";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::DirectAddressType type) {
	switch(type) {
	case c7222::Gap::DirectAddressType::Public:
		return os << "Public";
	case c7222::Gap::DirectAddressType::Random:
		return os << "Random";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingChannelMap map) {
	const uint8_t bits = static_cast<uint8_t>(map);
	if(bits == 0) {
		return os << "None";
	}
	if(bits == static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All)) {
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
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel37)) {
		append("Channel37");
	}
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel38)) {
		append("Channel38");
	}
	if(bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel39)) {
		append("Channel39");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingFilterPolicy policy) {
	switch(policy) {
	case c7222::Gap::AdvertisingFilterPolicy::ScanAnyConnectAny:
		return os << "ScanAnyConnectAny";
	case c7222::Gap::AdvertisingFilterPolicy::ScanWhitelistConnectAny:
		return os << "ScanWhitelistConnectAny";
	case c7222::Gap::AdvertisingFilterPolicy::ScanAnyConnectWhitelist:
		return os << "ScanAnyConnectWhitelist";
	case c7222::Gap::AdvertisingFilterPolicy::ScanWhitelistConnectWhitelist:
		return os << "ScanWhitelistConnectWhitelist";
	}
	return os << "Unknown";
}
