#include "freertos_mutex.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_register_mutex(FreeRtosMutex* mutex);
void c7222_grader_unregister_mutex(FreeRtosMutex* mutex);
bool c7222_grader_lock_mutex(FreeRtosMutex* mutex, std::uint32_t ticks_to_wait);
bool c7222_grader_unlock_mutex(FreeRtosMutex* mutex);

bool c7222_grader_register_recursive_mutex(FreeRtosRecursiveMutex* mutex);
void c7222_grader_unregister_recursive_mutex(FreeRtosRecursiveMutex* mutex);
bool c7222_grader_lock_recursive_mutex(FreeRtosRecursiveMutex* mutex, std::uint32_t ticks_to_wait);
bool c7222_grader_unlock_recursive_mutex(FreeRtosRecursiveMutex* mutex);
}

FreeRtosMutex::FreeRtosMutex() {
	(void)Initialize();
}

bool FreeRtosMutex::Initialize() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_mutex(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_mutex(this);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosMutex::~FreeRtosMutex() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_mutex(this);
		handle_ = nullptr;
	}
}

bool FreeRtosMutex::Lock(std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_lock_mutex(this, ticks_to_wait);
}

bool FreeRtosMutex::TryLock() {
	return Lock(0);
}

bool FreeRtosMutex::Unlock() {
	return (handle_ != nullptr) && c7222_grader_unlock_mutex(this);
}

bool FreeRtosMutex::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosRecursiveMutex::FreeRtosRecursiveMutex() {
	(void)Initialize();
}

bool FreeRtosRecursiveMutex::Initialize() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_recursive_mutex(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_recursive_mutex(this);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosRecursiveMutex::~FreeRtosRecursiveMutex() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_recursive_mutex(this);
		handle_ = nullptr;
	}
}

bool FreeRtosRecursiveMutex::Lock(std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_lock_recursive_mutex(this, ticks_to_wait);
}

bool FreeRtosRecursiveMutex::TryLock() {
	return Lock(0);
}

bool FreeRtosRecursiveMutex::Unlock() {
	return (handle_ != nullptr) && c7222_grader_unlock_recursive_mutex(this);
}

bool FreeRtosRecursiveMutex::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
