#include "security_manager.hpp"

namespace c7222 {

bool SecurityManager::ValidateConfiguration(bool authentication_required,
											bool authorization_required,
											bool encryption_required) const {
	if(params_.min_encryption_key_size > params_.max_encryption_key_size) {
		return false;
	}

	if(authentication_required || authorization_required || encryption_required) {
		if(params_.authentication == AuthenticationRequirement::kNone) {
			return false;
		}
	}

	if(authentication_required || authorization_required) {
		const uint8_t auth_bits = static_cast<uint8_t>(params_.authentication);
		const bool has_mitm =
			(auth_bits &
			 static_cast<uint8_t>(AuthenticationRequirement::kMitmProtection)) != 0;
		if(!has_mitm) {
			return false;
		}
	}

	if(authorization_required) {
		if(params_.io_capability == IoCapability::kDisplayOnly) {
			return false;
		}
	}

	if(params_.secure_connections_only) {
		const uint8_t auth_bits = static_cast<uint8_t>(params_.authentication);
		const bool has_sc =
			(auth_bits &
			 static_cast<uint8_t>(AuthenticationRequirement::kSecureConnections)) != 0;
		if(!has_sc) {
			return false;
		}
	}

	return true;
}

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
