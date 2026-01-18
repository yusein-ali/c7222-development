#include "characteristic.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>

namespace c7222 {

Characteristic::Characteristic(const Uuid& uuid,
							   uint8_t properties,
							   uint16_t value_handle,
							   uint16_t declaration_handle)
	: uuid_(uuid),
	  properties_(static_cast<Properties>(properties)),
	  connection_handle_(0),
	  notification_pending_(false),
	  declaration_attr_(Attribute::CharacteristicDeclaration(properties, value_handle, uuid, declaration_handle)),
	  value_attr_(uuid, 0) {
	// Set handles on the attributes
	value_attr_.SetHandle(value_handle);

	// Map characteristic properties (bitfield) to attribute properties (bitfield)
	// Properties is a bitfield enum: each bit represents a capability (Read, Write, Notify,
	// Indicate, etc.)
	uint16_t attr_props = 0;

	// Map read capability
	if((static_cast<uint8_t>(properties) & static_cast<uint8_t>(Properties::kRead)) != 0)
		attr_props |= static_cast<uint16_t>(Attribute::Properties::kRead);

	// Map write capability
	if((static_cast<uint8_t>(properties) & static_cast<uint8_t>(Properties::kWrite)) != 0)
		attr_props |= static_cast<uint16_t>(Attribute::Properties::kWrite);

	// Map write without response capability
	if((static_cast<uint8_t>(properties) &
		static_cast<uint8_t>(Properties::kWriteWithoutResponse)) != 0)
		attr_props |= static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse);

	// Mark as dynamic if it supports server-initiated notifications or indications
	// These require callbacks and dynamic value management
	uint8_t notify_indicate =
		(static_cast<uint8_t>(properties) &
		 (static_cast<uint8_t>(Properties::kNotify) | static_cast<uint8_t>(Properties::kIndicate)));
	if(notify_indicate != 0)
		attr_props |= static_cast<uint16_t>(Attribute::Properties::kDynamic);

	// Map authenticated signed writes capability
	if((static_cast<uint8_t>(properties) &
		static_cast<uint8_t>(Properties::kAuthenticatedSignedWrites)) != 0)
		attr_props |= static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite);

	value_attr_.SetProperties(attr_props);

	// Set up read and write callbacks for the value attribute
	value_attr_.SetReadCallback([this](uint16_t offset, uint8_t* buffer, uint16_t buffer_size) {
		return HandleValueRead(offset, buffer, buffer_size);
	});
	value_attr_.SetWriteCallback([this](uint16_t offset, const uint8_t* data, uint16_t size) {
		return HandleValueWrite(offset, data, size);
	});
}

std::optional<Characteristic>
Characteristic::ParseFromAttributes(std::list<Attribute>& attributes) {
	auto decl_it = attributes.begin();
	for(; decl_it != attributes.end(); ++decl_it) {
		if(Attribute::IsCharacteristicDeclaration(*decl_it)) {
			break;
		}
	}

	if(decl_it == attributes.end()) {
		return std::nullopt;
	}

	auto end_it = std::next(decl_it);
	for(; end_it != attributes.end(); ++end_it) {
		if(Attribute::IsCharacteristicDeclaration(*end_it) || Attribute::IsServiceDeclaration(*end_it)) {
			break;
		}
	}

	std::list<Attribute> characteristic_attributes;
	characteristic_attributes.splice(characteristic_attributes.end(), attributes, decl_it, end_it);

	const Attribute& decl_attr = characteristic_attributes.front();
	const uint8_t* decl_data = decl_attr.GetValueData();
	const size_t decl_size = decl_attr.GetValueSize();
	if(decl_data == nullptr || decl_size < 5) {
		return std::nullopt;
	}

	uint8_t props = decl_data[0];
	uint16_t value_handle = static_cast<uint16_t>(decl_data[1]) |
							(static_cast<uint16_t>(decl_data[2]) << 8);

	Uuid char_uuid;
	if(decl_size == 5) {
		uint16_t uuid16 = static_cast<uint16_t>(decl_data[3]) |
						  (static_cast<uint16_t>(decl_data[4]) << 8);
		char_uuid = Uuid(uuid16);
	} else if(decl_size == 19) {
		std::array<uint8_t, 16> uuid128;
		std::copy(decl_data + 3, decl_data + 19, uuid128.begin());
		char_uuid = Uuid(uuid128);
	} else {
		return std::nullopt;
	}

	Characteristic characteristic(char_uuid, props, value_handle, decl_attr.GetHandle());

	auto value_it = characteristic_attributes.end();
	for(auto it = std::next(characteristic_attributes.begin()); it != characteristic_attributes.end();
		++it) {
		if(it->GetHandle() == value_handle) {
			value_it = it;
			break;
		}
	}

	if(value_it != characteristic_attributes.end()) {
		const Attribute& value_attr = *value_it;
		const uint8_t* value_data = value_attr.GetValueData();
		size_t value_size = value_attr.GetValueSize();
		if(value_data != nullptr && value_size > 0) {
			characteristic.SetValue(value_data, value_size);
		}
	}

	auto desc_it = (value_it != characteristic_attributes.end())
					   ? std::next(value_it)
					   : std::next(characteristic_attributes.begin());
	for(; desc_it != characteristic_attributes.end(); ++desc_it) {
		const Attribute& next_attr = *desc_it;

		if(Attribute::IsClientCharacteristicConfiguration(next_attr)) {
			Attribute& cccd = characteristic.EnableCCCD();
			const uint8_t* cccd_data = next_attr.GetValueData();
			size_t cccd_size = next_attr.GetValueSize();
			if(cccd_data != nullptr && cccd_size > 0) {
				cccd.SetValue(cccd_data, cccd_size);
			}
		} else if(Attribute::IsCharacteristicUserDescription(next_attr)) {
			const uint8_t* desc_data = next_attr.GetValueData();
			size_t desc_size = next_attr.GetValueSize();
			if(desc_data != nullptr && desc_size > 0) {
				std::string desc_str(reinterpret_cast<const char*>(desc_data), desc_size);
				characteristic.SetUserDescription(desc_str);
			}
		} else if(Attribute::IsDescriptor(next_attr) || !next_attr.GetUuid().Is16Bit()) {
			std::vector<uint8_t> desc_value;
			const uint8_t* desc_data = next_attr.GetValueData();
			size_t desc_size = next_attr.GetValueSize();
			if(desc_data != nullptr && desc_size > 0) {
				desc_value.assign(desc_data, desc_data + desc_size);
			}

			characteristic.AddDescriptor(next_attr.GetUuid(),
										 static_cast<Attribute::Properties>(next_attr.GetProperties()),
										 desc_value,
										 next_attr.GetHandle());
		} else {
			break;
		}
	}

	return characteristic;
}

