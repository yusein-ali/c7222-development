/**
 * @file security_manager.hpp
 * @brief BLE Security Manager configuration and events.
 */
#ifndef ELEC_C7222_BLE_SECURITY_MANAGER_H_
#define ELEC_C7222_BLE_SECURITY_MANAGER_H_

#include <cstdint>
#include <iosfwd>
#include <list>

#include "ble_error.hpp"
#include "gap.hpp"
#include "non_copyable.hpp"

namespace c7222 {

/**
 * @class SecurityManager
 * @brief Manages BLE security configuration and pairing-related events.
 *
 * @details
 * This class provides a small, platform-agnostic surface for configuring
 * security requirements before a connection is established (for example, IO
 * capabilities, authentication requirements, and key size range). It also
 * exposes an event-handler interface for pairing prompts and results.
 */
class SecurityManager : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Security-related event identifiers.
	 */
	enum class EventId : uint8_t {
		kJustWorksRequest,
		kNumericComparisonRequest,
		kPasskeyDisplay,
		kPasskeyInput,
		kPairingComplete,
		kReencryptionComplete,
		kAuthorizationRequest,
		kAuthorizationResult,
	};

	/**
	 * @brief Local device IO capabilities used during pairing.
	 */
	enum class IoCapability : uint8_t {
		kDisplayOnly,
		kDisplayYesNo,
		kKeyboardOnly,
		kNoInputNoOutput,
		kKeyboardDisplay,
	};

	/**
	 * @brief Authentication requirement flags.
	 *
	 * These are modeled as a bitfield so they can be combined with |.
	 */
	enum class AuthenticationRequirement : uint8_t {
		kNone = 0,
		kBonding = 1u << 0,
		kMitmProtection = 1u << 1,
		kSecureConnections = 1u << 2,
		kKeypressNotifications = 1u << 3,
	};

	/**
	 * @brief Pairing outcome classification.
	 */
	enum class PairingStatus : uint8_t {
		kSuccess = 0x00,
		kFailed = 0x01,
		kTimeout = 0x02,
		kUnsupported = 0x03,
		kUnknown = 0xFF,
	};

	/**
	 * @brief Authorization decision.
	 */
	enum class AuthorizationResult : uint8_t {
		kDenied = 0,
		kGranted = 1,
	};

	/**
	 * @brief Role used for fixed passkey configuration.
	 */
	enum class FixedPasskeyRole : uint8_t {
		kNone = 0,
		kDisplay = 1,
		kKeyboard = 2,
	};

	/**
	 * @brief Cached security configuration parameters.
	 */
	struct SecurityParameters {
		IoCapability io_capability = IoCapability::kNoInputNoOutput;
		AuthenticationRequirement authentication = AuthenticationRequirement::kBonding;
		uint8_t min_encryption_key_size = 7;
		uint8_t max_encryption_key_size = 16;
		bool bondable = true;
		bool secure_connections_only = false;
		/**
		 * @brief Required security level for the GATT client (0 disables).
		 *
		 * This maps to gatt_client_set_required_security_level(level).
		 * Typical values are 0 (off) or 2 (authenticated/encrypted).
		 */
		uint8_t gatt_client_required_security_level = 0;
		/**
		 * @brief Fixed passkey configuration (0 disables).
		 */
		uint32_t fixed_passkey = 0;
		FixedPasskeyRole fixed_passkey_role = FixedPasskeyRole::kNone;
	};

	/**
	 * @brief Security Manager event callback interface.
	 *
	 * Event data references are only valid during the callback.
	 */
	struct EventHandler {
		virtual void OnJustWorksRequest(ConnectionHandle) const {}
		virtual void OnNumericComparisonRequest(ConnectionHandle, uint32_t) const {}
		virtual void OnPasskeyDisplay(ConnectionHandle, uint32_t) const {}
		virtual void OnPasskeyInput(ConnectionHandle) const {}
		virtual void OnPairingComplete(ConnectionHandle,
										  PairingStatus,
										  uint8_t /*status_code*/) const {}
		virtual void OnReencryptionComplete(ConnectionHandle, uint8_t /*status*/) const {}
		virtual void OnAuthorizationRequest(ConnectionHandle) const {}
		virtual void OnAuthorizationResult(ConnectionHandle, AuthorizationResult) const {}

	   protected:
		~EventHandler() = default;
	};

	/**
	 * @brief Get the singleton instance.
	 */
	static SecurityManager* GetInstance();

	// -----------------------------------------------------------------
	// Configuration (intended to be called before connections)
	// -----------------------------------------------------------------

	BleError Configure(const SecurityParameters& params);

	BleError SetIoCapability(IoCapability capability);
	BleError SetAuthenticationRequirements(AuthenticationRequirement auth);
	BleError SetEncryptionKeySizeRange(uint8_t min_key_size, uint8_t max_key_size);
	BleError SetBondable(bool bondable);
	BleError SetSecureConnectionsOnly(bool enabled);
	BleError SetGattClientRequiredSecurityLevel(uint8_t level);
	BleError SetFixedPasskeyDisplay(uint32_t passkey);
	BleError SetFixedPasskeyKeyboard(uint32_t passkey);
	BleError ClearFixedPasskey();

