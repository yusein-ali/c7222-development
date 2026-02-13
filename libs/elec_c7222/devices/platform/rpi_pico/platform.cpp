// Raspberry Pi Pico platform implementation.
#include "platform.hpp"

#include <cstdint>

#include "pico/cyw43_arch.h"

#include "c7222_pico_w_board.hpp"



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
	initialized_ = true;
	return initialized_;
}




} // namespace c7222
