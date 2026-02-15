#include "freertos_task_notification.hpp"

#include "FreeRTOS.h"
#include "task.h"

namespace c7222 {
namespace {

eNotifyAction ToNativeAction(FreeRtosTaskNotification::Action action) {
	switch(action) {
		case FreeRtosTaskNotification::Action::kNoAction:
			return eNoAction;
		case FreeRtosTaskNotification::Action::kSetBits:
			return eSetBits;
		case FreeRtosTaskNotification::Action::kIncrement:
			return eIncrement;
		case FreeRtosTaskNotification::Action::kSetValueWithOverwrite:
			return eSetValueWithOverwrite;
		case FreeRtosTaskNotification::Action::kSetValueWithoutOverwrite:
			return eSetValueWithoutOverwrite;
		default:
			return eNoAction;
	}
}

} // namespace

bool FreeRtosTaskNotification::Notify(TaskHandle task_handle, std::uint32_t value, Action action) {
	return xTaskNotify(static_cast<TaskHandle_t>(task_handle),
					   static_cast<uint32_t>(value),
					   ToNativeAction(action)) == pdPASS;
}

bool FreeRtosTaskNotification::NotifyFromISR(TaskHandle task_handle, std::uint32_t value, Action action) {
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xTaskNotifyFromISR(static_cast<TaskHandle_t>(task_handle),
							  static_cast<uint32_t>(value),
							  ToNativeAction(action),
							  &higher_priority_task_woken) == pdPASS;
}

bool FreeRtosTaskNotification::NotifyIndexed(TaskHandle task_handle,
											 std::uint32_t index,
											 std::uint32_t value,
											 Action action) {
	return xTaskNotifyIndexed(static_cast<TaskHandle_t>(task_handle),
							  static_cast<UBaseType_t>(index),
							  static_cast<uint32_t>(value),
							  ToNativeAction(action)) == pdPASS;
}

bool FreeRtosTaskNotification::NotifyIndexedFromISR(TaskHandle task_handle,
													std::uint32_t index,
													std::uint32_t value,
													Action action) {
	BaseType_t higher_priority_task_woken = pdFALSE;
	return xTaskNotifyIndexedFromISR(static_cast<TaskHandle_t>(task_handle),
									 static_cast<UBaseType_t>(index),
									 static_cast<uint32_t>(value),
									 ToNativeAction(action),
									 &higher_priority_task_woken) == pdPASS;
}

bool FreeRtosTaskNotification::Wait(std::uint32_t bits_to_clear_on_entry,
									std::uint32_t bits_to_clear_on_exit,
									std::uint32_t* out_value,
									std::uint32_t ticks_to_wait) {
	uint32_t notified_value = 0;
	const BaseType_t result = xTaskNotifyWait(static_cast<uint32_t>(bits_to_clear_on_entry),
											   static_cast<uint32_t>(bits_to_clear_on_exit),
											   &notified_value,
											   static_cast<TickType_t>(ticks_to_wait));
	if(out_value != nullptr) {
		*out_value = static_cast<std::uint32_t>(notified_value);
	}
	return result == pdTRUE;
}

bool FreeRtosTaskNotification::WaitIndexed(std::uint32_t index,
										   std::uint32_t bits_to_clear_on_entry,
										   std::uint32_t bits_to_clear_on_exit,
										   std::uint32_t* out_value,
										   std::uint32_t ticks_to_wait) {
	uint32_t notified_value = 0;
	const BaseType_t result = xTaskNotifyWaitIndexed(static_cast<UBaseType_t>(index),
													  static_cast<uint32_t>(bits_to_clear_on_entry),
													  static_cast<uint32_t>(bits_to_clear_on_exit),
													  &notified_value,
													  static_cast<TickType_t>(ticks_to_wait));
	if(out_value != nullptr) {
		*out_value = static_cast<std::uint32_t>(notified_value);
	}
	return result == pdTRUE;
}

std::uint32_t FreeRtosTaskNotification::Take(bool clear_count_on_exit, std::uint32_t ticks_to_wait) {
	return static_cast<std::uint32_t>(ulTaskNotifyTake(clear_count_on_exit ? pdTRUE : pdFALSE,
													   static_cast<TickType_t>(ticks_to_wait)));
}

std::uint32_t FreeRtosTaskNotification::TakeIndexed(std::uint32_t index,
													bool clear_count_on_exit,
													std::uint32_t ticks_to_wait) {
	return static_cast<std::uint32_t>(ulTaskNotifyTakeIndexed(static_cast<UBaseType_t>(index),
															  clear_count_on_exit ? pdTRUE : pdFALSE,
															  static_cast<TickType_t>(ticks_to_wait)));
}

} // namespace c7222
