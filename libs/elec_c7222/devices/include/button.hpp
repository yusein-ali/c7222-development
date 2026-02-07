/**
 * @file button.hpp
 * @brief Input-only GPIO abstraction for push-buttons with IRQ handler support.
 *
 * The Button class wraps a GPIO pin configured as input with pull-up/down and
 * provides a simple IRQ handler mechanism. Platform backends keep a mapping
 * from GPIO pin to one or more Button instances; when an IRQ fires, all
 * registered Button objects for that pin are notified.
 */
#ifndef C7222_BUTTON_HPP
#define C7222_BUTTON_HPP

#include <cstdint>
#include <functional>

#include "gpio.hpp"
#include "non_copyable.hpp"

/**
 * @namespace c7222
 * @brief C7222 course abstractions namespace.
 */
namespace c7222 {

/**
 * @class Button
 * @brief Input-only GPIO wrapper intended for push-buttons.
 *
 * This class derives from GpioPin but restricts configuration to input mode.
 * It supports registering a handler that is invoked on a falling-edge IRQ.
 *
 * The platform backend keeps a per-pin list of Button objects. This allows
 * multiple Button instances to observe the same GPIO pin. IRQ enable/disable
 * respects the presence of other handlers on the same pin.
 *
 * Board assumption (typical for the C7222 Pico W board): the button is wired
 * as active-low with a pull-up, so a press generates a HIGH->LOW transition.
 */
class Button : public GpioPin, public NonCopyableNonMovable {
  public:
	/**
	 * @brief Construct a button pin in input mode.
	 *
	 * @param pin GPIO number.
	 * @param pull Pull configuration (default: PullUp).
	 */
	explicit Button(uint32_t pin, PullMode pull = PullMode::PullUp);
	/**
	 * @brief Destructor unregisters the button from the platform map.
	 */
	virtual ~Button();

	/**
	 * @brief Disallow generic reconfiguration.
	 */
	void Configure(const Config& config) = delete;

	/**
	 * @brief Register a falling-edge IRQ handler.
	 *
	 * @param handler Callback to invoke on falling edge (may be empty).
	 */
	void RegisterHandler(const std::function<void(uint32_t events)>& handler);

	/**
	 * @brief Unregister the handler and disable the IRQ.
	 *
	 * Also removes this instance from the platform's per-pin list.
	 */
	void UnregisterHandler();

	/**
	 * @brief Enable or disable the falling-edge IRQ.
	 *
	 * When disabling, the platform checks if other Button objects on the same
	 * pin still have handlers; if so, IRQ remains enabled.
	 */
	void EnableIrq(bool enable);

	/**
	 * @brief Return true if the button is currently pressed (active-low).
	 */
	bool IsPressed() const;

	/**
	 * @brief Compare buttons by pin id.
	 */
	bool operator==(const Button& other) const;
	bool operator!=(const Button& other) const;

	/**
	 * @brief Call the registered handler from IRQ context.
	 *
	 * Used by the platform IRQ dispatcher.
	 */
	void CallIrqHandler(uint32_t events) const;
	/**
	 * @brief Return true if a handler is currently registered.
	 */
	bool HasHandler() const;

  private:
	/**
	 * @brief Platform hook to register this instance in the per-pin map.
	 */
	static void PlatformRegister(Button* button);
	/**
	 * @brief Platform hook to unregister this instance from the per-pin map.
	 */
	static void PlatformUnregister(Button* button);

	/// Current IRQ handler for this button.
	std::function<void(uint32_t events)> handler_{};
};

} // namespace c7222

#endif // C7222_BUTTON_HPP
