// Raspberry Pi Pico platform implementation.
#include "platform.hpp"

#include "pico/cyw43_arch.h"

namespace c7222 {


bool Platform::EnsureArchInitialized() {
#if defined(CYW43_WL_GPIO_LED_PIN)
	if (arch_initialized_) {
		return true;
	}
	if (cyw43_arch_init() != 0) {
		arch_initialized_ = false;
		return false;
	}
	arch_initialized_ = true;
	return true;
#else
	arch_initialized_ = true;
	return true;
#endif
}

bool Platform::Initialize() {
	if (initialized_) {
		return true;
	}
	if (!EnsureArchInitialized()) {
		initialized_ = false;
		return false;
	}
	const bool led_ok = OnBoardLED::GetInstance()->Initialize();
	const bool temp_ok = OnChipTemperatureSensor::GetInstance()->Initialize();
	initialized_ = led_ok && temp_ok;
	return initialized_;
}

} // namespace c7222
