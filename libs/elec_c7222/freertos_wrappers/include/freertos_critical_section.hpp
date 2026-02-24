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
 * @brief RAII-style wrapper for entering/leaving a critical section.
 *
 * This wrapper protects short non-blocking regions that must not be interrupted
 * by context switches/interrupt handlers (platform dependent).
 */
class FreeRtosCriticalSection : public NonCopyableNonMovable {
  public:
	/** @brief Construct a critical-section wrapper in the unlocked state. */
	FreeRtosCriticalSection() = default;
	/** @brief Ensure all entered critical levels are exited on destruction. */
	~FreeRtosCriticalSection();

	/** @brief Enter one critical-section nesting level. */
	void Enter();
	/**
	 * @brief Exit one critical-section nesting level.
	 * @return false if not currently entered.
	 */
	bool Exit();
	/** @brief @return true when at least one level is entered. */
	bool IsEntered() const;

  private:
	std::uint32_t depth_{0};
};

} // namespace c7222

#endif // ELEC_C7222_UTILS_FREERTOS_CRITICAL_SECTION_HPP
