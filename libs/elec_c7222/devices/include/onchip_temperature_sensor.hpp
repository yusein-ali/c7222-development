/**
 * @file onchip_temperature_sensor.hpp
 * @brief RP2040 on-chip temperature sensor wrapper.
 */
#ifndef TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
#define TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP

#include "non_copyable.hpp"
namespace c7222 {

/**
 * @class OnChipTemperatureSensor
 * @brief Reads the RP2040 on-chip temperature sensor via the ADC.
 */
class OnChipTemperatureSensor : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Get the singleton instance.
	 */
	static OnChipTemperatureSensor* GetInstance();

	/**
	 * @brief Initialize the ADC and temperature sensor.
	 * @return true when initialization succeeds.
	 */
	bool Initialize();

	/**
	 * @brief Read temperature in degrees Celsius.
	 * @return Temperature in Celsius, or NaN if not initialized.
	 */
	float GetCelsius() const;

	/**
	 * @brief Read temperature in degrees Fahrenheit.
	 * @return Temperature in Fahrenheit, or NaN if not initialized.
	 */
	float GetFahrenheit() const;

	/**
	 * @brief Check if the sensor has been initialized.
	 */
	bool IsInitialized() const {
		return initialized_;
	}

   private:
	OnChipTemperatureSensor();
	static OnChipTemperatureSensor* instance_;

	bool initialized_{false};
};

}  // namespace c7222

#endif	// TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
