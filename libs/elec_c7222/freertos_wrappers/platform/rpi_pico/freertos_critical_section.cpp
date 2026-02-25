#include "freertos_critical_section.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace c7222 {

FreeRtosCriticalSection::~FreeRtosCriticalSection() {
	(void)Exit();
}

void FreeRtosCriticalSection::Enter() {
	if(depth_ != 0) {
		return;
	}
	taskENTER_CRITICAL();
	depth_ = 1;
}

bool FreeRtosCriticalSection::Exit() {
	if(depth_ == 0) {
		return false;
	}
	taskEXIT_CRITICAL();
	depth_ = 0;
	return true;
}

bool FreeRtosCriticalSection::IsEntered() const {
	return depth_ != 0;
}

} // namespace c7222
