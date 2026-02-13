// Platform-agnostic LED implementation.
#include "led.hpp"

namespace c7222 {

Led::Led(uint32_t pin,
		 bool initial_on,
		 GpioDriveStrength drive,
		 bool active_low)
	: GpioOut(pin),
	  _active_low(active_low) {
	Config cfg(static_cast<int32_t>(pin));
	cfg.pull = GpioPullMode::None;
	cfg.output_type = GpioOutputType::PushPull;
	cfg.drive = drive;
	cfg.initial_state = active_low ? !initial_on : initial_on;
	GpioOut::Configure(cfg);
}

void Led::Reconfigure(bool initial_on, GpioDriveStrength drive, bool active_low) {
	_active_low = active_low;
	Config cfg(static_cast<int32_t>(GetPin()));
	cfg.pull = GpioPullMode::None;
	cfg.output_type = GpioOutputType::PushPull;
	cfg.drive = drive;
	cfg.initial_state = active_low ? !initial_on : initial_on;
	GpioOut::Configure(cfg);
}
void Led::Set(bool on) {
	Write(_active_low ? !on : on);
}
void Led::On() {
	Write(_active_low ? false : true);
}

void Led::Off() {
	Write(_active_low ? true : false);
}

void Led::Toggle() {
	GpioOut::Toggle();
}

} // namespace c7222
