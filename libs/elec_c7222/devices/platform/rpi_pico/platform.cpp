// Raspberry Pi Pico platform implementation.
#include "platform.hpp"

#include <cstdint>
#include <chrono>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"

#include "c7222_pico_w_board.hpp"



namespace c7222 {


bool Platform::EnsureArchInitialized() {
#if defined(CYW43_WL_GPIO_LED_PIN)
	if (arch_initialized_) {
		return true;
	}
	stdio_init_all();
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

void Platform::SleepMs(uint32_t ms) {
	sleep_ms(ms);
}

void Platform::SleepUs(uint64_t us) {
	sleep_us(us);
}

void Platform::SleepUntil(std::chrono::steady_clock::time_point target) {
	const auto now = std::chrono::steady_clock::now();
	if (target <= now) {
		return;
	}
	const auto delta = target - now;
	const auto us = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
	sleep_until(delayed_by_us(get_absolute_time(), static_cast<uint64_t>(us)));
}

void Platform::TightLoopContents() {
	tight_loop_contents();
}




} // namespace c7222
