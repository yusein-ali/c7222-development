/**
 * @file button_event.hpp
 * @brief Thread-safe button event helper for FreeRTOS/C++ examples.
 */
#ifndef ELEC_C7222_EXAMPLES_BUTTON_EVENT_HPP
#define ELEC_C7222_EXAMPLES_BUTTON_EVENT_HPP

#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace c7222 {

/**
 * @class ButtonEvent
 * @brief Thread-safe event wrapper for button IRQ notifications.
 *
 * Synchronization model and purpose:
 * - Uses a `std::mutex` to protect the shared event state (`events_` and
 *   `event_ready_`) from concurrent access by the ISR-dispatcher path and
 *   the consumer task.
 * - Uses a `std::condition_variable` to block the consumer task efficiently
 *   until new events arrive, with optional timeout.
 * - The flag `event_ready_` is the condition predicate that prevents missed
 *   wake-ups (the consumer re-checks the flag after waking).
 * - RAII **is** used for lock management inside `GetEvents()` via
 *   `std::unique_lock`, ensuring the mutex is released on all exit paths.
 * - RAII is **not** used to represent the event itself because event ownership
 *   spans multiple threads and timeouts. The event is produced asynchronously
 *   (ISR-dispatcher path) and consumed later by a waiting task. An RAII guard
 *   would tie event lifetime to a single scope and thread, which is not
 *   appropriate for a cross-thread notification.
 *
 * Intended usage:
 * - `SetEvents()` is called by the IRQ dispatcher task or timer callback
 *   when a GPIO interrupt occurs.
 * - `GetEvents()` is called by a consumer task that wants to wait for the
 *   next event without polling.
 */
class ButtonEvent {
  public:
	/**
	 * @brief Set the event bitmask and notify waiters.
	 */
	void SetEvents(std::uint32_t new_events);

	/**
	 * @brief Wait for events with a timeout.
	 *
	 * @param timeout_ms Maximum time to wait in milliseconds.
	 * @param events_out Filled with event bitmask on success.
	 * @return true if events were received, false on timeout.
	 */
	bool GetEvents(std::uint32_t timeout_ms, std::uint32_t& events_out);

  private:
	/** @brief Last observed GPIO event bitmask. */
	std::uint32_t events_{0};
	/** @brief Predicate flag indicating that new events are available. */
	bool event_ready_{false};
	/** @brief Guards shared event state between producer and consumer. */
	std::mutex mutex_;
	/** @brief Notifies the consumer when new events arrive. */
	std::condition_variable cv_;
};

} // namespace c7222

#endif // ELEC_C7222_EXAMPLES_BUTTON_EVENT_HPP
