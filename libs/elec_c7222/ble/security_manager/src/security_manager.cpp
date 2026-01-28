#include "security_manager.hpp"
#include "ble_utils.hpp"

#include <algorithm>
#include <ostream>

namespace c7222 {
namespace {

const char* ToString(SecurityManager::IoCapability cap) {
	switch(cap) {
		case SecurityManager::IoCapability::kDisplayOnly:
			return "DisplayOnly";
		case SecurityManager::IoCapability::kDisplayYesNo:
			return "DisplayYesNo";
		case SecurityManager::IoCapability::kKeyboardOnly:
			return "KeyboardOnly";
		case SecurityManager::IoCapability::kNoInputNoOutput:
			return "NoInputNoOutput";
		case SecurityManager::IoCapability::kKeyboardDisplay:
			return "KeyboardDisplay";
		default:
			return "Unknown";
	}
}

const char* ToString(SecurityManager::FixedPasskeyRole role) {
	switch(role) {
		case SecurityManager::FixedPasskeyRole::kNone:
			return "None";
		case SecurityManager::FixedPasskeyRole::kDisplay:
			return "Display";
		case SecurityManager::FixedPasskeyRole::kKeyboard:
			return "Keyboard";
		default:
			return "Unknown";
	}
}

bool HasAuthFlag(SecurityManager::AuthenticationRequirement auth,
				 SecurityManager::AuthenticationRequirement flag) {
	return static_cast<uint8_t>(auth & flag) != 0;
}

void WriteAuthenticationFlags(std::ostream& os, SecurityManager::AuthenticationRequirement auth) {
	if(auth == SecurityManager::AuthenticationRequirement::kNone) {
		os << "None";
		return;
	}

	bool first = true;
	auto write_flag = [&](const char* name) {
		if(!first) {
			os << "|";
		}
		os << name;
		first = false;
	};

	if(HasAuthFlag(auth, SecurityManager::AuthenticationRequirement::kBonding)) {
		write_flag("Bonding");
	}
	if(HasAuthFlag(auth, SecurityManager::AuthenticationRequirement::kMitmProtection)) {
		write_flag("MitmProtection");
	}
	if(HasAuthFlag(auth, SecurityManager::AuthenticationRequirement::kSecureConnections)) {
		write_flag("SecureConnections");
	}
	if(HasAuthFlag(auth, SecurityManager::AuthenticationRequirement::kKeypressNotifications)) {
		write_flag("KeypressNotifications");
	}
}

}  // namespace

SecurityManager* SecurityManager::instance_ = nullptr;

SecurityManager* SecurityManager::GetInstance() {
	if(instance_ == nullptr) {
		instance_ = new SecurityManager();
	}
	return instance_;
}

BleError SecurityManager::Configure(const SecurityParameters& params) {
	params_ = params;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT(
		"[BLE][SM] Configure: io=%s auth=0x%02x keysize=%u..%u bondable=%u sc_only=%u gatt_level=%u\n",
		ToString(params_.io_capability),
		static_cast<unsigned>(params_.authentication),
		static_cast<unsigned>(params_.min_encryption_key_size),
		static_cast<unsigned>(params_.max_encryption_key_size),
		static_cast<unsigned>(params_.bondable),
		static_cast<unsigned>(params_.secure_connections_only),
		static_cast<unsigned>(params_.gatt_client_required_security_level));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetIoCapability(IoCapability capability) {
	params_.io_capability = capability;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set IO capability: %s\n", ToString(capability));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetAuthenticationRequirements(AuthenticationRequirement auth) {
	params_.authentication = auth;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set auth requirements: 0x%02x\n",
		static_cast<unsigned>(auth));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetEncryptionKeySizeRange(uint8_t min_key_size, uint8_t max_key_size) {
	if(min_key_size == 0 || max_key_size == 0 || min_key_size > max_key_size) {
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Reject key size range: %u..%u\n",
			static_cast<unsigned>(min_key_size),
			static_cast<unsigned>(max_key_size));
		return BleError::kInvalidHciCommandParameters;
	}
	params_.min_encryption_key_size = min_key_size;
	params_.max_encryption_key_size = max_key_size;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set key size range: %u..%u\n",
		static_cast<unsigned>(min_key_size),
		static_cast<unsigned>(max_key_size));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetBondable(bool bondable) {
	params_.bondable = bondable;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set bondable: %u\n",
		static_cast<unsigned>(bondable));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetSecureConnectionsOnly(bool enabled) {
	params_.secure_connections_only = enabled;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set SC only: %u\n",
		static_cast<unsigned>(enabled));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetGattClientRequiredSecurityLevel(GattClientSecurityLevel level) {
	params_.gatt_client_required_security_level = level;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set GATT client security level: %u\n",
		static_cast<unsigned>(level));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetFixedPasskeyDisplay(uint32_t passkey) {
	params_.fixed_passkey = passkey;
	params_.fixed_passkey_role = FixedPasskeyRole::kDisplay;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set fixed passkey display: %u\n",
		static_cast<unsigned>(passkey));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::SetFixedPasskeyKeyboard(uint32_t passkey) {
	params_.fixed_passkey = passkey;
	params_.fixed_passkey_role = FixedPasskeyRole::kKeyboard;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Set fixed passkey keyboard: %u\n",
		static_cast<unsigned>(passkey));
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

BleError SecurityManager::ClearFixedPasskey() {
	params_.fixed_passkey = 0;
	params_.fixed_passkey_role = FixedPasskeyRole::kNone;
	configured_ = true;
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Clear fixed passkey\n");
	const BleError err = ApplyConfiguration();
	applied_ = (err == BleError::kSuccess);
	return err;
}

// ValidateConfiguration is platform-specific (implemented in platform layer).

void SecurityManager::AddEventHandler(const EventHandler& handler) {
	auto it = std::find(handlers_.begin(), handlers_.end(), &handler);
	if(it == handlers_.end()) {
		handlers_.push_back(&handler);
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Add event handler: count=%u\n",
			static_cast<unsigned>(handlers_.size()));
	}
}

