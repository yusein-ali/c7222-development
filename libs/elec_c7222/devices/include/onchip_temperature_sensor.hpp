/**
 * @file onchip_temperature_sensor.hpp
 * @brief RP2040 on-chip temperature sensor wrapper.
 */
#ifndef TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
#define TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP

namespace c7222 {

/**
 * @class OnChipTemperatureSensor
 * @brief Reads the RP2040 on-chip temperature sensor via the ADC.
 */
class OnChipTemperatureSensor {
  public:
	/**
	 * @brief Get the singleton instance.
	 */
	static OnChipTemperatureSensor& GetInstance();

	/**
	 * @brief Initialize the ADC and temperature sensor.
	 * @return true when initialization succeeds.
	 */
	bool Initialize();

	/**
	 * @brief Read temperature in degrees Celsius.
	 * @return Temperature in Celsius, or NaN if not initialized.
	 */
	float ReadCelsius() const;

	/**
	 * @brief Read temperature in degrees Fahrenheit.
	 * @return Temperature in Fahrenheit, or NaN if not initialized.
	 */
	float ReadFahrenheit() const;

	/**
	 * @brief Check if the sensor has been initialized.
	 */
	bool IsInitialized() const {
		return initialized_;
	}

  private:
	OnChipTemperatureSensor();

	OnChipTemperatureSensor(const OnChipTemperatureSensor&) = delete;
	OnChipTemperatureSensor& operator=(const OnChipTemperatureSensor&) = delete;
	OnChipTemperatureSensor(OnChipTemperatureSensor&&) = delete;
	OnChipTemperatureSensor& operator=(OnChipTemperatureSensor&&) = delete;

	bool initialized_{false};
};

} // namespace c7222

#endif // TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
