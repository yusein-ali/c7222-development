#include "pwm.hpp"

#include <cassert>

namespace c7222 {

PwmOut::Config::Config(uint32_t pin) {
	pin_ = pin;
}

bool PwmOut::Config::Validate() const {
	if(period_us <= 0.0f) {
		return false;
	}
	if(duty_cycle < 0.0f || duty_cycle > 1.0f) {
		return false;
	}
	return true;
}

PwmOut::PwmOut(uint32_t pin) : PwmOut(pin, Config(pin)) {}

void PwmOut::SetActiveLow(bool active_low) {
	config_.active_low = active_low;
	ApplyConfig();
}

} // namespace c7222
