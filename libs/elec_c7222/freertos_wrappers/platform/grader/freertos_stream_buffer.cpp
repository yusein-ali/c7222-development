#include "freertos_stream_buffer.hpp"

namespace c7222 {

extern "C" {
bool c7222_grader_register_stream_buffer(FreeRtosStreamBuffer* stream_buffer,
										 std::size_t buffer_size_bytes,
										 std::size_t trigger_level_bytes);
void c7222_grader_unregister_stream_buffer(FreeRtosStreamBuffer* stream_buffer);
std::size_t c7222_grader_stream_buffer_send(FreeRtosStreamBuffer* stream_buffer,
											const void* data,
											std::size_t data_length,
											std::uint32_t ticks_to_wait);
std::size_t c7222_grader_stream_buffer_send_from_isr(FreeRtosStreamBuffer* stream_buffer,
													 const void* data,
													 std::size_t data_length);
std::size_t c7222_grader_stream_buffer_receive(FreeRtosStreamBuffer* stream_buffer,
											   void* out_data,
											   std::size_t out_length,
											   std::uint32_t ticks_to_wait);
std::size_t c7222_grader_stream_buffer_receive_from_isr(FreeRtosStreamBuffer* stream_buffer,
														void* out_data,
														std::size_t out_length);
bool c7222_grader_stream_buffer_reset(FreeRtosStreamBuffer* stream_buffer);
std::size_t c7222_grader_stream_buffer_bytes_available(const FreeRtosStreamBuffer* stream_buffer);
std::size_t c7222_grader_stream_buffer_spaces_available(const FreeRtosStreamBuffer* stream_buffer);

bool c7222_grader_register_message_buffer(FreeRtosMessageBuffer* message_buffer, std::size_t buffer_size_bytes);
void c7222_grader_unregister_message_buffer(FreeRtosMessageBuffer* message_buffer);
std::size_t c7222_grader_message_buffer_send(FreeRtosMessageBuffer* message_buffer,
											 const void* message,
											 std::size_t message_length,
											 std::uint32_t ticks_to_wait);
std::size_t c7222_grader_message_buffer_send_from_isr(FreeRtosMessageBuffer* message_buffer,
													  const void* message,
													  std::size_t message_length);
std::size_t c7222_grader_message_buffer_receive(FreeRtosMessageBuffer* message_buffer,
												void* out_message,
												std::size_t out_length,
												std::uint32_t ticks_to_wait);
std::size_t c7222_grader_message_buffer_receive_from_isr(FreeRtosMessageBuffer* message_buffer,
														 void* out_message,
														 std::size_t out_length);
bool c7222_grader_message_buffer_reset(FreeRtosMessageBuffer* message_buffer);
std::size_t c7222_grader_message_buffer_spaces_available(const FreeRtosMessageBuffer* message_buffer);
std::size_t c7222_grader_message_buffer_next_message_length(const FreeRtosMessageBuffer* message_buffer);
}

FreeRtosStreamBuffer::FreeRtosStreamBuffer(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes) {
	(void)Initialize(buffer_size_bytes, trigger_level_bytes);
}

bool FreeRtosStreamBuffer::Initialize(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_stream_buffer(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_stream_buffer(this, buffer_size_bytes, trigger_level_bytes);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosStreamBuffer::~FreeRtosStreamBuffer() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_stream_buffer(this);
		handle_ = nullptr;
	}
}

std::size_t FreeRtosStreamBuffer::Send(const void* data, std::size_t data_length, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_send(this, data, data_length, ticks_to_wait);
}

std::size_t FreeRtosStreamBuffer::SendFromISR(const void* data, std::size_t data_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_send_from_isr(this, data, data_length);
}

std::size_t FreeRtosStreamBuffer::Receive(void* out_data, std::size_t out_length, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_receive(this, out_data, out_length, ticks_to_wait);
}

std::size_t FreeRtosStreamBuffer::ReceiveFromISR(void* out_data, std::size_t out_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_receive_from_isr(this, out_data, out_length);
}

bool FreeRtosStreamBuffer::Reset() {
	return (handle_ != nullptr) && c7222_grader_stream_buffer_reset(this);
}

std::size_t FreeRtosStreamBuffer::BytesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_bytes_available(this);
}

std::size_t FreeRtosStreamBuffer::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_stream_buffer_spaces_available(this);
}

bool FreeRtosStreamBuffer::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosMessageBuffer::FreeRtosMessageBuffer(std::size_t buffer_size_bytes) {
	(void)Initialize(buffer_size_bytes);
}

bool FreeRtosMessageBuffer::Initialize(std::size_t buffer_size_bytes) {
	if(handle_ != nullptr) {
		c7222_grader_unregister_message_buffer(this);
		handle_ = nullptr;
	}
	const bool ok = c7222_grader_register_message_buffer(this, buffer_size_bytes);
	handle_ = ok ? this : nullptr;
	return ok;
}

FreeRtosMessageBuffer::~FreeRtosMessageBuffer() {
	if(handle_ != nullptr) {
		c7222_grader_unregister_message_buffer(this);
		handle_ = nullptr;
	}
}

std::size_t FreeRtosMessageBuffer::Send(const void* message,
										std::size_t message_length,
										std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_send(this, message, message_length, ticks_to_wait);
}

std::size_t FreeRtosMessageBuffer::SendFromISR(const void* message, std::size_t message_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_send_from_isr(this, message, message_length);
}

std::size_t FreeRtosMessageBuffer::Receive(void* out_message,
										   std::size_t out_length,
										   std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_receive(this, out_message, out_length, ticks_to_wait);
}

std::size_t FreeRtosMessageBuffer::ReceiveFromISR(void* out_message, std::size_t out_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_receive_from_isr(this, out_message, out_length);
}

bool FreeRtosMessageBuffer::Reset() {
	return (handle_ != nullptr) && c7222_grader_message_buffer_reset(this);
}

std::size_t FreeRtosMessageBuffer::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_spaces_available(this);
}

std::size_t FreeRtosMessageBuffer::NextMessageLength() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return c7222_grader_message_buffer_next_message_length(this);
}

bool FreeRtosMessageBuffer::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
