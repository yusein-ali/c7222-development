// Grader platform implementation (no hardware init).
#include "platform.hpp"

#include <cstdint>
#include <chrono>
#include <thread>

namespace c7222 {

bool Platform::EnsureArchInitialized() {
	arch_initialized_ = true;
	return true;
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
	(void)ms;
}

void Platform::SleepUs(uint64_t us) {
	(void)us;
}

void Platform::SleepUntil(std::chrono::steady_clock::time_point target) {
	std::this_thread::sleep_until(target);
}

void Platform::TightLoopContents() {
}


} // namespace c7222
