/**
 * @file freertos_mutex.hpp
 * @brief C++ wrappers for FreeRTOS-style mutex primitives.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_MUTEX_HPP
#define ELEC_C7222_UTILS_FREERTOS_MUTEX_HPP

#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosMutex
 * @brief RAII wrapper for a standard mutex.
 *
 * This mutex is intended for mutual exclusion in task context.
 */
class FreeRtosMutex : public NonCopyableNonMovable {
  public:
	/** @brief Construct and initialize a mutex. */
	FreeRtosMutex();
	/** @brief Initialize (or re-initialize) the mutex. */
	bool Initialize();
	/** @brief Destroy mutex resources if initialized. */
	~FreeRtosMutex();

	/**
	 * @brief Acquire the mutex.
	 * @param ticks_to_wait Max ticks to wait (0 = non-blocking).
	 * @return true if acquired.
	 */
	bool Lock(std::uint32_t ticks_to_wait = 0);
	/** @brief Non-blocking lock attempt. */
	bool TryLock();
	/** @brief Release the mutex. */
	bool Unlock();
	/** @brief @return true if the wrapper owns a valid mutex handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

/**
 * @class FreeRtosRecursiveMutex
 * @brief RAII wrapper for a recursive mutex.
 *
 * A recursive mutex may be locked multiple times by the same owner and must be
 * unlocked the same number of times.
 */
class FreeRtosRecursiveMutex : public NonCopyableNonMovable {
  public:
	/** @brief Construct and initialize a recursive mutex. */
	FreeRtosRecursiveMutex();
	/** @brief Initialize (or re-initialize) the recursive mutex. */
	bool Initialize();
	/** @brief Destroy recursive mutex resources if initialized. */
	~FreeRtosRecursiveMutex();

	/**
	 * @brief Acquire the recursive mutex.
	 * @param ticks_to_wait Max ticks to wait (0 = non-blocking).
	 * @return true if acquired.
	 */
	bool Lock(std::uint32_t ticks_to_wait = 0);
	/** @brief Non-blocking lock attempt. */
	bool TryLock();
	/** @brief Release one recursion level. */
	bool Unlock();
	/** @brief @return true if the wrapper owns a valid recursive mutex handle. */
	bool IsValid() const;

  private:
	void* handle_{nullptr};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_MUTEX_HPP
