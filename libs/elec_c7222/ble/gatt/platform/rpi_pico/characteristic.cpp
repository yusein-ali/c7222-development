#include "characteristic.hpp"

#include <btstack.h>

namespace c7222 {

BleError Characteristic::UpdateValue() {
	// Only send if we have a valid connection handle
	if(connection_handle_ == 0) {
		return BleError::kSuccess;
	}

	// Get the CCCD to check if notifications/indications are enabled
	const Attribute* cccd = GetCCCD();
	if(cccd == nullptr || cccd->GetValueSize() < 2) {
		return BleError::kSuccess;
	}

	// Read CCCD value (2 bytes, little-endian)
	const uint8_t* cccd_data = cccd->GetValueData();
	const uint16_t cccd_value = *reinterpret_cast<const uint16_t*>(cccd_data);

	// Check which updates are enabled (from CCCDProperties enum)
	const bool notify_enabled = (cccd_value & static_cast<uint16_t>(CCCDProperties::kNotifications)) != 0;
	const bool indicate_enabled = (cccd_value & static_cast<uint16_t>(CCCDProperties::kIndications)) != 0;

	// If neither is enabled, nothing to do
	if(!notify_enabled && !indicate_enabled) {
		return BleError::kSuccess;
	}

	// Get the value to send
	const uint8_t* value_data = GetValueData();
	auto value_size = static_cast<uint16_t>(GetValueSize());

	if(value_data == nullptr) {
		return BleError::kSuccess;
	}

	int status = 0;
	
	assert((connection_handle_ != 0) && "Invalid connection handle. Must be connected before updating the CCCD value!");
	// Prioritize indication over notification if both are enabled
	if(indicate_enabled) {
		// Send indication using BTstack's att_server_indicate
		status = att_server_indicate(connection_handle_, value_attr_.GetHandle(), value_data, value_size);
	} else { // if(notify_enabled)
		// Send notification using BTstack's att_server_notify
		status = att_server_notify(connection_handle_, value_attr_.GetHandle(), value_data, value_size);
	}

	// Check if buffers are full
	if(status == BTSTACK_ACL_BUFFERS_FULL) {
		// Stack is busy. Mark as pending and request the callback.
		notification_pending_ = true;
		att_server_request_can_send_now_event(connection_handle_);
	} else {
		// Successfully sent or other status - clear pending flag
		notification_pending_ = false;
	}

	return BleError::kSuccess;
}

BleError Characteristic::DispatchBleHciPacket(uint8_t packet_type,
                                                const uint8_t* packet_data,
                                                uint16_t packet_data_size) {
	if(packet_type != HCI_EVENT_PACKET || packet_data == nullptr || packet_data_size == 0) {
		return BleError::kSuccess;
	}

	const uint8_t event_code = hci_event_packet_get_type(packet_data);
	
	// Check for ATT events
	if(event_code == ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE) {
		return DispatchEvent(EventId::kHandleValueIndicationComplete, packet_data, packet_data_size);
	}
	
	// Handle ATT_EVENT_CAN_SEND_NOW by calling UpdateValue to send pending data
	if(event_code == ATT_EVENT_CAN_SEND_NOW) {
		return UpdateValue();
	}

	return BleError::kSuccess;
}

BleError Characteristic::DispatchEvent(EventId event_id,
                                        const uint8_t* event_data,
                                        uint16_t event_data_size) {
	(void)event_data_size;
	// handle on ValueIndicationComplete event
	switch(event_id) {
	case EventId::kHandleValueIndicationComplete: {
		// Extract status from the ATT event
		uint8_t status = att_event_handle_value_indication_complete_get_status(event_data);
		
		// Call OnConfirmationComplete on all registered event handlers
		for(auto* handler: event_handlers_) {
			if(handler) {
				handler->OnConfirmationReceived(status != 0);
			}
		}
		break;
	}
	case EventId::kAttEventEnd:
		// Handle other ATT events if needed in the future
		break;
	default:
		break;
	}

	return BleError::kSuccess;
}

} // namespace c7222
