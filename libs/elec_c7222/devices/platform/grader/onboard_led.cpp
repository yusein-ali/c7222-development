// Simulated environment stub for BoardLED.
#include "board_led.hpp"

namespace c7222 {

OnBoardLED& OnBoardLED::GetInstance() {
	static OnBoardLED instance;
	return instance;
}

OnBoardLED::OnBoardLED() = default;

bool OnBoardLED::Initialize() {
	_initialized = true;
	_state = false;
	return true;
}

void OnBoardLED::Set(bool on) {
	_state = on;
}

void OnBoardLED::On() { Set(true); }	
void OnBoardLED::Off() { Set(false); }

void OnBoardLED::Toggle() { Set(!_state); }

} // namespace c7222
