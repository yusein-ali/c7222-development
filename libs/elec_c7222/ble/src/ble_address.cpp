#include "ble_address.hpp"


namespace c7222 {

std::ostream& operator<<(std::ostream& os, const BleAddress& addr) {
	os << "BleAddress(";
	switch(addr.GetType()) {
	case BleAddress::AddressType::kLePublic:
		os << "LE Public) ";
		break;
	case BleAddress::AddressType::kLeRandom:
		os << "LE Random) ";
		break;
	case BleAddress::AddressType::kLePublicIdentity:
		os << "LE Public Identity) ";
		break;
	case BleAddress::AddressType::kLeRandomIdentity:
		os << "LE Random Identity) ";
		break;
	case BleAddress::AddressType::kSco:
		os << "SCO) ";
		break;
	case BleAddress::AddressType::kAcl:
		os << "ACL) ";
		break;
	case BleAddress::AddressType::kUnknown:
		os << "Unknown) ";
		break;
	default:
		os << "Invalid) ";
		break;
	}
	os << std::hex;
	for(size_t i = 0; i < BleAddress::kLength; ++i) {
		if(i != 0) {
			os << ":";
		}
		os << static_cast<int>(addr.GetBytes()[BleAddress::kLength - 1 - i]);
	}
	os << std::dec;
	return os;
}

} // namespace c7222
