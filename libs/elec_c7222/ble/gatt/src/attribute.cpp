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
namespace {

void AppendLe16(std::vector<uint8_t>& out, uint16_t value) {
	out.push_back(static_cast<uint8_t>(value & 0xFF));
	out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

void AppendUuidBytes(std::vector<uint8_t>& out, const Uuid& uuid) {
	if(uuid.Is16Bit()) {
		AppendLe16(out, uuid.Get16Bit());
	} else if(uuid.Is128Bit()) {
		const auto& bytes = uuid.Get128Bit();
		out.insert(out.end(), bytes.begin(), bytes.end());
	}
}

}  // namespace

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
	if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) == 0) {
		return false;
	}

	// For dynamic attributes, store in vector
	if(data == nullptr || size == 0) {
		dynamic_value_.clear();
		return true;
	}
	dynamic_value_.assign(data, data + size);
	return true;
}

bool Attribute::SetStaticValue(const uint8_t* data, size_t size) {
	if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
		return false;
	}

	static_value_ptr_ = nullptr;
	static_value_size_ = 0;

	if(data == nullptr || size == 0) {
		return true;
	}

	static_value_ptr_ = data;
	static_value_size_ = size;
	return true;
}

bool Attribute::SetValue(const std::vector<uint8_t>& data) {
	if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) == 0) {
		return false;
	}

	if(data.empty()) {
		dynamic_value_.clear();
	} else {
		dynamic_value_ = data;
	}
	return true;
}

bool Attribute::SetStaticValue(const std::vector<uint8_t>& data) {
	return SetStaticValue(data.data(), data.size());
}

bool Attribute::SetValue(std::vector<uint8_t>&& data) {
	if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) == 0) {
		return false;
	}

	// Move into dynamic storage (empty input clears)
	dynamic_value_ = std::move(data);
	return true;
}

// ========== Static Helper Functions for Attribute Type Checking ==========

bool Attribute::IsPrimaryServiceDeclaration(const Attribute& attr) {
	return Uuid::IsPrimaryServiceDeclaration(attr.uuid_);
}

bool Attribute::IsSecondaryServiceDeclaration(const Attribute& attr) {
	return Uuid::IsSecondaryServiceDeclaration(attr.uuid_);
}

bool Attribute::IsIncludedServiceDeclaration(const Attribute& attr) {
	return Uuid::IsIncludedServiceDeclaration(attr.uuid_);
}

bool Attribute::IsCharacteristicDeclaration(const Attribute& attr) {
	return Uuid::IsCharacteristicDeclaration(attr.uuid_);
}

bool Attribute::IsServiceDeclaration(const Attribute& attr) {
	return Uuid::IsServiceDeclaration(attr.uuid_);
}

bool Attribute::IsClientCharacteristicConfiguration(const Attribute& attr) {
	return Uuid::IsClientCharacteristicConfiguration(attr.uuid_);
}

bool Attribute::IsCharacteristicUserDescription(const Attribute& attr) {
	return Uuid::IsCharacteristicUserDescription(attr.uuid_);
}

bool Attribute::IsDescriptor(const Attribute& attr) {
	return Uuid::IsDescriptor(attr.uuid_);
}

