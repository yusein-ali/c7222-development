/**
 * @file platform.hpp
 * @brief Platform encapsulation for board-level devices.
 */
#ifndef ELEC_C7222_PLATFORM_HPP
#define ELEC_C7222_PLATFORM_HPP

#include "onboard_led.hpp"
#include "onchip_temperature_sensor.hpp"

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

	bool IsInitialized() const {
		return initialized_;
	}

  private:
	Platform();
	~Platform() = default;
	static Platform* instance_;

	bool initialized_{false};
	bool arch_initialized_{false};
};

} // namespace c7222

#endif // ELEC_C7222_PLATFORM_HPP
