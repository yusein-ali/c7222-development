/**
 * @file freertos_semaphore.hpp
 * @brief C++ wrappers for FreeRTOS binary and counting semaphores.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_SEMAPHORE_HPP
#define ELEC_C7222_UTILS_FREERTOS_SEMAPHORE_HPP

#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosBinarySemaphore
 * @brief RAII wrapper for a FreeRTOS-style binary semaphore.
 *
 * Typical usage is event signaling between execution contexts
 * (task-to-task or ISR-to-task). Unlike a mutex, binary semaphores do not
 * provide ownership tracking or priority inheritance.
 */
class FreeRtosBinarySemaphore : public NonCopyableNonMovable {
  public:
	/** @brief Construct an uninitialized semaphore wrapper. */
	FreeRtosBinarySemaphore() = default;
	/**
	 * @brief Construct and initialize a binary semaphore.
	 * @param initially_given If true, semaphore starts in the available state.
	 */
	explicit FreeRtosBinarySemaphore(bool initially_given);
	/**
	 * @brief Initialize (or re-initialize) the semaphore.
	 * @param initially_given If true, semaphore starts in the available state.
	 * @return true on success.
	 */
	bool Initialize(bool initially_given = false);
	/** @brief Destroy semaphore resources if initialized. */
	~FreeRtosBinarySemaphore();

	/**
	 * @brief Take the semaphore.
	 * @param ticks_to_wait Max ticks to wait (0 = non-blocking).
	 * @return true if acquired.
	 */
	bool Take(std::uint32_t ticks_to_wait = 0);
	/**
	 * @brief Give the semaphore from task context.
	 * @return true on success.
	 */
	bool Give();
	/**
	 * @brief Give the semaphore from ISR context.
	 * @return true on success.
	 */
	bool GiveFromISR();
	/** @brief @return true if the wrapper owns a valid semaphore handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

/**
 * @class FreeRtosCountingSemaphore
 * @brief RAII wrapper for a FreeRTOS-style counting semaphore.
 *
 * Counting semaphores model a bounded resource count and can be used for
 * producer/consumer pacing, pooled-resource limits, and event accumulation.
 */
class FreeRtosCountingSemaphore : public NonCopyableNonMovable {
  public:
	/** @brief Construct an uninitialized counting semaphore wrapper. */
	FreeRtosCountingSemaphore() = default;
	/**
	 * @brief Construct and initialize a counting semaphore.
	 * @param max_count Maximum count value.
	 * @param initial_count Initial count value.
	 */
	FreeRtosCountingSemaphore(std::uint32_t max_count, std::uint32_t initial_count);
	/**
	 * @brief Initialize (or re-initialize) a counting semaphore.
	 * @param max_count Maximum count value.
	 * @param initial_count Initial count value.
	 * @return true on success.
	 */
	bool Initialize(std::uint32_t max_count, std::uint32_t initial_count);
	/** @brief Destroy semaphore resources if initialized. */
	~FreeRtosCountingSemaphore();

	/**
	 * @brief Take one token from the semaphore.
	 * @param ticks_to_wait Max ticks to wait (0 = non-blocking).
	 * @return true if a token was acquired.
	 */
	bool Take(std::uint32_t ticks_to_wait = 0);
	/**
	 * @brief Return one token to the semaphore from task context.
	 * @return true on success.
	 */
	bool Give();
	/**
	 * @brief Return one token to the semaphore from ISR context.
	 * @return true on success.
	 */
	bool GiveFromISR();
	/** @brief @return Current semaphore count. */
	std::uint32_t GetCount() const;
	/** @brief @return true if the wrapper owns a valid semaphore handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_SEMAPHORE_HPP
