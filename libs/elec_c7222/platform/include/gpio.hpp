/**
 * @file gpio.hpp
 * @brief Generic GPIO pin abstraction with configuration options.
 */
#ifndef TEMPLATE_GPIO_HPP
#define TEMPLATE_GPIO_HPP

#include <cstdint>
#include <cassert>

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
	 */
	struct Config {

		int32_t _pin; ///< GPIO pin number (non-negative integer).
		Direction direction{Direction::Input}; ///< Direction mode (default: Input).
		PullMode pull{PullMode::None}; ///< Pull mode (default: None).
		OutputType output_type{OutputType::PushPull}; ///< Output type (default: PushPull).
		DriveStrength drive{DriveStrength::mA4}; ///< Drive strength (default: 4 mA).
		bool initial_state{false}; ///< Initial logic state when configured as output.
		/**
		 * @brief Create config with a pin number.
		 * @param pin GPIO number (non-negative).
		 */
		explicit Config(int32_t pin) {
			assert(pin >= 0);
			_pin = static_cast<uint32_t>(pin);
		}
		/**
		 * @brief Validate configuration for basic constraints.
		 * @return true if configuration is valid.
		 */
		bool validate() const {
			if(output_type == OutputType::OpenDrain && pull == PullMode::None) {
				return false;
			}
			return true;
		}
	};

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
	void configure(const Config& config);
	/**
	 * @brief Write an output value (no-op if configured as input).
	 * @param value Logic level to drive.
	 */
	void write(bool value);
	/**
	 * @brief Read the current GPIO level.
	 * @return Current logic level.
	 */
	bool read() const;
	/**
	 * @brief Toggle the pin output.
	 */
	void toggle();

	/**
	 * @brief Return the GPIO number.
	 */
	uint32_t pin() const {
		return _pin;
	}
	/**
	 * @brief Return the cached configuration.
	 */
	Config config() const {
		return _config;
	}

  private:
	/**
	 * @brief Apply internal configuration to the hardware.
	 */
	void apply_config();

	uint32_t _pin;
	Config _config;
};

} // namespace c7222

#endif // TEMPLATE_GPIO_HPP
