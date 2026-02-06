/**
 * @file button.hpp
 * @brief Input-only GPIO abstraction for push-buttons with IRQ handler support.
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
 * This class derives from GpioPin but restricts configuration to input mode.
 * It supports registering a handler that is invoked on a falling-edge IRQ.
 *
 * Board assumption (typical for the C7222 Pico W board): the button is wired
 * as active-low with a pull-up, so a press generates a HIGH->LOW transition.
 */
class Button : public GpioPin {
  public:
	/**
	 * @brief Handler function type.
	 *
	 * The handler is invoked from the GPIO IRQ context.
	 */
	using Handler = void (*)(void* user_data);

	/**
	 * @brief Internal IRQ trampoline used by the platform backend.
	 *
	 * Do not call directly.
	 */
	static void IrqTrampoline(uint32_t gpio, uint32_t events);

	/**
	 * @brief Construct a button pin in input mode.
	 *
	 * @param pin GPIO number.
	 * @param pull Pull configuration (default: PullUp).
	 */
	explicit Button(uint32_t pin, PullMode pull = PullMode::PullUp);

	/**
	 * @brief Disallow generic reconfiguration.
	 */
	void Configure(const Config& config) = delete;

	/**
	 * @brief Register a falling-edge IRQ handler.
	 *
	 * @param handler Callback to invoke on falling edge (may be nullptr).
	 * @param user_data Opaque pointer passed back to the handler.
	 * @param enable Whether to enable the IRQ immediately.
	 */
	void RegisterHandler(Handler handler, void* user_data = nullptr, bool enable = true);

	/**
	 * @brief Unregister the handler and disable the IRQ.
	 */
	void UnregisterHandler();

	/**
	 * @brief Enable or disable the falling-edge IRQ.
	 */
	void EnableIrq(bool enable);

	/**
	 * @brief Return true if the button is currently pressed (active-low).
	 */
	bool IsPressed() const;

  private:
	static Config MakeConfig(uint32_t pin, PullMode pull);

	void InvokeHandlerFromIrq();

	Handler _handler{nullptr};
	void* _user_data{nullptr};
};

} // namespace c7222

#endif // C7222_BUTTON_HPP
