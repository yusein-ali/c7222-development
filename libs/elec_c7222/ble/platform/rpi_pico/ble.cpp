#include "ble.hpp"
#include "attribute_server.hpp"
#include "ble_utils.hpp"
#include <btstack.h>
#include <assert.h>

#include "hci_dump_embedded_stdout.h"

namespace c7222 {

namespace btstack_map {
bool ToBtStack(BleError error, uint8_t& out);
bool FromBtStackError(uint8_t code, BleError& out);

}  // namespace btstack_map
namespace {

void ble_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
	(void)Ble::GetInstance()->DispatchBleHciPacket(packet_type, channel, packet, size);
}

void sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
	UNUSED(channel);
	UNUSED(size);
	if(packet_type != HCI_EVENT_PACKET) {
		return;
	}
	auto* sm = Ble::GetInstance()->GetSecurityManager();
	if(sm != nullptr) {
		(void) sm->DispatchBleHciPacket(packet_type, packet, size);
	}
}

} // namespace

struct BleContext {
	bool l2cap_initialized = false;
	bool sm_initialized = false;
	bool sm_events_registered = false;
	btstack_packet_callback_registration_t hci_event_registration{};
	btstack_packet_callback_registration_t sm_event_registration{};
};

BleError Ble::TurnOn() {
	if(turned_on_) {
		C7222_BLE_DEBUG_PRINT("[BLE] TurnOn: already on\n");
		return BleError::kSuccess;
	}
	// intialize L2CAP if not already done
	auto context = static_cast<BleContext*>(context_);
	if(!context->l2cap_initialized) {
		l2cap_init();
		context->l2cap_initialized = true;
		C7222_BLE_DEBUG_PRINT("[BLE] L2CAP initialized\n");
	}

	// initialize SM if not already done
	if(!context->sm_initialized) {
		sm_init();
		context->sm_initialized = true;
		C7222_BLE_DEBUG_PRINT("[BLE] SM initialized\n");
	}
	if(security_manager_ != nullptr) {
		// Re-apply cached security configuration after SM init.
		C7222_BLE_DEBUG_PRINT("[BLE] Re-apply SM configuration\n");
		security_manager_->Configure(security_manager_->GetSecurityParameters());
	}
	EnsureSmEventHandlerRegistered();

	context->hci_event_registration.callback = &ble_packet_handler;
	hci_add_event_handler(&context->hci_event_registration);
	C7222_BLE_DEBUG_PRINT("[BLE] HCI event handler registered\n");
	// Turn on the Bluetooth hardware
	const int err = hci_power_control(HCI_POWER_ON);
	if(err != 0) {
		C7222_BLE_DEBUG_PRINT("[BLE] HCI power on failed: %d\n", err);
		if(err > 0) {
			BleError mapped = BleError::kUnspecifiedError;
			if(btstack_map::FromBtStackError(static_cast<uint8_t>(err), mapped)) {
				return mapped;
			}
		}
		return BleError::kUnspecifiedError;
	}

	turned_on_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE] TurnOn: success\n");
	return BleError::kSuccess;
}

void Ble::TurnOff() {
	if(!turned_on_) {
		C7222_BLE_DEBUG_PRINT("[BLE] TurnOff: already off\n");
		return;
	}
	// Turn off the Bluetooth hardware
	hci_power_control(HCI_POWER_OFF);
	turned_on_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] TurnOff: success\n");
}

