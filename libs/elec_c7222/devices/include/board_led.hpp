/**
 * @file board_led.hpp
 * @brief Board LED abstraction for the C7222 course.
 */
#ifndef TEMPLATE_BOARD_LED_HPP
#define TEMPLATE_BOARD_LED_HPP

namespace c7222 {

/**
 * @class BoardLED
 * @brief Simple on-board LED controller.
 */
class BoardLED {
  public:
	BoardLED();

	bool Initialize();
	void Set(bool on);
	void On();
	void Off();
	void Toggle();
  private:
	bool _initialized{false};
	bool _state{false};
};

} // namespace c7222

#endif // TEMPLATE_BOARD_LED_HPP
