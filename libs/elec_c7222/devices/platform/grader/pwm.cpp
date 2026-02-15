// Simulated environment stub implementation for PWM output.
#include "pwm.hpp"

#include <cassert>

namespace c7222 {
extern "C" {
bool c7222_grader_register_pwm_out(uint32_t pin, PwmOut* pwm_out);
void c7222_grader_unregister_pwm_out(uint32_t pin);
void c7222_grader_apply_pwm_config(uint32_t pin, const PwmOut::Config* config);
}

PwmOut::PwmOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid PWM configuration");
	const bool registered = c7222_grader_register_pwm_out(GetPin(), this);
	assert(registered && "GPIO pin already in use by another PwmOut instance");
	if(config.enabled) {
		ApplyConfig();
	}
}

PwmOut::~PwmOut() {
	c7222_grader_unregister_pwm_out(GetPin());
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
	c7222_grader_apply_pwm_config(GetPin(), &config_);
}

} // namespace c7222