BleError Ble::DispatchBleHciPacket(uint8_t packet_type,
									uint8_t channel,
								   const uint8_t* packet_data,
								   uint16_t packet_data_size) {
	UNUSED(channel);								
	UNUSED(packet_data_size);
	// C7222_BLE_DEBUG_PRINT("[BLE] HCI packet type=0x%02x size=%u\n",
	// 	static_cast<unsigned>(packet_type),
	// 	static_cast<unsigned>(packet_data_size));
	assert(gap_ != nullptr && "Gap instance is null in Ble::DispatchBleHciPacket");
	if(packet_type != HCI_EVENT_PACKET)
		return BleError::kUnsupportedFeatureOrParameterValue;

	uint8_t event = hci_event_packet_get_type(packet_data);
	// C7222_BLE_DEBUG_PRINT("[BLE] HCI event=0x%02x\n", static_cast<unsigned>(event));
	if(event == GAP_EVENT_SECURITY_LEVEL) {
		const auto con_handle =
			static_cast<ConnectionHandle>(gap_event_security_level_get_handle(packet_data));
		const uint8_t level = gap_event_security_level_get_security_level(packet_data);
		if(attribute_server_ != nullptr) {
			attribute_server_->SetSecurityLevel(con_handle, level);
		}
	}
	switch(event) {
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
		// the following must be return immediately since we are not handling them
		case HCI_EVENT_COMMAND_COMPLETE:
		case HCI_EVENT_TRANSPORT_READY:
		case HCI_EVENT_TRANSPORT_PACKET_SENT:
		case HCI_EVENT_TRANSPORT_SLEEP_MODE:
		case HCI_EVENT_TRANSPORT_USB_INFO:
			return BleError::kSuccess;

		default:
			break;
	}
	C7222_BLE_DEBUG_PRINT("BLE EVENT 0x%02X\r\n", event);
	BleError gap_status = gap_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	BleError attribute_server_status = BleError::kSuccess;
	if(attribute_server_ != nullptr) {
		attribute_server_status =
			attribute_server_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	}

	BleError security_status = BleError::kSuccess;
	if(security_manager_ != nullptr) {
		security_status = security_manager_->DispatchBleHciPacket(packet_type, packet_data, packet_data_size);
	}

	if(static_cast<BleError>(static_cast<int>(gap_status) +
							 static_cast<int>(attribute_server_status) +
							 static_cast<int>(security_status)) != BleError::kSuccess) {
		C7222_BLE_DEBUG_PRINT(
			"[BLE] Error handling HCI packet: gap=%u attribute_server=%u security_manager=%u\n",
			static_cast<unsigned>(gap_status),
			static_cast<unsigned>(attribute_server_status),
			static_cast<unsigned>(security_status));
		return BleError::kUnspecifiedError;
	} else {
		return BleError::kSuccess;
	}
}

void Ble::EnableHCILoggingToStdout() {
	hci_logging_enabled_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE] HCI logging enabled\n");
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	hci_dump_init(hci_dump_embedded_stdout_get_instance());
#else
	hci_logging_enabled_ = false;
#endif
}

void Ble::DisableHCILoggingToStdout() {
	hci_logging_enabled_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] HCI logging disabled\n");
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	hci_dump_enable_packet_log(false);
	hci_dump_enable_log_level(HCI_DUMP_LOG_LEVEL_INFO, 0);
	hci_dump_enable_log_level(HCI_DUMP_LOG_LEVEL_ERROR, 0);
#endif
}

void Ble::DumpAttributeServerContext() {
#if defined(ENABLE_LOG_INFO) || defined(ENABLE_LOG_ERROR)
	if(instance_ != nullptr && instance_->attribute_server_ != nullptr && instance_->hci_logging_enabled_) {
		C7222_BLE_DEBUG_PRINT("[BLE] Dump attribute server context\n");
		att_dump_attributes();
	}
#endif
}

Ble::Ble()
	: gap_(Gap::GetInstance()),
	  security_manager_(nullptr),
	  attribute_server_(nullptr) {
	auto context = new BleContext();
	context->hci_event_registration.callback = &ble_packet_handler;
	context_ = context;
	C7222_BLE_DEBUG_PRINT("[BLE] Constructed\n");
}

Ble::~Ble() {
	auto context = static_cast<BleContext*>(context_);
	if(context != nullptr) {
		delete context;
	}
	C7222_BLE_DEBUG_PRINT("[BLE] Destroyed\n");
}

void Ble::EnsureSmEventHandlerRegistered() {
	auto context = static_cast<BleContext*>(context_);
	if(context == nullptr) {
		C7222_BLE_DEBUG_PRINT("[BLE] SM handler: missing context\n");
		return;
	}
	if(!context->sm_initialized) {
		sm_init();
		context->sm_initialized = true;
		C7222_BLE_DEBUG_PRINT("[BLE] SM initialized (late)\n");
	}
	if(!context->sm_events_registered) {
		context->sm_event_registration.callback = &sm_packet_handler;
		sm_add_event_handler(&context->sm_event_registration);
		context->sm_events_registered = true;
		C7222_BLE_DEBUG_PRINT("[BLE] SM event handler registered\n");
	}
}
} // namespace c7222
