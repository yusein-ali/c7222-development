// Raspberry Pi Pico implementation of PWM output.
#include "pwm.hpp"

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include <cassert>
#include <cmath>
#include <map>

namespace c7222 {
namespace {
static std::map<uint32_t, PwmOut*> pwm_out_to_object_map{};

static void compute_pwm_params(float period_us, uint16_t* top, float* divider) {
	const uint32_t sys_hz = clock_get_hz(clk_sys);
	const double period_s = static_cast<double>(period_us) * 1e-6;
	double counts = period_s * static_cast<double>(sys_hz);
	if(counts < 2.0) {
		counts = 2.0;
	}

	double div = std::ceil(counts / 65536.0);
	if(div < 1.0) {
		div = 1.0;
	}
	if(div > 255.0) {
		div = 255.0;
	}

	double top_f = (counts / div) - 1.0;
	if(top_f < 1.0) {
		top_f = 1.0;
	}
	if(top_f > 65535.0) {
		top_f = 65535.0;
	}

	*top = static_cast<uint16_t>(top_f);
	*divider = static_cast<float>(div);
}
} // namespace

PwmOut::PwmOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(pin_ < NUM_BANK0_GPIOS && "Invalid GPIO pin number");
	assert(config_.Validate() && "Invalid PWM configuration");
	auto it = pwm_out_to_object_map.find(GetPin());
	assert(it == pwm_out_to_object_map.end() && "GPIO pin already in use by another PwmOut instance");
	pwm_out_to_object_map[GetPin()] = this;
	if(config.enabled) {
		ApplyConfig();
	} 
}

PwmOut::~PwmOut() {
	// Return pin to default GPIO function on destruction.
	gpio_set_function(pin_, GPIO_FUNC_SIO);
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
	if(on) {
		ApplyConfig();
	} else {
		pwm_set_enabled(pwm_gpio_to_slice_num(pin_), false);
		gpio_set_function(pin_, GPIO_FUNC_SIO);
	}
}

void PwmOut::ApplyConfig() {
	assert(config_.Validate() && "Invalid PWM configuration");

	gpio_set_function(pin_, GPIO_FUNC_PWM);
	const uint slice = pwm_gpio_to_slice_num(pin_);
	const uint channel = pwm_gpio_to_channel(pin_);

	uint16_t top = 0;
	float divider = 1.0f;
	compute_pwm_params(config_.period_us, &top, &divider);

	pwm_config cfg = pwm_get_default_config();
	pwm_config_set_wrap(&cfg, top);
	pwm_config_set_clkdiv(&cfg, divider);
	pwm_init(slice, &cfg, config_.enabled);

	if(channel == PWM_CHAN_A) {
		pwm_set_output_polarity(slice, config_.active_low, false);
	} else {
		pwm_set_output_polarity(slice, false, config_.active_low);
	}

	float duty = config_.duty_cycle;
	if(duty < 0.0f) {
		duty = 0.0f;
	} else if(duty > 1.0f) {
		duty = 1.0f;
	}
	const uint16_t level = static_cast<uint16_t>(duty * static_cast<float>(top + 1));
	pwm_set_chan_level(slice, channel, level);
}

} // namespace c7222
