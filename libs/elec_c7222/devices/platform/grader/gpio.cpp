// Simulated environment stub implementation.
#include "gpio.hpp"
#include <cassert>

namespace c7222 {

GpioPin::GpioPin(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	ApplyConfig();
}

void GpioPin::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}


void GpioPin::Write(bool) {}

bool GpioPin::Read() const {
	return false;
}

void GpioPin::Toggle() {
	
}

void GpioPin::ApplyConfig() {}

} // namespace c7222
