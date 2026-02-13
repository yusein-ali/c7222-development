/**
 * @file gpio.hpp
 * @brief GPIO abstraction split into explicit input and output classes.
 *
 * Design principles:
 * - **Separate input and output roles**: `GpioIn` and `GpioOut` are distinct
 *   classes. This prevents accidental misuse (e.g., writing to inputs or
 *   reconfiguring outputs as inputs) and keeps configuration options focused
 *   on what each direction actually needs.
 * - **Single ownership per pin**: Each GPIO pin is owned by at most one
 *   `GpioIn` or `GpioOut` instance at a time. Platform backends track pin
 *   ownership and assert if a second object is constructed for the same pin.
 * - **Config-as-state**: Each class has a `Config` struct that captures all
 *   relevant configuration fields. The object stores a cached config and
 *   `Configure()` reapplies it on the platform backend.
 * - **Minimal, explicit API**: Inputs expose `Read()` and IRQ management;
 *   outputs expose `Write()` and `Toggle()`. This keeps the surface small for
 *   junior programmers and reduces ambiguous behaviors.
 *
 * Platform integration (Raspberry Pi Pico W):
 * - The Pico backend lives in `platform/rpi_pico/gpio.cpp` and maps these
 *   abstractions onto the Pico SDK (`hardware/gpio.h`).
 * - **Input handling**:
 *   - `gpio_init()` initializes the pin.
 *   - Pull configuration uses `gpio_disable_pulls`, `gpio_pull_up`, or
 *     `gpio_pull_down`.
 *   - Direction is set to input (`GPIO_IN`).
 *   - Optional IRQs are enabled via `gpio_set_irq_enabled_with_callback`.
 *     The backend stores a per-pin map from GPIO number to a single `GpioIn`
 *     instance. This enforces **one input object per pin** and ensures a
 *     single callback dispatch target.
 * - **Output handling**:
 *   - `gpio_set_drive_strength()` applies pad drive strength.
 *   - For push-pull output, the pin is set `GPIO_OUT` and driven high/low.
 *   - For open-drain output, the SDK has no native open-drain mode; the
 *     backend emulates it by switching direction:
 *       - Drive low: set output low and `GPIO_OUT`.
 *       - Release high: set `GPIO_IN` (high-Z) so an external/internal pull-up
 *         can pull the line high.
 *   - The backend tracks one `GpioOut` per pin and asserts on duplicates.
 *
 * Platform integration (grader backend):
 * - The grader backend provides stub implementations that compile and mirror
 *   the API shape without touching hardware.
 * - It also enforces the same one-object-per-pin rule for consistency.
 */
#ifndef ELEC_C7222_DEVICES_GPIO_H_
#define ELEC_C7222_DEVICES_GPIO_H_

#include <cstdint>
#include <functional>
#include "non_copyable.hpp"

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 *
 * This namespace is shared by all hardware abstractions used in the course.
 */
namespace c7222 {

/**
 * @brief GPIO pull configuration.
 */
enum class GpioPullMode {
	None,     ///< Disable pulls.
	PullUp,   ///< Enable pull-up.
	PullDown  ///< Enable pull-down.
};

/**
 * @brief GPIO output type.
 */
enum class GpioOutputType {
	HighZ, ///< High-impedance (not driven).
	PushPull, ///< Drive high/low actively.
	OpenDrain ///< Drive low or float.
};

/**
 * @brief GPIO drive strength.
 */
enum class GpioDriveStrength {
	mA2,  ///< 2 mA drive.
	mA4,  ///< 4 mA drive.
	mA8,  ///< 8 mA drive.
	mA12  ///< 12 mA drive.
};

enum class GpioInputEvent : uint32_t {
	None = 0x00,
	LevelLow  = 0x01,
	LevelHigh = 0x02,
	FallingEdge = 0x04,
	RisingEdge = 0x08
};

/**
 * @class GpioIn
 * @brief GPIO input-only wrapper with pull configuration and optional IRQ.
 *
 * This class owns an input pin configuration and exposes `Read()` plus IRQ
 * enable/disable helpers. Only pull mode and input events are configurable.
 *
 * IRQ behavior:
 * - `EnableIrq(events, handler)` registers a callback and enables GPIO IRQs
 *   on the platform backend.
 * - The `events` argument is a bitmask of `GpioInputEvent` values. Multiple
 *   events can be OR-ed together to enable several triggers at once.
 * - The backend passes the raw event bitmask to `handler` whenever an IRQ
 *   fires, so the handler can inspect which edge/level caused the interrupt.
 *
 * Event masking:
 * - `GpioInputEvent` values are bit flags (LevelLow, LevelHigh, FallingEdge,
 *   RisingEdge). The platform maps these directly to the underlying Pico SDK
 *   IRQ masks.
 * - Example: `EnableIrq(GpioInputEvent::FallingEdge | GpioInputEvent::RisingEdge, cb)`
 *   enables both edges on the same pin.
 */
class GpioIn: public NonCopyableNonMovable {
  public:
	/**
	 * @brief Configuration structure for input GPIO setup.
	 */
	struct Config {
		uint32_t pin_; ///< GPIO pin number (non-negative integer).
		GpioPullMode pull{GpioPullMode::None}; ///< Pull mode (default: None).
		GpioInputEvent input_events{GpioInputEvent::None}; ///< Input events to trigger IRQ (default: None).
		std::function<void(uint32_t)> irq_handler{}; ///< Optional IRQ handler callback. The function argument is bit field of events that triggered the IRQ.

