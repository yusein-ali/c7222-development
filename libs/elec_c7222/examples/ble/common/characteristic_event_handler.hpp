/**
 * @file characteristic_event_handler.hpp
 * @brief Minimal Characteristic event handler used by BLE examples.
 *
 * Declares a simple event handler that logs characteristic activity and a
 * lightweight helper to bind the handler to temperature and configuration
 * characteristics in example code.
 */
#ifndef PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
#define PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP

#include <cstdint>

#include "characteristic.hpp"
#include "non_copyable.hpp"

/**
 * @brief Minimal characteristic event handler for examples.
 *
 * Prints events (read/write/notify/indicate) to the console. The handler is
 * intentionally minimal so students can observe event flow without extra logic.
 */
class CharacteristicEventHandler : public c7222::Characteristic::EventHandler {
public:
	/// Called when notifications or indications are enabled.
	void OnUpdatesEnabled(bool is_indication) override;
	/// Called when notifications or indications are disabled.
	void OnUpdatesDisabled() override;
	/// Called after an indication is confirmed by the client.
	void OnIndicationComplete(uint8_t status) override;
	/// Called when broadcast is enabled on the characteristic.
	void OnBroadcastEnabled() override;
	/// Called when broadcast is disabled on the characteristic.
	void OnBroadcastDisabled() override;
	/// Called when the characteristic value is read.
	void OnRead() override;
	/// Called when the characteristic value is written.
	void OnWrite(const std::vector<uint8_t>& data) override;
	/// Called when a confirmation is received.
	void OnConfirmationReceived(bool status) override;

	/**
	 * @brief Set a label used in log output.
	 *
	 * This makes example logs easier to read when multiple characteristics exist.
	 */
	void SetCharacteristicName(const std::string& name) {
		characteristic_name_ = name;
	}

	/**
	 * @brief Get the current log label for the handler.
	 */
	std::string GetCharacteristicName() const {
		return characteristic_name_;
	}

private:
	std::string characteristic_name_;
};


#endif	// PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
