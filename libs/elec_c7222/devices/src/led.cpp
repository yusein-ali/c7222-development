// Platform-agnostic LED implementation.
#include "led.hpp"

namespace c7222 {

Led::Led(uint32_t pin,
		 bool initial_on,
		 DriveStrength drive,
		 bool active_low)
	: GpioPin(pin),
	  _active_low(active_low) {
	Config cfg(static_cast<int32_t>(pin));
	cfg.direction = Direction::Output;
	cfg.pull = PullMode::None;
	cfg.output_type = OutputType::PushPull;
	cfg.drive = drive;
	cfg.initial_state = active_low ? !initial_on : initial_on;
	GpioPin::Configure(cfg);
}

void Led::Set(bool on) {
	Write(PhysicalLevel(on));
}

void Led::On() {
	Write(PhysicalLevel(true));
}

void Led::Off() {
	Write(PhysicalLevel(false));
}

void Led::Toggle() {
	GpioPin::Toggle();
}

bool Led::PhysicalLevel(bool led_on) const {
	// active_low: LED on => GPIO low
	return _active_low ? !led_on : led_on;
}

} // namespace c7222
