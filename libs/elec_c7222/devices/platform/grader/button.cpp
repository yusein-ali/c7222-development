// Simulated environment stub implementation of Button.
#include "button.hpp"

namespace c7222 {

void Button::UnregisterHandler() {
	handler_ = nullptr;
}

void Button::EnableIrq(bool) {}

void Button::PlatformRegister(Button*) {
	// No IRQ support in grader backend.
}

void Button::PlatformUnregister(Button*) {
	// No IRQ support in grader backend.
}

} // namespace c7222
