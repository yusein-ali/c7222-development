#include "ble_address.hpp"
#include "ble_error.hpp"


namespace c7222 {

std::ostream& operator<<(std::ostream& os, const BleAddress& addr) {
	os << "BleAddress(";
	switch(addr.type()) {
	case BleAddress::AddressType::LePublic:
		os << "LE Public) ";
		break;
	case BleAddress::AddressType::LeRandom:
		os << "LE Random) ";
		break;
	case BleAddress::AddressType::LePublicIdentity:
		os << "LE Public Identity) ";
		break;
	case BleAddress::AddressType::LeRandomIdentity:
		os << "LE Random Identity) ";
		break;
	case BleAddress::AddressType::Sco:
		os << "SCO) ";
		break;
	case BleAddress::AddressType::Acl:
		os << "ACL) ";
		break;
	case BleAddress::AddressType::Unknown:
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
		os << static_cast<int>(addr.bytes()[BleAddress::kLength - 1 - i]);
	}
	os << std::dec;
	return os;
}

} // namespace c7222
