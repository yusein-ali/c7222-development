/**
 * @file freertos_timer.hpp
 * @brief C++ wrapper for FreeRTOS software timers.
 */
#ifndef ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP
#define ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP

#include <cstdint>
#include <functional>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @brief Internal use only. This function is invoked as a callback when the FreeRTOS timer expires.
 *
 * This function should not be used directly by the user. It is intended to be called by the
 * FreeRTOS timer infrastructure to handle timer expiration events. Ensure that any logic that needs
 * to be executed on timer expiry is encapsulated within the FreeRtosTimer class and its associated
 * callback mechanism.
 *
 * @param timer A pointer to the timer instance that has expired.
 */
void FreeRtosTimerCallback(void* timer);

/**
 * @class FreeRtosTimer
 * @brief Ownership-based C++ wrapper for FreeRTOS software timers.
 *
 * This class provides a small, ownership-based interface around a FreeRTOS
 * software timer handle. It uses a single internal callback trampoline
 * (`FreeRtosTimerCallback`) to bridge the C callback signature to a stored
 * `std::function<void(void*)>` in the C++ object.
 *
 * Design principles:
 * - **RAII ownership:** the class owns a timer handle and deletes it in the
 *   destructor, avoiding leaks and dangling handles.
 * - **Explicit initialization:** construction is lightweight; actual RTOS
 *   resources are allocated in `Initialize()` (or the non-default constructor).
 * - **Move/Copy safety:** inherits `NonCopyableNonMovable` to prevent copying
 *   timer handles or callbacks across objects.
 * - **Thin wrapper:** keeps FreeRTOS semantics visible (ticks, start/stop/reset),
 *   rather than hiding them behind higher-level abstractions.
 *
 * FreeRTOS timer summary and usage in this class:
 * - FreeRTOS software timers run in the **Timer Service Task** context, not
 *   in interrupt context. Callbacks must be non-blocking and thread-safe with
 *   respect to the rest of the system.
 * - Timers are created with a period in **ticks** and a type (one-shot or
 *   periodic). The scheduler triggers the callback when the timer expires.
 * - This class stores a `std::function<void(void*)>` that is invoked by the
 *   C callback trampoline with a user-supplied argument. `SetCallback()`
 *   updates that function at runtime.
 * - Start/Stop/Reset/ChangePeriod map directly to the underlying FreeRTOS
 *   APIs and accept optional `ticks_to_wait` parameters to control command
 *   queue blocking time.
 *
 * Typical usage:
 * @code
 * c7222::FreeRtosTimer timer("blink",
 *                            c7222::FreeRtosTask::MsToTicks(500),
 *                            c7222::FreeRtosTimer::Type::kPeriodic,
 *                            [](void* arg) {
 *                                (void)arg;
 *                                // periodic work
 *                            });
 * (void)timer.Start();
 * @endcode
 */
class FreeRtosTimer : public NonCopyableNonMovable {
  public:
	enum class Type : uint8_t {
		/** @brief One-shot timer: fires once then stops. */
		kOneShot,
		/** @brief Periodic timer: fires repeatedly at the configured period. */
		kPeriodic
	};

	/**
	 * @brief Create an uninitialized timer wrapper.
	 *
	 * The handle is null until `Initialize()` is called. This allows
	 * construction without allocating RTOS resources.
	 */
	FreeRtosTimer() = default;
	/**
	 * @brief Create a FreeRTOS software timer.
	 * @param name Human-readable timer name.
	 * @param period_ticks Timer period in ticks.
	 * @param type One-shot or periodic.
	 * @param callback Callback invoked on expiry (optional).
	 */
	FreeRtosTimer(const char* name,
				  std::uint32_t period_ticks,
				  Type type,
				  std::function<void(void*)> callback = nullptr);

	/**
	 * @brief Initialize (or re-initialize) the timer wrapper.
	 *
	 * Allocates the underlying FreeRTOS timer and binds the callback trampoline.
	 * If the wrapper was previously initialized, the prior handle is deleted
	 * before creating a new one.
	 *
	 * @param name Human-readable timer name.
	 * @param period_ticks Timer period in ticks.
	 * @param type One-shot or periodic.
	 * @param callback Callback invoked on expiry (optional).
	 * @return true if the timer was created successfully, false otherwise.
	 */
	bool Initialize(const char* name,
					std::uint32_t period_ticks,
					Type type,
					std::function<void(void*)> callback);