bool Characteristic::IsValid() const {
	return uuid_.IsValid() && value_attr_.GetHandle() != 0 && declaration_attr_.GetHandle() != 0;
}

bool Characteristic::IsThisCharacteristic(const Uuid& uuid) const {
	return uuid_ == uuid;
}

bool Characteristic::IsThisCharacteristic(uint16_t handle) const {
	// Check declaration and value attributes
	if(value_attr_.GetHandle() == handle || declaration_attr_.GetHandle() == handle) {
		return true;
	}

	// Check CCCD descriptor
	if(cccd_ && cccd_->GetHandle() == handle) {
		return true;
	}

	// Check SCCD descriptor
	if(sccd_ && sccd_->GetHandle() == handle) {
		return true;
	}

	// Check Extended Properties descriptor
	if(extended_properties_ && extended_properties_->GetHandle() == handle) {
		return true;
	}

	// Check User Description descriptor
	if(user_description_ && user_description_->GetHandle() == handle) {
		return true;
	}

	// Check custom descriptors
	for(const auto& descriptor: descriptors_) {
		if(descriptor.GetHandle() == handle) {
			return true;
		}
	}

	return false;
}

bool Characteristic::IsThisCharacteristic(const Uuid& uuid, uint16_t handle) const {
	return (uuid_ == uuid) && (IsThisCharacteristic(handle));
}

const uint8_t* Characteristic::GetValueData() const {
	return value_attr_.GetValueData();
}

size_t Characteristic::GetValueSize() const {
	return value_attr_.GetValueSize();
}

std::vector<uint8_t> Characteristic::GetValueAsVector() const {
	const uint8_t* data = GetValueData();
	size_t size = GetValueSize();
	if(data != nullptr) {
		return std::vector<uint8_t>(data, data + size);
	}
	return std::vector<uint8_t>{};
}

bool Characteristic::SetValue(const uint8_t* data, size_t size) {
	if(!value_attr_.SetValue(data, size)) {
		return false;
	}
	UpdateValue();
	return true;
}

bool Characteristic::SetValue(std::vector<uint8_t>&& data) {
	if(!value_attr_.SetValue(std::move(data))) {
		return false;
	}
	UpdateValue();
	return true;
}

bool Characteristic::SetValue(const std::vector<uint8_t>& data) {
	if(!value_attr_.SetValue(data)) {
		return false;
	}
	UpdateValue();
	return true;
}

void Characteristic::SetCharacteristicValueReadCallback(Attribute::ReadCallback callback) {
	value_attr_.SetReadCallback(std::move(callback));
}

void Characteristic::SetCharacteristicValueWriteCallback(Attribute::WriteCallback callback) {
	value_attr_.SetWriteCallback(std::move(callback));
}

bool Characteristic::HasReadCallback() const {
	return value_attr_.HasReadCallback();
}

bool Characteristic::HasWriteCallback() const {
	return value_attr_.HasWriteCallback();
}

