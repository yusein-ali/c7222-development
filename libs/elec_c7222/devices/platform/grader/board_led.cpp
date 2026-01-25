// Simulated environment stub for BoardLED.
#include "board_led.hpp"

namespace c7222 {

BoardLED::BoardLED() = default;

bool BoardLED::Initialize() {
	_initialized = true;
	_state = false;
	return true;
}

void BoardLED::Set(bool on) {
	_state = on;
}

void BoardLED::On() { Set(true); }	
void BoardLED::Off() { Set(false); }

void BoardLED::Toggle() { Set(!_state); }

} // namespace c7222
