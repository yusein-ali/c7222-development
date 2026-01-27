#include "security_manager.hpp"

namespace c7222 {

BleError SecurityManager::ApplyConfiguration() {
	return BleError::kSuccess;
}

BleError SecurityManager::ConfirmJustWorks(ConnectionHandle) {
	return BleError::kUnsupportedFeatureOrParameterValue;
}

BleError SecurityManager::ConfirmNumericComparison(ConnectionHandle, bool) {
	return BleError::kUnsupportedFeatureOrParameterValue;
}

BleError SecurityManager::ProvidePasskey(ConnectionHandle, uint32_t) {
	return BleError::kUnsupportedFeatureOrParameterValue;
}

BleError SecurityManager::RequestPairing(ConnectionHandle) {
	return BleError::kUnsupportedFeatureOrParameterValue;
}

BleError SecurityManager::SetAuthorization(ConnectionHandle, AuthorizationResult) {
	return BleError::kUnsupportedFeatureOrParameterValue;
}

BleError SecurityManager::DispatchBleHciPacket(uint8_t, const uint8_t*, uint16_t) {
	return BleError::kSuccess;
}

}  // namespace c7222
