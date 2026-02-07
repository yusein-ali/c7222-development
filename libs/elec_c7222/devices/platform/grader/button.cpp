// Simulated environment stub implementation of Button.
#include "button.hpp"

namespace c7222 {

Button::Config Button::MakeConfig(uint32_t pin, PullMode pull) {
	Config cfg(static_cast<int32_t>(pin));
	cfg.direction = Direction::Input;
	cfg.pull = pull;
	cfg.output_type = OutputType::PushPull;
	cfg.drive = DriveStrength::mA4;
	cfg.initial_state = false;
	return cfg;
}

Button::Button(uint32_t pin, PullMode pull)
	: GpioPin(pin, MakeConfig(pin, pull)) {}

void Button::RegisterHandler(Handler handler, void* user_data, bool) {
	_handler = handler;
	_user_data = user_data;
}

void Button::UnregisterHandler() {
	_handler = nullptr;
	_user_data = nullptr;
}

void Button::EnableIrq(bool) {}

bool Button::IsPressed() const {
	return !Read();
}

void Button::InvokeHandlerFromIrq() {
	if(_handler != nullptr) {
		_handler(_user_data);
	}
}

void Button::IrqTrampoline(uint32_t, uint32_t) {
	// No IRQ support in grader backend.
}

} // namespace c7222
