/**
 * @file onboard_led.hpp
 * @brief On-board LED abstraction for the C7222 course.
 *
 * This class wraps the board's built-in status LED, providing a small,
 * beginner-friendly API. The platform backends select the appropriate
 * hardware LED (either the Pico default LED pin or the CYW43 Wi-Fi LED on
 * Pico W boards).
 */
#ifndef TEMPLATE_BOARD_LED_HPP
#define TEMPLATE_BOARD_LED_HPP

#include <memory>

#include "non_copyable.hpp"
namespace c7222 {

/**
 * @class OnBoardLED
 * @brief Simple on-board LED controller.
 *
 * Design intent:
 * - **Singleton access**: only one on-board LED exists, so a singleton is used.
 * - **Explicit initialization**: call `Initialize()` once before using `On()`,
 *   `Off()`, or `Toggle()`. The Platform does not auto-initialize this device.
 *
 * Example:
 * @code{.cpp}
 * auto* led = c7222::OnBoardLED::GetInstance();
 * led->Initialize();
 * led->On();
 * @endcode
 */
class OnBoardLED : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Get the singleton instance.
	 */
	static OnBoardLED* GetInstance();

	/**
	 * @brief Initialize the on-board LED hardware.
	 *
	 * On Pico W, this may initialize the CYW43 subsystem if required.
	 * Returns true on success.
	 */
	bool Initialize();
	/**
	 * @brief Set the LED state.
	 * @param on true to turn on, false to turn off.
	 */
	void Set(bool on);
	/**
	 * @brief Turn the LED on.
	 */
	void On();
	/**
	 * @brief Turn the LED off.
	 */
	void Off();
	/**
	 * @brief Toggle the LED state.
	 */
	void Toggle();
	/**
	 * @brief Destructor.
	 *
	 * Public to allow std::unique_ptr default deleter.
	 */
	~OnBoardLED() = default;
  private:
	/**
	 * @brief Private constructor for singleton usage.
	 */
	OnBoardLED();
	/// Singleton instance (lazy-initialized).
	static std::unique_ptr<OnBoardLED> instance_;
	/// True after successful `Initialize()`.
	bool initialized_{false};
	/// Cached LED state (best-effort for platforms with no readback).
	bool state_{false};
};

} // namespace c7222

#endif // TEMPLATE_BOARD_LED_HPP
