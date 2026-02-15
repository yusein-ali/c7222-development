#include "freertos_semaphore.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_register_binary_semaphore(FreeRtosBinarySemaphore* semaphore, bool initially_given);
void c7222_grader_unregister_binary_semaphore(FreeRtosBinarySemaphore* semaphore);
bool c7222_grader_take_binary_semaphore(FreeRtosBinarySemaphore* semaphore, std::uint32_t ticks_to_wait);
bool c7222_grader_give_binary_semaphore(FreeRtosBinarySemaphore* semaphore);
bool c7222_grader_give_binary_semaphore_from_isr(FreeRtosBinarySemaphore* semaphore);

bool c7222_grader_register_counting_semaphore(FreeRtosCountingSemaphore* semaphore,
											   std::uint32_t max_count,
											   std::uint32_t initial_count);
void c7222_grader_unregister_counting_semaphore(FreeRtosCountingSemaphore* semaphore);
bool c7222_grader_take_counting_semaphore(FreeRtosCountingSemaphore* semaphore, std::uint32_t ticks_to_wait);
bool c7222_grader_give_counting_semaphore(FreeRtosCountingSemaphore* semaphore);
bool c7222_grader_give_counting_semaphore_from_isr(FreeRtosCountingSemaphore* semaphore);
std::uint32_t c7222_grader_get_counting_semaphore_count(const FreeRtosCountingSemaphore* semaphore);
}

FreeRtosBinarySemaphore::FreeRtosBinarySemaphore(bool initially_given) {
	(void)Initialize(initially_given);
}

bool FreeRtosBinarySemaphore::Initialize(bool initially_given) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_binary_semaphore(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_binary_semaphore(this, initially_given);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosBinarySemaphore::~FreeRtosBinarySemaphore() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_binary_semaphore(this);
		handle_ = nullptr;
	}
}

bool FreeRtosBinarySemaphore::Take(std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_take_binary_semaphore(this, ticks_to_wait);
}

bool FreeRtosBinarySemaphore::Give() {
	return (handle_ != nullptr) && c7222_grader_give_binary_semaphore(this);
}

bool FreeRtosBinarySemaphore::GiveFromISR() {
	return (handle_ != nullptr) && c7222_grader_give_binary_semaphore_from_isr(this);
}

bool FreeRtosBinarySemaphore::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosCountingSemaphore::FreeRtosCountingSemaphore(std::uint32_t max_count, std::uint32_t initial_count) {
	(void)Initialize(max_count, initial_count);
}

bool FreeRtosCountingSemaphore::Initialize(std::uint32_t max_count, std::uint32_t initial_count) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_counting_semaphore(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_counting_semaphore(this, max_count, initial_count);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosCountingSemaphore::~FreeRtosCountingSemaphore() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_counting_semaphore(this);
		handle_ = nullptr;
	}
}

bool FreeRtosCountingSemaphore::Take(std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_take_counting_semaphore(this, ticks_to_wait);
}

bool FreeRtosCountingSemaphore::Give() {
	return (handle_ != nullptr) && c7222_grader_give_counting_semaphore(this);
}

bool FreeRtosCountingSemaphore::GiveFromISR() {
	return (handle_ != nullptr) && c7222_grader_give_counting_semaphore_from_isr(this);
}

std::uint32_t FreeRtosCountingSemaphore::GetCount() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_get_counting_semaphore_count(this);
}

bool FreeRtosCountingSemaphore::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
