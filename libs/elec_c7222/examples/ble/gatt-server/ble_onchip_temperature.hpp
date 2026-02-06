//
// Created by husey on 2/5/2026.
//

#ifndef PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
#define PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP

#include <cstdint>

#include "characteristic.hpp"
#include "../common/characteristic_event_handler.hpp"
#include "non_copyable.hpp"
// #include "characteristic_event_handler.hpp" // Ensure this is included

class BleOnchipTemperature: public c7222::NonCopyableNonMovable {
public:
	static BleOnchipTemperature* GetInstance(c7222::Characteristic* temperature_characteristic,
									  c7222::Characteristic* configuration_characteristic);
	
	
	private:
	 BleOnchipTemperature();
	 c7222::Characteristic* temperature_characteristic_;
	 c7222::Characteristic* configuration_characteristic_;
	 CharacteristicEventHandler temperature_event_handler_;
	 CharacteristicEventHandler configuration_event_handler_;

	 static BleOnchipTemperature* instance_;
};

#endif	// PICOBLEGATTTEST_BLE_ONCHIP_TEMPERATURE_HPP
