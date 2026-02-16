// Grader stub for on-chip temperature sensor.
#include "onchip_temperature_sensor.hpp"

#include <cmath>
#include <limits>

namespace c7222 {
extern "C" {
void c7222_grader_set_temperature(float celsius);
bool c7222_grader_get_temperature_set(float* celsius_out);
}


bool OnChipTemperatureSensor::Initialize() {
	if(initialized_) {
		return true;
	}
	initialized_ = true;
	float celsius = std::numeric_limits<float>::quiet_NaN();
	c7222_grader_get_temperature_set(&celsius);
	if(std::isnan(celsius)) {
		c7222_grader_set_temperature(25.0f);
	}
	return true;
}

float OnChipTemperatureSensor::GetCelsius() const {
	if(!initialized_) {
		return std::numeric_limits<float>::quiet_NaN();
	}
	float celsius = std::numeric_limits<float>::quiet_NaN();
	c7222_grader_get_temperature_set(&celsius);
	return celsius;
}

float OnChipTemperatureSensor::GetFahrenheit() const {
	const float celsius = GetCelsius();
	if(std::isnan(celsius)) {
		return std::numeric_limits<float>::quiet_NaN();
	}
	return (celsius * 9.0f / 5.0f) + 32.0f;
}

} // namespace c7222