		/**
		 * @brief Create config with a pin number.
		 * @param pin GPIO number (non-negative).
		 */
		explicit Config(uint32_t pin);
		/**
		 * @brief Validate configuration for basic constraints.
		 * @return true if configuration is valid.
		 */
		bool Validate() const;

		bool operator==(const Config& other) const;
	};

	/**
	 * @brief Create a GPIO input with default configuration.
	 */
	explicit GpioIn(uint32_t pin) : GpioIn(pin, Config(pin)) {}
	/**
	 * @brief Create a GPIO input with the given configuration.
	 * @param pin GPIO number.
	 * @param config Initial configuration.
	 */
	explicit GpioIn(uint32_t pin, const Config& config);

	/**
	 * @brief Destructor.
	 *
	 * Platform backends may use this to unregister the pin from internal maps.
	 */
	virtual ~GpioIn();

	/**
	 * @brief Enable GPIO IRQs for the given event set and handler.
	 *
	 * @param events Bitmask of input events to enable.
	 * @param handler Callback invoked from IRQ context.
	 */
	void EnableIrq(GpioInputEvent events, std::function<void(uint32_t)> handler);

	/**
	 * @brief Disable any IRQs configured for this input.
	 */
	void DisableIrq();

	/**
	 * @brief Apply a new configuration.
	 * @param config New configuration.
	 */
	void Configure(const Config& config);
	/**
	 * @brief Read the current GPIO level.
	 * @return Current logic level.
	 */
	bool Read() const;

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

	/**
	 * @brief Return true if an IRQ handler is currently registered.
	 */
	bool HasHandler() const{
		return static_cast<bool>(config_.irq_handler);
	}

	/**
	 * @brief Invoke the registered IRQ handler (if any).
	 *
	 * Used by platform IRQ dispatchers.
	 */
	void CallIrqHandler(uint32_t events) const;

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

/**
 * @class GpioOut
 * @brief GPIO output-only wrapper with output configuration and drive strength.
 *
 * This class owns an output pin configuration and exposes `Write()`/`Toggle()`.
 * Output configuration includes output type (push-pull vs. open-drain), pull
 * mode, drive strength, and the initial output state.
 */
class GpioOut : public NonCopyableNonMovable {
  public:
	/**
	 * @brief Configuration structure for output GPIO setup.
	 */
	struct Config {
		uint32_t pin_; ///< GPIO pin number (non-negative integer).
		GpioPullMode pull{GpioPullMode::None}; ///< Pull mode (default: None).
		GpioOutputType output_type{GpioOutputType::PushPull}; ///< Output type (default: PushPull).
		GpioDriveStrength drive{GpioDriveStrength::mA4}; ///< Drive strength (default: 4 mA).
		bool initial_state{false}; ///< Initial logic state when configured as output.

		/**
		 * @brief Create config with a pin number.
		 * @param pin GPIO number (non-negative).
		 */
		explicit Config(uint32_t pin);
		/**
		 * @brief Validate configuration for basic constraints.
		 * @return true if configuration is valid.
		 */
		bool Validate() const;

		bool operator==(const Config& other) const;
	};

	/**
	 * @brief Create a GPIO output with default configuration.
	 */
	explicit GpioOut(uint32_t pin) : GpioOut(pin, Config(pin)) {}

	/**
	 * @brief Destructor.
	 *
	 * Platform backends may use this to unregister the pin from internal maps.
	 */
	virtual ~GpioOut();
	/**
	 * @brief Create a GPIO output with the given configuration.
	 * @param pin GPIO number.
	 * @param config Initial configuration.
	 */
	explicit GpioOut(uint32_t pin, const Config& config);

	/**
	 * @brief Apply a new configuration.
	 * @param config New configuration.
	 */
	void Configure(const Config& config);
	/**
	 * @brief Write an output value.
	 * @param value Logic level to drive.
	 */
	void Write(bool value);
	/**
	 * @brief Toggle the pin output.
	 */
	void Toggle();

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

#endif  // ELEC_C7222_DEVICES_GPIO_H_
