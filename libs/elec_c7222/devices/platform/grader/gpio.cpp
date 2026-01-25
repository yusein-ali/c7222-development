// Simulated environment stub implementation.
#include "gpio.hpp"
#include <cassert>

namespace c7222 {

GpioPin::GpioPin(uint32_t pin, const Config& config) : _pin(pin), _config(config) {}

void GpioPin::Configure(const Config& config) {
	assert(config.validate() && "Invalid GPIO configuration");
	_config = config;
}


void GpioPin::Write(bool) {}

bool GpioPin::Read() const {
	return false;
}

void GpioPin::Toggle() {}

void GpioPin::ApplyConfig() {}

} // namespace c7222
