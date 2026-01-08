#include "gap.hpp"

#include <ostream>
#include <cassert>


std::ostream& operator<<(std::ostream& os, const c7222::Gap::AdvertisingReport& ar) {
	os << "AdvertisingReport{ event_type: " << static_cast<int>(ar.advertising_event_type)
	   << ", address: " << ar.address << ", rssi: " << static_cast<int>(ar.rssi)
	   << ", data_length: " << static_cast<int>(ar.data_length) << " }";
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingEventType type) {
	const uint16_t bits = static_cast<uint16_t>(type);
	if (bits == 0) {
		return os << "None";
	}
	bool first = true;
	const auto append = [&](const char* name) {
		if (!first) {
			os << "|";
		}
		os << name;
		first = false;
	};
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Connectable)) {
		append("Connectable");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Scannable)) {
		append("Scannable");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Directed)) {
		append("Directed");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::HighDutyCycle)) {
		append("HighDutyCycle");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Legacy)) {
		append("Legacy");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::Anonymous)) {
		append("Anonymous");
	}
	if (bits & static_cast<uint16_t>(c7222::Gap::AdvertisingEventType::IncludeTxPower)) {
		append("IncludeTxPower");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::Phy phy) {
	switch (phy) {
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
	switch (type) {
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
	switch (type) {
		case c7222::Gap::DirectAddressType::Public:
			return os << "Public";
		case c7222::Gap::DirectAddressType::Random:
			return os << "Random";
	}
	return os << "Unknown";
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingChannelMap map) {
	const uint8_t bits = static_cast<uint8_t>(map);
	if (bits == 0) {
		return os << "None";
	}
	if (bits == static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All)) {
		return os << "All";
	}
	bool first = true;
	const auto append = [&](const char* name) {
		if (!first) {
			os << "|";
		}
		os << name;
		first = false;
	};
	if (bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel37)) {
		append("Channel37");
	}
	if (bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel38)) {
		append("Channel38");
	}
	if (bits & static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::Channel39)) {
		append("Channel39");
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, c7222::Gap::AdvertisingFilterPolicy policy) {
	switch (policy) {
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

constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	return static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs);
}
constexpr uint8_t operator|(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator|=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs | static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator|(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) | rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
}

constexpr uint8_t operator&(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs & static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator&(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}

constexpr uint8_t operator^(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator^=(uint8_t lhs, c7222::Gap::AdvertisingChannelMap rhs) {
	uint8_t ret = lhs ^ static_cast<uint8_t>(rhs);
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
constexpr uint8_t operator^(c7222::Gap::AdvertisingChannelMap lhs, uint8_t rhs) {
	uint8_t ret = static_cast<uint8_t>(lhs) & rhs;
	assert(ret <= static_cast<uint8_t>(c7222::Gap::AdvertisingChannelMap::All));
	return ret;
}
