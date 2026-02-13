// Simulated environment stub implementation for PWM output.
#include "pwm.hpp"

#include <cassert>
#include <map>

namespace c7222 {
namespace {
static std::map<uint32_t, PwmOut*> pwm_out_to_object_map{};
} // namespace

PwmOut::PwmOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid PWM configuration");
	auto it = pwm_out_to_object_map.find(GetPin());
	assert(it == pwm_out_to_object_map.end() && "GPIO pin already in use by another PwmOut instance");
	pwm_out_to_object_map[GetPin()] = this;
	if(config.enabled) {
		ApplyConfig();
	}
}

PwmOut::~PwmOut() {
	pwm_out_to_object_map.erase(GetPin());
}

void PwmOut::Configure(const Config& config) {
	assert(config.Validate() && "Invalid PWM configuration");
	config_ = config;
	ApplyConfig();
}

void PwmOut::SetPeriodUs(float period_us) {
	config_.period_us = period_us;
	ApplyConfig();
}

void PwmOut::SetDutyCycle(float duty_cycle) {
	config_.duty_cycle = duty_cycle;
	ApplyConfig();
}

void PwmOut::Enable(bool on) {
	config_.enabled = on;
	ApplyConfig();
}

void PwmOut::ApplyConfig() {
	assert(config_.Validate() && "Invalid PWM configuration");
}

} // namespace c7222
