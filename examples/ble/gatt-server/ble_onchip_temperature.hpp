//
// Created by husey on 2/5/2026.
//

#ifndef PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
#define PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP

#include <cstdint>

#include "characteristic.hpp"
#include "non_copyable.hpp"

class TemperatureEventHandler : public c7222::Characteristic::EventHandler {
public:
	void OnUpdatesEnabled(bool is_indication) override;
	void OnUpdatesDisabled() override;
	void OnIndicationComplete(uint8_t status) override;
	void OnBroadcastEnabled() override;
	void OnBroadcastDisabled() override;
	void OnRead() override;
	void OnWrite(const std::vector<uint8_t>& data) override;
	void OnConfirmationReceived(bool status) override;

	void SetCharacteristicName(const std::string& name) {
		characteristic_name_ = name;
	}

	std::string GetCharacteristicName() const {
		return characteristic_name_;
	}

private: 
	std::string characteristic_name_;
};

class BleOnchipTemperature: public c7222::NonCopyableNonMovable {
public:
	static BleOnchipTemperature* GetInstance(c7222::Characteristic* temperature_characteristic,
									  c7222::Characteristic* configuration_characteristic);
	
	
	private:
	 BleOnchipTemperature();
	 c7222::Characteristic* temperature_characteristic_;
	 c7222::Characteristic* configuration_characteristic_;
	 TemperatureEventHandler temperature_event_handler_;
	 TemperatureEventHandler configuration_event_handler_;

	 static BleOnchipTemperature* instance_;
};

#endif	// PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
