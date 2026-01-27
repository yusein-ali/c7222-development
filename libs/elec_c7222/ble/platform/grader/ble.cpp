#include "ble.hpp"

namespace c7222 {

Ble::Ble()
	: gap_(Gap::GetInstance()),
	  security_manager_(nullptr),
	  attribute_server_(nullptr) {}

Ble::~Ble() = default;

BleError Ble::TurnOn() {
	turned_on_ = true;
	if(security_manager_ != nullptr) {
		security_manager_->Configure(security_manager_->GetSecurityParameters());
	}
	return BleError::kSuccess;
}

void Ble::TurnOff() {
	turned_on_ = false;
}

BleError Ble::DispatchBleHciPacket(uint8_t packet_type,
								   uint8_t channel,
								   const uint8_t* packet_data,
								   uint16_t packet_data_size) {
	(void)packet_type;
	(void)channel;
	(void)packet_data;
	(void)packet_data_size;
	return BleError::kSuccess;
}

void Ble::EnableHCILoggingToStdout() {
	hci_logging_enabled_ = true;
}

void Ble::DisableHCILoggingToStdout() {
	hci_logging_enabled_ = false;
}

void DumpAttributeServerContext() {
}

}  // namespace c7222