bool Characteristic::IsNotificationsEnabled() const {
	if(!cccd_) {
		return false;
	}
	const uint8_t* data = cccd_->GetValueData();
	if(data == nullptr || cccd_->GetValueSize() < 2) {
		return false;
	}
	const uint16_t value = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
	return (value & static_cast<uint16_t>(CCCDProperties::kNotifications)) != 0;
}

bool Characteristic::IsIndicationsEnabled() const {
	if(!cccd_) {
		return false;
	}
	const uint8_t* data = cccd_->GetValueData();
	if(data == nullptr || cccd_->GetValueSize() < 2) {
		return false;
	}
	const uint16_t value = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
	return (value & static_cast<uint16_t>(CCCDProperties::kIndications)) != 0;
}

bool Characteristic::IsBroadcastEnabled() const {
	if(!sccd_) {
		return false;
	}
	const uint8_t* data = sccd_->GetValueData();
	if(data == nullptr || sccd_->GetValueSize() < 2) {
		return false;
	}
	const uint16_t value = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
	return (value & static_cast<uint16_t>(SCCDProperties::kBroadcasts)) != 0;
}

Attribute& Characteristic::EnableCCCD() {
	if(!cccd_) {
		// CCCD initial value: 0x0000 = notifications and indications disabled
		cccd_ = std::make_unique<Attribute>(
			Attribute::ClientCharacteristicConfiguration(0x0000, 0 /* handle assigned later */));
		cccd_->SetWriteCallback([this](uint16_t offset, const uint8_t* data, uint16_t size) {
			return HandleCccdWrite(offset, data, size);
		});
	}
	return *cccd_;
}

Attribute& Characteristic::SetCCCDValue(CCCDProperties config) {
	EnableCCCD();
	// Convert the 16-bit config value to little-endian bytes
	uint16_t config_value = static_cast<uint16_t>(config);
	std::vector<uint8_t> cccd_bytes = {
		static_cast<uint8_t>(config_value & 0xFF),		  // LSB
		static_cast<uint8_t>((config_value >> 8) & 0xFF)  // MSB
	};
	cccd_->SetValue(cccd_bytes);
	return *cccd_;
}

Attribute& Characteristic::EnableSCCD() {
	if(!sccd_) {
		// SCCD initial value: 0x0000 = broadcasts disabled
		sccd_ = std::make_unique<Attribute>(
			Attribute::ServerCharacteristicConfiguration(0x0000, 0 /* handle assigned later */));
		sccd_->SetWriteCallback([this](uint16_t offset, const uint8_t* data, uint16_t size) {
			return HandleSccdWrite(offset, data, size);
		});
	}
	return *sccd_;
}

Attribute& Characteristic::SetSCCDValue(SCCDProperties config) {
	EnableSCCD();
	// Convert the 16-bit config value to little-endian bytes
	uint16_t config_value = static_cast<uint16_t>(config);
	std::vector<uint8_t> sccd_bytes = {
		static_cast<uint8_t>(config_value & 0xFF),		  // LSB
		static_cast<uint8_t>((config_value >> 8) & 0xFF)  // MSB
	};
	sccd_->SetValue(sccd_bytes);
	return *sccd_;
}

Attribute& Characteristic::EnableExtendedProperties() {
	if(!extended_properties_) {
		// Extended Properties initial value: 0x0000 = no extended properties
		extended_properties_ = std::make_unique<Attribute>(
			Attribute::CharacteristicExtendedProperties(0x0000, 0 /* handle assigned later */));
	}
	return *extended_properties_;
}

Attribute& Characteristic::SetExtendedPropertiesValue(ExtendedProperties config) {
	EnableExtendedProperties();
	// Convert the 16-bit config value to little-endian bytes
	uint16_t config_value = static_cast<uint16_t>(config);
	std::vector<uint8_t> ext_prop_bytes = {
		static_cast<uint8_t>(config_value & 0xFF),		  // LSB
		static_cast<uint8_t>((config_value >> 8) & 0xFF)  // MSB
	};
	extended_properties_->SetStaticValue(ext_prop_bytes);
	return *extended_properties_;
}

Attribute& Characteristic::SetUserDescription(const std::string& description) {
	if(!user_description_) {
		user_description_ = std::make_unique<Attribute>(
			Attribute::CharacteristicUserDescription(description, 0 /* handle assigned later */));
	}
	// Update existing description (copy into static storage)
	std::vector<uint8_t> desc_data(description.begin(), description.end());
	user_description_->SetStaticValue(desc_data);
	return *user_description_;
}

