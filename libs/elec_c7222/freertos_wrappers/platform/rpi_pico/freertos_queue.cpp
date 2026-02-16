#include "freertos_queue.hpp"

#include "FreeRTOS.h"
#include "queue.h"

namespace c7222 {

FreeRtosQueue::FreeRtosQueue(std::size_t length, std::size_t item_size) {
	(void)Initialize(length, item_size);
}

bool FreeRtosQueue::Initialize(std::size_t length, std::size_t item_size) {
	if(handle_ != nullptr) {
		vQueueDelete(static_cast<QueueHandle_t>(handle_));
		handle_ = nullptr;
	}
	length_ = length;
	item_size_ = item_size;
	handle_ = xQueueCreate(static_cast<UBaseType_t>(length), static_cast<UBaseType_t>(item_size));
	return handle_ != nullptr;
}

FreeRtosQueue::~FreeRtosQueue() {
	if(handle_ != nullptr) {
		vQueueDelete(static_cast<QueueHandle_t>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosQueue::Send(const void* item, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xQueueSend(static_cast<QueueHandle_t>(handle_),
					  item,
					  static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosQueue::SendFromISR(const void* item) {
	if(handle_ == nullptr) {
		return false;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xQueueSendFromISR(static_cast<QueueHandle_t>(handle_),
							 item,
							 &higher_priority_task_woken) == pdTRUE;
}

bool FreeRtosQueue::Receive(void* out_item, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xQueueReceive(static_cast<QueueHandle_t>(handle_),
						 out_item,
						 static_cast<TickType_t>(ticks_to_wait)) == pdTRUE;
}

bool FreeRtosQueue::ReceiveFromISR(void* out_item) {
	if(handle_ == nullptr) {
		return false;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xQueueReceiveFromISR(static_cast<QueueHandle_t>(handle_),
								out_item,
								&higher_priority_task_woken) == pdTRUE;
}

bool FreeRtosQueue::Overwrite(const void* item) {
	if(handle_ == nullptr) {
		return false;
	}
	return xQueueOverwrite(static_cast<QueueHandle_t>(handle_), item) == pdTRUE;
}

bool FreeRtosQueue::Reset() {
	if(handle_ == nullptr) {
		return false;
	}
	return xQueueReset(static_cast<QueueHandle_t>(handle_)) == pdPASS;
}

std::size_t FreeRtosQueue::MessagesWaiting() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(uxQueueMessagesWaiting(static_cast<QueueHandle_t>(handle_)));
}

std::size_t FreeRtosQueue::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(uxQueueSpacesAvailable(static_cast<QueueHandle_t>(handle_)));
}

bool FreeRtosQueue::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
