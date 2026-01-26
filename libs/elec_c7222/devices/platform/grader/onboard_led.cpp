// Simulated environment stub for BoardLED.
#include "onboard_led.hpp"

namespace c7222 {

bool OnBoardLED::Initialize() {
	initialized_ = true;
	state_ = false;
	return true;
}

void OnBoardLED::Set(bool on) {
	state_ = on;
}

void OnBoardLED::On() { Set(true); }	
void OnBoardLED::Off() { Set(false); }

void OnBoardLED::Toggle() { Set(!state_); }

} // namespace c7222
