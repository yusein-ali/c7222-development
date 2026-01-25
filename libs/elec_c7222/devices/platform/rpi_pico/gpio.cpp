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

GpioPin::GpioPin(uint32_t pin, const Config& config) : _pin(pin), _config(config) {
	assert(_config.validate() && "Invalid GPIO configuration");
	ApplyConfig();
}

void GpioPin::Configure(const Config& config) {
	assert(config.validate() && "Invalid GPIO configuration");
	_config = config;
	ApplyConfig();
}


void GpioPin::Write(bool value) {
	assert(_config.direction == Direction::Output && "Cannot write to GPIO configured as input");

	if(_config.output_type == OutputType::OpenDrain) {
		if(value) {
			// Float the line for logic high in open-drain mode.
			gpio_set_dir(_pin, GPIO_IN);
		} else {
			gpio_put(_pin, 0);
			gpio_set_dir(_pin, GPIO_OUT);
		}
	} else {
		gpio_set_dir(_pin, GPIO_OUT);
		gpio_put(_pin, value);
	}
}

bool GpioPin::Read() const {
	return gpio_get(_pin);
}

void GpioPin::Toggle() {
	Write(!Read());
}

void GpioPin::ApplyConfig() {
	gpio_init(_pin);

	gpio_set_drive_strength(_pin, to_drive_strength(_config.drive));

	switch(_config.pull) {
	case PullMode::None:
		gpio_disable_pulls(_pin);
		break;
	case PullMode::PullUp:
		gpio_pull_up(_pin);
		break;
	case PullMode::PullDown:
		gpio_pull_down(_pin);
		break;
	}

	if(_config.direction == Direction::Output) {
		if(_config.output_type == OutputType::OpenDrain) {
			if(_config.initial_state) {
				gpio_set_dir(_pin, GPIO_IN);
			} else {
				gpio_put(_pin, 0);
				gpio_set_dir(_pin, GPIO_OUT);
			}
		} else {
			gpio_set_dir(_pin, GPIO_OUT);
			gpio_put(_pin, _config.initial_state);
		}
	} else {
		gpio_set_dir(_pin, GPIO_IN);
	}
}

} // namespace c7222
