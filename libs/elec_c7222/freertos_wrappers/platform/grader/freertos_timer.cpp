// Grader implementation of FreeRtosTimer.
// Uses an internal hosted timer engine and also reports state via external grader hooks.
#include "freertos_timer.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <new>
#include <thread>
#include <utility>

namespace c7222 {

extern "C" {
bool c7222_grader_register_freertos_timer(FreeRtosTimer* timer,
											   std::uint32_t period_ticks,
											   bool periodic);
void c7222_grader_unregister_freertos_timer(FreeRtosTimer* timer);

bool c7222_grader_start_freertos_timer(FreeRtosTimer* timer,
									std::uint32_t ticks_to_wait,
									void* callback_arg);
bool c7222_grader_stop_freertos_timer(FreeRtosTimer* timer, std::uint32_t ticks_to_wait);
bool c7222_grader_reset_freertos_timer(FreeRtosTimer* timer, std::uint32_t ticks_to_wait);
bool c7222_grader_change_freertos_timer_period(FreeRtosTimer* timer,
										  std::uint32_t period_ticks,
										  std::uint32_t ticks_to_wait);
bool c7222_grader_is_freertos_timer_active(const FreeRtosTimer* timer);
}

namespace {

struct TimerState {
	std::mutex mutex;
	std::condition_variable cv;
	bool stop_worker{false};
	bool active{false};
	bool periodic{false};
	std::chrono::milliseconds period{0};
	FreeRtosTimer* owner{nullptr};
	std::thread worker;
};

std::chrono::milliseconds TicksToDuration(std::uint32_t ticks) {
	// Hosted grader mode: treat RTOS ticks as milliseconds.
	return std::chrono::milliseconds(static_cast<std::int64_t>(ticks));
}

void WorkerLoop(TimerState* state) {
	std::unique_lock<std::mutex> lock(state->mutex);
	while(!state->stop_worker) {
		if(!state->active) {
			state->cv.wait(lock, [state] { return state->stop_worker || state->active; });
			continue;
		}

		const auto period = state->period;
		const bool interrupted = state->cv.wait_for(lock, period, [state] {
			return state->stop_worker || !state->active;
		});

		if(interrupted || state->stop_worker || !state->active) {
			continue;
		}

		const bool one_shot_fired = !state->periodic;
		if(one_shot_fired) {
			state->active = false;
		}

		FreeRtosTimer* owner = state->owner;
		lock.unlock();
		FreeRtosTimerCallback(static_cast<void*>(owner));
		if(one_shot_fired) {
			(void)c7222_grader_stop_freertos_timer(owner, 0);
		}
		lock.lock();
	}
}

TimerState* CreateState(FreeRtosTimer* owner, std::uint32_t period_ticks, FreeRtosTimer::Type type) {
	auto* state = new (std::nothrow) TimerState();
	if(state == nullptr) {
		return nullptr;
	}
	state->owner = owner;
	state->periodic = (type == FreeRtosTimer::Type::kPeriodic);
	state->period = TicksToDuration(period_ticks);
	state->worker = std::thread(WorkerLoop, state);
	return state;
}

void DestroyState(TimerState* state) {
	if(state == nullptr) {
		return;
	}
	{
		std::lock_guard<std::mutex> lock(state->mutex);
		state->stop_worker = true;
		state->active = false;
	}
	state->cv.notify_all();
	if(state->worker.joinable()) {
		state->worker.join();
	}
	delete state;
}

} // namespace

void FreeRtosTimerCallback(void* timer) {
	auto* instance = static_cast<FreeRtosTimer*>(timer);
	if(instance == nullptr || instance->handle_ == nullptr) {
		return;
	}

	auto* state = static_cast<TimerState*>(instance->handle_);
	std::function<void(void*)> callback;
	void* callback_arg = nullptr;
	{
		std::lock_guard<std::mutex> lock(state->mutex);
		callback = instance->callback_;
		callback_arg = instance->callback_arg_;
	}

	if(callback) {
		callback(callback_arg);
	}
}

extern "C" void c7222_grader_fire_freertos_timer(FreeRtosTimer* timer) {
	FreeRtosTimerCallback(static_cast<void*>(timer));
}

FreeRtosTimer::FreeRtosTimer(const char* /*name*/,
							 std::uint32_t period_ticks,
							 Type type,
							 std::function<void(void*)> callback)
	: callback_(std::move(callback)) {
	auto* state = CreateState(this, period_ticks, type);
	if(state == nullptr) {
		handle_ = nullptr;
		return;
	}

	const bool periodic = (type == Type::kPeriodic);
	const bool registered = c7222_grader_register_freertos_timer(this, period_ticks, periodic);
	assert(registered && "Failed to register grader FreeRtosTimer");
	if(!registered) {
		DestroyState(state);
		handle_ = nullptr;
		return;
	}
	handle_ = state;
}

bool FreeRtosTimer::Initialize(const char* /*name*/,
							   std::uint32_t period_ticks,
							   Type type,
							   std::function<void(void*)> callback) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_freertos_timer(this);
		DestroyState(static_cast<TimerState*>(handle_));
		handle_ = nullptr;
	}

	callback_ = std::move(callback);
	auto* state = CreateState(this, period_ticks, type);
	if(state == nullptr) {
		return false;
	}

	const bool periodic = (type == Type::kPeriodic);
	const bool registered = c7222_grader_register_freertos_timer(this, period_ticks, periodic);
	if(!registered) {
		DestroyState(state);
		return false;
	}

	handle_ = state;
	return true;
}

