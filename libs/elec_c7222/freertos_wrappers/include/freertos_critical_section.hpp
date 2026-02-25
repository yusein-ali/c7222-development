/**
 * @file freertos_critical_section.hpp
 * @brief C++ wrapper for FreeRTOS-style critical sections.
 */
#ifndef ELEC_C7222_UTILS_FREERTOS_CRITICAL_SECTION_HPP
#define ELEC_C7222_UTILS_FREERTOS_CRITICAL_SECTION_HPP

#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class FreeRtosCriticalSection
 * @brief Critical-section wrapper with destructor-based exit cleanup.
 *
 * This wrapper protects short non-blocking regions that must not be interrupted
 * by context switches/interrupt handlers (platform dependent).
 *
 * The object tracks a binary entered state (`depth_` is 0 or 1). Calling
 * `Enter()` while already entered is a no-op for this object, and destruction
 * exits the critical section if it is still entered.
 *
 * Typical usage:
 * @code
 * c7222::FreeRtosCriticalSection cs;
 *
 * cs.Enter();
 * // very short non-blocking critical region
 * (void)cs.Exit();
 * @endcode
 */
class FreeRtosCriticalSection : public NonCopyableNonMovable {
  public:
	/** @brief Construct a critical-section wrapper in the unlocked state. */
	FreeRtosCriticalSection() = default;
	/** @brief Ensure the critical section is exited on destruction if entered. */
	~FreeRtosCriticalSection();

	/** @brief Enter the critical section (no-op if already entered by this object). */
	void Enter();
	/**
	 * @brief Exit the critical section.
	 * @return false if not currently entered.
	 */
	bool Exit();
	/** @brief @return true if this object is currently entered. */
	bool IsEntered() const;

  private:
	std::uint32_t depth_{0};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_CRITICAL_SECTION_HPP