Attribute& Characteristic::AddDescriptor(const Uuid& uuid,
										 Attribute::Properties properties,
										 const std::vector<uint8_t>& value,
										 uint16_t handle) {
	// Check if this is a User Description descriptor
	if(uuid.Is16Bit() &&
	   uuid.Get16Bit() ==
		   static_cast<uint16_t>(Uuid::AttributeType::kCharacteristicUserDescription)) {
		// Handle as User Description - convert bytes to string
		std::string description(value.begin(), value.end());
		Attribute& desc = SetUserDescription(description);
		if(handle != 0) {
			desc.SetHandle(handle);
		}
		return desc;
	}

	// Create descriptor with all attributes in one constructor call
	descriptors_.emplace_back(uuid, static_cast<uint16_t>(properties), handle);
	Attribute& descriptor = descriptors_.back();
	if((static_cast<uint16_t>(properties) &
		static_cast<uint16_t>(Attribute::Properties::kDynamic)) != 0) {
		descriptor.SetValue(value);
	} else {
		descriptor.SetStaticValue(value);
	}
	return descriptor;
}

Attribute* Characteristic::GetDescriptor(size_t index) {
	if(index >= descriptors_.size()) {
		return nullptr;
	}
	auto it = descriptors_.begin();
	std::advance(it, index);
	return &(*it);
}

const Attribute* Characteristic::GetDescriptor(size_t index) const {
	if(index >= descriptors_.size()) {
		return nullptr;
	}
	auto it = descriptors_.begin();
	std::advance(it, index);
	return &(*it);
}

std::ostream& operator<<(std::ostream& os, Characteristic::Properties props) {
	uint8_t value = static_cast<uint8_t>(props);

	if(value == 0) {
		os << "None";
		return os;
	}

	bool first = true;
	auto print_flag = [&](Characteristic::Properties flag, const char* name) {
		if((value & static_cast<uint8_t>(flag)) != 0) {
			if(!first)
				os << " | ";
			os << name;
			first = false;
		}
	};

	print_flag(Characteristic::Properties::kBroadcast, "Broadcast");
	print_flag(Characteristic::Properties::kRead, "Read");
	print_flag(Characteristic::Properties::kWriteWithoutResponse, "WriteWithoutResponse");
	print_flag(Characteristic::Properties::kWrite, "Write");
	print_flag(Characteristic::Properties::kNotify, "Notify");
	print_flag(Characteristic::Properties::kIndicate, "Indicate");
	print_flag(Characteristic::Properties::kAuthenticatedSignedWrites, "AuthenticatedSignedWrites");
	print_flag(Characteristic::Properties::kExtendedProperties, "ExtendedProperties");

	return os;
}

