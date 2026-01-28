#include "ble.hpp"

#include <cstdio>

namespace c7222 {

#if defined(C7222_BLE_DEBUG)
#define C7222_BLE_DEBUG_PRINT(...) std::printf(__VA_ARGS__)
#else
#define C7222_BLE_DEBUG_PRINT(...) do { } while(0)
#endif

Ble::Ble()
	: gap_(Gap::GetInstance()),
	  security_manager_(nullptr),
	  attribute_server_(nullptr) {
	C7222_BLE_DEBUG_PRINT("[BLE] Constructed (grader)\n");
}

Ble::~Ble() = default;

BleError Ble::TurnOn() {
	turned_on_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE] TurnOn (grader)\n");
	if(security_manager_ != nullptr) {
		security_manager_->Configure(security_manager_->GetSecurityParameters());
	}
	return BleError::kSuccess;
}

void Ble::TurnOff() {
	turned_on_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] TurnOff (grader)\n");
}

BleError Ble::DispatchBleHciPacket(uint8_t packet_type,
								   uint8_t channel,
								   const uint8_t* packet_data,
								   uint16_t packet_data_size) {
	(void)packet_type;
	(void)channel;
	(void)packet_data;
	(void)packet_data_size;
	C7222_BLE_DEBUG_PRINT("[BLE] Dispatch HCI packet (grader)\n");
	return BleError::kSuccess;
}

void Ble::EnableHCILoggingToStdout() {
	hci_logging_enabled_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE] HCI logging enabled (grader)\n");
}

void Ble::DisableHCILoggingToStdout() {
	hci_logging_enabled_ = false;
	C7222_BLE_DEBUG_PRINT("[BLE] HCI logging disabled (grader)\n");
}

void Ble::DumpAttributeServerContext() {
	C7222_BLE_DEBUG_PRINT("[BLE] Dump attribute server context (grader)\n");
}

void Ble::EnsureSmEventHandlerRegistered() {
	C7222_BLE_DEBUG_PRINT("[BLE] Ensure SM handler (grader)\n");
}

}  // namespace c7222
