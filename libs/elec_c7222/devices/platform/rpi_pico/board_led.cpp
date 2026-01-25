// Raspberry Pi Pico implementation of BoardLED.
#include "board_led.hpp"

#include "pico/stdlib.h"

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

namespace c7222 {

BoardLED::BoardLED() = default;

bool BoardLED::Initialize() {
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
	_initialized = true;
	_state = false;
	return true;
#elif defined(CYW43_WL_GPIO_LED_PIN)
	if (cyw43_arch_init() != 0) {
		_initialized = false;
		return false;
	}
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	_initialized = true;
	_state = false;
	return true;
#else
	_initialized = false;
	return false;
#endif
}

void BoardLED::Set(bool on) {
	if (!_initialized) {
		return;
	}
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_put(PICO_DEFAULT_LED_PIN, on ? 1 : 0);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
#endif
	_state = on;
}

void BoardLED::On() { Set(true); }

void BoardLED::Off() { Set(false); }

void BoardLED::Toggle() { Set(!_state); }
} // namespace c7222
