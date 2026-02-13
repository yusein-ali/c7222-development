// Thread-safe LED wrapper implementation.
#include "safe_led.hpp"

#include <cassert>

namespace c7222 {

SafeLed::SafeLed(Led& led) : led_(led) {}

void SafeLed::Acquire() {
	std::unique_lock<std::mutex> lock(mutex_);
	cv_.wait(lock, [this]{ return !locked_; });
	locked_ = true;
}

bool SafeLed::AcquireFor(uint32_t timeout) {
	std::unique_lock<std::mutex> lock(mutex_);
	if(!cv_.wait_for(lock, std::chrono::milliseconds(timeout), [this]{ return !locked_; })) {
		return false;
	}
	locked_ = true;
	return true;
}

void SafeLed::Release() {
	std::unique_lock<std::mutex> lock(mutex_);
	if(!locked_) {
		return;
	}
	locked_ = false;
	lock.unlock();
	cv_.notify_one();
}

bool SafeLed::IsHeld() const {
	std::lock_guard<std::mutex> lock(mutex_);
	return locked_;
}

void SafeLed::Set(bool on) {
	assert(locked_ && "SafeLed::Set requires Acquire()");
	led_.Set(on);
}

void SafeLed::On() {
	assert(locked_ && "SafeLed::On requires Acquire()");
	led_.On();
}

void SafeLed::Off() {
	assert(locked_ && "SafeLed::Off requires Acquire()");
	led_.Off();
}

void SafeLed::Toggle() {
	assert(locked_ && "SafeLed::Toggle requires Acquire()");
	led_.Toggle();
}

} // namespace c7222
