#include "attribute_server.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <utility>

#include "attribute.hpp"

namespace c7222 {
namespace {

// Binary parsing helpers
constexpr size_t kEntryHeaderSize = 6;  // Size + Flags + Handle
constexpr size_t kUuid16Size = 2;
constexpr size_t kUuid128Size = 16;
constexpr size_t kValue16Offset = kEntryHeaderSize + kUuid16Size;    // 8
constexpr size_t kValue128Offset = kEntryHeaderSize + kUuid128Size;  // 22

uint16_t ReadLe16(const uint8_t* data) {
	return *(reinterpret_cast<const uint16_t*>(data));
}

std::array<uint8_t, 16> ReverseUuid128(const uint8_t* data) {
	std::array<uint8_t, 16> out{};
	for(size_t i = 0; i < out.size(); ++i) {
		out[i] = data[out.size() - 1 - i];
	}
	return out;
}

Attribute ParseEntry(const uint8_t* ptr, uint16_t entry_size) {
	const uint16_t flags = ReadLe16(ptr + 2);
	const uint8_t* uuid_ptr = ptr + 6;

	if((flags & static_cast<uint16_t>(Attribute::Properties::kUuid128)) != 0) {
		if(entry_size < kValue128Offset) {
			return Attribute();
		}
		const uint8_t* value_ptr = ptr + kValue128Offset;
		const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue128Offset);
		const std::array<uint8_t, 16> uuid_bytes = ReverseUuid128(uuid_ptr);
		return Attribute(Uuid(uuid_bytes), flags, value_ptr, value_len);
	}

	if(entry_size < kValue16Offset) {
		return Attribute();
	}
	const uint16_t uuid16 = ReadLe16(uuid_ptr);
	const uint8_t* value_ptr = ptr + kValue16Offset;
	const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue16Offset);
	return Attribute(Uuid(uuid16), flags, value_ptr, value_len);
}

std::unique_ptr<std::list<Attribute>> ParseAttributesFromDb(const uint8_t* db) {
	auto attributes = std::make_unique<std::list<Attribute>>();

	if(db == nullptr) {
		return attributes;
	}

	const uint8_t* ptr = db + 1;

	while(true) {
		const uint16_t entry_size = ReadLe16(ptr);
		if(entry_size == 0) {
			break;
		}
		if(entry_size < kEntryHeaderSize) {
			break;
		}

		attributes->emplace_back(std::move(ParseEntry(ptr, entry_size)));
		ptr += entry_size;
	}

	return attributes;
}

}  // namespace

AttributeServer* AttributeServer::instance_ = nullptr;

BleError AttributeServer::Init(const uint8_t* att_db) {
	services_.clear();
	connection_handle_ = 0;
	initialized_ = false;

	if(att_db == nullptr) {
		return BleError::kUnspecifiedError;
	}

	ParseDatabase(att_db);

	BleError status = InitPlatform(att_db);
	if(status != BleError::kSuccess) {
		return status;
	}

	initialized_ = true;
	return BleError::kSuccess;
}

void AttributeServer::ParseDatabase(const uint8_t* att_db) {
	auto attributes = ParseAttributesFromDb(att_db);
	if(!attributes) {
		return;
	}
	services_ = Service::ParseFromAttributes(*attributes);
}

