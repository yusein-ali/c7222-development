#include "freertos_stream_buffer.hpp"

#include "FreeRTOS.h"
#include "message_buffer.h"
#include "stream_buffer.h"

namespace c7222 {

FreeRtosStreamBuffer::FreeRtosStreamBuffer(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes) {
	(void)Initialize(buffer_size_bytes, trigger_level_bytes);
}

bool FreeRtosStreamBuffer::Initialize(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes) {
	if(handle_ != nullptr) {
		vStreamBufferDelete(static_cast<StreamBufferHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xStreamBufferCreate(buffer_size_bytes, trigger_level_bytes);
	return handle_ != nullptr;
}

FreeRtosStreamBuffer::~FreeRtosStreamBuffer() {
	if(handle_ != nullptr) {
		vStreamBufferDelete(static_cast<StreamBufferHandle_t>(handle_));
		handle_ = nullptr;
	}
}

std::size_t FreeRtosStreamBuffer::Send(const void* data, std::size_t data_length, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xStreamBufferSend(static_cast<StreamBufferHandle_t>(handle_),
													  data,
													  data_length,
													  static_cast<TickType_t>(ticks_to_wait)));
}

std::size_t FreeRtosStreamBuffer::SendFromISR(const void* data, std::size_t data_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return static_cast<std::size_t>(xStreamBufferSendFromISR(static_cast<StreamBufferHandle_t>(handle_),
															 data,
															 data_length,
															 &higher_priority_task_woken));
}

std::size_t FreeRtosStreamBuffer::Receive(void* out_data, std::size_t out_length, std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xStreamBufferReceive(static_cast<StreamBufferHandle_t>(handle_),
														 out_data,
														 out_length,
														 static_cast<TickType_t>(ticks_to_wait)));
}

std::size_t FreeRtosStreamBuffer::ReceiveFromISR(void* out_data, std::size_t out_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return static_cast<std::size_t>(xStreamBufferReceiveFromISR(static_cast<StreamBufferHandle_t>(handle_),
																out_data,
																out_length,
																&higher_priority_task_woken));
}

bool FreeRtosStreamBuffer::Reset() {
	if(handle_ == nullptr) {
		return false;
	}
	return xStreamBufferReset(static_cast<StreamBufferHandle_t>(handle_)) == pdPASS;
}

std::size_t FreeRtosStreamBuffer::BytesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xStreamBufferBytesAvailable(static_cast<StreamBufferHandle_t>(handle_)));
}

std::size_t FreeRtosStreamBuffer::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xStreamBufferSpacesAvailable(static_cast<StreamBufferHandle_t>(handle_)));
}

bool FreeRtosStreamBuffer::IsValid() const {
	return handle_ != nullptr;
}

FreeRtosMessageBuffer::FreeRtosMessageBuffer(std::size_t buffer_size_bytes) {
	(void)Initialize(buffer_size_bytes);
}

bool FreeRtosMessageBuffer::Initialize(std::size_t buffer_size_bytes) {
	if(handle_ != nullptr) {
		vMessageBufferDelete(static_cast<MessageBufferHandle_t>(handle_));
		handle_ = nullptr;
	}
	handle_ = xMessageBufferCreate(buffer_size_bytes);
	return handle_ != nullptr;
}

FreeRtosMessageBuffer::~FreeRtosMessageBuffer() {
	if(handle_ != nullptr) {
		vMessageBufferDelete(static_cast<MessageBufferHandle_t>(handle_));
		handle_ = nullptr;
	}
}

std::size_t FreeRtosMessageBuffer::Send(const void* message,
										std::size_t message_length,
										std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xMessageBufferSend(static_cast<MessageBufferHandle_t>(handle_),
													   message,
													   message_length,
													   static_cast<TickType_t>(ticks_to_wait)));
}

std::size_t FreeRtosMessageBuffer::SendFromISR(const void* message, std::size_t message_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return static_cast<std::size_t>(xMessageBufferSendFromISR(static_cast<MessageBufferHandle_t>(handle_),
															  message,
															  message_length,
															  &higher_priority_task_woken));
}

std::size_t FreeRtosMessageBuffer::Receive(void* out_message,
										   std::size_t out_length,
										   std::uint32_t ticks_to_wait) {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xMessageBufferReceive(static_cast<MessageBufferHandle_t>(handle_),
														  out_message,
														  out_length,
														  static_cast<TickType_t>(ticks_to_wait)));
}

std::size_t FreeRtosMessageBuffer::ReceiveFromISR(void* out_message, std::size_t out_length) {
	if(handle_ == nullptr) {
		return 0;
	}
	BaseType_t higher_priority_task_woken = pdFALSE;
	return static_cast<std::size_t>(xMessageBufferReceiveFromISR(static_cast<MessageBufferHandle_t>(handle_),
																 out_message,
																 out_length,
																 &higher_priority_task_woken));
}

bool FreeRtosMessageBuffer::Reset() {
	if(handle_ == nullptr) {
		return false;
	}
	return xMessageBufferReset(static_cast<MessageBufferHandle_t>(handle_)) == pdPASS;
}

std::size_t FreeRtosMessageBuffer::SpacesAvailable() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(xMessageBufferSpaceAvailable(static_cast<MessageBufferHandle_t>(handle_)));
}

std::size_t FreeRtosMessageBuffer::NextMessageLength() const {
	if(handle_ == nullptr) {
		return 0;
	}
	return static_cast<std::size_t>(
		xMessageBufferNextLengthBytes(static_cast<MessageBufferHandle_t>(handle_)));
}

bool FreeRtosMessageBuffer::IsValid() const {
	return handle_ != nullptr;
}

} // namespace c7222
