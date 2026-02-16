#include "freertos_queue.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_register_queue(FreeRtosQueue* queue, std::size_t length, std::size_t item_size);
void c7222_grader_unregister_queue(FreeRtosQueue* queue);
bool c7222_grader_queue_send(FreeRtosQueue* queue, const void* item, std::uint32_t ticks_to_wait);
bool c7222_grader_queue_send_from_isr(FreeRtosQueue* queue, const void* item);
bool c7222_grader_queue_receive(FreeRtosQueue* queue, void* out_item, std::uint32_t ticks_to_wait);
bool c7222_grader_queue_receive_from_isr(FreeRtosQueue* queue, void* out_item);
bool c7222_grader_queue_overwrite(FreeRtosQueue* queue, const void* item);
bool c7222_grader_queue_reset(FreeRtosQueue* queue);
std::size_t c7222_grader_queue_messages_waiting(const FreeRtosQueue* queue);
std::size_t c7222_grader_queue_spaces_available(const FreeRtosQueue* queue);
}

FreeRtosQueue::FreeRtosQueue(std::size_t length, std::size_t item_size) {
	(void)Initialize(length, item_size);
}

bool FreeRtosQueue::Initialize(std::size_t length, std::size_t item_size) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_queue(this);
		handle_ = nullptr;
	}
	length_ = length;
	item_size_ = item_size;
	const bool ok = c7222_grader_register_queue(this, length, item_size);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosQueue::~FreeRtosQueue() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_queue(this);
		handle_ = nullptr;
	}
}

bool FreeRtosQueue::Send(const void* item, std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_queue_send(this, item, ticks_to_wait);
}

bool FreeRtosQueue::SendFromISR(const void* item) {
	return (handle_ != nullptr) && c7222_grader_queue_send_from_isr(this, item);
}

bool FreeRtosQueue::Receive(void* out_item, std::uint32_t ticks_to_wait) {
	return (handle_ != nullptr) && c7222_grader_queue_receive(this, out_item, ticks_to_wait);
}

bool FreeRtosQueue::ReceiveFromISR(void* out_item) {
	return (handle_ != nullptr) && c7222_grader_queue_receive_from_isr(this, out_item);
}

bool FreeRtosQueue::Overwrite(const void* item) {
	return (handle_ != nullptr) && c7222_grader_queue_overwrite(this, item);
}

bool FreeRtosQueue::Reset() {
	return (handle_ != nullptr) && c7222_grader_queue_reset(this);
}

std::size_t FreeRtosQueue::MessagesWaiting() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_queue_messages_waiting(this);
}

std::size_t FreeRtosQueue::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_queue_spaces_available(this);
}

bool FreeRtosQueue::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