bool SecurityManager::RemoveEventHandler(const EventHandler& handler) {
	const auto before = handlers_.size();
	handlers_.remove(&handler);
	if(handlers_.size() != before) {
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Remove event handler: count=%u\n",
			static_cast<unsigned>(handlers_.size()));
	}
	return handlers_.size() != before;
}
void SecurityManager::AddEventHandler(const EventHandler* handler) {
	auto it = std::find(handlers_.begin(), handlers_.end(), handler);
	if(it == handlers_.end()) {
		handlers_.push_back(handler);
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Add event handler ptr: count=%u\n",
			static_cast<unsigned>(handlers_.size()));
	}
}
bool SecurityManager::RemoveEventHandler(const EventHandler* handler) {
	const auto before = handlers_.size();
	handlers_.remove(handler);
	if(handlers_.size() != before) {
		C7222_BLE_DEBUG_PRINT("[BLE][SM] Remove event handler ptr: count=%u\n",
			static_cast<unsigned>(handlers_.size()));
	}
	return handlers_.size() != before;
}

void SecurityManager::ClearEventHandlers() {
	handlers_.clear();
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Clear event handlers\n");
}

void SecurityManager::DispatchJustWorksRequest(ConnectionHandle con_handle) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Just Works request handle=0x%04x\n",
		static_cast<unsigned>(con_handle));
	for(const auto* handler : handlers_) {
		handler->OnJustWorksRequest(con_handle);
	}
}

void SecurityManager::DispatchNumericComparisonRequest(ConnectionHandle con_handle, uint32_t number) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Numeric comparison handle=0x%04x value=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(number));
	for(const auto* handler : handlers_) {
		handler->OnNumericComparisonRequest(con_handle, number);
	}
}

void SecurityManager::DispatchPasskeyDisplay(ConnectionHandle con_handle, uint32_t passkey) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Passkey display handle=0x%04x passkey=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(passkey));
	for(const auto* handler : handlers_) {
		handler->OnPasskeyDisplay(con_handle, passkey);
	}
}

void SecurityManager::DispatchPasskeyInput(ConnectionHandle con_handle) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Passkey input handle=0x%04x\n",
		static_cast<unsigned>(con_handle));
	for(const auto* handler : handlers_) {
		handler->OnPasskeyInput(con_handle);
	}
}

void SecurityManager::DispatchPairingComplete(ConnectionHandle con_handle,
										 PairingStatus status,
										 uint8_t status_code) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Pairing complete handle=0x%04x status=%u code=0x%02x\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(status),
		static_cast<unsigned>(status_code));
	for(const auto* handler : handlers_) {
		handler->OnPairingComplete(con_handle, status, status_code);
	}
}

void SecurityManager::DispatchReencryptionComplete(ConnectionHandle con_handle, uint8_t status) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Re-encryption complete handle=0x%04x status=0x%02x\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(status));
	for(const auto* handler : handlers_) {
		handler->OnReencryptionComplete(con_handle, status);
	}
}

void SecurityManager::DispatchAuthorizationRequest(ConnectionHandle con_handle) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Authorization request handle=0x%04x\n",
		static_cast<unsigned>(con_handle));
	for(const auto* handler : handlers_) {
		handler->OnAuthorizationRequest(con_handle);
	}
}

void SecurityManager::DispatchAuthorizationResult(ConnectionHandle con_handle, AuthorizationResult result) const {
	C7222_BLE_DEBUG_PRINT("[BLE][SM] Event: Authorization result handle=0x%04x result=%u\n",
		static_cast<unsigned>(con_handle),
		static_cast<unsigned>(result));
	for(const auto* handler : handlers_) {
		handler->OnAuthorizationResult(con_handle, result);
	}
}

std::ostream& operator<<(std::ostream& os, const SecurityManager& sm) {
	const auto params = sm.GetSecurityParameters();

	os << "SecurityManager{";
	os << " configured=" << (sm.IsConfigured() ? "true" : "false");
	os << ", applied=" << (sm.IsApplied() ? "true" : "false");
	os << ", io_capability=" << ToString(params.io_capability);
	os << ", authentication=";
	WriteAuthenticationFlags(os, params.authentication);
	os << ", key_size_range=" << static_cast<unsigned>(params.min_encryption_key_size) << "-"
	   << static_cast<unsigned>(params.max_encryption_key_size);
	os << ", secure_connections_only=" << (params.secure_connections_only ? "true" : "false");
	os << ", gatt_client_required_security_level="
	   << static_cast<unsigned>(params.gatt_client_required_security_level);
	os << ", fixed_passkey_role=" << ToString(params.fixed_passkey_role);
	os << ", fixed_passkey=" << static_cast<unsigned long>(params.fixed_passkey);
	os << " }";
	return os;
}

}  // namespace c7222
