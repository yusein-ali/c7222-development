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
	while(depth_ > 0) {
		GetGlobalCriticalMutex().unlock();
		--depth_;
	}
}

void FreeRtosCriticalSection::Enter() {
	GetGlobalCriticalMutex().lock();
	++depth_;
}

bool FreeRtosCriticalSection::Exit() {
	if(depth_ == 0) {
		return false;
	}
	GetGlobalCriticalMutex().unlock();
	--depth_;
	return true;
}

bool FreeRtosCriticalSection::IsEntered() const {
	return depth_ > 0;
}

} // namespace c7222
