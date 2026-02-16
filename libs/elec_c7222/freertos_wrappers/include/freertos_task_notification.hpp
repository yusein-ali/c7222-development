/**
 * @file freertos_task_notification.hpp
 * @brief Static helper wrapper for FreeRTOS task notifications.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_TASK_NOTIFICATION_HPP
#define ELEC_C7222_UTILS_FREERTOS_TASK_NOTIFICATION_HPP

#include <cstdint>

namespace c7222 {

/**
 * @class FreeRtosTaskNotification
 * @brief Utility interface for direct task notifications.
 *
 * Task notifications are lightweight per-task signaling values and are often
 * used as fast alternatives to semaphores/queues for many one-to-one cases.
 */
class FreeRtosTaskNotification final {
  public:
	/** @brief Opaque task handle type. */
	using TaskHandle = void*;

	/** @brief Notification update action. */
	enum class Action : std::uint8_t {
		kNoAction = 0,
		kSetBits,
		kIncrement,
		kSetValueWithOverwrite,
		kSetValueWithoutOverwrite
	};

	/** @brief Send a notification to a task. */
	static bool Notify(TaskHandle task_handle, std::uint32_t value, Action action);
	/** @brief Send a notification to a task from ISR context. */
	static bool NotifyFromISR(TaskHandle task_handle, std::uint32_t value, Action action);
	/** @brief Indexed notification variant. */
	static bool NotifyIndexed(TaskHandle task_handle,
							 std::uint32_t index,
							 std::uint32_t value,
							 Action action);
	/** @brief Indexed ISR notification variant. */
	static bool NotifyIndexedFromISR(TaskHandle task_handle,
								 std::uint32_t index,
								 std::uint32_t value,
								 Action action);
	/**
	 * @brief Wait on current task notification value.
	 * @param bits_to_clear_on_entry Bits cleared before waiting.
	 * @param bits_to_clear_on_exit Bits cleared before returning.
	 * @param out_value Optional resulting notification value.
	 * @param ticks_to_wait Max ticks to wait.
	 */
	static bool Wait(std::uint32_t bits_to_clear_on_entry,
					 std::uint32_t bits_to_clear_on_exit,
					 std::uint32_t* out_value,
					 std::uint32_t ticks_to_wait);
	/** @brief Indexed wait variant. */
	static bool WaitIndexed(std::uint32_t index,
						 std::uint32_t bits_to_clear_on_entry,
						 std::uint32_t bits_to_clear_on_exit,
						 std::uint32_t* out_value,
						 std::uint32_t ticks_to_wait);
	/** @brief Notification-as-count take helper for current task. */
	static std::uint32_t Take(bool clear_count_on_exit, std::uint32_t ticks_to_wait);
	/** @brief Indexed notification-as-count take helper. */
	static std::uint32_t TakeIndexed(std::uint32_t index,
							 bool clear_count_on_exit,
							 std::uint32_t ticks_to_wait);
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_TASK_NOTIFICATION_HPP
