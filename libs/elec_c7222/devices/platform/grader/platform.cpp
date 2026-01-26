// Grader platform implementation (no hardware init).
#include "platform.hpp"

namespace c7222 {

bool Platform::EnsureArchInitialized() {
	arch_initialized_ = true;
	return true;
}

bool Platform::Initialize() {
	if (initialized_) {
		return true;
	}
	if (!EnsureArchInitialized()) {
		initialized_ = false;
		return false;
	}
	const bool led_ok = OnBoardLED::GetInstance().Initialize();
	const bool temp_ok = OnChipTemperatureSensor::GetInstance().Initialize();
	initialized_ = led_ok && temp_ok;
	return initialized_;
}

} // namespace c7222
