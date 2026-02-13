// Button event helper implementation.
#include "button_event.hpp"

#include <chrono>

namespace c7222 {

void ButtonEvent::SetEvents(std::uint32_t new_events) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		events_ = new_events;
		event_ready_ = true;
	}
	cv_.notify_one();
}

bool ButtonEvent::GetEvents(std::uint32_t timeout_ms, std::uint32_t& events_out) {
	std::unique_lock<std::mutex> lock(mutex_);
	if(!cv_.wait_for(lock,
	                 std::chrono::milliseconds(timeout_ms),
	                 [this]{ return event_ready_; })) {
		return false; // timeout
	}
	events_out = events_;
	bool ready = event_ready_;
	event_ready_ = false;
	return ready;
}

} // namespace c7222