Attribute Attribute::PrimaryServiceDeclaration(const Uuid& service_uuid, uint16_t handle) {
	std::vector<uint8_t> value;
	AppendUuidBytes(value, service_uuid);
	return Attribute(Uuid::PrimaryServiceDeclaration(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::SecondaryServiceDeclaration(const Uuid& service_uuid, uint16_t handle) {
	std::vector<uint8_t> value;
	AppendUuidBytes(value, service_uuid);
	return Attribute(Uuid::SecondaryServiceDeclaration(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::IncludedServiceDeclaration(uint16_t start_handle,
												uint16_t end_handle,
												const Uuid& service_uuid,
												uint16_t handle) {
	std::vector<uint8_t> value;
	AppendLe16(value, start_handle);
	AppendLe16(value, end_handle);
	AppendUuidBytes(value, service_uuid);
	return Attribute(Uuid::IncludedServiceDeclaration(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::CharacteristicDeclaration(uint8_t properties,
											   uint16_t value_handle,
											   const Uuid& characteristic_uuid,
											   uint16_t handle) {
	std::vector<uint8_t> value;
	value.push_back(properties);
	AppendLe16(value, value_handle);
	AppendUuidBytes(value, characteristic_uuid);
	return Attribute(Uuid::CharacteristicDeclaration(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::ClientCharacteristicConfiguration(uint16_t value, uint16_t handle) {
	std::vector<uint8_t> bytes;
	AppendLe16(bytes, value);
	return Attribute(Uuid::ClientCharacteristicConfiguration(),
					 static_cast<uint16_t>(Properties::kRead |
										   Properties::kWrite |
										   Properties::kDynamic),
					 bytes.data(),
					 bytes.size(),
					 handle);
}

Attribute Attribute::ServerCharacteristicConfiguration(uint16_t value, uint16_t handle) {
	std::vector<uint8_t> bytes;
	AppendLe16(bytes, value);
	return Attribute(Uuid::ServerCharacteristicConfiguration(),
					 static_cast<uint16_t>(Properties::kRead |
										   Properties::kWrite |
										   Properties::kDynamic),
					 bytes.data(),
					 bytes.size(),
					 handle);
}

Attribute Attribute::CharacteristicUserDescription(const std::string& description,
												   uint16_t handle) {
	const std::vector<uint8_t> value(description.begin(), description.end());
	return Attribute(Uuid::CharacteristicUserDescription(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::CharacteristicExtendedProperties(uint16_t value, uint16_t handle) {
	std::vector<uint8_t> bytes;
	AppendLe16(bytes, value);
	return Attribute(Uuid::CharacteristicExtendedProperties(),
					 static_cast<uint16_t>(Properties::kRead),
					 bytes.data(),
					 bytes.size(),
					 handle);
}

Attribute Attribute::CharacteristicPresentationFormat(uint8_t format,
													  int8_t exponent,
													  uint16_t unit,
													  uint8_t name_space,
													  uint16_t description,
													  uint16_t handle) {
	std::vector<uint8_t> value;
	value.push_back(format);
	value.push_back(static_cast<uint8_t>(exponent));
	AppendLe16(value, unit);
	value.push_back(name_space);
	AppendLe16(value, description);
	return Attribute(Uuid::CharacteristicPresentationFormat(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
}

Attribute Attribute::CharacteristicAggregateFormat(const std::vector<uint16_t>& handles,
												   uint16_t handle) {
	std::vector<uint8_t> value;
	value.reserve(handles.size() * 2);
	for(uint16_t entry: handles) {
		AppendLe16(value, entry);
	}
	return Attribute(Uuid::CharacteristicAggregateFormat(),
					 static_cast<uint16_t>(Properties::kRead),
					 value.data(),
					 value.size(),
					 handle);
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
	const bool is_dynamic = (properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0;
	const bool is_write_permitted =
		(properties_ & static_cast<uint16_t>(Properties::kWrite)) != 0 ||
		(properties_ & static_cast<uint16_t>(Properties::kWriteWithoutResponse)) != 0;
	if(!is_write_permitted) {
		return BleError::kAttErrorWriteNotPermitted;
	}
	
	if(is_dynamic) {
		if(write_callback_) {
			const BleError status = write_callback_(offset, data, size);
			if(status != BleError::kSuccess) {
				return status;
			}
		}

		// Apply write into the dynamic storage after successful callback.
		// Store only the written data chunk (no offset padding).
		if(data != nullptr && size > 0) {
			dynamic_value_.assign(data, data + size);
		}
		return BleError::kSuccess;
	}

	// Static attributes cannot be written.
	if(write_callback_) {
		return write_callback_(offset, data, size);
	}
	return BleError::kAttErrorWriteNotPermitted;
}

}  // namespace c7222
