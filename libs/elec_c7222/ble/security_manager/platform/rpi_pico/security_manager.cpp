#include "security_manager.hpp"
#include "attribute_server.hpp"
#include "ble_utils.hpp"

#include <btstack.h>
#include <btstack_config.h>

namespace c7222 {
namespace {

io_capability_t ToBtstackIoCapability(SecurityManager::IoCapability capability) {
	switch(capability) {
		case SecurityManager::IoCapability::kDisplayOnly:
			return IO_CAPABILITY_DISPLAY_ONLY;
		case SecurityManager::IoCapability::kDisplayYesNo:
			return IO_CAPABILITY_DISPLAY_YES_NO;
		case SecurityManager::IoCapability::kKeyboardOnly:
			return IO_CAPABILITY_KEYBOARD_ONLY;
		case SecurityManager::IoCapability::kNoInputNoOutput:
			return IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
		case SecurityManager::IoCapability::kKeyboardDisplay:
			return IO_CAPABILITY_KEYBOARD_DISPLAY;
		default:
			return IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
	}
}

uint8_t ToBtstackAuthReq(SecurityManager::AuthenticationRequirement auth) {
	const uint8_t bits = static_cast<uint8_t>(auth);
	uint8_t result = 0;
	if(bits & static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kBonding)) {
		result |= SM_AUTHREQ_BONDING;
	}
	if(bits & static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kMitmProtection)) {
		result |= SM_AUTHREQ_MITM_PROTECTION;
	}
	if(bits & static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kSecureConnections)) {
		result |= SM_AUTHREQ_SECURE_CONNECTION;
	}
	if(bits & static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kKeypressNotifications)) {
		result |= SM_AUTHREQ_KEYPRESS;
	}
	return result;
}

SecurityManager::PairingStatus ClassifyPairingStatus(uint8_t status_code) {
	if(status_code == ERROR_CODE_SUCCESS) {
		return SecurityManager::PairingStatus::kSuccess;
	}
	if(status_code == ERROR_CODE_CONNECTION_TIMEOUT) {
		return SecurityManager::PairingStatus::kTimeout;
	}
	if(status_code == ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE) {
		return SecurityManager::PairingStatus::kUnsupported;
	}
	return SecurityManager::PairingStatus::kFailed;
}

uint8_t ExpectedSecurityLevel(const SecurityManager::SecurityParameters& params) {
	const uint8_t auth_bits = static_cast<uint8_t>(params.authentication);
	const bool requires_mitm =
		(auth_bits &
		 static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kMitmProtection)) != 0;
	const bool requires_sc =
		(auth_bits &
		 static_cast<uint8_t>(SecurityManager::AuthenticationRequirement::kSecureConnections)) != 0;
	if(params.secure_connections_only || requires_sc) {
		return 3;
	}
	if(requires_mitm) {
		return 2;
	}
	return auth_bits == 0 ? 0 : 1;
}

}  // namespace

bool SecurityManager::ValidateConfiguration(bool authentication_required,
											bool authorization_required,
											bool encryption_required) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Validate: auth=%u authz=%u enc=%u\n",
		static_cast<unsigned>(authentication_required),
		static_cast<unsigned>(authorization_required),
		static_cast<unsigned>(encryption_required));
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

	const uint8_t auth_bits = static_cast<uint8_t>(params_.authentication);
	const bool bonding_required =
		(auth_bits &
		 static_cast<uint8_t>(AuthenticationRequirement::kBonding)) != 0;
	if(bonding_required) {
#if !defined(MAX_NR_LE_DEVICE_DB_ENTRIES) && !defined(NVM_LE_DEVICE_DB_ENTRIES)
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Validate failed: bonding backend not enabled\n");
		return false;
#endif
	}

	return true;
}

BleError SecurityManager::ApplyConfiguration() {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Apply configuration\n");
	sm_set_io_capabilities(ToBtstackIoCapability(params_.io_capability));
	sm_set_authentication_requirements(ToBtstackAuthReq(params_.authentication));
	sm_set_encryption_key_size_range(params_.min_encryption_key_size, params_.max_encryption_key_size);
	sm_set_secure_connections_only_mode(params_.secure_connections_only ? 1 : 0);

	if(params_.gatt_client_required_security_level !=
	   SecurityManager::GattClientSecurityLevel::kLevel0) {
		gatt_client_set_required_security_level(
			static_cast<gap_security_level_t>(params_.gatt_client_required_security_level));
	}

	switch(params_.fixed_passkey_role) {
		case FixedPasskeyRole::kDisplay:
			if(params_.fixed_passkey != 0) {
				sm_use_fixed_passkey_in_display_role(params_.fixed_passkey);
			}
			break;
		case FixedPasskeyRole::kKeyboard:
			if(params_.fixed_passkey != 0) {
				// Some BTstack builds do not expose a keyboard-role helper.
				// Fall back to the display-role helper to keep behavior defined.
				sm_use_fixed_passkey_in_display_role(params_.fixed_passkey);
			}
			break;
		case FixedPasskeyRole::kNone:
		default:
			break;
	}

	return BleError::kSuccess;
}

BleError SecurityManager::ConfirmJustWorks(ConnectionHandle con_handle) {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Confirm Just Works handle=0x%04x\n",
		static_cast<unsigned>(con_handle));
	sm_just_works_confirm(con_handle);
	return BleError::kSuccess;
}

