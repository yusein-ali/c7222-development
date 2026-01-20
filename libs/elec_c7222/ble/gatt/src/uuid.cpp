
#include "uuid.hpp"
#include <iomanip>

namespace c7222{
std::ostream& operator<<(std::ostream& os, const Uuid& uuid) {
	std::ios_base::fmtflags flags = os.flags();
	char fill = os.fill();
	os << std::hex << std::nouppercase << std::setfill('0');
	auto type = uuid.type();
	switch(type) {
		case Uuid::Type::k16Bit:
			os << "16-bit UUID: ";
			os << "0x" << std::setw(4) << uuid.Get16Bit();
			break;
		case Uuid::Type::k128Bit:
			os << "128-bit UUID: ";
			{
				const auto& bytes = uuid.Get128Bit();
				for(size_t i = 0; i < bytes.size(); ++i) {
					os << std::setw(2) << static_cast<int>(bytes[i]);
					if(i == 3 || i == 5 || i == 7 || i == 9) {
						os << '-';
					}
				}
			}
			break;
		default:
			os << "Invalid UUID";
			break;
	}
	os.flags(flags);
	os.fill(fill);
	return os;
}

Uuid::Uuid(const uint8_t* uuid, size_t size){
	switch(size) {
		case 2:
			type_ = Type::k16Bit;
			std::copy(uuid, uuid + 2, uuid_.begin());
			break;
		case 16:
			type_ = Type::k128Bit;
			std::copy(uuid, uuid + 16, uuid_.begin());
			break;
		default:
			assert(false && "UUID can be either 16 bits or 128 bits");
	}
}

bool Uuid::operator==(const Uuid& other) const {
	if(type_ != other.type_) {
		return false;
	}
	if(type_ == Type::k16Bit) {
		return Get16Bit() == other.Get16Bit();
	} else if(type_ == Type::k128Bit) {
		return uuid_ == other.uuid_;
	}
	return false;
}

Uuid Uuid::Convert16To128(const Uuid& uuid16) {
	assert(uuid16.type_ == Type::k16Bit && "Input UUID is not 16-bit");
	std::array<uint8_t, 16> uuid128 = {0x00,
									   0x00,
									   0x00,
									   0x00,
									   0x00,
									   0x00,
									   0x10,
									   0x00,
									   0x80,
									   0x00,
									   0x00,
									   0x80,
									   0x5F,
									   0x9B,
									   0x34,
									   0xFB};
	const uint8_t* short_uuid = uuid16.data();
	uuid128[2] = (short_uuid[1]) & 0xFF;
	uuid128[3] = (short_uuid[0]) & 0xFF;
	return Uuid(uuid128);
}

Uuid Uuid::PrimaryServiceDeclaration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kPrimaryServiceDeclaration));
}

Uuid Uuid::SecondaryServiceDeclaration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kSecondaryServiceDeclaration));
}

Uuid Uuid::IncludedServiceDeclaration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kIncludedServiceDeclaration));
}

Uuid Uuid::CharacteristicDeclaration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kCharacteristicDeclaration));
}

Uuid Uuid::ClientCharacteristicConfiguration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kClientCharacteristicConfiguration));
}

Uuid Uuid::ServerCharacteristicConfiguration() {
	return Uuid(static_cast<uint16_t>(AttributeType::kServerCharacteristicConfiguration));
}

Uuid Uuid::CharacteristicUserDescription() {
	return Uuid(static_cast<uint16_t>(AttributeType::kCharacteristicUserDescription));
}

Uuid Uuid::CharacteristicExtendedProperties() {
	return Uuid(static_cast<uint16_t>(AttributeType::kCharacteristicExtendedProperties));
}

Uuid Uuid::CharacteristicPresentationFormat() {
	return Uuid(static_cast<uint16_t>(AttributeType::kCharacteristicPresentationFormat));
}

Uuid Uuid::CharacteristicAggregateFormat() {
	return Uuid(static_cast<uint16_t>(AttributeType::kCharacteristicAggregateFormat));
}

bool Uuid::IsPrimaryServiceDeclaration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kPrimaryServiceDeclaration);
}

bool Uuid::IsSecondaryServiceDeclaration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kSecondaryServiceDeclaration);
}

bool Uuid::IsIncludedServiceDeclaration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kIncludedServiceDeclaration);
}

bool Uuid::IsCharacteristicDeclaration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kCharacteristicDeclaration);
}

bool Uuid::IsServiceDeclaration(const Uuid& uuid) {
	if(!uuid.Is16Bit()) {
		return false;
	}
	const uint16_t uuid16 = uuid.Get16Bit();
	return uuid16 == static_cast<uint16_t>(AttributeType::kPrimaryServiceDeclaration) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kSecondaryServiceDeclaration);
}

bool Uuid::IsClientCharacteristicConfiguration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() ==
			   static_cast<uint16_t>(AttributeType::kClientCharacteristicConfiguration);
}

bool Uuid::IsServerCharacteristicConfiguration(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() ==
			   static_cast<uint16_t>(AttributeType::kServerCharacteristicConfiguration);
}

bool Uuid::IsCharacteristicUserDescription(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kCharacteristicUserDescription);
}

bool Uuid::IsCharacteristicExtendedProperties(const Uuid& uuid) {
	return uuid.Is16Bit() &&
		   uuid.Get16Bit() ==
			   static_cast<uint16_t>(AttributeType::kCharacteristicExtendedProperties);
}

bool Uuid::IsDescriptor(const Uuid& uuid) {
	if(!uuid.Is16Bit()) {
		return false;
	}
	const uint16_t uuid16 = uuid.Get16Bit();
	return uuid16 == static_cast<uint16_t>(AttributeType::kClientCharacteristicConfiguration) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicUserDescription) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicExtendedProperties) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicPresentationFormat) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicAggregateFormat) ||
		   uuid16 == static_cast<uint16_t>(AttributeType::kServerCharacteristicConfiguration);
}


} //namespace c7222
