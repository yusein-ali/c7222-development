// Raspberry Pi Pico implementation of BoardLED.
#include "onboard_led.hpp"

#include "platform.hpp"

#include "pico/cyw43_arch.h"
namespace c7222 {


bool OnBoardLED::Initialize() {
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
	initialized_ = true;
	state_ = false;
	return true;
#elif defined(CYW43_WL_GPIO_LED_PIN)
	if (!Platform::GetInstance()->EnsureArchInitialized()) {
		initialized_ = false;
		return false;
	}
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	initialized_ = true;
	state_ = false;
	return true;
#else
	initialized_ = false;
	return false;
#endif
}

void OnBoardLED::Set(bool on) {
	if(!initialized_) {
		return;
	}
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_put(PICO_DEFAULT_LED_PIN, on ? 1 : 0);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on ? 1 : 0);
#endif
	state_ = on;
}

void OnBoardLED::On() {
	Set(true);
}

void OnBoardLED::Off() {
	Set(false);
}

void OnBoardLED::Toggle() {
	Set(!state_);
}
}  // namespace c7222
