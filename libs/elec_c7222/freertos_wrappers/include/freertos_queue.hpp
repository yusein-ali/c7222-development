/**
 * @file freertos_queue.hpp
 * @brief C++ wrapper for FreeRTOS-style queues.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_QUEUE_HPP
#define ELEC_C7222_UTILS_FREERTOS_QUEUE_HPP

#include <cstddef>
#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosQueue
 * @brief RAII wrapper for a fixed-size item queue.
 *
 * This wrapper models the classic FreeRTOS queue behavior with task-context
 * and ISR-context send/receive helpers.
 */
class FreeRtosQueue : public NonCopyableNonMovable {
  public:
	/** @brief Construct an uninitialized queue wrapper. */
	FreeRtosQueue() = default;
	/**
	 * @brief Construct and initialize a queue.
	 * @param length Number of queue elements.
	 * @param item_size Size of each element in bytes.
	 */
	FreeRtosQueue(std::size_t length, std::size_t item_size);
	/**
	 * @brief Initialize (or re-initialize) the queue.
	 * @param length Number of queue elements.
	 * @param item_size Size of each element in bytes.
	 * @return true on success.
	 */
	bool Initialize(std::size_t length, std::size_t item_size);
	/** @brief Destroy queue resources if initialized. */
	~FreeRtosQueue();

	/**
	 * @brief Enqueue one item from task context.
	 * @param item Pointer to source item data.
	 * @param ticks_to_wait Max ticks to wait if full.
	 * @return true on success.
	 */
	bool Send(const void* item, std::uint32_t ticks_to_wait = 0);
	/** @brief Enqueue one item from ISR context. */
	bool SendFromISR(const void* item);
	/**
	 * @brief Dequeue one item from task context.
	 * @param out_item Pointer to destination storage.
	 * @param ticks_to_wait Max ticks to wait if empty.
	 * @return true if an item was received.
	 */
	bool Receive(void* out_item, std::uint32_t ticks_to_wait = 0);
	/** @brief Dequeue one item from ISR context. */
	bool ReceiveFromISR(void* out_item);
	/**
	 * @brief Overwrite queue storage with a new item.
	 *
	 * Intended for queue length of 1 (mailbox pattern).
	 */
	bool Overwrite(const void* item);
	/** @brief Reset the queue contents. */
	bool Reset();
	/** @brief @return Number of queued items. */
	std::size_t MessagesWaiting() const;
	/** @brief @return Number of free item slots. */
	std::size_t SpacesAvailable() const;
	/** @brief @return true if the wrapper owns a valid queue handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
	std::size_t length_{0};
	std::size_t item_size_{0};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_QUEUE_HPP