Service& AttributeServer::GetService(size_t index) {
	assert(index < services_.size() && "Service index out of range");
	auto it = services_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

const Service& AttributeServer::GetService(size_t index) const {
	assert(index < services_.size() && "Service index out of range");
	auto it = services_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

Service* AttributeServer::FindServiceByUuid(const Uuid& uuid) {
	auto it = std::find_if(services_.begin(), services_.end(),
		[&uuid](const Service& service) { return service.GetUuid() == uuid; });
	return it != services_.end() ? &(*it) : nullptr;
}

const Service* AttributeServer::FindServiceByUuid(const Uuid& uuid) const {
	auto it = std::find_if(services_.begin(), services_.end(),
		[&uuid](const Service& service) { return service.GetUuid() == uuid; });
	return it != services_.end() ? &(*it) : nullptr;
}

Characteristic* AttributeServer::FindCharacteristicByUuid(const Uuid& uuid) {
	for(auto& service: services_) {
		if(auto* characteristic = service.FindCharacteristicByUuid(uuid)) {
			return characteristic;
		}
	}
	return nullptr;
}

const Characteristic* AttributeServer::FindCharacteristicByUuid(const Uuid& uuid) const {
	for(const auto& service: services_) {
		if(auto* characteristic = service.FindCharacteristicByUuid(uuid)) {
			return characteristic;
		}
	}
	return nullptr;
}

Characteristic* AttributeServer::FindCharacteristicByHandle(uint16_t handle) {
	for(auto& service: services_) {
		for(size_t i = 0; i < service.GetCharacteristicCount(); ++i) {
			auto& characteristic = service.GetCharacteristic(i);
			if(characteristic.HasHandle(handle)) {
				return &characteristic;
			}
		}
	}
	return nullptr;
}

const Characteristic* AttributeServer::FindCharacteristicByHandle(uint16_t handle) const {
	for(const auto& service: services_) {
		for(size_t i = 0; i < service.GetCharacteristicCount(); ++i) {
			const auto& characteristic = service.GetCharacteristic(i);
			if(characteristic.HasHandle(handle)) {
				return &characteristic;
			}
		}
	}
	return nullptr;
}

void AttributeServer::SetConnectionHandle(uint16_t connection_handle) {
	connection_handle_ = connection_handle;
	for(auto& service: services_) {
		service.SetConnectionHandle(connection_handle);
	}
}

BleError AttributeServer::DispatchBleHciPacket(uint8_t packet_type,
											   const uint8_t* packet_data,
											   uint16_t packet_data_size) {
	for(auto& service: services_) {
		for(size_t i = 0; i < service.GetCharacteristicCount(); ++i) {
			service.GetCharacteristic(i).DispatchBleHciPacket(packet_type,
															 packet_data,
															 packet_data_size);
		}
	}
	return BleError::kSuccess;
}

AttributeServer::ReadResult AttributeServer::ReadAttribute(uint16_t attribute_handle,
														   uint16_t offset,
														   uint8_t* buffer,
														   uint16_t buffer_size) {
	ReadResult result{};

	const Attribute* attribute = FindAttributeByHandle(attribute_handle);
	if(attribute == nullptr) {
		result.ok = false;
		result.error = BleError::kAttErrorReadNotPermitted;
		return result;
	}

	const bool read_permitted =
		(attribute->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) != 0;
	if(!read_permitted) {
		result.ok = false;
		result.error = BleError::kAttErrorReadNotPermitted;
		return result;
	}

	if(buffer == nullptr) {
		result.bytes = static_cast<uint16_t>(attribute->GetValueSize());
		return result;
	}

	if(auto* characteristic = FindCharacteristicByHandle(attribute_handle)) {
		const uint16_t bytes =
			characteristic->HandleAttributeRead(attribute_handle, offset, buffer, buffer_size);
		BleError att_error = BleError::kSuccess;
		if(IsAttErrorCode(bytes, att_error)) {
			result.ok = false;
			result.error = att_error;
			return result;
		}
		result.bytes = bytes;
		return result;
	}

	const uint16_t bytes = attribute->InvokeReadCallback(offset, buffer, buffer_size);
	BleError att_error = BleError::kSuccess;
	if(IsAttErrorCode(bytes, att_error)) {
		result.ok = false;
		result.error = att_error;
		return result;
	}
	result.bytes = bytes;
	return result;
}

BleError AttributeServer::WriteAttribute(uint16_t attribute_handle,
										 uint16_t offset,
										 const uint8_t* data,
										 uint16_t size) {
	if(auto* characteristic = FindCharacteristicByHandle(attribute_handle)) {
		return characteristic->HandleAttributeWrite(attribute_handle, offset, data, size);
	}

	Attribute* attribute = FindServiceAttributeByHandle(attribute_handle);
	if(attribute == nullptr) {
		return BleError::kAttErrorWriteNotPermitted;
	}

	const bool write_permitted =
		(attribute->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kWrite)) != 0 ||
		(attribute->GetProperties() &
		 static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) != 0;
	if(!write_permitted) {
		return BleError::kAttErrorWriteNotPermitted;
	}

	return attribute->InvokeWriteCallback(offset, data, size);
}

Attribute* AttributeServer::FindServiceAttributeByHandle(uint16_t handle) {
	for(auto& service: services_) {
		if(auto* attribute = service.FindServiceAttributeByHandle(handle)) {
			return attribute;
		}
	}
	return nullptr;
}

const Attribute* AttributeServer::FindServiceAttributeByHandle(uint16_t handle) const {
	for(const auto& service: services_) {
		if(auto* attribute = service.FindServiceAttributeByHandle(handle)) {
			return attribute;
		}
	}
	return nullptr;
}

Attribute* AttributeServer::FindAttributeByHandle(uint16_t handle) {
	for(auto& service: services_) {
		if(auto* attribute = service.FindServiceAttributeByHandle(handle)) {
			return attribute;
		}
		for(size_t i = 0; i < service.GetCharacteristicCount(); ++i) {
			auto& characteristic = service.GetCharacteristic(i);
			if(characteristic.GetDeclarationHandle() == handle) {
				return &characteristic.GetDeclarationAttribute();
			}
			if(characteristic.GetValueHandle() == handle) {
				return &characteristic.GetValueAttribute();
			}
			if(auto* cccd = characteristic.GetCCCD()) {
				if(cccd->GetHandle() == handle) {
					return cccd;
				}
			}
			if(auto* sccd = characteristic.GetSCCD()) {
				if(sccd->GetHandle() == handle) {
					return sccd;
				}
			}
			if(auto* extended = characteristic.GetExtendedProperties()) {
				if(extended->GetHandle() == handle) {
					return extended;
				}
			}
			if(auto* user_desc = characteristic.GetUserDescription()) {
				if(user_desc->GetHandle() == handle) {
					return user_desc;
				}
			}
			for(size_t j = 0; j < characteristic.GetDescriptorCount(); ++j) {
				Attribute* descriptor = characteristic.GetDescriptor(j);
				if(descriptor && descriptor->GetHandle() == handle) {
					return descriptor;
				}
			}
		}
	}
	return nullptr;
}

const Attribute* AttributeServer::FindAttributeByHandle(uint16_t handle) const {
	for(const auto& service: services_) {
		if(auto* attribute = service.FindServiceAttributeByHandle(handle)) {
			return attribute;
		}
		for(size_t i = 0; i < service.GetCharacteristicCount(); ++i) {
			const auto& characteristic = service.GetCharacteristic(i);
			if(characteristic.GetDeclarationHandle() == handle) {
				return &characteristic.GetDeclarationAttribute();
			}
			if(characteristic.GetValueHandle() == handle) {
				return &characteristic.GetValueAttribute();
			}
			if(auto* cccd = characteristic.GetCCCD()) {
				if(cccd->GetHandle() == handle) {
					return cccd;
				}
			}
			if(auto* sccd = characteristic.GetSCCD()) {
				if(sccd->GetHandle() == handle) {
					return sccd;
				}
			}
			if(auto* extended = characteristic.GetExtendedProperties()) {
				if(extended->GetHandle() == handle) {
					return extended;
				}
			}
			if(auto* user_desc = characteristic.GetUserDescription()) {
				if(user_desc->GetHandle() == handle) {
					return user_desc;
				}
			}
			for(size_t j = 0; j < characteristic.GetDescriptorCount(); ++j) {
				const Attribute* descriptor = characteristic.GetDescriptor(j);
				if(descriptor && descriptor->GetHandle() == handle) {
					return descriptor;
				}
			}
		}
	}
	return nullptr;
}

bool AttributeServer::IsAttErrorCode(uint16_t value, BleError& out_error) {
	switch(static_cast<BleError>(value)) {
	case BleError::kAttErrorReadNotPermitted:
	case BleError::kAttErrorWriteNotPermitted:
	case BleError::kAttErrorInvalidAttrValueLength:
		out_error = static_cast<BleError>(value);
		return true;
	default:
		break;
	}
	return false;
}

}  // namespace c7222
