#include "attribute_server.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iomanip>
#include <iostream>

namespace c7222 {

AttributeServer* AttributeServer::instance_ = nullptr;

void AttributeServer::InitServices(std::list<Attribute>& attributes) {
	services_ = Service::ParseFromAttributes(attributes);
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

std::list<Characteristic*> AttributeServer::FindCharacteristicByUuid(const Uuid& uuid) {
	std::list<Characteristic*> result;
	for(auto& service: services_) {
		auto* characteristic = service.FindCharacteristicByUuid(uuid);
		if(characteristic != nullptr) {
			result.push_back(characteristic);
		}
	}
	return result;
}	

std::list<const Characteristic*> AttributeServer::FindCharacteristicByUuid(const Uuid& uuid) const {
	std::list<const Characteristic*> result;
	for(const auto& service: services_) {
		auto* characteristic = service.FindCharacteristicByUuid(uuid);
		if(characteristic != nullptr) {
			result.push_back(characteristic);
		}
	}
	return result;
}

Characteristic* AttributeServer::FindCharacteristicByHandle(uint16_t handle) {
	for(auto& service: services_) {
		for(const auto& characteristic : service.GetCharacteristics()) {
			if(characteristic.HasHandle(handle)) {
				return &const_cast<Characteristic&>(characteristic);
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

void AttributeServer::SetDisconnected() {
	connection_handle_ = 0;
	for(auto& service: services_) {
		service.SetConnectionHandle(0);
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

std::ostream& operator<<(std::ostream& os, const AttributeServer& server) {
	os << "AttributeServer {";
	os << "\n  Initialized: " << (server.IsInitialized() ? "true" : "false");
	os << "\n  Service Count: " << server.GetServiceCount();
	if(server.GetConnectionHandle() == 0) {
		os << "\n  Connection: disconnected";
	} else {
		os << "\n  Connection: connected (handle=0x" << std::hex << std::setw(4)
		   << std::setfill('0') << server.GetConnectionHandle() << std::dec << ")";
	}
	os << "\n  Services:" << std::endl;
	size_t index = 1;
	for(const auto& service: server) {
		os << "  [" << index << "]:";
		os << service << std::endl;
		++index;
	}
	os << "\n}";
	return os;
}

}  // namespace c7222
