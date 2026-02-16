#include "freertos_task_notification.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_task_notify(void* task_handle, std::uint32_t value, std::uint32_t action);
bool c7222_grader_task_notify_from_isr(void* task_handle, std::uint32_t value, std::uint32_t action);
bool c7222_grader_task_notify_indexed(void* task_handle,
									  std::uint32_t index,
									  std::uint32_t value,
									  std::uint32_t action);
bool c7222_grader_task_notify_indexed_from_isr(void* task_handle,
												std::uint32_t index,
												std::uint32_t value,
												std::uint32_t action);
bool c7222_grader_task_notify_wait(std::uint32_t bits_to_clear_on_entry,
								   std::uint32_t bits_to_clear_on_exit,
								   std::uint32_t* out_value,
								   std::uint32_t ticks_to_wait);
bool c7222_grader_task_notify_wait_indexed(std::uint32_t index,
										   std::uint32_t bits_to_clear_on_entry,
										   std::uint32_t bits_to_clear_on_exit,
										   std::uint32_t* out_value,
										   std::uint32_t ticks_to_wait);
std::uint32_t c7222_grader_task_notify_take(bool clear_count_on_exit, std::uint32_t ticks_to_wait);
std::uint32_t c7222_grader_task_notify_take_indexed(std::uint32_t index,
													bool clear_count_on_exit,
													std::uint32_t ticks_to_wait);
}

bool FreeRtosTaskNotification::Notify(TaskHandle task_handle, std::uint32_t value, Action action) {
	return c7222_grader_task_notify(task_handle, value, static_cast<std::uint32_t>(action));
}

bool FreeRtosTaskNotification::NotifyFromISR(TaskHandle task_handle, std::uint32_t value, Action action) {
	return c7222_grader_task_notify_from_isr(task_handle, value, static_cast<std::uint32_t>(action));
}

bool FreeRtosTaskNotification::NotifyIndexed(TaskHandle task_handle,
											 std::uint32_t index,
											 std::uint32_t value,
											 Action action) {
	return c7222_grader_task_notify_indexed(task_handle, index, value, static_cast<std::uint32_t>(action));
}

bool FreeRtosTaskNotification::NotifyIndexedFromISR(TaskHandle task_handle,
													std::uint32_t index,
													std::uint32_t value,
													Action action) {
	return c7222_grader_task_notify_indexed_from_isr(task_handle,
													 index,
													 value,
													 static_cast<std::uint32_t>(action));
}

bool FreeRtosTaskNotification::Wait(std::uint32_t bits_to_clear_on_entry,
									std::uint32_t bits_to_clear_on_exit,
									std::uint32_t* out_value,
									std::uint32_t ticks_to_wait) {
	return c7222_grader_task_notify_wait(bits_to_clear_on_entry,
										 bits_to_clear_on_exit,
										 out_value,
										 ticks_to_wait);
}

bool FreeRtosTaskNotification::WaitIndexed(std::uint32_t index,
										   std::uint32_t bits_to_clear_on_entry,
										   std::uint32_t bits_to_clear_on_exit,
										   std::uint32_t* out_value,
										   std::uint32_t ticks_to_wait) {
	return c7222_grader_task_notify_wait_indexed(index,
												 bits_to_clear_on_entry,
												 bits_to_clear_on_exit,
												 out_value,
												 ticks_to_wait);
}

std::uint32_t FreeRtosTaskNotification::Take(bool clear_count_on_exit, std::uint32_t ticks_to_wait) {
	return c7222_grader_task_notify_take(clear_count_on_exit, ticks_to_wait);
}

std::uint32_t FreeRtosTaskNotification::TakeIndexed(std::uint32_t index,
													bool clear_count_on_exit,
													std::uint32_t ticks_to_wait) {
	return c7222_grader_task_notify_take_indexed(index, clear_count_on_exit, ticks_to_wait);
}

} // namespace c7222
