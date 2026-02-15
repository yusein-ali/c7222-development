#include "freertos_semaphore.hpp"

#include "FreeRTOS.h"
#include "semphr.h"

namespace c7222 {

FreeRtosBinarySemaphore::FreeRtosBinarySemaphore(bool initially_given) {
	(void)Initialize(initially_given);
}

bool FreeRtosBinarySemaphore::Initialize(bool initially_given) {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xSemaphoreCreateBinary();
	if(handle_ == nullptr) {
		return false;
	}
	if(initially_given) {
		(void)xSemaphoreGive(static_cast<SemaphoreHandle_t>(handle_));
	}
	return true;
}

FreeRtosBinarySemaphore::~FreeRtosBinarySemaphore() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosBinarySemaphore::Take(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_),
						  static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosBinarySemaphore::Give() {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreGive(static_cast<SemaphoreHandle_t>(handle_)) == pdTRUE;
}

bool FreeRtosBinarySemaphore::GiveFromISR() {
	if(handle_ == nullptr) {
		return false;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xSemaphoreGiveFromISR(static_cast<SemaphoreHandle_t>(handle_),
								 &higher_priority_task_woken) == pdTRUE;
}

bool FreeRtosBinarySemaphore::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosCountingSemaphore::FreeRtosCountingSemaphore(std::uint32_t max_count, std::uint32_t initial_count) {
	(void)Initialize(max_count, initial_count);
}

bool FreeRtosCountingSemaphore::Initialize(std::uint32_t max_count, std::uint32_t initial_count) {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xSemaphoreCreateCounting(static_cast<UBaseType_t>(max_count),
									   static_cast<UBaseType_t>(initial_count));
	return handle_ != nullptr;
}

FreeRtosCountingSemaphore::~FreeRtosCountingSemaphore() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosCountingSemaphore::Take(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_),
						  static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosCountingSemaphore::Give() {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreGive(static_cast<SemaphoreHandle_t>(handle_)) == pdTRUE;
}

bool FreeRtosCountingSemaphore::GiveFromISR() {
	if(handle_ == nullptr) {
		return false;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xSemaphoreGiveFromISR(static_cast<SemaphoreHandle_t>(handle_),
								 &higher_priority_task_woken) == pdTRUE;
}

std::uint32_t FreeRtosCountingSemaphore::GetCount() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(uxSemaphoreGetCount(static_cast<SemaphoreHandle_t>(handle_)));
}

bool FreeRtosCountingSemaphore::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
