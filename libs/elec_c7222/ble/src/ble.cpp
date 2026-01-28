#include "ble.hpp"

#include <cassert>

namespace c7222 {

Ble* Ble::instance_ = nullptr;

Ble* Ble::GetInstance(bool enable_hci_logging) {
	if(instance_ == nullptr) {
		instance_ = new Ble();
	}
	assert(instance_ != nullptr && "Failed to allocate Ble singleton instance");
	if(enable_hci_logging) {
		instance_->EnableHCILoggingToStdout();
	}
	return instance_;
}

void Ble::SetDeviceName(const std::string& name) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(
		AdvertisementData(AdvertisementDataType::kCompleteLocalName, name.c_str(), name.size()));
	bool ok = builder.Build();
	assert(ok && "AdvertisementDataBuilder contains invalid data after setting device name.");
	device_name_ = name;
}

void Ble::SetAdvertisementFlags(uint8_t flags) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(AdvertisementData(AdvertisementDataType::kFlags, &flags, sizeof(flags)));
	bool ok = builder.Build();
	assert(ok && "AdvertisementDataBuilder contains invalid data after setting flags.");
	advertisement_flags_ = flags;
}

AttributeServer* Ble::EnableAttributeServer(const void* context) {
	if(attribute_server_ != nullptr) {
		return attribute_server_;
	}
	if(attribute_server_ == nullptr) {
		attribute_server_ = AttributeServer::GetInstance();
		attribute_server_->Init(context);

		const bool requires_encryption = attribute_server_->HasServicesRequiringEncryption();
		const bool requires_authentication =
			attribute_server_->HasServicesRequiringAuthentication();
		const bool requires_authorization =
			attribute_server_->HasServicesRequiringAuthorization();

		if(requires_encryption || requires_authentication || requires_authorization) {
			assert(security_manager_ != nullptr &&
				   "SecurityManager is required: AttributeServer contains secured characteristics. "
				   "Call Ble::EnableSecurityManager() before enabling the attribute server.");
		}

		if(security_manager_ != nullptr &&
		   (requires_encryption || requires_authentication || requires_authorization)) {
			assert(security_manager_->IsConfigured() &&
				   "SecurityManager must be configured before enabling a secured AttributeServer.");
			const auto handler_count = security_manager_->GetEventHandlerCount();
			assert(handler_count > 0 && "SecurityManager must have at least one event handler registered.");
			assert(security_manager_->ValidateConfiguration(requires_authentication,
															requires_authorization,
															requires_encryption) &&
				   "SecurityManager configuration mismatch: requirements cannot be satisfied "
				   "with current SecurityManager settings.");
		}
	}
	return attribute_server_;
}

SecurityManager* Ble::EnableSecurityManager(const SecurityManager::SecurityParameters& params) {
	if(security_manager_ == nullptr) {
		security_manager_ = SecurityManager::GetInstance();
		security_manager_->Configure(params);
		if(turned_on_) {
			EnsureSmEventHandlerRegistered();
		}
	}
	if(attribute_server_ != nullptr) {
		const bool requires_encryption = attribute_server_->HasServicesRequiringEncryption();
		const bool requires_authentication =
			attribute_server_->HasServicesRequiringAuthentication();
		const bool requires_authorization =
			attribute_server_->HasServicesRequiringAuthorization();
		assert(security_manager_->ValidateConfiguration(requires_authentication,
														requires_authorization,
														requires_encryption) &&
			   "SecurityManager configuration mismatch: requirements cannot be satisfied "
			   "with current SecurityManager settings.");
	}
	return security_manager_;
}

}  // namespace c7222
