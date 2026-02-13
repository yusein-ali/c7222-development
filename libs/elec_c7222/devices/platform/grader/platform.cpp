// Grader platform implementation (no hardware init).
#include "platform.hpp"

#include <cstdint>

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


} // namespace c7222
