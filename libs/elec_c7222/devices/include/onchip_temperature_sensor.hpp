/**
 * @file onchip_temperature_sensor.hpp
 * @brief RP2040 on-chip temperature sensor wrapper.
 *
 * This class provides a minimal, beginner-friendly API for reading the
 * RP2040's built-in temperature sensor via the ADC. The platform backend
 * handles the ADC configuration and conversion logic.
 */
#ifndef TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
#define TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP

#include <memory>

#include "non_copyable.hpp"
namespace c7222 {

/**
 * @class OnChipTemperatureSensor
 * @brief Reads the RP2040 on-chip temperature sensor via the ADC.
 *
 * Design intent:
 * - **Singleton access**: only one on-chip sensor exists, so a singleton is used.
 * - **Explicit initialization**: call `Initialize()` once before reading.
 *   The Platform does not auto-initialize this device.
 *
 * Example:
 * @code{.cpp}
 * auto* sensor = c7222::OnChipTemperatureSensor::GetInstance();
 * sensor->Initialize();
 * float temp_c = sensor->GetCelsius();
 * @endcode
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
	/**
	 * @brief Destructor.
	 *
	 * Public to allow std::unique_ptr default deleter.
	 */
	~OnChipTemperatureSensor() = default;

   private:
	/**
	 * @brief Private constructor for singleton usage.
	 */
	OnChipTemperatureSensor();
	/**
	 * @brief Singleton instance (lazy-initialized).
	 */
	static std::unique_ptr<OnChipTemperatureSensor> instance_;

	/**
	 * @brief True after successful `Initialize()`.
	 */
	bool initialized_{false};
};

}  // namespace c7222

#endif	// TEMPLATE_ONCHIP_TEMPERATURE_SENSOR_HPP