	/**
	 * @brief Delete the timer if it was created.
	 *
	 * This is safe to call even if the timer was never initialized.
	 */
	~FreeRtosTimer();

	/**
	 * @brief Start the timer.
	 *
	 * Enqueues a start command to the FreeRTOS timer service task.
	 *
	 * @param ticks_to_wait Maximum ticks to block if the timer command queue
	 *        is full (0 = no wait).
	 * @param callback_arg Argument passed to the timer callback on expiry.
	 * @return true if the command was accepted, false otherwise.
	 */
	bool Start(std::uint32_t ticks_to_wait = 0, void* callback_arg = nullptr);
	/**
	 * @brief Start the timer from ISR context (no immediate yield).
	 *
	 * This variant does not expose `higher_priority_task_woken`; any unblocked
	 * task will run on the next tick/schedule point.
	 *
	 * @param callback_arg Argument passed to the timer callback on expiry.
	 * @return true if the command was accepted, false otherwise.
	 */
	bool StartFromISR(void* callback_arg = nullptr);
	/**
	 * @brief Stop the timer.
	 *
	 * Enqueues a stop command to the FreeRTOS timer service task.
	 *
	 * @param ticks_to_wait Maximum ticks to block if the timer command queue
	 *        is full (0 = no wait).
	 * @return true if the command was accepted, false otherwise.
	 */
	bool Stop(std::uint32_t ticks_to_wait = 0);

	/**
	 * @brief Stop the timer from ISR context (no immediate yield).
	 *
	 * Any unblocked task will run on the next tick/schedule point.
	 *
	 * @return true if the command was accepted, false otherwise.
	 */
	bool StopFromISR();
	/**
	 * @brief Reset the timer to start counting from zero.
	 *
	 * For periodic timers, this restarts the period. For one-shot timers, this
	 * arms the timer again.
	 *
	 * @param ticks_to_wait Maximum ticks to block if the timer command queue
	 *        is full (0 = no wait).
	 * @return true if the command was accepted, false otherwise.
	 */
	bool Reset(std::uint32_t ticks_to_wait = 0);

	/**
	 * @brief Reset the timer from ISR context (no immediate yield).
	 *
	 * Any unblocked task will run on the next tick/schedule point.
	 *
	 * @return true if the command was accepted, false otherwise.
	 */
	bool ResetFromISR();
	/**
	 * @brief Change the timer period.
	 *
	 * The new period takes effect after the command is processed by the timer
	 * service task.
	 *
	 * @param period_ticks New period in ticks.
	 * @param ticks_to_wait Maximum ticks to block if the timer command queue
	 *        is full (0 = no wait).
	 * @return true if the command was accepted, false otherwise.
	 */
	bool ChangePeriod(std::uint32_t period_ticks, std::uint32_t ticks_to_wait = 0);

	/**
	 * @brief Change the timer period from ISR context (no immediate yield).
	 *
	 * Any unblocked task will run on the next tick/schedule point.
	 *
	 * @return true if the command was accepted, false otherwise.
	 */
	bool ChangePeriodFromISR(std::uint32_t period_ticks);

	/**
	 * @brief Register or replace the timer callback.
	 *
	 * The callback runs in the FreeRTOS timer service task context. It must be
	 * short, non-blocking, and thread-safe with respect to shared resources.
	 * Passing `nullptr` clears the callback.
	 */
	void SetCallback(std::function<void(void*)> callback);

	/**
	 * @brief Check if the timer handle is valid.
	 * @return true if `Initialize()` succeeded and the handle is non-null.
	 */
	bool IsValid() const;

	/**
	 * @brief Check whether the timer is active.
	 * @return true if the timer is currently running, false otherwise.
	 */
	bool IsActive() const;

  private:
	friend void FreeRtosTimerCallback(void* timer);

	/**
	 * @brief Underlying FreeRTOS timer handle (opaque).
	 *
	 * Owned by this class. `nullptr` indicates an uninitialized timer.
	 */
	void* handle_{nullptr};
	/**
	 * @brief User callback invoked on timer expiry.
	 *
	 * Invoked by the C callback trampoline in the timer service task context.
	 */
	std::function<void(void*)> callback_{nullptr};
	/**
	 * @brief User argument passed to the callback on expiry.
	 */
	void* callback_arg_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP
