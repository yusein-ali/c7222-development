#include "ble.hpp"
#include <btstack.h>
#include <assert.h>

namespace c7222 {
namespace {

void ble_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
	(void)Ble::getInstance()->dispatchBleHciPacket(packet_type, channel, packet, size);
}

} // namespace

struct BleContext : public btstack_packet_callback_registration_t {
	bool l2cap_initialized = false;
	bool sm_initialized = false;
};

BleError Ble::turnOn() {
	if(turned_on_) {
		return BleError::kSuccess;
	}
	// intialize L2CAP if not already done
	auto context = static_cast<BleContext*>(context_);
	if(!context->l2cap_initialized) {
		l2cap_init();
		context->l2cap_initialized = true;
	}

	// initialize SM if not already done
	if(!context->sm_initialized) {
		sm_init();
		context->sm_initialized = true;
	}

	btstack_packet_callback_registration_t* context_registration = context;
	hci_add_event_handler(context_registration);
	// Turn on the Bluetooth hardware
	const int err = hci_power_control(HCI_POWER_ON);
	if(err != 0) {
		if(err > 0) {
			BleError mapped = BleError::kUnspecifiedError;
			if(btstack_map::from_btstack_error(static_cast<uint8_t>(err), mapped)) {
				return mapped;
			}
		}
		return BleError::kUnspecifiedError;
	}

	turned_on_ = true;
	return BleError::kSuccess;
}

void Ble::turnOff() {
	if(!turned_on_) {
		return;
	}
	// Turn off the Bluetooth hardware
	hci_power_control(HCI_POWER_OFF);
	turned_on_ = false;
}

BleError Ble::dispatchBleHciPacket(uint8_t packet_type,
									uint8_t channel,
								   const uint8_t* packet_data,
								   uint16_t packet_data_size) {
	UNUSED(channel);								
	UNUSED(packet_data_size);

	if(packet_type != HCI_EVENT_PACKET)
		return BleError::kUnsupportedFeatureOrParameterValue;

	switch(hci_event_packet_get_type(packet_data)) {
		// This event fires when the stack state changes (e.g.
		// OFF -> ON)
	case BTSTACK_EVENT_STATE:
		assert(gap_ != nullptr);
		if(btstack_event_state_get_state(packet_data) == HCI_STATE_WORKING) {
			if(callback_on_ble_stack_on_)
				callback_on_ble_stack_on_();
		} else {
			if(callback_on_ble_stack_off_)
				callback_on_ble_stack_off_();
		}
		return BleError::kSuccess;
	default:
		return gap_->dispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	}
}

Ble::Ble() : gap_(Gap::getInstance()) {
	auto context = new BleContext();
	context->callback = &ble_packet_handler;
	context_ = context;
}

Ble::~Ble() {
	auto context = static_cast<BleContext*>(context_);
	if(context != nullptr) {
		delete context;
	}
}
} // namespace c7222
