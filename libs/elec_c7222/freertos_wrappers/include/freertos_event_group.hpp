/**
 * @file freertos_event_group.hpp
 * @brief C++ wrapper for FreeRTOS-style event groups.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_EVENT_GROUP_HPP
#define ELEC_C7222_UTILS_FREERTOS_EVENT_GROUP_HPP

#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosEventGroup
 * @brief Ownership-based wrapper for an event bit group.
 *
 * Event groups are useful for waiting on one or more flags across tasks. Wait
 * and bit operations are explicit; RAII here refers to handle cleanup.
 *
 * Typical usage:
 * @code
 * c7222::FreeRtosEventGroup events;
 * constexpr std::uint32_t kReadyBit = 1u << 0;
 *
 * // Producer:
 * (void)events.SetBits(kReadyBit);
 *
 * // Consumer:
 * const std::uint32_t bits = events.WaitBits(kReadyBit, true, true, 100);
 * if((bits & kReadyBit) != 0U) {
 *     // ready condition observed
 * }
 * @endcode
 */
class FreeRtosEventGroup : public NonCopyableNonMovable {
  public:
	/** @brief Construct and initialize an event group. */
	FreeRtosEventGroup();
	/** @brief Initialize (or re-initialize) the event group. */
	bool Initialize();
	/** @brief Destroy event group resources if initialized. */
	~FreeRtosEventGroup();

	/**
	 * @brief Set bits in the event group.
	 * @param bits Bit mask to set.
	 * @return Event bits value after setting.
	 */
	std::uint32_t SetBits(std::uint32_t bits);
	/**
	 * @brief Set bits from ISR context.
	 * @param bits Bit mask to set.
	 * @return true on success.
	 */
	bool SetBitsFromISR(std::uint32_t bits);
	/**
	 * @brief Clear bits in the event group.
	 * @param bits Bit mask to clear.
	 * @return Event bits value before clearing.
	 */
	std::uint32_t ClearBits(std::uint32_t bits);
	/**
	 * @brief Wait until required event bits are satisfied.
	 * @param bits_to_wait_for Target bits.
	 * @param clear_on_exit If true, clear requested bits before return.
	 * @param wait_for_all_bits If true, wait for all bits; otherwise any bit.
	 * @param ticks_to_wait Max ticks to wait.
	 * @return Event bits value on wakeup.
	 */
	std::uint32_t WaitBits(std::uint32_t bits_to_wait_for,
						   bool clear_on_exit,
						   bool wait_for_all_bits,
						   std::uint32_t ticks_to_wait);
	/** @brief @return Current event bits from task context. */
	std::uint32_t GetBits() const;
	/** @brief @return Current event bits from ISR context. */
	std::uint32_t GetBitsFromISR() const;
	/** @brief @return true if the wrapper owns a valid event group handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_EVENT_GROUP_HPP