std::ostream& operator<<(std::ostream& os, const Characteristic& characteristic) {
	os << "Characteristic {" << "\n";
	os << "  UUID: " << characteristic.GetUuid() << "\n";
	os << "  Properties: " << characteristic.GetProperties() << "\n";
	os << "  Declaration Handle: 0x" << std::hex << characteristic.GetDeclarationHandle()
	   << std::dec << "\n";
	os << "  Value Handle: 0x" << std::hex << characteristic.GetValueHandle() << std::dec << "\n";

	// Print security requirements
	os << "  Security Requirements:" << "\n";
	os << "    Read Level: ";
	switch(characteristic.GetReadSecurityLevel()) {
		case Characteristic::SecurityLevel::kNone:
			os << "None (open access)";
			break;
		case Characteristic::SecurityLevel::kEncryptionRequired:
			os << "Encryption Required";
			break;
		case Characteristic::SecurityLevel::kAuthenticationRequired:
			os << "Authentication Required";
			break;
		case Characteristic::SecurityLevel::kAuthorizationRequired:
			os << "Authorization Required";
			break;
	}
	if(characteristic.ReadRequiresSC()) {
		os << " + Secure Connections";
	}
	os << "\n";

	os << "    Write Level: ";
	switch(characteristic.GetWriteSecurityLevel()) {
		case Characteristic::SecurityLevel::kNone:
			os << "None (open access)";
			break;
		case Characteristic::SecurityLevel::kEncryptionRequired:
			os << "Encryption Required";
			break;
		case Characteristic::SecurityLevel::kAuthenticationRequired:
			os << "Authentication Required";
			break;
		case Characteristic::SecurityLevel::kAuthorizationRequired:
			os << "Authorization Required";
			break;
	}
	if(characteristic.WriteRequiresSC()) {
		os << " + Secure Connections";
	}
	os << "\n";

	uint16_t enc_key = characteristic.GetEncryptionKeySize();
	if(enc_key > 0) {
		os << "    Encryption Key Size: " << static_cast<int>(enc_key) << " bytes" << "\n";
	}

	// Print value data
	const uint8_t* data = characteristic.GetValueData();
	size_t size = characteristic.GetValueSize();
	os << "  Value (" << size << " bytes): ";
	if(data != nullptr && size > 0) {
		os << "0x";
		for(size_t i = 0; i < size; ++i) {
			os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
			if(i < size - 1)
				os << " ";
		}
		os << std::dec;
		// Try to print as ASCII if printable
		bool all_printable = true;
		for(size_t i = 0; i < size; ++i) {
			if(data[i] < 32 || data[i] > 126) {
				all_printable = false;
				break;
			}
		}
		if(all_printable) {
			os << " (\"" << std::string(reinterpret_cast<const char*>(data), size) << "\")";
		}
	} else {
		os << "(empty)";
	}
	os << "\n";

	// Print CCCD if present
	const Attribute* cccd = characteristic.GetCCCD();
	if(cccd) {
		os << "  CCCD Handle: 0x" << std::hex << cccd->GetHandle() << std::dec;
		const uint8_t* cccd_data = cccd->GetValueData();
		if(cccd_data != nullptr && cccd->GetValueSize() >= 2) {
			uint16_t cccd_value = static_cast<uint16_t>(cccd_data[0]) |
								  (static_cast<uint16_t>(cccd_data[1]) << 8);
			os << ", Value: 0x" << std::hex << std::setfill('0') << std::setw(4) << cccd_value
			   << std::dec;
			if(cccd_value & static_cast<uint16_t>(Characteristic::CCCDProperties::kNotifications)) {
				os << " (Notifications Enabled)";
			}
			if(cccd_value & static_cast<uint16_t>(Characteristic::CCCDProperties::kIndications)) {
				os << " (Indications Enabled)";
			}
		}
		os << "\n";
	}

	// Print SCCD if present
	const Attribute* sccd = characteristic.GetSCCD();
	if(sccd) {
		os << "  SCCD Handle: 0x" << std::hex << sccd->GetHandle() << std::dec;
		const uint8_t* sccd_data = sccd->GetValueData();
		if(sccd_data != nullptr && sccd->GetValueSize() >= 2) {
			uint16_t sccd_value = static_cast<uint16_t>(sccd_data[0]) |
								  (static_cast<uint16_t>(sccd_data[1]) << 8);
			os << ", Value: 0x" << std::hex << std::setfill('0') << std::setw(4) << sccd_value
			   << std::dec;
			if(sccd_value & static_cast<uint16_t>(Characteristic::SCCDProperties::kBroadcasts)) {
				os << " (Broadcasts Enabled)";
			}
		}
		os << "\n";
	}

	// Print Extended Properties if present
	const Attribute* ext = characteristic.GetExtendedProperties();
	if(ext) {
		os << "  Extended Properties Handle: 0x" << std::hex << ext->GetHandle() << std::dec;
		const uint8_t* ext_data = ext->GetValueData();
		if(ext_data != nullptr && ext->GetValueSize() >= 2) {
			uint16_t ext_value = static_cast<uint16_t>(ext_data[0]) |
								 (static_cast<uint16_t>(ext_data[1]) << 8);
			os << ", Value: 0x" << std::hex << std::setfill('0') << std::setw(4) << ext_value
			   << std::dec;
			if(ext_value &
			   static_cast<uint16_t>(Characteristic::ExtendedProperties::kReliableWrite)) {
				os << " (Reliable Write)";
			}
			if(ext_value &
			   static_cast<uint16_t>(Characteristic::ExtendedProperties::kWritableAuxiliaries)) {
				os << " (Writable Auxiliaries)";
			}
		}
		os << "\n";
	}

	// Print User Description if present
	const Attribute* desc = characteristic.GetUserDescription();
	if(desc) {
		os << "  User Description Handle: 0x" << std::hex << desc->GetHandle() << std::dec;
		const uint8_t* desc_data = desc->GetValueData();
		if(desc_data != nullptr && desc->GetValueSize() > 0) {
			os << ", Value: \""
			   << std::string(reinterpret_cast<const char*>(desc_data), desc->GetValueSize())
			   << "\"";
		}
		os << "\n";
	}

	// Print custom descriptors
	size_t descriptor_count = characteristic.GetDescriptorCount();
	if(descriptor_count > 0) {
		os << "  Custom Descriptors (" << descriptor_count << "):" << "\n";
		for(size_t i = 0; i < descriptor_count; ++i) {
			const Attribute* desc = characteristic.GetDescriptor(i);
			if(desc) {
				os << "    [" << i << "] UUID: " << desc->GetUuid() << ", Handle: 0x" << std::hex
				   << desc->GetHandle() << std::dec;
				const uint8_t* desc_data = desc->GetValueData();
				size_t desc_size = desc->GetValueSize();
				if(desc_data != nullptr && desc_size > 0) {
					os << ", Value (" << desc_size << " bytes): 0x";
					for(size_t j = 0; j < desc_size; ++j) {
						os << std::hex << std::setfill('0') << std::setw(2)
						   << static_cast<int>(desc_data[j]);
						if(j < desc_size - 1)
							os << " ";
					}
					os << std::dec;
				}
				os << "\n";
			}
		}
	}

	os << "}";
	return os;
}
// ========== Event Handler Management ==========

void Characteristic::AddEventHandler(const EventHandlers& handler) {
	event_handlers_.push_back(&handler);
}

bool Characteristic::RemoveEventHandler(const EventHandlers& handler) {
	auto it = std::find(event_handlers_.begin(), event_handlers_.end(), &handler);
	if(it != event_handlers_.end()) {
		event_handlers_.erase(it);
		return true;
	}
	return false;
}

