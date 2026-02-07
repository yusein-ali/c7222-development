// Raspberry Pi Pico implementation of generic GPIO.
#include "gpio.hpp"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <cassert>

namespace c7222 {

static gpio_drive_strength to_drive_strength(GpioPin::DriveStrength drive) {
	switch(drive) {
	case GpioPin::DriveStrength::mA2:
		return GPIO_DRIVE_STRENGTH_2MA;
	case GpioPin::DriveStrength::mA4:
		return GPIO_DRIVE_STRENGTH_4MA;
	case GpioPin::DriveStrength::mA8:
		return GPIO_DRIVE_STRENGTH_8MA;
	case GpioPin::DriveStrength::mA12:
		return GPIO_DRIVE_STRENGTH_12MA;
	}
	return GPIO_DRIVE_STRENGTH_4MA;
}

GpioPin::GpioPin(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	ApplyConfig();
}

void GpioPin::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}


void GpioPin::Write(bool value) {
	assert(config_.direction == Direction::Output && "Cannot write to GPIO configured as input");

	if(config_.output_type == OutputType::OpenDrain) {
		if(value) {
			// Float the line for logic high in open-drain mode.
			gpio_set_dir(pin_, GPIO_IN);
		} else {
			gpio_put(pin_, 0);
			gpio_set_dir(pin_, GPIO_OUT);
		}
	} else {
		gpio_set_dir(pin_, GPIO_OUT);
		gpio_put(pin_, value);
	}
}

bool GpioPin::Read() const {
	return gpio_get(pin_);
}

void GpioPin::Toggle() {
	assert(config_.direction == Direction::Output && "Cannot toggle GPIO configured as input");

	gpio_xor_mask(1 << config_.pin_);
}

void GpioPin::ApplyConfig() {
	gpio_init(pin_);

	gpio_set_drive_strength(pin_, to_drive_strength(config_.drive));

	switch(config_.pull) {
	case PullMode::None:
		gpio_disable_pulls(pin_);
		break;
	case PullMode::PullUp:
		gpio_pull_up(pin_);
		break;
	case PullMode::PullDown:
		gpio_pull_down(pin_);
		break;
	}

	if(config_.direction == Direction::Output) {
		if(config_.output_type == OutputType::OpenDrain) {
			if(config_.initial_state) {
				gpio_set_dir(pin_, GPIO_IN);
			} else {
				gpio_put(pin_, 0);
				gpio_set_dir(pin_, GPIO_OUT);
			}
		} else {
			gpio_set_dir(pin_, GPIO_OUT);
			gpio_put(pin_, config_.initial_state);
		}
	} else {
		gpio_set_dir(pin_, GPIO_IN);
	}
}

} // namespace c7222
