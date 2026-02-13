// Raspberry Pi Pico implementation of generic GPIO.
#include "gpio.hpp"
#include <hardware/sync.h>
#include <pico/types.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <cassert>
#include <map>

namespace c7222 {
namespace{
// Route GPIO interrupts to Gpio In instances (keyed by GPIO number).
static std::map<uint32_t, GpioIn*> gpio_in_to_object_map{};
static std::map<uint32_t, GpioOut*> gpio_out_to_object_map{};

// Pico SDK callback (single global callback) that forwards to GpioIn instances.
static void pico_gpio_irq_callback(uint gpio, uint32_t events) {
	auto it = gpio_in_to_object_map.find(static_cast<uint32_t>(gpio));
	if(it == gpio_in_to_object_map.end()) {
		return;
	}

	GpioIn* gpio_in = it->second;
	if(gpio_in != nullptr) {
		gpio_in->CallIrqHandler(events);
	}
}

static gpio_drive_strength to_drive_strength(GpioDriveStrength drive) {
	switch(drive) {
	case GpioDriveStrength::mA2:
		return GPIO_DRIVE_STRENGTH_2MA;
	case GpioDriveStrength::mA4:
		return GPIO_DRIVE_STRENGTH_4MA;
	case GpioDriveStrength::mA8:
		return GPIO_DRIVE_STRENGTH_8MA;
	case GpioDriveStrength::mA12:
		return GPIO_DRIVE_STRENGTH_12MA;
	}
	return GPIO_DRIVE_STRENGTH_4MA;
}
}

GpioIn::GpioIn(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(pin_ < NUM_BANK0_GPIOS && "Invalid GPIO pin number");
	assert(config_.Validate() && "Invalid GPIO configuration");
	auto it = gpio_in_to_object_map.find(GetPin());
	assert(it == gpio_in_to_object_map.end() && "GPIO pin already in use by another GpioIn instance");
	gpio_in_to_object_map[GetPin()] = this;
	ApplyConfig();
}

GpioIn::~GpioIn() {
	gpio_in_to_object_map.erase(GetPin());
}

void GpioIn::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}

bool GpioIn::Read() const {
	return gpio_get(pin_);
}

void GpioIn::ApplyConfig() {
	gpio_init(pin_);

	switch(config_.pull) {
	case GpioPullMode::None:
		gpio_disable_pulls(pin_);
		break;
	case GpioPullMode::PullUp:
		gpio_pull_up(pin_);
		break;
	case GpioPullMode::PullDown:
		gpio_pull_down(pin_);
		break;
	}

	gpio_set_dir(pin_, GPIO_IN);

	// if irq function if configured, we must enable irqs 
	if(HasHandler() && config_.input_events != GpioInputEvent::None) {
		EnableIrq(config_.input_events, config_.irq_handler);
	}	
}

void GpioIn::EnableIrq(GpioInputEvent events, std::function<void(uint32_t)> handler) {
	assert(events != GpioInputEvent::None && "At least one event must be specified to enable IRQ");
	assert(handler != nullptr && "Handler must be provided to enable IRQ");
	
	uint32_t irq_state = save_and_disable_interrupts();
	config_.input_events = static_cast<GpioInputEvent>(events);
	config_.irq_handler = std::move(handler);
	
	gpio_set_irq_enabled_with_callback(GetPin(),
									   static_cast<uint32_t>(events),
									   true,
									   pico_gpio_irq_callback);
	restore_interrupts(irq_state);
}

void GpioIn::DisableIrq() {
	if(!HasHandler()) {
		return;
	}
	uint32_t irq_state = save_and_disable_interrupts();

	// check if we have any other handlers for this pin, if not we can disable the IRQ for this pin
	gpio_set_irq_enabled_with_callback(GetPin(),
									   static_cast<uint32_t>(config_.input_events),
									   false,
									   nullptr);
	config_.input_events = GpioInputEvent::None;
	config_.irq_handler = nullptr;
	restore_interrupts(irq_state);
}

GpioOut::GpioOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(pin_ < NUM_BANK0_GPIOS && "Invalid GPIO pin number");
	assert(config_.Validate() && "Invalid GPIO configuration");
	auto it = gpio_out_to_object_map.find(GetPin());
	assert(it == gpio_out_to_object_map.end() && "GPIO pin already in use by another GpioOut instance");
	gpio_out_to_object_map[GetPin()] = this;
	ApplyConfig();
}

GpioOut::~GpioOut() {
	gpio_out_to_object_map.erase(GetPin());
}

void GpioOut::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}

void GpioOut::Write(bool value) {
	if(config_.output_type == GpioOutputType::OpenDrain) {
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

void GpioOut::Toggle() {
	gpio_xor_mask(1 << pin_);
}

void GpioOut::ApplyConfig() {
	gpio_init(pin_);

	gpio_set_drive_strength(pin_, to_drive_strength(config_.drive));

	switch(config_.pull) {
	case GpioPullMode::None:
		gpio_disable_pulls(pin_);
		break;
	case GpioPullMode::PullUp:
		gpio_pull_up(pin_);
		break;
	case GpioPullMode::PullDown:
		gpio_pull_down(pin_);
		break;
	}

	if(config_.output_type == GpioOutputType::OpenDrain) {
		// Pico SDK has no native open-drain mode. Emulate it by:
		// - driving low: output enabled, pin forced low
		// - releasing high: switch to input (high-Z), external/internal pull-up brings line high
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
}

} // namespace c7222
