/**
 * @file board_led.hpp
 * @brief Board LED abstraction for the C7222 course.
 */
#ifndef TEMPLATE_BOARD_LED_HPP
#define TEMPLATE_BOARD_LED_HPP

#include "non_copyable.hpp"
namespace c7222 {

/**
 * @class BoardLED
 * @brief Simple on-board LED controller.
 */
class OnBoardLED : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Get the singleton instance.
	 */
	static OnBoardLED* GetInstance();

	bool Initialize();
	void Set(bool on);
	void On();
	void Off();
	void Toggle();
  private:
	OnBoardLED();
	~OnBoardLED() = default;
	static OnBoardLED* instance_;
	bool initialized_{false};
	bool state_{false};
};

} // namespace c7222

#endif // TEMPLATE_BOARD_LED_HPP
