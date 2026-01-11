// Simulated environment stub for BoardLED.
#include "board_led.hpp"

namespace c7222 {

BoardLED::BoardLED() = default;

bool BoardLED::init() {
	_initialized = true;
	_state = false;
	return true;
}

void BoardLED::set(bool on) {
	_state = on;
}

void BoardLED::on() { set(true); }

void BoardLED::off() { set(false); }

void BoardLED::toggle() { set(!_state); }

} // namespace c7222
