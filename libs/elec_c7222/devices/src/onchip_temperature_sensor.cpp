// Grader stub for on-chip temperature sensor.
#include "onchip_temperature_sensor.hpp"

#include <memory>

namespace c7222 {

std::unique_ptr<OnChipTemperatureSensor> OnChipTemperatureSensor::instance_{nullptr};

OnChipTemperatureSensor* OnChipTemperatureSensor::GetInstance() {
	if(!instance_) {
		instance_ = std::unique_ptr<OnChipTemperatureSensor>(new OnChipTemperatureSensor());
	}
	return instance_.get();
}

OnChipTemperatureSensor::OnChipTemperatureSensor() : initialized_(false) {}

}  // namespace c7222
