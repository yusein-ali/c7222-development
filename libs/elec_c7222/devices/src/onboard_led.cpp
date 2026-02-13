// Simulated environment stub for BoardLED.
#include "onboard_led.hpp"

#include <memory>

namespace c7222 {

std::unique_ptr<OnBoardLED> OnBoardLED::instance_{nullptr};

OnBoardLED* OnBoardLED::GetInstance() {
	if(!instance_) {
		instance_ = std::unique_ptr<OnBoardLED>(new OnBoardLED());
	}
	return instance_.get();
}

OnBoardLED::OnBoardLED() : initialized_(false), state_(false) {}

}  // namespace c7222
