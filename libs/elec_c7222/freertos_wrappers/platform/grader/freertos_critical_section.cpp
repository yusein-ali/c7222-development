#include "freertos_critical_section.hpp"

#include <mutex>

namespace c7222 {
namespace {

std::recursive_mutex& GetGlobalCriticalMutex() {
	static std::recursive_mutex critical_mutex;
	return critical_mutex;
}

} // namespace

FreeRtosCriticalSection::~FreeRtosCriticalSection() {
	if(depth_ != 0) {
		GetGlobalCriticalMutex().unlock();
		depth_ = 0;
	}
}

void FreeRtosCriticalSection::Enter() {
	if(depth_ != 0) {
		return;
	}
	GetGlobalCriticalMutex().lock();
	depth_ = 1;
}

bool FreeRtosCriticalSection::Exit() {
	if(depth_ == 0U) {
		return false;
	}
	GetGlobalCriticalMutex().unlock();
	depth_ = 0;
	return true;
}

bool FreeRtosCriticalSection::IsEntered() const {
	return depth_ != 0;
}

} // namespace c7222