void Characteristic::ClearEventHandlers() {
	event_handlers_.clear();
}

// ========== Internal descriptor write handlers ==========

BleError Characteristic::HandleCccdWrite(uint16_t offset, const uint8_t* data, uint16_t size) {
	if(offset != 0 || data == nullptr || size != 2) {
		return BleError::kAttErrorInvalidAttrValueLength;
	}

	uint16_t old_config = 0;
	const uint8_t* current = cccd_ ? cccd_->GetValueData() : nullptr;
	if(current) {
		old_config = static_cast<uint16_t>(current[0]) | (static_cast<uint16_t>(current[1]) << 8);
	}

	uint16_t new_config = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
	std::vector<uint8_t> new_bytes{data[0], data[1]};
	cccd_->SetValue(std::move(new_bytes));

	bool old_notify = (old_config & static_cast<uint16_t>(CCCDProperties::kNotifications)) != 0;
	bool old_indicate = (old_config & static_cast<uint16_t>(CCCDProperties::kIndications)) != 0;
	bool new_notify = (new_config & static_cast<uint16_t>(CCCDProperties::kNotifications)) != 0;
	bool new_indicate = (new_config & static_cast<uint16_t>(CCCDProperties::kIndications)) != 0;

	if(new_notify && !old_notify) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnUpdatesEnabled) {
				handler->OnUpdatesEnabled(false);
			}
		}
	}
	if(new_indicate && !old_indicate) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnUpdatesEnabled) {
				handler->OnUpdatesEnabled(true);
			}
		}
	}
	if(!new_notify && old_notify) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnUpdatesDisabled) {
				handler->OnUpdatesDisabled();
			}
		}
	}
	if(!new_indicate && old_indicate) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnUpdatesDisabled) {
				handler->OnUpdatesDisabled();
			}
		}
	}

	return BleError::kSuccess;	// success
}

BleError Characteristic::HandleSccdWrite(uint16_t offset, const uint8_t* data, uint16_t size) {
	if(offset != 0 || data == nullptr || size != 2) {
		return BleError::kAttErrorInvalidAttrValueLength;
	}

	uint16_t old_config = 0;
	const uint8_t* current = sccd_ ? sccd_->GetValueData() : nullptr;
	if(current) {
		old_config = static_cast<uint16_t>(current[0]) | (static_cast<uint16_t>(current[1]) << 8);
	}

	uint16_t new_config = static_cast<uint16_t>(data[0]) | (static_cast<uint16_t>(data[1]) << 8);
	std::vector<uint8_t> new_bytes{data[0], data[1]};
	sccd_->SetValue(std::move(new_bytes));

	bool old_broadcast = (old_config & static_cast<uint16_t>(SCCDProperties::kBroadcasts)) != 0;
	bool new_broadcast = (new_config & static_cast<uint16_t>(SCCDProperties::kBroadcasts)) != 0;

	if(new_broadcast && !old_broadcast) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnBroadcastEnabled) {
				handler->OnBroadcastEnabled();
			}
		}
	}
	if(!new_broadcast && old_broadcast) {
		for(const auto* handler: event_handlers_) {
			if(handler && handler->OnBroadcastDisabled) {
				handler->OnBroadcastDisabled();
			}
		}
	}

	return BleError::kSuccess;	// success
}

uint16_t Characteristic::HandleValueRead(uint16_t offset, uint8_t* buffer, uint16_t buffer_size) {
	// Check if read is permitted on this characteristic
	if((static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kRead)) == 0) {
		return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
	}

	// Notify OnRead handlers that a read is happening
	for(const auto* handler: event_handlers_) {
		if(handler && handler->OnRead) {
			handler->OnRead();
		}
	}

	// Return current stored value
	const uint8_t* current_data = GetValueData();
	size_t current_size = GetValueSize();

	if(current_data != nullptr && current_size > 0) {
		if(offset >= current_size) {
			return 0;  // Offset beyond data size
		}

		uint16_t remaining = static_cast<uint16_t>(current_size - offset);
		uint16_t copy_size = std::min(remaining, buffer_size);

		if(copy_size > 0 && buffer != nullptr) {
			std::copy(current_data + offset, current_data + offset + copy_size, buffer);
		}
		return remaining;
	}

	return 0;
}

BleError Characteristic::HandleValueWrite(uint16_t offset, const uint8_t* data, uint16_t size) {
	// Check if write is permitted on this characteristic
	uint8_t props = static_cast<uint8_t>(properties_);
	if((props & static_cast<uint8_t>(Properties::kWrite)) == 0 &&
	   (props & static_cast<uint8_t>(Properties::kWriteWithoutResponse)) == 0 &&
	   (props & static_cast<uint8_t>(Properties::kAuthenticatedSignedWrites)) == 0) {
		return BleError::kAttErrorWriteNotPermitted;
	}

	if(offset != 0 || data == nullptr) {
		return BleError::kAttErrorInvalidAttrValueLength;
	}

	// Store the data in the value attribute
	if(!value_attr_.SetValue(data, size)) {
		return BleError::kAttErrorInvalidAttrValueLength;
	}

	// Create vector from the incoming data for notification
	std::vector<uint8_t> write_data(data, data + size);

	// Notify OnWrite handlers
	for(const auto* handler: event_handlers_) {
		if(handler && handler->OnWrite) {
			handler->OnWrite(write_data);
		}
	}

	return BleError::kSuccess;	// success
}

