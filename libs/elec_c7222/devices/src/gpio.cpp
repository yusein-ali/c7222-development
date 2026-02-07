#include "gpio.hpp"

#include <cassert>

namespace c7222 {

GpioPin::Config::Config(int32_t pin) {
	assert(pin >= 0);
	pin_ = static_cast<uint32_t>(pin);
}

bool GpioPin::Config::Validate() const {
	if(output_type == OutputType::OpenDrain && pull == PullMode::None) {
		return false;
	}
	return true;
}

GpioPin::GpioPin(uint32_t pin) : GpioPin(pin, Config(static_cast<int32_t>(pin))) {}

uint32_t GpioPin::GetPin() const {
	return pin_;
}

GpioPin::Config GpioPin::GetConfig() const {
	return config_;
}

} 
