#include "ble.hpp"
#include <btstack.h>
#include <assert.h>

#include "hci_dump_embedded_stdout.h"

namespace c7222 {
namespace {

void ble_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
	(void)Ble::GetInstance()->DispatchBleHciPacket(packet_type, channel, packet, size);
}

} // namespace

struct BleContext : public btstack_packet_callback_registration_t {
	bool l2cap_initialized = false;
	bool sm_initialized = false;
};

BleError Ble::TurnOn() {
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
	if(security_manager_ != nullptr) {
		// Re-apply cached security configuration after SM init.
		security_manager_->Configure(security_manager_->GetSecurityParameters());
	}

	btstack_packet_callback_registration_t* context_registration = context;
	hci_add_event_handler(context_registration);
	// Turn on the Bluetooth hardware
	const int err = hci_power_control(HCI_POWER_ON);
	if(err != 0) {
		if(err > 0) {
			BleError mapped = BleError::kUnspecifiedError;
			if(btstack_map::FromBtStackError(static_cast<uint8_t>(err), mapped)) {
				return mapped;
			}
		}
		return BleError::kUnspecifiedError;
	}

	turned_on_ = true;
	return BleError::kSuccess;
}

void Ble::TurnOff() {
	if(!turned_on_) {
		return;
	}
	// Turn off the Bluetooth hardware
	hci_power_control(HCI_POWER_OFF);
	turned_on_ = false;
}

BleError Ble::DispatchBleHciPacket(uint8_t packet_type,
									uint8_t channel,
								   const uint8_t* packet_data,
								   uint16_t packet_data_size) {
	UNUSED(channel);								
	UNUSED(packet_data_size);
	assert(gap_ != nullptr && "Gap instance is null in Ble::DispatchBleHciPacket");
	assert(attribute_server_ != nullptr &&
		   "AttributeServer instance is null in Ble::DispatchBleHciPacket");
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
		break;
	}

	BleError gap_status = gap_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	BleError attribute_server_status =
		attribute_server_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	BleError security_status = BleError::kSuccess;
	if(security_manager_ != nullptr) {
		security_status = security_manager_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	}

	return static_cast<BleError>(static_cast<int>(gap_status) + static_cast<int>(attribute_server_status) +
								 static_cast<int>(security_status));
}

void Ble::EnableHCILoggingToStdout() {
	hci_logging_enabled_ = true;
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	hci_dump_init(hci_dump_embedded_stdout_get_instance());
#else
	hci_logging_enabled_ = false;
#endif
}

void Ble::DisableHCILoggingToStdout() {
	hci_logging_enabled_ = false;
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	hci_dump_enable_packet_log(false);
	hci_dump_enable_log_level(HCI_DUMP_LOG_LEVEL_INFO, 0);
	hci_dump_enable_log_level(HCI_DUMP_LOG_LEVEL_ERROR, 0);
#endif
}

void Ble::DumpAttributeServerContext() {
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	if(instance_ != nullptr && instance_->attribute_server_ != nullptr && instance_->hci_logging_enabled_) {
		att_dump_attributes();
	}
#endif
}

Ble::Ble()
	: gap_(Gap::GetInstance()),
	  security_manager_(nullptr),
	  attribute_server_(nullptr) {
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
