/**
 * @file attribute.cpp
 * @brief Implementation of GATT attribute wrapper.
 */

#include "attribute.hpp"
#include <algorithm>
#include <iomanip>
#include <utility>
#include "ble_error.hpp"

namespace c7222 {

std::ostream& operator<<(std::ostream& os, const Attribute& attr) {
	os << "Attribute(handle=0x" << std::hex << std::setw(4) << std::setfill('0') << attr.GetHandle()
	   << ", uuid=" << attr.uuid_ << ", properties=0x" << std::setw(4) << attr.properties_ << " [";

	// Parse and output property flags
	bool first = true;

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kRead)) {
		if(!first) os << " | ";
		os << "Read";
		first = false;
	}

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) {
		if(!first) os << " | ";
		os << "WriteWithoutResponse";
		first = false;
	}

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kWrite)) {
		if(!first) os << " | ";
		os << "Write";
		first = false;
	}

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) {
		if(!first) os << " | ";
		os << "AuthenticatedSignedWrite";
		first = false;
	}

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kDynamic)) {
		if(!first) os << " | ";
		os << "Dynamic";
		first = false;
	}

	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kUuid128)) {
		if(!first) os << " | ";
		os << "UUID128";
		first = false;
	}

	// Check write permission bits (bits 0 and 4)
	uint16_t write_perm = (attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit0)) |
						 ((attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit1)) >> 4);
	if(write_perm != 0) {
		if(!first) os << " | ";
		os << "WritePerm(" << write_perm << ")";
		first = false;
	}

	// Check write permission SC bit (bit 7)
	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kWritePermissionSc)) {
		if(!first) os << " | ";
		os << "WritePermSC";
		first = false;
	}

	// Check read permission bits (bits 10 and 11)
	uint16_t read_perm = ((attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit0)) >> 10) |
						((attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit1)) >> 10);
	if(read_perm != 0) {
		if(!first) os << " | ";
		os << "ReadPerm(" << read_perm << ")";
		first = false;
	}

	// Check read permission SC bit (bit 5)
	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kReadPermissionSc)) {
		if(!first) os << " | ";
		os << "ReadPermSC";
		first = false;
	}

	// Check encryption key size mask (bits 12-15)
	uint16_t enc_key_size = (attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kEncryptionKeySizeMask)) >> 12;
	if(enc_key_size != 0) {
		if(!first) os << " | ";
		os << "EncKeySize(" << enc_key_size << ")";
		first = false;
	}

	if(first) {
		os << "None";
	}

	// Output value size from appropriate storage
	size_t value_size;
	if(attr.properties_ & static_cast<uint16_t>(Attribute::Properties::kDynamic)) {
		value_size = attr.dynamic_value_.size();
	} else {
		value_size = attr.static_value_size_;
	}
	
	os << "], value_size=" << std::dec << value_size << ")";

	return os;
}

Attribute::Attribute(const Uuid& uuid, uint16_t properties, uint16_t handle) {
	uuid_ = uuid;
	properties_ = properties;
	handle_ = handle;
	UpdateUuidProperty();
}

// Constructor with value data
Attribute::Attribute(const Uuid& uuid, uint16_t properties, const uint8_t* data, size_t size, uint16_t handle)
	: uuid_(uuid), properties_(properties), handle_(handle) {
	UpdateUuidProperty();
	if(data != nullptr && size > 0) {
		if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
			// Dynamic: copy to vector for mutable storage
			dynamic_value_.assign(data, data + size);
			static_value_ptr_ = nullptr;
			static_value_size_ = 0;
		} else {
			// Static: just store pointer to const DB data
			static_value_ptr_ = data;
			static_value_size_ = size;
		}
	}
}

bool Attribute::SetValue(const uint8_t* data, size_t size) {
	// Reject writes to static attributes
	assert((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0 && "Cannot set value of static attribute");

	// For dynamic attributes, store in vector
	if(data == nullptr || size == 0) {
		dynamic_value_.clear();
		return true;
	}
	dynamic_value_.assign(data, data + size);
	return true;
}


bool Attribute::SetValue(const std::vector<uint8_t>& data) {
	// Reject writes to static attributes
	assert((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0 && "Cannot set value of static attribute");

	if(data.empty()) {
		dynamic_value_.clear();
	} else {
		dynamic_value_ = data;
	}
	return true;
}

bool Attribute::SetValue(std::vector<uint8_t>&& data) {
	// Reject writes to static attributes
	assert((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0 &&
		   "Cannot set value of static attribute");

	// Move into dynamic storage (empty input clears)
	dynamic_value_ = std::move(data);
	return true;
}

// ========== Static Helper Functions for Attribute Type Checking ==========

bool Attribute::IsPrimaryServiceDeclaration(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kPrimaryServiceDeclaration);
}

bool Attribute::IsSecondaryServiceDeclaration(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kSecondaryServiceDeclaration);
}

bool Attribute::IsServiceDeclaration(const Attribute& attr) {
	return IsPrimaryServiceDeclaration(attr) || IsSecondaryServiceDeclaration(attr);
}

bool Attribute::IsIncludedServiceDeclaration(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kIncludedServiceDeclaration);
}

bool Attribute::IsCharacteristicDeclaration(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kCharacteristicDeclaration);
}

bool Attribute::IsClientCharacteristicConfiguration(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kClientCharacteristicConfiguration);
}

bool Attribute::IsCharacteristicUserDescription(const Attribute& attr) {
	const Uuid& uuid = attr.uuid_;
	return uuid.Is16Bit() && uuid.Get16Bit() == static_cast<uint16_t>(AttributeType::kCharacteristicUserDescription);
}

bool Attribute::IsDescriptor(const Attribute& attr) {
	// Check if UUID matches any known descriptor type
	if (!attr.uuid_.Is16Bit()) {
		return false;
	}
	
	uint16_t uuid16 = attr.uuid_.Get16Bit();
	return uuid16 == static_cast<uint16_t>(AttributeType::kClientCharacteristicConfiguration) ||
	       uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicUserDescription) ||
	       uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicExtendedProperties) ||
	       uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicPresentationFormat) ||
	       uuid16 == static_cast<uint16_t>(AttributeType::kCharacteristicAggregateFormat) ||
	       uuid16 == static_cast<uint16_t>(AttributeType::kServerCharacteristicConfiguration);
}

uint16_t Attribute::InvokeReadCallback(uint16_t offset, uint8_t* buffer, uint16_t buffer_size) const {
	if(read_callback_) {
		return read_callback_(offset, buffer, buffer_size);
	}
	// Default: copy from value storage
	const uint8_t* value_data = GetValueData();
	size_t value_size = GetValueSize();
	
	if(offset >= value_size) {
		return 0;
	}
	uint16_t bytes_to_copy = std::min(static_cast<uint16_t>(value_size - offset), buffer_size);
	if(buffer && bytes_to_copy > 0 && value_data) {
		std::copy(value_data + offset, value_data + offset + bytes_to_copy, buffer);
	}
	return bytes_to_copy;
}

BleError Attribute::InvokeWriteCallback(uint16_t offset, const uint8_t* data, uint16_t size) {
	if(write_callback_) {
		return write_callback_(offset, data, size);
	}
	// Default behavior: reject writes when no callback is installed
	// Dynamic attributes require a write callback
	if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
		return BleError::kAttErrorWriteNotPermitted;
	}
	// Static attributes cannot be written
	return BleError::kAttErrorWriteNotPermitted;
}

}  // namespace c7222
