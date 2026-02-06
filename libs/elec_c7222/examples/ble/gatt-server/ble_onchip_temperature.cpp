//
// Created by husey on 2/5/2026.
//

#include <cassert>
#include <cstdio>

#include "ble_onchip_temperature.hpp"

BleOnchipTemperature* BleOnchipTemperature::instance_ = nullptr;

BleOnchipTemperature::BleOnchipTemperature()
	: temperature_characteristic_(nullptr),
	  configuration_characteristic_(nullptr) {}

BleOnchipTemperature*
BleOnchipTemperature::GetInstance(c7222::Characteristic* temperature_characteristic,
								  c7222::Characteristic* configuration_characteristic) {
	assert(temperature_characteristic != nullptr && "Temperature characteristic cannot be null");
	assert(configuration_characteristic != nullptr &&
		   "Configuration characteristic cannot be null");

	if(instance_ == nullptr) {
		instance_ = new BleOnchipTemperature();
		assert(instance_ && "BleOnchipTemperature instance cannot be allocated!");
	}

	if(instance_->temperature_characteristic_ != nullptr ||
	   instance_->configuration_characteristic_ != nullptr) {
		std::printf(
			"[TEMP] Warning: BleOnchipTemperature instance already has characteristics set. "
			"Overwriting with new characteristics.\n");
	}

	instance_->temperature_characteristic_ = temperature_characteristic;
	instance_->configuration_characteristic_ = configuration_characteristic;

	if(temperature_characteristic != nullptr) {
		const auto& temp_name = temperature_characteristic->GetUserDescriptionText();
		std::string char_name = temp_name.empty() ? "Temperature" : temp_name;
		instance_->temperature_event_handler_.SetCharacteristicName(char_name);
		temperature_characteristic->AddEventHandler(instance_->temperature_event_handler_);
	}
	if(configuration_characteristic != nullptr) {
		const auto& config_name = configuration_characteristic->GetUserDescriptionText();
		std::string char_name = config_name.empty() ? "Configuration" : config_name;
		instance_->configuration_event_handler_.SetCharacteristicName(char_name);
		configuration_characteristic->AddEventHandler(instance_->configuration_event_handler_);
	}

	return instance_;
}
