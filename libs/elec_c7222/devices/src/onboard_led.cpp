// Simulated environment stub for BoardLED.
#include "onboard_led.hpp"

namespace c7222 {

OnBoardLED* OnBoardLED::instance_{nullptr};

OnBoardLED* OnBoardLED::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new OnBoardLED();
	}
	return instance_;
}

OnBoardLED::OnBoardLED() : initialized_(false), state_(false) {}

}  // namespace c7222
