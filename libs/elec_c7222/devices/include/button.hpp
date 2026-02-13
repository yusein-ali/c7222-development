/**
 * @file button.hpp
 * @brief Input-only GPIO abstraction for push-buttons.
 *
 * The Button class wraps a GPIO pin configured as input with pull-up/down and
 * provides a small, button-friendly API. It intentionally keeps the interface
 * focused: construct, read state, and optionally use the base `GpioIn` IRQ
 * facilities if needed.
 */
#ifndef C7222_BUTTON_HPP
#define C7222_BUTTON_HPP

#include <cstdint>
#include "gpio.hpp"

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 */
namespace c7222 {

/**
 * @class Button
 * @brief Input-only GPIO wrapper intended for push-buttons.
 *
 * Design intent:
 * - **Input-only**: derives from `GpioIn` and deletes `Configure()` to prevent
 *   reconfiguration into output mode.
 * - **Simple polling**: `IsPressed()` provides a clear, easy-to-read API for
 *   beginners. IRQ support is still available through the inherited `GpioIn`
 *   API when more advanced usage is needed.
 *
 * Wiring assumption (C7222 Pico W board):
 * - Buttons are wired **active-low** with a pull-up, so a press produces a
 *   HIGH->LOW transition.
 * - `IsPressed()` returns true when the logical state indicates a press and
 *   abstracts the active-low wiring from callers.
 *
 * Reconfiguration note:
 * - If the button pin was temporarily used for PWM, call `Reconfigure()` after
 *   releasing PWM ownership to restore input configuration.
 */
class Button : public GpioIn {
  public:
	/**
	 * @brief Construct a button pin in input mode.
	 *
	 * @param pin GPIO number.
	 * @param pull Pull configuration (default: PullUp).
	 */
	explicit Button(uint32_t pin, GpioPullMode pull = GpioPullMode::PullUp);
	/**
	 * @brief Destructor unregisters the button from the platform map.
	 */
	virtual ~Button() = default;

	/**
	 * @brief Disallow generic reconfiguration.
	 */
	void Configure(const Config& config) = delete;

	/**
	 * @brief Reconfigure the button pull mode.
	 *
	 * This is intended for cases where the pin was temporarily used for PWM
	 * and then returned to GPIO input control.
	 *
	 * @param pull Pull configuration (default: PullUp).
	 */
	void Reconfigure(GpioPullMode pull = GpioPullMode::PullUp);

	/**
	 * @brief Return true if the button is currently pressed (active-low).
	 */
	bool IsPressed() const;

};

} // namespace c7222

#endif // C7222_BUTTON_HPP
