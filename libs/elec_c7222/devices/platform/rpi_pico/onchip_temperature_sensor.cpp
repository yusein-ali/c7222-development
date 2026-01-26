// Raspberry Pi Pico implementation of on-chip temperature sensor.
#include "onchip_temperature_sensor.hpp"

#include "hardware/adc.h"


namespace c7222 {
namespace {

constexpr float kAdcReferenceVoltage = 3.3f;
constexpr float kAdcMaxValue = 4096.0f; // 12-bit ADC
constexpr float kVoltageAt27C = 0.706f;
constexpr float kVoltageSlope = 0.001721f; // Volts per degree C
constexpr uint kTempSensorAdcInput = 4;

float adc_to_celsius(uint16_t adc_value) {
	const float voltage = (static_cast<float>(adc_value) * kAdcReferenceVoltage) / kAdcMaxValue;
	return 27.0f - (voltage - kVoltageAt27C) / kVoltageSlope;
}

} // namespace


bool OnChipTemperatureSensor::Initialize() {
	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(kTempSensorAdcInput);
	initialized_ = true;
	return true;
}

float OnChipTemperatureSensor::GetCelsius() const {
	assert(initialized_ && "Temperature sensor not initialized");
	adc_select_input(kTempSensorAdcInput);
	const uint16_t raw = adc_read();
	return adc_to_celsius(raw);
}

float OnChipTemperatureSensor::GetFahrenheit() const {
	const float celsius = GetCelsius();
	return (celsius * 9.0f / 5.0f) + 32.0f;
}

} // namespace c7222
