// Platform-agnostic Button implementation.
#include "button.hpp"

namespace c7222 {

Button::Button(uint32_t pin, PullMode pull)
	: GpioPin(pin)
{

	Config cfg(pin);
	cfg.direction = Direction::Input;
	cfg.pull = pull;
	cfg.output_type = OutputType::PushPull;
	cfg.drive = DriveStrength::mA4;
	cfg.initial_state = false;
	GpioPin::Configure(cfg);
}

Button::~Button() {
	UnregisterHandler();
	PlatformUnregister(this);
}

bool Button::IsPressed() const {
	// Active-low by default when using pull-up.
	return !Read();
}

void Button::RegisterHandler(const std::function<void(uint32_t events)>& handler) {
	
	if(handler != nullptr && handler_ != nullptr) {
		handler_ = handler;
		return;
	} else if(handler == nullptr && handler_ == nullptr) {
		return;
	} else if(handler != nullptr && handler_ == nullptr) {
		handler_ = handler;
		EnableIrq(true);
		return;
	} else if(handler == nullptr && handler_ != nullptr) {
		EnableIrq(false);
		handler_ = nullptr;
		return;
	}
}

bool Button::operator==(const Button& other) const {
	return GetPin() == other.GetPin();
}

bool Button::operator!=(const Button& other) const {
	return !(*this == other);
}


void Button::CallIrqHandler(uint32_t events) const {
	if(handler_) {
		handler_(events);
	}
}

bool Button::HasHandler() const {
	return static_cast<bool>(handler_);
}

} // namespace c7222
