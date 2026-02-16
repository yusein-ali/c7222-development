// Simulated environment stub for BoardLED.
#include "onboard_led.hpp"

namespace c7222 {
extern "C" {
void c7222_grader_set_onboard_led_state(bool on);
}

bool OnBoardLED::Initialize() {
	if(initialized_) {
		return true;
	}
	initialized_ = true;
	state_ = false;
	c7222_grader_set_onboard_led_state(false);
	return true;
}

void OnBoardLED::Set(bool on) {
	state_ = on;
	c7222_grader_set_onboard_led_state(on);
}

void OnBoardLED::On() { Set(true); }	
void OnBoardLED::Off() { Set(false); }

void OnBoardLED::Toggle() { Set(!state_); }

} // namespace c7222
