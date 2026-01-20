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
	}
	return attribute_server_;
}

}  // namespace c7222
