#include "freertos_event_group.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_register_event_group(FreeRtosEventGroup* event_group);
void c7222_grader_unregister_event_group(FreeRtosEventGroup* event_group);
std::uint32_t c7222_grader_set_event_group_bits(FreeRtosEventGroup* event_group, std::uint32_t bits);
bool c7222_grader_set_event_group_bits_from_isr(FreeRtosEventGroup* event_group, std::uint32_t bits);
std::uint32_t c7222_grader_clear_event_group_bits(FreeRtosEventGroup* event_group, std::uint32_t bits);
std::uint32_t c7222_grader_wait_event_group_bits(FreeRtosEventGroup* event_group,
												  std::uint32_t bits_to_wait_for,
												  bool clear_on_exit,
												  bool wait_for_all_bits,
												  std::uint32_t ticks_to_wait);
std::uint32_t c7222_grader_get_event_group_bits(const FreeRtosEventGroup* event_group);
std::uint32_t c7222_grader_get_event_group_bits_from_isr(const FreeRtosEventGroup* event_group);
}

FreeRtosEventGroup::FreeRtosEventGroup() {
	(void)Initialize();
}

bool FreeRtosEventGroup::Initialize() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_event_group(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_event_group(this);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosEventGroup::~FreeRtosEventGroup() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_event_group(this);
		handle_ = nullptr;
	}
}

std::uint32_t FreeRtosEventGroup::SetBits(std::uint32_t bits) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_set_event_group_bits(this, bits);
}

bool FreeRtosEventGroup::SetBitsFromISR(std::uint32_t bits) {
	return (handle_ != nullptr) && c7222_grader_set_event_group_bits_from_isr(this, bits);
}

std::uint32_t FreeRtosEventGroup::ClearBits(std::uint32_t bits) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_clear_event_group_bits(this, bits);
}

std::uint32_t FreeRtosEventGroup::WaitBits(std::uint32_t bits_to_wait_for,
										   bool clear_on_exit,
										   bool wait_for_all_bits,
										   std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_wait_event_group_bits(this,
											  bits_to_wait_for,
											  clear_on_exit,
											  wait_for_all_bits,
											  ticks_to_wait);
}

std::uint32_t FreeRtosEventGroup::GetBits() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_get_event_group_bits(this);
}

std::uint32_t FreeRtosEventGroup::GetBitsFromISR() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_get_event_group_bits_from_isr(this);
}

bool FreeRtosEventGroup::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