uint16_t Characteristic::HandleAttributeRead(uint16_t attribute_handle,
											 uint16_t offset,
											 uint8_t* buffer,
											 uint16_t buffer_size) {
	// Check declaration attribute
	if(declaration_attr_.GetHandle() == attribute_handle) {
		// Verify read permission
		if((declaration_attr_.GetProperties() &
			static_cast<uint16_t>(Attribute::Properties::kRead)) == 0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return declaration_attr_.InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check value attribute
	if(value_attr_.GetHandle() == attribute_handle) {
		// Verify read permission
		if((value_attr_.GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) ==
		   0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return value_attr_.InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check CCCD
	if(cccd_ && cccd_->GetHandle() == attribute_handle) {
		// Verify read permission
		if((cccd_->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) == 0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return cccd_->InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check SCCD
	if(sccd_ && sccd_->GetHandle() == attribute_handle) {
		// Verify read permission
		if((sccd_->GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) == 0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return sccd_->InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check Extended Properties
	if(extended_properties_ && extended_properties_->GetHandle() == attribute_handle) {
		// Verify read permission
		if((extended_properties_->GetProperties() &
			static_cast<uint16_t>(Attribute::Properties::kRead)) == 0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return extended_properties_->InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check User Description
	if(user_description_ && user_description_->GetHandle() == attribute_handle) {
		// Verify read permission
		if((user_description_->GetProperties() &
			static_cast<uint16_t>(Attribute::Properties::kRead)) == 0) {
			return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
		}
		return user_description_->InvokeReadCallback(offset, buffer, buffer_size);
	}

	// Check custom descriptors
	for(auto& descriptor: descriptors_) {
		if(descriptor.GetHandle() == attribute_handle) {
			// Verify read permission
			if((descriptor.GetProperties() & static_cast<uint16_t>(Attribute::Properties::kRead)) ==
			   0) {
				return static_cast<uint16_t>(BleError::kAttErrorReadNotPermitted);
			}
			return descriptor.InvokeReadCallback(offset, buffer, buffer_size);
		}
	}

	return 0;
}

BleError Characteristic::HandleAttributeWrite(uint16_t attribute_handle,
											  uint16_t offset,
											  const uint8_t* data,
											  uint16_t size) {
	// Check value attribute
	if(value_attr_.GetHandle() == attribute_handle) {
		// Verify write permission
		uint16_t props = value_attr_.GetProperties();
		if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) == 0) {
			return BleError::kAttErrorWriteNotPermitted;
		}
		return value_attr_.InvokeWriteCallback(offset, data, size);
	}

	// Check CCCD
	if(cccd_ && cccd_->GetHandle() == attribute_handle) {
		// Verify write permission
		uint16_t props = cccd_->GetProperties();
		if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) == 0) {
			return BleError::kAttErrorWriteNotPermitted;
		}
		return cccd_->InvokeWriteCallback(offset, data, size);
	}

	// Check SCCD
	if(sccd_ && sccd_->GetHandle() == attribute_handle) {
		// Verify write permission
		uint16_t props = sccd_->GetProperties();
		if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) == 0) {
			return BleError::kAttErrorWriteNotPermitted;
		}
		return sccd_->InvokeWriteCallback(offset, data, size);
	}

	// Check Extended Properties
	if(extended_properties_ && extended_properties_->GetHandle() == attribute_handle) {
		// Verify write permission
		uint16_t props = extended_properties_->GetProperties();
		if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) == 0) {
			return BleError::kAttErrorWriteNotPermitted;
		}
		return extended_properties_->InvokeWriteCallback(offset, data, size);
	}

	// Check User Description
	if(user_description_ && user_description_->GetHandle() == attribute_handle) {
		// Verify write permission
		uint16_t props = user_description_->GetProperties();
		if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
		   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) == 0) {
			return BleError::kAttErrorWriteNotPermitted;
		}
		return user_description_->InvokeWriteCallback(offset, data, size);
	}

	// Check custom descriptors
	for(auto& descriptor: descriptors_) {
		if(descriptor.GetHandle() == attribute_handle) {
			// Verify write permission
			uint16_t props = descriptor.GetProperties();
			if((props & static_cast<uint16_t>(Attribute::Properties::kWrite)) == 0 &&
			   (props & static_cast<uint16_t>(Attribute::Properties::kWriteWithoutResponse)) == 0 &&
			   (props & static_cast<uint16_t>(Attribute::Properties::kAuthenticatedSignedWrite)) ==
				   0) {
				return BleError::kAttErrorWriteNotPermitted;
			}
			return descriptor.InvokeWriteCallback(offset, data, size);
		}
	}

	return BleError::kSuccess;
}

