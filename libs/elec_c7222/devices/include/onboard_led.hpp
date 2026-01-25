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
class OnBoardLED {
  public:
	/**
	 * @brief Get the singleton instance.
	 */
	static OnBoardLED& GetInstance();

	bool Initialize();
	void Set(bool on);
	void On();
	void Off();
	void Toggle();
  private:
	OnBoardLED();
	OnBoardLED(const OnBoardLED&) = delete;
	OnBoardLED& operator=(const OnBoardLED&) = delete;
	OnBoardLED(OnBoardLED&&) = delete;
	OnBoardLED& operator=(OnBoardLED&&) = delete;

	bool _initialized{false};
	bool _state{false};
};

} // namespace c7222

#endif // TEMPLATE_BOARD_LED_HPP
