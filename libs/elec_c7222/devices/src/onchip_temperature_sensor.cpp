// Grader stub for on-chip temperature sensor.
#include "onchip_temperature_sensor.hpp"

#include <cassert>

namespace c7222 {

OnChipTemperatureSensor* OnChipTemperatureSensor::instance_{nullptr};

OnChipTemperatureSensor* OnChipTemperatureSensor::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new OnChipTemperatureSensor();
		assert(instance_ != nullptr && "Cannot allocate new OnChipTemperatureSensor instance!");
	}
	return instance_;
}

OnChipTemperatureSensor::OnChipTemperatureSensor() : initialized_(false) {}

}  // namespace c7222
