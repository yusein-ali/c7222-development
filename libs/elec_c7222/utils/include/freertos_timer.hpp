/**
 * @file free_rtos_timer.hpp
 * @brief C++ wrapper for FreeRTOS software timers.
 */
#ifndef ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP
#define ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP
#include "freertos_timer.hpp"

#include <cstdint>
#include <functional>

#include "non_copyable.hpp"

namespace c7222 {

void FreeRtosTimerCallback(void* timer);

/**
 * @class FreeRtosTimer
 * @brief Encapsulates FreeRTOS one-shot and periodic software timers.
 */
class FreeRtosTimer : public NonCopyableNonMovable {
  public:
	enum class Type : uint8_t {
		kOneShot,
		kPeriodic
	};

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
				  std::function<void()> callback = nullptr);

	bool Initialize(const char* name,
					std::uint32_t period_ticks,
					Type type,
					std::function<void()> callback);

	/**
	 * @brief Delete the timer if it was created.
	 */
	~FreeRtosTimer();

	bool Start(std::uint32_t ticks_to_wait = 0);
	bool Stop(std::uint32_t ticks_to_wait = 0);
	bool Reset(std::uint32_t ticks_to_wait = 0);
	bool ChangePeriod(std::uint32_t period_ticks, std::uint32_t ticks_to_wait = 0);

	/**
	 * @brief Register or replace the timer callback.
	 */
	void SetCallback(std::function<void()> callback);

	/**
	 * @brief Check if the timer handle is valid.
	 */
	bool IsValid() const;

	/**
	 * @brief Check whether the timer is active.
	 */
	bool IsActive() const;

  private:
	friend void FreeRtosTimerCallback(void* timer);

	void* handle_{nullptr};
	std::function<void()> callback_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREE_RTOS_TIMER_HPP
