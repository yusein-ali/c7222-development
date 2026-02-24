#include "freertos_critical_section.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace c7222 {

FreeRtosCriticalSection::~FreeRtosCriticalSection() = default;

void FreeRtosCriticalSection::Enter() {
	taskENTER_CRITICAL();
}

bool FreeRtosCriticalSection::Exit() {
	taskEXIT_CRITICAL();
	return true;
}

bool FreeRtosCriticalSection::IsEntered() const {
	return false;
}

} // namespace c7222
