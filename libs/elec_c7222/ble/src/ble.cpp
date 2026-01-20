#include "ble.hpp"

#include <cassert>

namespace c7222 {

void Ble::SetDeviceName(const std::string& name) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(AdvertisementData(AdvertisementDataType::kCompleteLocalName,
										   name.c_str(),
										   name.size()));
	bool ok = builder.Build();
	assert(ok &&
		   "AdvertisementDataBuilder contains invalid data after setting device name.");
	device_name_ = name;
}

void Ble::SetAdvertisementFlags(uint8_t flags) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(AdvertisementData(AdvertisementDataType::kFlags,
										   &flags,
										   sizeof(flags)));
	bool ok = builder.Build();
	assert(ok &&
		   "AdvertisementDataBuilder contains invalid data after setting flags.");
	advertisement_flags_ = flags;
}

AttributeServer* Ble::EnableAttributeServer(const void* context) {
	if(attribute_server_ != nullptr) {
		return attribute_server_;
	}
	auto* attribute_server = GetAttributeServer();
	assert(attribute_server != nullptr &&
		   "AttributeServer instance is null in Ble::EnableAttributeServer");
	if(attribute_server_ == nullptr) {
		attribute_server->Init(context);
		attribute_server_ = attribute_server;
	}
	return attribute_server_;
}

}  // namespace c7222
