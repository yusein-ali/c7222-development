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
 * Drive strength is applied in the platform-specific backend via GpioPin.
 */
class Led : public GpioPin {
  public:
	/**
	 * @brief Construct an LED output pin.
	 *
	 * @param pin GPIO number.
	 * @param initial_on Initial output state (default: false/off).
	 * @param drive Pad drive strength (default: 4 mA).
	 */
	explicit Led(uint32_t pin,
	             bool initial_on = false,
	             DriveStrength drive = DriveStrength::mA4)
	    : GpioPin(pin, MakeConfig(pin, initial_on, drive)) {}

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
		Write(on);
	}

	/**
	 * @brief Turn the LED on.
	 */
	void On() {
		Write(true);
	}

	/**
	 * @brief Turn the LED off.
	 */
	void Off() {
		Write(false);
	}

	/**
	 * @brief Toggle the LED state.
	 */
	void Toggle() {
		GpioPin::Toggle();
	}

  private:
	/**
	 * @brief Create a GpioPin::Config for output-only LED usage.
	 */
	static Config MakeConfig(uint32_t pin, bool initial_on, DriveStrength drive) {
		Config cfg(static_cast<int32_t>(pin));
		cfg.direction = Direction::Output;
		cfg.pull = PullMode::None;
		cfg.output_type = OutputType::PushPull;
		cfg.drive = drive;
		cfg.initial_state = initial_on;
		return cfg;
	}
};

} // namespace c7222

#endif // TEMPLATE_LED_HPP
