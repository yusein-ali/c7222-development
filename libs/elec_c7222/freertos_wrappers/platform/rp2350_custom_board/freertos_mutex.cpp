#include "freertos_mutex.hpp"

#include "FreeRTOS.h"
#include "semphr.h"

namespace c7222 {

FreeRtosMutex::FreeRtosMutex() {
	(void)Initialize();
}

bool FreeRtosMutex::Initialize() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xSemaphoreCreateMutex();
	return handle_ != nullptr;
}

FreeRtosMutex::~FreeRtosMutex() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosMutex::Lock(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreTake(static_cast<SemaphoreHandle_t>(handle_),
						  static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosMutex::TryLock() {
	return Lock(0);
}

bool FreeRtosMutex::Unlock() {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreGive(static_cast<SemaphoreHandle_t>(handle_)) == pdTRUE;
}

bool FreeRtosMutex::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosRecursiveMutex::FreeRtosRecursiveMutex() {
	(void)Initialize();
}

bool FreeRtosRecursiveMutex::Initialize() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xSemaphoreCreateRecursiveMutex();
	return handle_ != nullptr;
}

FreeRtosRecursiveMutex::~FreeRtosRecursiveMutex() {
	if(handle_ != nullptr) {
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosRecursiveMutex::Lock(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreTakeRecursive(static_cast<SemaphoreHandle_t>(handle_),
								   static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosRecursiveMutex::TryLock() {
	return Lock(0);
}

bool FreeRtosRecursiveMutex::Unlock() {
	if(handle_ == nullptr) {
		return false;
	}
	return xSemaphoreGiveRecursive(static_cast<SemaphoreHandle_t>(handle_)) == pdTRUE;
}

bool FreeRtosRecursiveMutex::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
