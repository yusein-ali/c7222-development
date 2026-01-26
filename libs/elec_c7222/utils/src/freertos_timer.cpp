// FreeRTOS software timer wrapper.
#include "freertos_timer.hpp"

#include "FreeRTOS.h"
#include "timers.h"


namespace c7222 {

void FreeRtosTimerCallback(void* timer) {
	void* id = pvTimerGetTimerID(static_cast<TimerHandle_t>(timer));
	auto* instance = static_cast<FreeRtosTimer*>(id);
	if(instance && instance->callback_) {
		instance->callback_();
	}
}

namespace {

void TimerCallback(TimerHandle_t timer) {
	FreeRtosTimerCallback(static_cast<void*>(timer));
}

} // namespace

FreeRtosTimer::FreeRtosTimer(const char* name,
							 std::uint32_t period_ticks,
							 Type type,
							 std::function<void()> callback)
	: callback_(std::move(callback)) {
	const UBaseType_t auto_reload = (type == Type::kPeriodic) ? pdTRUE : pdFALSE;
	handle_ = xTimerCreate(name,
						   static_cast<TickType_t>(period_ticks),
						   auto_reload,
						   this,
						   &TimerCallback);
}

bool FreeRtosTimer::Initialize(const char* name,
							   std::uint32_t period_ticks,
							   Type type,
							   std::function<void()> callback) {
	if(handle_ != nullptr) {
		xTimerDelete(static_cast<TimerHandle_t>(handle_), 0);
		handle_ = nullptr;
	}
	callback_ = std::move(callback);
	const UBaseType_t auto_reload = (type == Type::kPeriodic) ? pdTRUE : pdFALSE;
	handle_ = xTimerCreate(name,
						   static_cast<TickType_t>(period_ticks),
						   auto_reload,
						   this,
						   &TimerCallback);
	return handle_ != nullptr;
}

FreeRtosTimer::~FreeRtosTimer() {
	if(handle_ != nullptr) {
		xTimerDelete(static_cast<TimerHandle_t>(handle_), 0);
		handle_ = nullptr;
	}
}

bool FreeRtosTimer::Start(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xTimerStart(static_cast<TimerHandle_t>(handle_),
					   static_cast<TickType_t>(ticks_to_wait)) == pdPASS;
}

bool FreeRtosTimer::Stop(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xTimerStop(static_cast<TimerHandle_t>(handle_),
					  static_cast<TickType_t>(ticks_to_wait)) == pdPASS;
}

bool FreeRtosTimer::Reset(std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xTimerReset(static_cast<TimerHandle_t>(handle_),
					   static_cast<TickType_t>(ticks_to_wait)) == pdPASS;
}

bool FreeRtosTimer::ChangePeriod(std::uint32_t period_ticks, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return false;
	}
	return xTimerChangePeriod(static_cast<TimerHandle_t>(handle_),
							  static_cast<TickType_t>(period_ticks),
							  static_cast<TickType_t>(ticks_to_wait)) == pdPASS;
}

void FreeRtosTimer::SetCallback(std::function<void()> callback) {
	callback_ = std::move(callback);
}

bool FreeRtosTimer::IsValid() const {
	return handle_ != nullptr;
}

bool FreeRtosTimer::IsActive() const {
	if(handle_ == nullptr) {
		return false;
	}
	return xTimerIsTimerActive(static_cast<TimerHandle_t>(handle_)) == pdTRUE;
}

} // namespace c7222
