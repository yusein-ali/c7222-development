/**
 * @file freertos_stream_buffer.hpp
 * @brief C++ wrappers for FreeRTOS-style stream and message buffers.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_STREAM_BUFFER_HPP
#define ELEC_C7222_UTILS_FREERTOS_STREAM_BUFFER_HPP

#include <cstddef>
#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosStreamBuffer
 * @brief RAII wrapper for byte-stream buffering.
 *
 * Stream buffers provide contiguous byte FIFO semantics and are useful for
 * variable chunk transport where record/message boundaries are not required.
 */
class FreeRtosStreamBuffer : public NonCopyableNonMovable {
  public:
	/** @brief Construct an uninitialized stream buffer wrapper. */
	FreeRtosStreamBuffer() = default;
	/**
	 * @brief Construct and initialize a stream buffer.
	 * @param buffer_size_bytes Total buffer storage size.
	 * @param trigger_level_bytes Wake threshold for readers.
	 */
	FreeRtosStreamBuffer(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes);
	/**
	 * @brief Initialize (or re-initialize) the stream buffer.
	 * @return true on success.
	 */
	bool Initialize(std::size_t buffer_size_bytes, std::size_t trigger_level_bytes);
	/** @brief Destroy stream buffer resources if initialized. */
	~FreeRtosStreamBuffer();

	/** @brief Send bytes from task context. @return bytes accepted. */
	std::size_t Send(const void* data, std::size_t data_length, std::uint32_t ticks_to_wait = 0);
	/** @brief Send bytes from ISR context. @return bytes accepted. */
	std::size_t SendFromISR(const void* data, std::size_t data_length);
	/** @brief Receive bytes from task context. @return bytes received. */
	std::size_t Receive(void* out_data, std::size_t out_length, std::uint32_t ticks_to_wait = 0);
	/** @brief Receive bytes from ISR context. @return bytes received. */
	std::size_t ReceiveFromISR(void* out_data, std::size_t out_length);
	/** @brief Reset stream buffer state. */
	bool Reset();
	/** @brief @return Number of readable bytes. */
	std::size_t BytesAvailable() const;
	/** @brief @return Number of writable bytes. */
	std::size_t SpacesAvailable() const;
	/** @brief @return true if the wrapper owns a valid stream buffer handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

/**
 * @class FreeRtosMessageBuffer
 * @brief RAII wrapper for discrete message buffering.
 *
 * Message buffers preserve message boundaries and are suitable for variable-size
 * packet/message transfer between contexts.
 */
class FreeRtosMessageBuffer : public NonCopyableNonMovable {
  public:
	/** @brief Construct an uninitialized message buffer wrapper. */
	FreeRtosMessageBuffer() = default;
	/** @brief Construct and initialize a message buffer. */
	explicit FreeRtosMessageBuffer(std::size_t buffer_size_bytes);
	/** @brief Initialize (or re-initialize) a message buffer. */
	bool Initialize(std::size_t buffer_size_bytes);
	/** @brief Destroy message buffer resources if initialized. */
	~FreeRtosMessageBuffer();

	/** @brief Send one message from task context. @return bytes accepted. */
	std::size_t Send(const void* message, std::size_t message_length, std::uint32_t ticks_to_wait = 0);
	/** @brief Send one message from ISR context. @return bytes accepted. */
	std::size_t SendFromISR(const void* message, std::size_t message_length);
	/** @brief Receive one message from task context. @return bytes received. */
	std::size_t Receive(void* out_message, std::size_t out_length, std::uint32_t ticks_to_wait = 0);
	/** @brief Receive one message from ISR context. @return bytes received. */
	std::size_t ReceiveFromISR(void* out_message, std::size_t out_length);
	/** @brief Reset message buffer state. */
	bool Reset();
	/** @brief @return Writable bytes available for future messages. */
	std::size_t SpacesAvailable() const;
	/** @brief @return Length of next queued message in bytes. */
	std::size_t NextMessageLength() const;
	/** @brief @return true if the wrapper owns a valid message buffer handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_STREAM_BUFFER_HPP