// ========== Security Level Setters ==========

void Characteristic::SetReadSecurityLevel(SecurityLevel level) {
	uint16_t props = value_attr_.GetProperties();

	// Clear existing read permission bits (bits 10 and 11)
	props &= ~static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit0);
	props &= ~static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit1);

	// Set new read permission bits based on security level
	uint8_t level_val = static_cast<uint8_t>(level);
	if(level_val & 0x01) {	// Bit 0 of security level
		props |= static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit0);
	}
	if(level_val & 0x02) {	// Bit 1 of security level
		props |= static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit1);
	}

	value_attr_.SetProperties(props);
}

void Characteristic::SetWriteSecurityLevel(SecurityLevel level) {
	uint16_t props = value_attr_.GetProperties();

	// Clear existing write permission bits (bits 0 and 4)
	props &= ~static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit0);
	props &= ~static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit1);

	// Set new write permission bits based on security level
	uint8_t level_val = static_cast<uint8_t>(level);
	if(level_val & 0x01) {	// Bit 0 of security level
		props |= static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit0);
	}
	if(level_val & 0x02) {	// Bit 1 of security level
		props |= static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit1);
	}

	value_attr_.SetProperties(props);
}

void Characteristic::SetReadRequiresSecureConnections(bool required) {
	uint16_t props = value_attr_.GetProperties();

	if(required) {
		props |= static_cast<uint16_t>(Attribute::Properties::kReadPermissionSc);
	} else {
		props &= ~static_cast<uint16_t>(Attribute::Properties::kReadPermissionSc);
	}

	value_attr_.SetProperties(props);
}

void Characteristic::SetWriteRequiresSecureConnections(bool required) {
	uint16_t props = value_attr_.GetProperties();

	if(required) {
		props |= static_cast<uint16_t>(Attribute::Properties::kWritePermissionSc);
	} else {
		props &= ~static_cast<uint16_t>(Attribute::Properties::kWritePermissionSc);
	}

	value_attr_.SetProperties(props);
}

void Characteristic::SetEncryptionKeySize(uint8_t key_size) {
	uint16_t props = value_attr_.GetProperties();

	// Clear existing encryption key size bits (bits 12-15)
	props &= ~static_cast<uint16_t>(Attribute::Properties::kEncryptionKeySizeMask);

	// Set new encryption key size (clamped to 4 bits: 0-15)
	props |= (static_cast<uint16_t>(key_size & 0x0F) << 12);

	value_attr_.SetProperties(props);
}

Characteristic::SecurityLevel Characteristic::GetReadSecurityLevel() const {
	uint16_t props = value_attr_.GetProperties();

	// Extract read permission bits (bits 10 and 11) and shift to LSB positions
	uint8_t bit0 = (props & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit0)) ? 1
																							   : 0;
	uint8_t bit1 = (props & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit1)) ? 2
																							   : 0;

	return static_cast<SecurityLevel>(bit0 | bit1);
}

Characteristic::SecurityLevel Characteristic::GetWriteSecurityLevel() const {
	uint16_t props = value_attr_.GetProperties();

	// Extract write permission bits (bits 0 and 4) and combine to 2-bit value
	uint8_t bit0 = (props & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit0)) ? 1
																								: 0;
	uint8_t bit1 = (props & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit1)) ? 2
																								: 0;

	return static_cast<SecurityLevel>(bit0 | bit1);
}

// ========== Permission Checking Functions ==========

bool Characteristic::IsReadPermitted(bool authorized, bool authenticated) const {
	SecurityLevel required = GetReadSecurityLevel();

	switch(required) {
		case SecurityLevel::kNone:
			// No security required
			return true;

		case SecurityLevel::kEncryptionRequired:
			// Requires encrypted connection (authenticated=true means encryption established)
			return authenticated;

		case SecurityLevel::kAuthenticationRequired:
			// Requires authenticated pairing with encryption
			return authenticated;

		case SecurityLevel::kAuthorizationRequired:
			// Requires both authentication and application-level authorization
			return authenticated && authorized;

		default:
			return false;
	}
}

bool Characteristic::IsWritePermitted(bool authorized, bool authenticated) const {
	SecurityLevel required = GetWriteSecurityLevel();

	switch(required) {
		case SecurityLevel::kNone:
			// No security required
			return true;

		case SecurityLevel::kEncryptionRequired:
			// Requires encrypted connection
			return authenticated;

		case SecurityLevel::kAuthenticationRequired:
			// Requires authenticated pairing with encryption
			return authenticated;

		case SecurityLevel::kAuthorizationRequired:
			// Requires both authentication and application-level authorization
			return authenticated && authorized;

		default:
			return false;
	}
}

}  // namespace c7222
