#include "freertos_event_group.hpp"

#include "FreeRTOS.h"
#include "event_groups.h"

namespace c7222 {

FreeRtosEventGroup::FreeRtosEventGroup() {
	(void)Initialize();
}

bool FreeRtosEventGroup::Initialize() {
	if(handle_ != nullptr) {
		vEventGroupDelete(static_cast<EventGroupHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xEventGroupCreate();
	return handle_ != nullptr;
}

FreeRtosEventGroup::~FreeRtosEventGroup() {
	if(handle_ != nullptr) {
		vEventGroupDelete(static_cast<EventGroupHandle_t>(handle_));
		handle_ = nullptr;
	}
}

std::uint32_t FreeRtosEventGroup::SetBits(std::uint32_t bits) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(
		xEventGroupSetBits(static_cast<EventGroupHandle_t>(handle_), static_cast<EventBits_t>(bits)));
}

bool FreeRtosEventGroup::SetBitsFromISR(std::uint32_t bits) {
	if(handle_ == nullptr) {
		return false;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xEventGroupSetBitsFromISR(static_cast<EventGroupHandle_t>(handle_),
									 static_cast<EventBits_t>(bits),
									 &higher_priority_task_woken) == pdPASS;
}

std::uint32_t FreeRtosEventGroup::ClearBits(std::uint32_t bits) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(
		xEventGroupClearBits(static_cast<EventGroupHandle_t>(handle_), static_cast<EventBits_t>(bits)));
}

std::uint32_t FreeRtosEventGroup::WaitBits(std::uint32_t bits_to_wait_for,
										   bool clear_on_exit,
										   bool wait_for_all_bits,
										   std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(xEventGroupWaitBits(static_cast<EventGroupHandle_t>(handle_),
														  static_cast<EventBits_t>(bits_to_wait_for),
														  clear_on_exit ? pdTRUE : pdFALSE,
														  wait_for_all_bits ? pdTRUE : pdFALSE,
														  static_cast<TickType_t>(ticks_to_wait)));
}

std::uint32_t FreeRtosEventGroup::GetBits() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(xEventGroupGetBits(static_cast<EventGroupHandle_t>(handle_)));
}

std::uint32_t FreeRtosEventGroup::GetBitsFromISR() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::uint32_t>(xEventGroupGetBitsFromISR(static_cast<EventGroupHandle_t>(handle_)));
}

bool FreeRtosEventGroup::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
