#include "gpio.hpp"

#include <cassert>

namespace c7222 {


bool GpioIn::Config::Validate() const {
	if(input_events != GpioInputEvent::None && irq_handler == nullptr) {
		return false;
	}
	if(input_events == GpioInputEvent::None && irq_handler != nullptr) {
		return false;
	}
	return true;
}

bool GpioIn::Config::operator==(const Config& other) const {
	return pin_ == other.pin_ &&
		   pull == other.pull &&
		   input_events == other.input_events;
}



void GpioIn::CallIrqHandler(uint32_t events) const {
	uint32_t event_mask = static_cast<uint32_t>(config_.input_events);
	if(config_.irq_handler && (events & event_mask)) {
		config_.irq_handler(events);
	}
}

GpioOut::Config::Config(uint32_t pin) {
	assert(pin >= 0);
	pin_ = static_cast<uint32_t>(pin);
}

bool GpioOut::Config::Validate() const {
	return true;
}

bool GpioOut::Config::operator==(const Config& other) const {
	return pin_ == other.pin_ &&
		   pull == other.pull &&
		   output_type == other.output_type &&
		   drive == other.drive &&
		   initial_state == other.initial_state;
}



}  // namespace c7222
