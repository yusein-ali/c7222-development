#ifndef C7222_PICO_W_BOARD_HPP
#define C7222_PICO_W_BOARD_HPP

/**
 * @file c7222_pico_w_board.hpp
 * @brief C++ board-level helper for the C7222 Pico W board.
 *
 * This class provides a small, beginner-friendly API for accessing the
 * board's LEDs and buttons through the higher-level `Led` and `Button` types.
 * It hides GPIO pin numbers behind enums and ensures a consistent, simple
 * access pattern across platforms.
 *
 * Design intent:
 * - **Board-centric API**: Use logical IDs (`LedId`, `ButtonId`) instead of
 *   raw GPIO numbers.
 * - **Simple lifecycle**: Construction performs initialization; then retrieve
 *   references to LEDs/buttons using `GetLed()`/`GetButton()`.
 * - **No accidental copies**: Inherits from `NonCopyableNonMovable` to avoid
 *   copying the board abstraction.
 *
 * Mapping assumptions (C7222 Pico W board):
 * - LEDs are active-low and mapped to GPIOs 16–21.
 * - Buttons are mapped to GPIOs 2–5 (active-low with pull-ups by convention).
 */

#include <array>
#include <memory>

#include "button.hpp"
#include "led.hpp"

namespace c7222 {

/**
 * @class PicoWBoard
 * @brief Board-level convenience wrapper for the C7222 Pico W board.
 *
 * Construct the board to initialize IO objects, then access them by logical
 * ID. This avoids scattering GPIO pin numbers through application code and
 * keeps beginner code readable.
 */
class PicoWBoard : public NonCopyableNonMovable {
   public:
	/// Number of board LEDs exposed by this API.
	static constexpr size_t kLedCount = 6;
	/// Number of board buttons exposed by this API.
	static constexpr size_t kButtonCount = 4;

	/**
	 * @brief Logical LED identifiers mapped to GPIO pins.
	 */
	enum class LedId : uint8_t {
		LED1_RED = 21,
		LED1_GREEN = 20,
		LED2_RED = 19,
		LED2_GREEN = 18,
		LED3_RED = 17,
		LED3_GREEN = 16
	};

	/**
	 * @brief Logical button identifiers mapped to GPIO pins.
	 */
	enum class ButtonId : uint8_t { BUTTON_B1 = 2, BUTTON_B2 = 3, BUTTON_B3 = 4, BUTTON_B4 = 5 };

	/**
	 * @brief Access the singleton instance (if used).
	 */
	static PicoWBoard* GetInstance();

	/**
	 * @brief Return true if construction-time initialization has completed.
	 */
	bool IsInitialized() const {
		return initialized_;
	}

	/**
	 * @brief Retrieve a board LED by logical ID.
	 */
	Led& GetLed(LedId id);
	/**
	 * @brief Retrieve a board button by logical ID.
	 */
	Button& GetButton(ButtonId id);

   private:
	/**
	 * @brief Private constructor for singleton-style usage.
	 *
	 * Instances are intended to be created through `GetInstance()` or on the
	 * stack by trusted code that understands the lifetime implications.
	 */
	PicoWBoard();
	/**
	 * @brief Singleton instance (if used by the project).
	 */
	static std::unique_ptr<PicoWBoard> instance_;

	/**
	 * @brief True once construction-time initialization has completed.
	 */
	bool initialized_;
	/**
	 * @brief Storage for all board LED objects.
	 *
	 * Indices align with `LedId` ordering used by `GetLed()`.
	 */
	std::array<Led, kLedCount> leds_;
	/**
	 * @brief Storage for all board Button objects.
	 *
	 * Indices align with `ButtonId` ordering used by `GetButton()`.
	 */
	std::array<Button, kButtonCount> buttons_;
};
}  // namespace c7222

#endif	// C7222_PICO_W_BOARD_HPP
