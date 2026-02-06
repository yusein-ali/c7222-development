/**
 * @file led.hpp
 * @brief Output-only GPIO abstraction for LEDs.
 */
#ifndef TEMPLATE_LED_HPP
#define TEMPLATE_LED_HPP

#include <cstdint>

#include "gpio.hpp"

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 *
 * This namespace is shared by all hardware abstractions used in the course.
 */
namespace c7222 {

/**
 * @class Led
 * @brief Output-only GPIO wrapper intended for LEDs.
 *
 * This class derives from GpioPin and enforces an output-only configuration.
 * It provides a small LED-friendly API (On/Off/Toggle).
 *
 * The constructor configures the pin as:
 *  - Direction::Output
 *  - OutputType::PushPull
 *  - PullMode::None
 * and optionally sets the pad drive strength.
 *
 * Some boards use active-low LEDs (logic low turns the LED on). This class
 * supports that via the active_low flag.
 */
class Led : public GpioPin {
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
	             DriveStrength drive = DriveStrength::mA4,
	             bool active_low = false)
	    : GpioPin(pin, MakeConfig(pin, initial_on, drive, active_low)),
	      _active_low(active_low) {}

	/**
	 * @brief Disallow generic reconfiguration.
	 *
	 * This prevents accidentally switching the LED pin into input mode.
	 */
	void Configure(const Config& config) = delete;

	/**
	 * @brief Set the LED state.
	 * @param on true = on, false = off.
	 */
	void Set(bool on) {
		Write(PhysicalLevel(on));
	}

	/**
	 * @brief Turn the LED on.
	 */
	void On() {
		Write(PhysicalLevel(true));
	}

	/**
	 * @brief Turn the LED off.
	 */
	void Off() {
		Write(PhysicalLevel(false));
	}

	/**
	 * @brief Toggle the LED state.
	 *
	 * Note: For active-low LEDs, toggling the GPIO level still toggles the LED.
	 */
	void Toggle() {
		GpioPin::Toggle();
	}

  private:
	bool _active_low{false};

	bool PhysicalLevel(bool led_on) const {
		// active_low: LED on => GPIO low
		return _active_low ? !led_on : led_on;
	}

	/**
	 * @brief Create a GpioPin::Config for output-only LED usage.
	 */
	static Config MakeConfig(uint32_t pin,
	                         bool initial_on,
	                         DriveStrength drive,
	                         bool active_low) {
		Config cfg(static_cast<int32_t>(pin));
		cfg.direction = Direction::Output;
		cfg.pull = PullMode::None;
		cfg.output_type = OutputType::PushPull;
		cfg.drive = drive;
		cfg.initial_state = active_low ? !initial_on : initial_on;
		return cfg;
	}
};

} // namespace c7222

#endif // TEMPLATE_LED_HPP
