/**
 * @file led.hpp
 * @brief Output-only GPIO abstraction for LEDs.
 */
#ifndef ELEC_C7222_DEVICES_LED_H_
#define ELEC_C7222_DEVICES_LED_H_

#include <cstdint>

#include "gpio.hpp"

namespace c7222 {

/**
 * @class Led
 * @brief Output-only GPIO wrapper intended for LEDs.
 *
 * Design intent:
 * - **Output-only**: derives from `GpioOut` and deletes `Configure()` to
 *   prevent accidental reconfiguration into input mode.
 * - **LED-friendly API**: exposes `On()`, `Off()`, `Set()`, and `Toggle()`
 *   instead of raw GPIO writes.
 *
 * Default configuration:
 * - OutputType::PushPull
 * - PullMode::None
 * - Drive strength configurable (default 4 mA)
 *
 * Active-low support:
 * - Some boards (including the C7222 Pico W board LEDs) are wired active-low
 *   so a GPIO low turns the LED on.
 * - The `active_low` flag controls the logical-to-physical mapping so callers
 *   can use `On()`/`Off()` without worrying about wiring.
 *
 * Reconfiguration note:
 * - If the LED pin was temporarily used for PWM, call `Reconfigure()` after
 *   releasing PWM ownership to restore LED-safe GPIO settings.
 */
class Led : public GpioOut {
  public:
	/**
	 * @brief Construct an LED output pin.
	 *
	 * @param pin GPIO number.
	 * @param initial_on Initial LED state (default: false/off).
	 * @param drive Pad drive strength (default: 4 mA).
	 * @param active_low If true, LED is active-low (GPIO low = on).
	 */
	explicit Led(uint32_t pin,
	             bool initial_on = false,
	             GpioDriveStrength drive = GpioDriveStrength::mA4,
	             bool active_low = false);

	/**
	 * @brief Disallow generic reconfiguration.
	 *
	 * This prevents accidentally switching the LED pin into input mode.
	 */
	void Configure(const Config& config) = delete;

	/**
	 * @brief Reconfigure LED output settings.
	 *
	 * This is intended for cases where the pin was temporarily used for PWM
	 * and then returned to GPIO control. It only exposes LED-safe settings.
	 *
	 * @param initial_on Initial LED state (default: false/off).
	 * @param drive Pad drive strength (default: 4 mA).
	 * @param active_low If true, LED is active-low (GPIO low = on).
	 */
	void Reconfigure(bool initial_on = false,
	                 GpioDriveStrength drive = GpioDriveStrength::mA4,
	                 bool active_low = false);

	/**
	 * @brief Set the LED state.
	 * @param on true = on, false = off.
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
	 *
	 * Note: For active-low LEDs, toggling the GPIO level still toggles the LED.
	 */
	void Toggle();

  private:
	bool _active_low{false};
};

} // namespace c7222

#endif  // ELEC_C7222_DEVICES_LED_H_
