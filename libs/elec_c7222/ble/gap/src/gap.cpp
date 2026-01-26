#include "gap.hpp"

namespace c7222 {

Gap* Gap::instance_ = nullptr;


Gap* Gap::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new Gap();
	}
	return instance_;
}

void Gap::SetAdvertisingData(const AdvertisementDataBuilder& data_builder) {
	if(&data_builder == &advertisement_data_builder_) {
		// Avoid copying from self
		advertisement_data_builder_.Build();
		return;
	}
	advertisement_data_builder_ = data_builder;
	bool ok = advertisement_data_builder_.Build();
	assert(ok && "AdvertisementDataBuilder contains invalid data or is empty");
	SetAdvertisingData(advertisement_data_builder_.data());
}

bool Gap::GetConnectionParameters(ConnectionHandle con_handle, ConnectionParameters& out) const {
	const auto it = connection_parameters_.find(con_handle);
	if(it == connection_parameters_.end()) {
		return false;
	}
	out = it->second;
	return true;
}


bool Gap::GetAdvertisingParameters(AdvertisementParameters& params) const {
	if(!advertising_params_set_) {
		return false;
	}
	params = advertising_params_;
	return true;
}

bool Gap::GetRandomAddress(BleAddress& address) const {
	if(!random_address_set_) {
		return false;
	}
	address = random_address_;
	return true;
}


}