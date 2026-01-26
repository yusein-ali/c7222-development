// Grader stub for on-chip temperature sensor.
#include "onchip_temperature_sensor.hpp"

#include <limits>

namespace c7222 {


bool OnChipTemperatureSensor::Initialize() {
	if(initialized_) {
		return true;
	}
	initialized_ = true;
	return true;
}

float OnChipTemperatureSensor::GetCelsius() const {
	return std::numeric_limits<float>::quiet_NaN();
}

float OnChipTemperatureSensor::GetFahrenheit() const {
	return std::numeric_limits<float>::quiet_NaN();
}

} // namespace c7222