	SecurityParameters GetSecurityParameters() const {
		return params_;
	}

	/**
	 * @brief Check whether security has been explicitly configured by the application.
	 *
	 * This becomes true after any configuration setter (or Configure()) is called.
	 */
	[[nodiscard]] bool IsConfigured() const {
		return configured_;
	}

	/**
	 * @brief Check whether the configuration has been applied to the platform stack.
	 *
	 * This becomes true after a successful ApplyConfiguration() call.
	 */
	[[nodiscard]] bool IsApplied() const {
		return applied_;
	}

	// -----------------------------------------------------------------
	// Event handlers
	// -----------------------------------------------------------------

	void AddEventHandler(const EventHandler& handler);
	bool RemoveEventHandler(const EventHandler& handler);
	void ClearEventHandlers();

	// -----------------------------------------------------------------
	// Pairing / authorization responses
	// -----------------------------------------------------------------

	BleError ConfirmJustWorks(ConnectionHandle con_handle);
	BleError ConfirmNumericComparison(ConnectionHandle con_handle, bool accept);
	BleError ProvidePasskey(ConnectionHandle con_handle, uint32_t passkey);
	BleError RequestPairing(ConnectionHandle con_handle);
	BleError SetAuthorization(ConnectionHandle con_handle, AuthorizationResult result);

	/**
	 * @brief Dispatch a raw HCI packet into the security event pipeline.
	 */
	BleError DispatchBleHciPacket(uint8_t packet_type, const uint8_t* packet, uint16_t size);

   private:
	SecurityManager() = default;
	~SecurityManager() = default;

	BleError ApplyConfiguration();

	void DispatchJustWorksRequest(ConnectionHandle con_handle) const;
	void DispatchNumericComparisonRequest(ConnectionHandle con_handle, uint32_t number) const;
	void DispatchPasskeyDisplay(ConnectionHandle con_handle, uint32_t passkey) const;
	void DispatchPasskeyInput(ConnectionHandle con_handle) const;
	void DispatchPairingComplete(ConnectionHandle con_handle,
						   PairingStatus status,
						   uint8_t status_code) const;
	void DispatchReencryptionComplete(ConnectionHandle con_handle, uint8_t status) const;
	void DispatchAuthorizationRequest(ConnectionHandle con_handle) const;
	void DispatchAuthorizationResult(ConnectionHandle con_handle, AuthorizationResult result) const;

	static SecurityManager* instance_;

	SecurityParameters params_{};
	std::list<const EventHandler*> handlers_{};
	bool configured_ = false;
	bool applied_ = false;
};

// ---------------------------------------------------------------------
// Bitmask helpers for AuthenticationRequirement
// ---------------------------------------------------------------------

constexpr SecurityManager::AuthenticationRequirement
operator|(SecurityManager::AuthenticationRequirement lhs,
		  SecurityManager::AuthenticationRequirement rhs) {
	return static_cast<SecurityManager::AuthenticationRequirement>(
		static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr SecurityManager::AuthenticationRequirement
operator&(SecurityManager::AuthenticationRequirement lhs,
		  SecurityManager::AuthenticationRequirement rhs) {
	return static_cast<SecurityManager::AuthenticationRequirement>(
		static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr SecurityManager::AuthenticationRequirement
operator^(SecurityManager::AuthenticationRequirement lhs,
		  SecurityManager::AuthenticationRequirement rhs) {
	return static_cast<SecurityManager::AuthenticationRequirement>(
		static_cast<uint8_t>(lhs) ^ static_cast<uint8_t>(rhs));
}

constexpr SecurityManager::AuthenticationRequirement
operator~(SecurityManager::AuthenticationRequirement value) {
	return static_cast<SecurityManager::AuthenticationRequirement>(~static_cast<uint8_t>(value));
}

inline SecurityManager::AuthenticationRequirement&
operator|=(SecurityManager::AuthenticationRequirement& lhs,
		   SecurityManager::AuthenticationRequirement rhs) {
	lhs = lhs | rhs;
	return lhs;
}

inline SecurityManager::AuthenticationRequirement&
operator&=(SecurityManager::AuthenticationRequirement& lhs,
		   SecurityManager::AuthenticationRequirement rhs) {
	lhs = lhs & rhs;
	return lhs;
}

inline SecurityManager::AuthenticationRequirement&
operator^=(SecurityManager::AuthenticationRequirement& lhs,
		   SecurityManager::AuthenticationRequirement rhs) {
	lhs = lhs ^ rhs;
	return lhs;
}

/**
 * @brief Stream insertion operator for SecurityManager state and configuration.
 */
std::ostream& operator<<(std::ostream& os, const SecurityManager& sm);

}  // namespace c7222

#endif  // ELEC_C7222_BLE_SECURITY_MANAGER_H_
