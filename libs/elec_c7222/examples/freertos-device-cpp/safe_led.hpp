/**
 * @file safe_led.hpp
 * @brief Thread-safe LED wrapper that enforces exclusive control.
 */
#ifndef ELEC_C7222_DEVICES_SAFE_LED_HPP
#define ELEC_C7222_DEVICES_SAFE_LED_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "led.hpp"
#include "non_copyable.hpp"
#include "platform.hpp"

namespace c7222 {

/**
 * @class SafeLed
 * @brief LED wrapper that prevents simultaneous control from multiple threads.
 *
 * Usage pattern:
 * 1. Call `Acquire()` to obtain exclusive control.
 * 2. Perform one or more LED operations.
 * 3. Call `Release()` when done.
 *
 * All LED operations assert that the caller has acquired the lock.
 *
 * Synchronization model and purpose:
 * - Uses a `std::mutex` to guard ownership state (`locked_`) across multiple
 *   tasks that may try to control the LED.
 * - Uses a `std::condition_variable` to block waiters in `Acquire()`/`AcquireFor()`
 *   until the LED becomes available, avoiding busy-waiting.
 *
 * RAII considerations:
 * - RAII is intentionally **not** used to represent LED ownership because
 *   ownership spans multiple calls and can be transferred between tasks over
 *   time. A scoped guard would tie ownership to a single scope and thread,
 *   which does not match the intended usage pattern (press/release workflows).
 * - RAII *is* used internally for mutex management: `Acquire()`/`AcquireFor()`
 *   use `std::unique_lock` because `std::condition_variable` requires it for
 *   atomic unlock-and-wait semantics. The lock is released on all exit paths.
 */
class SafeLed : public NonCopyableNonMovable {
  public:
	/**
	 * @brief Construct a SafeLed wrapper around an existing Led instance.
	 */
	explicit SafeLed(Led& led);
	/**
	 * @brief Construct a SafeLed wrapper for a board LED by logical ID.
	 *
	 * This is a convenience constructor that looks up the LED from the
	 * Platform singleton. It assumes the Platform and PicoWBoard have been
	 * initialized and that the specified LED exists.
	 */
	explicit SafeLed(PicoWBoard::LedId id) : SafeLed(Platform::GetInstance()->GetLed(id)) {}

	/**
	 * @brief Acquire exclusive control of the LED (blocks until available).
	 */
	void Acquire();
	/**
	 * @brief Acquire exclusive control with a timeout.
	 *
	 * @param timeout Maximum time to wait.
	 * @return true if the lock was acquired, false on timeout.
	 */
	bool AcquireFor(uint32_t timeout);
	/**
	 * @brief Release exclusive control of the LED.
	 */
	void Release();
	/**
	 * @brief Return true if this object currently holds the lock.
	 */
	bool IsHeld() const;

	/**
	 * @brief Set the LED state (requires Acquire()).
	 */
	void Set(bool on);
	/**
	 * @brief Turn the LED on (requires Acquire()).
	 */
	void On();
	/**
	 * @brief Turn the LED off (requires Acquire()).
	 */
	void Off();
	/**
	 * @brief Toggle the LED (requires Acquire()).
	 */
	void Toggle();

 private:
	/** @brief Wrapped LED instance controlled by this guard. */
	Led& led_;
	/** @brief Guards ownership state and condition variable. */
	mutable std::mutex mutex_;
	/** @brief Signals waiters when ownership is released. */
	std::condition_variable cv_;
	/** @brief True when the LED is currently owned by a task. */
	bool locked_ = false;
};

} // namespace c7222

#endif // ELEC_C7222_DEVICES_SAFE_LED_HPP
