/**
 * @file platform.hpp
 * @brief Platform encapsulation for board-level devices.
 */
#ifndef ELEC_C7222_PLATFORM_HPP
#define ELEC_C7222_PLATFORM_HPP

#include <cstdint>

#include "onboard_led.hpp"
#include "onchip_temperature_sensor.hpp"

#include "led.hpp"
#include "button.hpp"



namespace c7222 {

/**
 * @class Platform
 * @brief Singleton access to platform-specific devices and initialization.
 */
class Platform: public NonCopyableNonMovable {
  public:
	/**
	 * @brief Get the singleton instance.
	 */
	static Platform* GetInstance();

	/**
	 * @brief Initialize platform-specific hardware.
	 * @return true when initialization succeeds.
	 */
	bool Initialize();

	/**
	 * @brief Ensure the platform architecture is initialized.
	 * @return true when architecture init succeeds.
	 */
	bool EnsureArchInitialized();

	/**
	 * @brief Access the on-board LED.
	 */
	OnBoardLED* GetOnBoardLed(){
		return OnBoardLED::GetInstance();
	}

	/**
	 * @brief Access the on-chip temperature sensor.
	 */
	OnChipTemperatureSensor* GetOnChipTemperatureSensor(){
		return OnChipTemperatureSensor::GetInstance();
	}

	/**
	 * @brief Board LED identifiers mapped to GPIO pins.
	 *
	 * Underlying type is uint8_t as requested. Values match the C7222 Pico W board
	 * layout (see c7222_pico_w_board.h).
	 */
	enum class BoardLed : uint8_t {
		LED3_GREEN = 16,
		LED3_RED   = 17,
		LED2_GREEN = 18,
		LED2_RED   = 19,
		LED1_GREEN = 20,
		LED1_RED   = 21
	};

	/**
	 * @brief Board button identifiers mapped to GPIO pins.
	 *
	 * Underlying type is uint8_t as requested. Values match the C7222 Pico W board
	 * layout (see c7222_pico_w_board.h).
	 */
	enum class BoardButton : uint8_t {
		B1 = 2,
		B2 = 3,
		B3 = 4,
		B4 = 5
	};

	/**
	 * @brief Initialize board IO (LEDs/buttons) if needed.
	 *
	 * On Pico this calls the board helper init. On grader it is a no-op.
	 */
	bool InitializeBoardIo();

	/**
	 * @brief Create an LED object for a given board LED enum.
	 *
	 * @param led Board LED identifier.
	 * @param initial_on Initial state (default off).
	 * @param drive Drive strength (default 4mA).
	 */
	Led CreateLed(BoardLed led,
	              bool initial_on = false,
	              GpioPin::DriveStrength drive = GpioPin::DriveStrength::mA4) const;

	/**
	 * @brief Create a Button object for a given board button enum.
	 *
	 * @param button Board button identifier.
	 * @param pull Pull mode (default PullUp).
	 */
	Button CreateButton(BoardButton button,
	                    GpioPin::PullMode pull = GpioPin::PullMode::PullUp) const;


	bool IsInitialized() const {
		return initialized_;
	}

  private:
	Platform();
	~Platform() = default;
	static Platform* instance_;

	bool initialized_{false};
	bool arch_initialized_{false};
	bool board_io_initialized_{false};

};

} // namespace c7222

#endif // ELEC_C7222_PLATFORM_HPP
