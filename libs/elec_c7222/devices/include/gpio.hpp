/**
 * @file gpio.hpp
 * @brief Generic GPIO pin abstraction with configuration options.
 *
 * Platform integration:
 * - The Raspberry Pico W backend (`platform/rpi_pico/gpio.cpp`) maps this API
 *   to the Pico SDK (`hardware/gpio.h`). It applies pull configuration,
 *   direction, output type, and drive strength using `gpio_init`,
 *   `gpio_set_drive_strength`, and the various `gpio_pull_*` helpers. For
 *   open-drain output, it switches the pin direction to float the line for
 *   logic high.
 * - The grader backend provides a stub implementation to allow tests to
 *   compile without hardware.
 */
#ifndef ELEC_C7222_DEVICES_GPIO_H_
#define ELEC_C7222_DEVICES_GPIO_H_

#include <cstdint>

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 *
 * This namespace is shared by all hardware abstractions used in the course.
 */
namespace c7222 {

/**
 * @class GpioPin
 * @brief Generic GPIO pin wrapper with configurable direction, pull mode,
 *        output type, and drive strength.
 *
 * This class stores a cached configuration and delegates hardware I/O to the
 * platform-specific backend. Use `Configure()` to apply a new configuration
 * and `Write()`/`Read()` to manipulate the pin.
 */
class GpioPin {
  public:
	/**
	 * @brief GPIO direction.
	 */
	enum class Direction {
		Input,  ///< Configure as input.
		Output ///< Configure as output.
	};

	/**
	 * @brief GPIO pull configuration.
	 */
	enum class PullMode {
		None,     ///< Disable pulls.
		PullUp,   ///< Enable pull-up.
		PullDown  ///< Enable pull-down.
	};

	/**
	 * @brief GPIO output type.
	 */
	enum class OutputType {
		PushPull, ///< Drive high/low actively.
		OpenDrain ///< Drive low or float.
	};

	/**
	 * @brief GPIO drive strength.
	 */
	enum class DriveStrength {
		mA2,  ///< 2 mA drive.
		mA4,  ///< 4 mA drive.
		mA8,  ///< 8 mA drive.
		mA12  ///< 12 mA drive.
	};
	
	/**
	 * @brief Configuration structure for GPIO pin setup.
	 *
	 * The `Validate()` helper enforces basic constraints (e.g. open-drain
	 * requires a pull-up or pull-down).
	 */
	struct Config {

		uint32_t pin_; ///< GPIO pin number (non-negative integer).
		Direction direction{Direction::Input}; ///< Direction mode (default: Input).
		PullMode pull{PullMode::None}; ///< Pull mode (default: None).
		OutputType output_type{OutputType::PushPull}; ///< Output type (default: PushPull).
		DriveStrength drive{DriveStrength::mA4}; ///< Drive strength (default: 4 mA).
		bool initial_state{false}; ///< Initial logic state when configured as output.
		/**
		 * @brief Create config with a pin number.
		 * @param pin GPIO number (non-negative).
		 */
		explicit Config(int32_t pin);
		/**
		 * @brief Validate configuration for basic constraints.
		 * @return true if configuration is valid.
		 */
		bool Validate() const;
	};

	/**
	 * @brief Create a GPIO pin with default configuration.
	 *
	 * The default configuration is input, no pull, push-pull, 4mA drive.
	 */
	explicit GpioPin(uint32_t pin);
	/**
	 * @brief Create a GPIO pin with the given configuration.
	 * @param pin GPIO number.
	 * @param config Initial configuration.
	 */
	explicit GpioPin(uint32_t pin, const Config& config);

	/**
	 * @brief Apply a new configuration.
	 * @param config New configuration.
	 */
	void Configure(const Config& config);
	/**
	 * @brief Write an output value (no-op if configured as input).
	 * @param value Logic level to drive.
	 */
	void Write(bool value);
	/**
	 * @brief Read the current GPIO level.
	 * @return Current logic level.
	 */
	bool Read() const;
	/**
	 * @brief Toggle the pin output.
	 *
	 * The exact behavior is implemented by the platform backend.
	 */
	void Toggle();

	/**
	 * @brief Return the GPIO number.
	 */
	uint32_t GetPin() const;
	/**
	 * @brief Return the cached configuration.
	 */
	Config GetConfig() const;

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

#endif  // ELEC_C7222_DEVICES_GPIO_H_