FreeRtosTimer::~FreeRtosTimer() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_freertos_timer(this);
		DestroyState(static_cast<TimerState*>(handle_));
		handle_ = nullptr;
	}
}

bool FreeRtosTimer::Start(std::uint32_t ticks_to_wait, void* callback_arg) {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(state->mutex);
		callback_arg_ = callback_arg;
		state->active = true;
	}
	state->cv.notify_all();

	return c7222_grader_start_freertos_timer(this, ticks_to_wait, callback_arg_);
}

bool FreeRtosTimer::StartFromISR(void* callback_arg) {
	return Start(0, callback_arg);
}

bool FreeRtosTimer::Stop(std::uint32_t ticks_to_wait) {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(state->mutex);
		state->active = false;
	}
	state->cv.notify_all();

	return c7222_grader_stop_freertos_timer(this, ticks_to_wait);
}

bool FreeRtosTimer::StopFromISR() {
	return Stop(0);
}

bool FreeRtosTimer::Reset(std::uint32_t ticks_to_wait) {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(state->mutex);
		state->active = true;
	}
	state->cv.notify_all();

	return c7222_grader_reset_freertos_timer(this, ticks_to_wait);
}

bool FreeRtosTimer::ResetFromISR() {
	return Reset(0);
}

bool FreeRtosTimer::ChangePeriod(std::uint32_t period_ticks, std::uint32_t ticks_to_wait) {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		return false;
	}

	{
		std::lock_guard<std::mutex> lock(state->mutex);
		state->period = TicksToDuration(period_ticks);
	}
	state->cv.notify_all();

	return c7222_grader_change_freertos_timer_period(this, period_ticks, ticks_to_wait);
}

bool FreeRtosTimer::ChangePeriodFromISR(std::uint32_t period_ticks) {
	return ChangePeriod(period_ticks, 0);
}

void FreeRtosTimer::SetCallback(std::function<void(void*)> callback) {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		callback_ = std::move(callback);
		return;
	}

	std::lock_guard<std::mutex> lock(state->mutex);
	callback_ = std::move(callback);
}

bool FreeRtosTimer::IsValid() const {
	return handle_ != nullptr;
}

bool FreeRtosTimer::IsActive() const {
	auto* state = static_cast<TimerState*>(handle_);
	if(state == nullptr) {
		return false;
	}

	const bool grader_active = c7222_grader_is_freertos_timer_active(this);
	(void)grader_active;

	std::lock_guard<std::mutex> lock(state->mutex);
	return state->active;
}

} // namespace c7222
