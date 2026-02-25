/**
 * @file freertos_task.hpp
 * @brief C++ wrapper for FreeRTOS-style task creation and control.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_TASK_HPP
#define ELEC_C7222_UTILS_FREERTOS_TASK_HPP

#include <cstdint>
#include <functional>
#include <limits>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosTask
 * @brief Ownership-based wrapper for a task/thread execution object.
 *
 * The wrapper owns a task handle and stores a C++ callable task body. Platform
 * implementations bridge the native task entry point to `RunTaskBody()`.
 * RAII here refers to task-handle cleanup on destruction.
 *
 * Typical usage:
 * @code
 * void Worker(void* arg) {
 *     (void)arg;
 *     for(;;) {
 *         c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(100));
 *     }
 * }
 *
 * c7222::FreeRtosTask task("worker", 1024, c7222::FreeRtosTask::IdlePriority() + 1, Worker, nullptr);
 * @endcode
 */
class FreeRtosTask : public NonCopyableNonMovable {
  public:
	/** @brief Task callable signature. */
	using TaskFunction = std::function<void(void*)>;
	/** @brief Sentinel tick value representing an infinite wait. */
	static constexpr std::uint32_t kInfinite = std::numeric_limits<std::uint32_t>::max();

	/** @brief Construct an uninitialized task wrapper. */
	FreeRtosTask() = default;
	/**
	 * @brief Construct and initialize a task.
	 * @param name Task name.
	 * @param stack_depth_words Stack depth in words.
	 * @param priority Initial task priority.
	 * @param task_function Callable run as the task body.
	 * @param task_arg Argument passed to `task_function`.
	 */
	FreeRtosTask(const char* name,
				 std::uint32_t stack_depth_words,
				 std::uint32_t priority,
				 TaskFunction task_function,
				 void* task_arg = nullptr);

	/**
	 * @brief Initialize (or re-initialize) a task.
	 * @return true on successful creation/start.
	 */
	bool Initialize(const char* name,
					std::uint32_t stack_depth_words,
					std::uint32_t priority,
					TaskFunction task_function,
					void* task_arg = nullptr);

	/** @brief Delete owned task resource if initialized. */
	~FreeRtosTask();

	/**
	 * @brief Delete the task.
	 * @param ticks_to_wait Optional platform-specific wait.
	 * @return true if a task was deleted.
	 */
	bool Delete(std::uint32_t ticks_to_wait = 0);
	/** @brief Suspend the task. */
	bool Suspend();
	/** @brief Resume the task from task context. */
	bool Resume();
	/** @brief Resume the task from ISR context. */
	bool ResumeFromISR();

	/** @brief Set task priority. */
	bool SetPriority(std::uint32_t priority);
	/** @brief Get current task priority. */
	std::uint32_t GetPriority() const;

	/** @brief @return true if wrapper owns a valid task handle. */
	bool IsValid() const;
	/** @brief @return true if the task is currently considered running/alive. */
	bool IsRunning() const;
	/** @brief @return Opaque platform task handle. */
	void* GetHandle() const;

	/** @brief Delay current task for a number of ticks. */
	static void Delay(std::uint32_t ticks);
	/** @brief Yield current task. */
	static void Yield();
	/** @brief @return Current scheduler tick count. */
	static std::uint32_t GetTickCount();
	/** @brief Start the scheduler. */
	static void StartScheduler();
	/** @brief Convert milliseconds to scheduler ticks. */
	static std::uint32_t MsToTicks(std::uint32_t milliseconds);
	/** @brief @return Idle task priority. */
	static std::uint32_t IdlePriority();

	/**
	 * @brief Internal bridge used by platform task-entry trampolines.
	 *
	 * Public by design to avoid friend declarations for C-linkage bridge code.
	 */
	void RunTaskBody();

  private:
	void* handle_{nullptr};
	TaskFunction task_function_{nullptr};
	void* task_arg_{nullptr};
	std::uint32_t priority_{0};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_TASK_HPP
