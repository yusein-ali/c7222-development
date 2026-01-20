#include "service.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

namespace c7222 {

Service::Service(const Uuid& uuid, ServiceType service_type, uint16_t declaration_handle)
	: uuid_(uuid),
	  service_type_(service_type),
	  declaration_attr_(service_type == ServiceType::kPrimary
							? Attribute::PrimaryServiceDeclaration(uuid, declaration_handle)
							: Attribute::SecondaryServiceDeclaration(uuid, declaration_handle)),
	  connection_handle_(0) {}

Service::Service(Attribute&& declaration_attr,
				 std::vector<Attribute>&& included_service_declarations,
				 std::list<Characteristic>&& characteristics)
	: uuid_(Uuid()),
	  service_type_(Attribute::IsPrimaryServiceDeclaration(declaration_attr)
						? ServiceType::kPrimary
						: ServiceType::kSecondary),
	  declaration_attr_(std::move(declaration_attr)),
	  connection_handle_(0),
	  characteristics_(std::move(characteristics)),
	  included_service_declarations_(std::move(included_service_declarations)) {
	const uint8_t* decl_data = declaration_attr_.GetValueData();
	const size_t decl_size = declaration_attr_.GetValueSize();
	if(decl_data != nullptr && (decl_size == 2 || decl_size == 16)) {
		if(decl_size == 2) {
			const uint16_t uuid16 = static_cast<uint16_t>(decl_data[0]) |
									(static_cast<uint16_t>(decl_data[1]) << 8);
			uuid_ = Uuid(uuid16);
		} else {
			uuid_ = Uuid(decl_data, decl_size);
		}
	}
}

std::list<Service> Service::ParseFromAttributes(std::list<Attribute>& attributes) {
	std::list<Service> services;

	while(true) {
		while(!attributes.empty() && !Attribute::IsServiceDeclaration(attributes.front())) {
			attributes.pop_front();
		}
		if(attributes.empty()) {
			break;
		}

		auto end_it = std::next(attributes.begin());
		for(; end_it != attributes.end(); ++end_it) {
			if(Attribute::IsServiceDeclaration(*end_it)) {
				break;
			}
		}

		std::list<Attribute> service_attributes;
		service_attributes.splice(service_attributes.end(), attributes, attributes.begin(), end_it);

		const Attribute& decl_attr = service_attributes.front();
		const uint8_t* decl_data = decl_attr.GetValueData();
		const size_t decl_size = decl_attr.GetValueSize();
		if(decl_data == nullptr || (decl_size != 2 && decl_size != 16)) {
			continue;
		}

		Attribute decl_attribute = std::move(service_attributes.front());
		service_attributes.pop_front();

		std::vector<Attribute> included_service_decls;
		for(auto it = service_attributes.begin(); it != service_attributes.end();) {
			if(Attribute::IsIncludedServiceDeclaration(*it)) {
				included_service_decls.push_back(std::move(*it));
				it = service_attributes.erase(it);
			} else {
				++it;
			}
		}

		std::list<Characteristic> characteristics;
		while(!service_attributes.empty()) {
			auto characteristic = Characteristic::ParseFromAttributes(service_attributes);
			assert(characteristic && "Failed to parse characteristic from attribute DB");
			characteristics.push_back(std::move(*characteristic));
		}
		// construct the service
		services.emplace_back(std::move(decl_attribute),
							  std::move(included_service_decls),
							  std::move(characteristics));
	}

	return services;
}

Characteristic& Service::GetCharacteristic(size_t index) {
	assert(index < characteristics_.size() && "Characteristic index out of range");
	auto it = characteristics_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

const Characteristic& Service::GetCharacteristic(size_t index) const {
	assert(index < characteristics_.size() && "Characteristic index out of range");
	auto it = characteristics_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

Characteristic* Service::FindCharacteristicByUuid(const Uuid& uuid) {
	auto it =
		std::find_if(characteristics_.begin(),
					 characteristics_.end(),
					 [&uuid](const Characteristic& ch) { return ch.IsThisCharacteristic(uuid); });
	return it != characteristics_.end() ? &(*it) : nullptr;
}

const Characteristic* Service::FindCharacteristicByUuid(const Uuid& uuid) const {
	auto it =
		std::find_if(characteristics_.begin(),
					 characteristics_.end(),
					 [&uuid](const Characteristic& ch) { return ch.IsThisCharacteristic(uuid); });
	return it != characteristics_.end() ? &(*it) : nullptr;
}

Characteristic* Service::FindCharacteristicByHandle(uint16_t handle) {
	auto it = std::find_if(characteristics_.begin(),
						   characteristics_.end(),
						   [handle](const Characteristic& ch) { return ch.HasHandle(handle); });
	return it != characteristics_.end() ? &(*it) : nullptr;
}

const Characteristic* Service::FindCharacteristicByHandle(uint16_t handle) const {
	auto it = std::find_if(characteristics_.begin(),
						   characteristics_.end(),
						   [handle](const Characteristic& ch) { return ch.HasHandle(handle); });
	return it != characteristics_.end() ? &(*it) : nullptr;
}

Service& Service::GetIncludedService(size_t index) {
	assert(index < included_services_.size() && "Included service index out of range");
	auto it = included_services_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

const Service& Service::GetIncludedService(size_t index) const {
	assert(index < included_services_.size() && "Included service index out of range");
	auto it = included_services_.begin();
	std::advance(it, static_cast<long>(index));
	return *it;
}

bool Service::IsValid() const {
	return uuid_.IsValid() && declaration_attr_.GetHandle() != 0 && characteristics_.size() > 0;
}

Attribute* Service::FindServiceAttributeByHandle(uint16_t handle) {
	if(declaration_attr_.GetHandle() == handle) {
		return &declaration_attr_;
	}
	for(auto& included_attr: included_service_declarations_) {
		if(included_attr.GetHandle() == handle) {
			return &included_attr;
		}
	}
	return nullptr;
}

const Attribute* Service::FindServiceAttributeByHandle(uint16_t handle) const {
	if(declaration_attr_.GetHandle() == handle) {
		return &declaration_attr_;
	}
	for(const auto& included_attr: included_service_declarations_) {
		if(included_attr.GetHandle() == handle) {
			return &included_attr;
		}
	}
	return nullptr;
}

void Service::SetConnectionHandle(uint16_t connection_handle) {
	connection_handle_ = connection_handle;
	for(auto& characteristic: characteristics_) {
		characteristic.SetConnectionHandle(connection_handle);
	}
	for(auto& included_service: included_services_) {
		included_service.SetConnectionHandle(connection_handle);
	}
}

Characteristic& Service::AddCharacteristic(Characteristic&& characteristic) {
	characteristics_.push_back(std::move(characteristic));
	return characteristics_.back();
}

Characteristic& Service::CreateCharacteristic(const Uuid& uuid,
											  uint8_t properties,
											  uint16_t value_handle,
											  uint16_t declaration_handle) {
	characteristics_.emplace_back(uuid, properties, value_handle, declaration_handle);
	return characteristics_.back();
}

bool Service::RemoveCharacteristic(size_t index) {
	if(index >= characteristics_.size()) {
		return false;
	}
	auto it = characteristics_.begin();
	std::advance(it, static_cast<long>(index));
	characteristics_.erase(it);
	return true;
}

void Service::ClearCharacteristics() {
	characteristics_.clear();
}

void Service::AddIncludedService(const Service& included_service, uint16_t declaration_handle) {
	// Create an included service declaration attribute
	// The value contains: 16-bit handle of included service, 128-bit UUID (if 128-bit)
	Attribute decl(included_service.GetUuid(), 0);
	decl.SetHandle(declaration_handle);

	// Build the declaration value with the included service handle and UUID
	std::vector<uint8_t> decl_value;

	// Add the included service start handle (2 bytes, little-endian)
	uint16_t included_handle = included_service.GetDeclarationHandle();
	decl_value.push_back(static_cast<uint8_t>(included_handle & 0xFF));
	decl_value.push_back(static_cast<uint8_t>((included_handle >> 8) & 0xFF));

	// Add the included service end handle (2 bytes, little-endian)
	// For simplicity, estimate as declaration handle + number of characteristics
	uint16_t end_handle = included_handle + included_service.GetCharacteristicCount() * 2;
	decl_value.push_back(static_cast<uint8_t>(end_handle & 0xFF));
	decl_value.push_back(static_cast<uint8_t>((end_handle >> 8) & 0xFF));

	// Add the UUID
	const Uuid& included_uuid = included_service.GetUuid();
	if(included_uuid.Is16Bit()) {
		// 16-bit UUID (2 bytes, little-endian)
		uint16_t uuid16 = included_uuid.Get16Bit();
		decl_value.push_back(static_cast<uint8_t>(uuid16 & 0xFF));
		decl_value.push_back(static_cast<uint8_t>((uuid16 >> 8) & 0xFF));
	} else if(included_uuid.Is128Bit()) {
		// 128-bit UUID (16 bytes)
		const auto& uuid_bytes = included_uuid.Get128Bit();
		decl_value.insert(decl_value.end(), uuid_bytes.begin(), uuid_bytes.end());
	}

	decl.SetValue(decl_value.data(), decl_value.size());
	decl.SetProperties(static_cast<uint16_t>(Attribute::Properties::kRead));

	included_service_declarations_.push_back(std::move(decl));
}

bool Service::RemoveIncludedService(size_t index) {
	if(index >= included_services_.size()) {
		return false;
	}
	auto it = included_services_.begin();
	std::advance(it, static_cast<long>(index));
	included_services_.erase(it);
	if(index < included_service_declarations_.size()) {
		included_service_declarations_.erase(included_service_declarations_.begin() + index);
	}
	return true;
}

void Service::ClearIncludedServices() {
	included_services_.clear();
	included_service_declarations_.clear();
}

std::ostream& operator<<(std::ostream& os, const Service& service) {
	os << "{";
	os << "\n  UUID: " << service.GetUuid();
	os << "\n  Type: "
	   << (service.GetServiceType() == Service::ServiceType::kPrimary ? "Primary" : "Secondary");
	os << "\n  Declaration Handle: 0x" << std::hex << std::setw(4) << std::setfill('0')
	   << service.GetDeclarationHandle() << std::dec;
	if(service.GetIncludedServiceCount() > 0) {
		os << "\n  Included Services: " << service.GetIncludedServiceCount();
	}
	os << "\n  Characteristics: " << std::endl;

	int i = 1;
	for(const auto& characteristic: service.GetCharacteristics()) {
		os << " [" << i << "]: " << characteristic << std::endl;
		++i;
	}
	os << "\n}";
	return os;
}

}  // namespace c7222
