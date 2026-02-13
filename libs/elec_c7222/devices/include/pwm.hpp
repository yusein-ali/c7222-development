/**
 * @file pwm.hpp
 * @brief Simple PWM output wrapper with period and duty cycle configuration.
 *
 * This class provides a minimal PWM interface focused on:
 * - Period (microseconds)
 * - Duty cycle (0.0 to 1.0)
 *
 * Platform-specific behavior (Pico):
 * - The Pico backend configures the PWM slice/channel associated with the
 *   given GPIO and computes TOP + divider from the requested period.
 * - Ownership is enforced per GPIO pin. Constructing two `PwmOut` objects for
 *   the same pin asserts in debug builds.
 * - When PWM is disabled (via `Enable(false)`), the pin function is returned
 *   to `GPIO_FUNC_SIO` so it can be used as a normal GPIO again.
 * - When the object is destroyed, the pin is also returned to `GPIO_FUNC_SIO`
 *   and ownership is released.
 *
 * Edge cases and usage notes:
 * - A GPIO cannot be driven by PWM and normal GPIO (e.g., `Led`) at the same
 *   time. To switch from PWM to LED control, call `Enable(false)` (or destroy
 *   the `PwmOut`) before constructing a `Led` on the same pin.
 * - If you re-enable PWM after using the pin as GPIO, call `Enable(true)` or
 *   `Configure(...)` to reapply the PWM configuration.
 *
 * Example: basic PWM output
 * @code{.cpp}
 * c7222::PwmOut pwm(15);
 * pwm.SetPeriodUs(1000.0f);   // 1 kHz
 * pwm.SetDutyCycle(0.25f);    // 25%
 * pwm.SetActiveLow(false);
 * @endcode
 *
 * Example: release PWM and reconfigure LED on same pin
 * @code{.cpp}
 * c7222::PwmOut pwm(20, {20, 2000.0f, 0.5f, true, true});
 * pwm.Enable(false);          // return pin to GPIO
 *
 * c7222::Led led(20);
 * led.Reconfigure(true, c7222::GpioDriveStrength::mA4, true);
 * @endcode
 */
#ifndef ELEC_C7222_DEVICES_PWM_HPP
#define ELEC_C7222_DEVICES_PWM_HPP

#include <cstdint>

#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class PwmOut
 * @brief Output-only PWM wrapper with period and duty configuration.
 *
 * This class provides a minimal PWM interface:
 * - Period in microseconds
 * - Duty cycle as a fraction [0.0, 1.0]
 *
 * Platform backends map this to the underlying PWM hardware and enforce
 * single ownership per GPIO pin.
 */
class PwmOut : public NonCopyableNonMovable {
  public:
	/**
	 * @brief Configuration structure for PWM output setup.
	 */
	struct Config {
		/// GPIO pin number.
		uint32_t pin_;
		/// PWM period in microseconds.
		float period_us{1000.0f};
		/// Duty cycle fraction [0.0, 1.0].
		float duty_cycle{0.0f};
		/// Enable PWM output on init.
		bool enabled{true};
		/// Invert PWM output polarity when true (active-low).
		bool active_low{false};

		/**
		 * @brief Create config with a pin number.
		 * @param pin GPIO number (non-negative).
		 */
		explicit Config(uint32_t pin);
		/**
		 * @brief Validate configuration.
		 * @return true if configuration is valid.
		 */
		bool Validate() const;
	};

	/**
	 * @brief Create a PWM output with default configuration.
	 */
	explicit PwmOut(uint32_t pin);
	/**
	 * @brief Create a PWM output with the given configuration.
	 * @param pin GPIO number.
	 * @param config Initial configuration.
	 */
	explicit PwmOut(uint32_t pin, const Config& config);

	/**
	 * @brief Destructor.
	 *
	 * Platform backends may unregister pin ownership here.
	 */
	virtual ~PwmOut();

	/**
	 * @brief Apply a new configuration.
	 * @param config New configuration.
	 */
	void Configure(const Config& config);
	/**
	 * @brief Set PWM period in microseconds.
	 */
	void SetPeriodUs(float period_us);
	/**
	 * @brief Set PWM duty cycle fraction [0.0, 1.0].
	 */
	void SetDutyCycle(float duty_cycle);
	/**
	 * @brief Set PWM polarity (active-low when true).
	 */
	void SetActiveLow(bool active_low);
	/**
	 * @brief Enable or disable the PWM output.
	 */
	void Enable(bool on);

	/**
	 * @brief Return the GPIO number.
	 */
	uint32_t GetPin() const {
		return pin_;
	}
	/**
	 * @brief Return the cached configuration.
	 */
	Config GetConfig() const {
		return config_;
	}

  protected:
	/**
	 * @brief Apply internal configuration to the hardware.
	 *
	 * Implemented by the platform backend.
	 */
	void ApplyConfig();

	/// GPIO pin number managed by this instance.
	uint32_t pin_;
	/// Cached configuration used to (re)apply platform settings.
	Config config_;
};

} // namespace c7222

#endif // ELEC_C7222_DEVICES_PWM_HPP
