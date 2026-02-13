// Platform-agnostic Button implementation.
#include "button.hpp"

namespace c7222 {

Button::Button(uint32_t pin, GpioPullMode pull)
	: GpioIn(pin){

	Config cfg(pin);
	cfg.pull = pull;
	GpioIn::Configure(cfg);
}

void Button::Reconfigure(GpioPullMode pull) {
	Config cfg(GetPin());
	cfg.pull = pull;
	GpioIn::Configure(cfg);
}


bool Button::IsPressed() const {
	// Active-low by default when using pull-up.
	return config_.pull == GpioPullMode::PullUp ? !Read() : Read();
}


} // namespace c7222