BleError SecurityManager::ConfirmNumericComparison(ConnectionHandle con_handle, bool accept) {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Confirm numeric comparison handle=0x%04x accept=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(accept));
	if(accept) {
		sm_numeric_comparison_confirm(con_handle);
	} else {
		sm_bonding_decline(con_handle);
	}
	return BleError::kSuccess;
}

BleError SecurityManager::ProvidePasskey(ConnectionHandle con_handle, uint32_t passkey) {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Provide passkey handle=0x%04x passkey=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(passkey));
	sm_passkey_input(con_handle, passkey);
	return BleError::kSuccess;
}

BleError SecurityManager::RequestPairing(ConnectionHandle con_handle) {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Request pairing handle=0x%04x\n",
		static_cast<unsigned>(con_handle));
	sm_request_pairing(con_handle);
	return BleError::kSuccess;
}

BleError SecurityManager::SetAuthorization(ConnectionHandle con_handle, AuthorizationResult result) {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set authorization handle=0x%04x result=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(result));
	if(result == AuthorizationResult::kGranted) {
		sm_authorization_grant(con_handle);
		return BleError::kSuccess;
	}
	sm_authorization_decline(con_handle);
	return BleError::kSuccess;
}

BleError SecurityManager::DispatchBleHciPacket(uint8_t packet_type, const uint8_t* packet, uint16_t size) {
	(void)size;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Dispatch HCI packet type=0x%02x\n",
		static_cast<unsigned>(packet_type));
	if(packet_type != HCI_EVENT_PACKET) {
		return BleError::kUnsupportedFeatureOrParameterValue;
	}
	uint8_t event = hci_event_packet_get_type(packet);
	C7222_BLE_DEBUG_PRINT("[BLE][SM] HCI event=0x%02x\n",
		static_cast<unsigned>(event));
	switch(event) {
		case SM_EVENT_JUST_WORKS_REQUEST: {
			const ConnectionHandle con_handle = sm_event_just_works_request_get_handle(packet);
			DispatchJustWorksRequest(con_handle);
			return BleError::kSuccess;
		}
		case SM_EVENT_NUMERIC_COMPARISON_REQUEST: {
			const ConnectionHandle con_handle = sm_event_numeric_comparison_request_get_handle(packet);
			const uint32_t number = sm_event_numeric_comparison_request_get_passkey(packet);
			DispatchNumericComparisonRequest(con_handle, number);
			return BleError::kSuccess;
		}
		case SM_EVENT_PASSKEY_DISPLAY_NUMBER: {
			const ConnectionHandle con_handle = sm_event_passkey_display_number_get_handle(packet);
			const uint32_t passkey = sm_event_passkey_display_number_get_passkey(packet);
			DispatchPasskeyDisplay(con_handle, passkey);
			return BleError::kSuccess;
		}
		case SM_EVENT_PASSKEY_INPUT_NUMBER: {
			const ConnectionHandle con_handle = sm_event_passkey_input_number_get_handle(packet);
			DispatchPasskeyInput(con_handle);
			return BleError::kSuccess;
		}
		case SM_EVENT_PAIRING_COMPLETE: {
			const ConnectionHandle con_handle = sm_event_pairing_complete_get_handle(packet);
			const uint8_t status_code = sm_event_pairing_complete_get_status(packet);
			if(auto* server = AttributeServer::GetInstance()) {
				if(status_code != ERROR_CODE_SUCCESS) {
					server->SetSecurityLevel(con_handle, 0);
				} else if(server->GetSecurityLevel(con_handle) == 0) {
					server->SetSecurityLevel(con_handle, ExpectedSecurityLevel(params_));
				}
			}
			DispatchPairingComplete(con_handle, ClassifyPairingStatus(status_code), status_code);
			return BleError::kSuccess;
		}
		case SM_EVENT_REENCRYPTION_COMPLETE: {
			const ConnectionHandle con_handle = sm_event_reencryption_complete_get_handle(packet);
			const uint8_t status = sm_event_reencryption_complete_get_status(packet);
			if(auto* server = AttributeServer::GetInstance()) {
				if(status != ERROR_CODE_SUCCESS) {
					server->SetSecurityLevel(con_handle, 0);
				} else if(server->GetSecurityLevel(con_handle) == 0) {
					server->SetSecurityLevel(con_handle, ExpectedSecurityLevel(params_));
				}
			}
			DispatchReencryptionComplete(con_handle, status);
			return BleError::kSuccess;
		}
		case SM_EVENT_AUTHORIZATION_REQUEST: {
			const ConnectionHandle con_handle = sm_event_authorization_request_get_handle(packet);
			DispatchAuthorizationRequest(con_handle);
			return BleError::kSuccess;
		}
		case SM_EVENT_AUTHORIZATION_RESULT: {
			const ConnectionHandle con_handle = sm_event_authorization_result_get_handle(packet);
			const uint8_t authorized = sm_event_authorization_result_get_authorization_result(packet);
			if(auto* server = AttributeServer::GetInstance()) {
				server->SetAuthorizationGranted(con_handle, authorized != 0);
			}
			DispatchAuthorizationResult(
				con_handle,
				authorized ? AuthorizationResult::kGranted : AuthorizationResult::kDenied);
			return BleError::kSuccess;
		}
		default:
			return BleError::kSuccess;
	}
}

}  // namespace c7222
