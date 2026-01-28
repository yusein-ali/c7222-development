#include "attribute_server.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iterator>
#include <iomanip>
#include <iostream>

namespace c7222 {

#if defined(C7222_BLE_DEBUG)
#define C7222_BLE_DEBUG_PRINT(...) std::printf(__VA_ARGS__)
#else
#define C7222_BLE_DEBUG_PRINT(...) do { } while(0)
#endif

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

bool AttributeServer::HasServicesRequiringAuthentication() const {
	for(const auto& service : services_) {
		if(service.HasCharacteristicsRequiringAuthentication()) {
			return true;
		}
	}
	return false;
}

bool AttributeServer::HasServicesRequiringEncryption() const {
	for(const auto& service : services_) {
		if(service.HasCharacteristicsRequiringEncryption()) {
			return true;
		}
	}
	return false;
}

bool AttributeServer::HasServicesRequiringAuthorization() const {
	for(const auto& service : services_) {
		if(service.HasCharacteristicsRequiringAuthorization()) {
			return true;
		}
	}
	return false;
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
	security_level_ = 0;
	authorization_granted_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: connection set handle=0x%04x\n",
		static_cast<unsigned>(connection_handle_));
	for(auto& service: services_) {
		service.SetConnectionHandle(connection_handle);
	}
}

void AttributeServer::SetDisconnected() {
	connection_handle_ = 0;
	security_level_ = 0;
	authorization_granted_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: disconnected\n");
	for(auto& service: services_) {
		service.SetConnectionHandle(0);
	}
}

void AttributeServer::SetSecurityLevel(uint16_t connection_handle, uint8_t security_level) {
	if(connection_handle_ == 0 || connection_handle_ != connection_handle) {
		C7222_BLE_DEBUG_PRINT(
			"[BLE] AttributeServer: ignoring security level update (conn=0x%04x current=0x%04x)\n",
			static_cast<unsigned>(connection_handle),
			static_cast<unsigned>(connection_handle_));
		return;
	}
	security_level_ = security_level;
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: security level=%u (handle=0x%04x)\n",
		static_cast<unsigned>(security_level_),
		static_cast<unsigned>(connection_handle_));
}

uint8_t AttributeServer::GetSecurityLevel(uint16_t connection_handle) const {
	if(connection_handle_ == 0 || connection_handle_ != connection_handle) {
		return 0;
	}
	return security_level_;
}

void AttributeServer::SetAuthorizationGranted(uint16_t connection_handle, bool granted) {
	if(connection_handle_ == 0 || connection_handle_ != connection_handle) {
		C7222_BLE_DEBUG_PRINT(
			"[BLE] AttributeServer: ignoring authorization update (conn=0x%04x current=0x%04x)\n",
			static_cast<unsigned>(connection_handle),
			static_cast<unsigned>(connection_handle_));
		return;
	}
	authorization_granted_ = granted;
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: authorization=%s (handle=0x%04x)\n",
		granted ? "granted" : "denied",
		static_cast<unsigned>(connection_handle_));
}

bool AttributeServer::IsAuthorizationGranted(uint16_t connection_handle) const {
	if(connection_handle_ == 0 || connection_handle_ != connection_handle) {
		return false;
	}
	return authorization_granted_;
}

BleError AttributeServer::DispatchBleHciPacket(uint8_t packet_type,
											   const uint8_t* packet_data,
											   uint16_t packet_data_size) {
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: HCI event packet=0x%02x size=%u\n",
		static_cast<unsigned>(packet_type),
		static_cast<unsigned>(packet_data_size));
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
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read handle=0x%04x offset=%u max=%u\n",
		static_cast<unsigned>(attribute_handle),
		static_cast<unsigned>(offset),
		static_cast<unsigned>(buffer_size));
	ReadResult result{};

	const Attribute* attribute = FindAttributeByHandle(attribute_handle);
	if(attribute == nullptr) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read rejected (not found)\n");
		result.ok = false;
		result.error = BleError::kAttErrorReadNotPermitted;
		return result;
	}

	const bool read_permitted =
		(attribute->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) != 0;
	if(!read_permitted) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read rejected (not permitted)\n");
		result.ok = false;
		result.error = BleError::kAttErrorReadNotPermitted;
		return result;
	}

	if(buffer == nullptr) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read size query bytes=%u\n",
			static_cast<unsigned>(attribute->GetValueSize()));
		result.bytes = static_cast<uint16_t>(attribute->GetValueSize());
		return result;
	}

	if(auto* characteristic = FindCharacteristicByHandle(attribute_handle)) {
		const uint16_t bytes =
			characteristic->HandleAttributeRead(attribute_handle, offset, buffer, buffer_size);
		BleError att_error = BleError::kSuccess;
		if(IsAttErrorCode(bytes, att_error)) {
			C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read error=%u\n",
				static_cast<unsigned>(att_error));
			result.ok = false;
			result.error = att_error;
			return result;
		}
		result.bytes = bytes;
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read bytes=%u\n",
			static_cast<unsigned>(result.bytes));
		return result;
	}

	const uint16_t bytes = attribute->InvokeReadCallback(offset, buffer, buffer_size);
	BleError att_error = BleError::kSuccess;
	if(IsAttErrorCode(bytes, att_error)) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read error=%u\n",
			static_cast<unsigned>(att_error));
		result.ok = false;
		result.error = att_error;
		return result;
	}
	result.bytes = bytes;
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: read bytes=%u\n",
		static_cast<unsigned>(result.bytes));
	return result;
}

BleError AttributeServer::WriteAttribute(uint16_t attribute_handle,
										 uint16_t offset,
										 const uint8_t* data,
										 uint16_t size) {
	C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: write handle=0x%04x offset=%u size=%u\n",
		static_cast<unsigned>(attribute_handle),
		static_cast<unsigned>(offset),
		static_cast<unsigned>(size));
	if(auto* characteristic = FindCharacteristicByHandle(attribute_handle)) {
		const BleError result =
			characteristic->HandleAttributeWrite(attribute_handle, offset, data, size);
		if(result != BleError::kSuccess) {
			C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: write error=%u\n",
				static_cast<unsigned>(result));
		}
		return result;
	}

	Attribute* attribute = FindServiceAttributeByHandle(attribute_handle);
	if(attribute == nullptr) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: write rejected (not found)\n");
		return BleError::kAttErrorWriteNotPermitted;
	}

	const bool write_permitted =
		(attribute->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kWrite)) != 0 ||
		(attribute->GetProperties() &
		 static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) != 0;
	if(!write_permitted) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: write rejected (not permitted)\n");
		return BleError::kAttErrorWriteNotPermitted;
	}

	const BleError result = attribute->InvokeWriteCallback(offset, data, size);
	if(result != BleError::kSuccess) {
		C7222_BLE_DEBUG_PRINT("[BLE] AttributeServer: write error=%u\n",
			static_cast<unsigned>(result));
	}
	return result;
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
