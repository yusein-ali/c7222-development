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
 * This class provides a platform-agnostic interface for configuring security
 * requirements before connections are established (IO capabilities, pairing
 * flags, key sizes, and GATT client requirements) and for handling pairing and
 * authorization events at runtime.
 *
 * ---
 * ### Responsibilities
 *
 * - **Configuration:** Collects security parameters and applies them to the
 *   underlying stack (BTstack on Pico W).
 * - **Validation:** Ensures the requested security level is achievable for the
 *   current IO capability and stack configuration.
 * - **Events:** Dispatches pairing prompts and results to registered handlers.
 * - **Responses:** Provides APIs to confirm or reject pairing operations.
 *
 * ---
 * ### Configuration Workflow
 *
 * Configure security before enabling the stack or accepting connections:
 * @code
 * auto* sm = c7222::SecurityManager::GetInstance();
 * c7222::SecurityManager::SecurityParameters params;
 * params.io_capability =
 *     c7222::SecurityManager::IoCapability::kKeyboardDisplay;
 * params.authentication =
 *     c7222::SecurityManager::AuthenticationRequirement::kMitmProtection;
 * params.min_encryption_key_size = 7;
 * params.max_encryption_key_size = 16;
 * sm->Configure(params);
 * @endcode
 *
 * The BLE layer will apply the configuration when SecurityManager is enabled.
 *
 * ---
 * ### Event Handling
 *
 * Implement `EventHandler` and register it to receive pairing prompts and
 * authorization callbacks. Event payloads are only valid during the call.
 *
 * ---
 * ### Validation
 *
 * Use `ValidateConfiguration(authentication_required, authorization_required,
 * encryption_required)` to ensure the current configuration can satisfy the
 * security requirements of your GATT database.
 *
 * ---
 * ### Typical Usage
 *
 * @code
 * class MySmHandler : public c7222::SecurityManager::EventHandler {
 *  public:
 *   void OnJustWorksRequest(c7222::ConnectionHandle handle) const override {
 *     c7222::SecurityManager::GetInstance()->ConfirmJustWorks(handle);
 *   }
 *   void OnPasskeyDisplay(c7222::ConnectionHandle, uint32_t passkey) const override {
 *     printf("Passkey: %u\n", passkey);
 *   }
 * };
 *
 * auto* sm = c7222::SecurityManager::GetInstance();
 * MySmHandler handler;
 * sm->AddEventHandler(handler);
 * sm->SetIoCapability(c7222::SecurityManager::IoCapability::kDisplayOnly);
 * sm->SetAuthenticationRequirements(
 *     c7222::SecurityManager::AuthenticationRequirement::kMitmProtection);
 * @endcode
 *
 * ---
 * ### Raspberry Pi Pico (BTstack) Port
 *
 * The Pico W/RP2040 implementation lives in
 * `libs/elec_c7222/ble/security_manager/platform/rpi_pico/`. The platform
 * layer applies configuration via BTstack APIs and dispatches SM events to
 * the handlers registered on this class. It also performs platform-specific
 * validation in `ValidateConfiguration()` based on the BTstack configuration
 * and available key storage backend.
 */
class SecurityManager : public NonCopyableNonMovable {
   public:
	/**
	 * @brief Security-related event identifiers.
	 */
	enum class EventId : uint8_t {
		/**
		 * @brief "Just Works" confirmation requested by the stack.
		 */
		kJustWorksRequest,
		/**
		 * @brief Numeric comparison requested by the stack.
		 */
		kNumericComparisonRequest,
		/**
		 * @brief Passkey display requested (display role).
		 */
		kPasskeyDisplay,
		/**
		 * @brief Passkey input requested (keyboard role).
		 */
		kPasskeyInput,
		/**
		 * @brief Pairing complete indication.
		 */
		kPairingComplete,
		/**
		 * @brief Re-encryption complete indication.
		 */
		kReencryptionComplete,
		/**
		 * @brief Authorization request for an authenticated operation.
		 */
		kAuthorizationRequest,
		/**
		 * @brief Authorization decision result.
		 */
		kAuthorizationResult,
	};

	/**
	 * @brief Local device IO capabilities used during pairing.
	 */
	enum class IoCapability : uint8_t {
		/**
		 * @brief Device can only display a passkey.
		 */
		kDisplayOnly,
		/**
		 * @brief Device can display and confirm yes/no.
		 */
		kDisplayYesNo,
		/**
		 * @brief Device can only input a passkey.
		 */
		kKeyboardOnly,
		/**
		 * @brief Device has no input and no output.
		 */
		kNoInputNoOutput,
		/**
		 * @brief Device can both display and input.
		 */
		kKeyboardDisplay,
	};

	/**
	 * @brief Authentication requirement flags.
	 *
	 * These are modeled as a bitfield so they can be combined with |.
	 */
	enum class AuthenticationRequirement : uint8_t {
		/**
		 * @brief No additional authentication requirements.
		 */
		kNone = 0,
		/**
		 * @brief Enable bonding (store keys for reconnection).
		 */
		kBonding = 1u << 0,
		/**
		 * @brief Require MITM protection.
		 */
		kMitmProtection = 1u << 1,
		/**
		 * @brief Require LE Secure Connections.
		 */
		kSecureConnections = 1u << 2,
		/**
		 * @brief Enable keypress notifications (SC only).
		 */
		kKeypressNotifications = 1u << 3,
	};

	/**
	 * @brief Pairing outcome classification.
	 */
	enum class PairingStatus : uint8_t {
		/**
		 * @brief Pairing completed successfully.
		 */
		kSuccess = 0x00,
		/**
		 * @brief Pairing failed.
		 */
		kFailed = 0x01,
		/**
		 * @brief Pairing timed out.
		 */
		kTimeout = 0x02,
		/**
		 * @brief Pairing not supported by peer or stack.
		 */
		kUnsupported = 0x03,
		/**
		 * @brief Unknown or unclassified result.
		 */
		kUnknown = 0xFF,
	};

	/**
	 * @brief Authorization decision.
	 */
	enum class AuthorizationResult : uint8_t {
		/**
		 * @brief Authorization denied.
		 */
		kDenied = 0,
		/**
		 * @brief Authorization granted.
		 */
		kGranted = 1,
	};

	/**
	 * @brief Role used for fixed passkey configuration.
	 */
	enum class FixedPasskeyRole : uint8_t {
		/**
		 * @brief Fixed passkey not configured.
		 */
		kNone = 0,
		/**
		 * @brief Device displays a fixed passkey.
		 */
		kDisplay = 1,
		/**
		 * @brief Device inputs a fixed passkey.
		 */
		kKeyboard = 2,
	};

	/**
	 * @brief Required security level for the BTstack GATT client.
	 *
	 * This maps to BTstack's gap_security_level_t used by
	 * gatt_client_set_required_security_level(level).
	 */
	enum class GattClientSecurityLevel : uint8_t {
		/**
		 * @brief No required security.
		 */
		kLevel0 = 0,
		/**
		 * @brief Encrypted link required.
		 */
		kLevel1 = 1,
		/**
		 * @brief Authenticated + encrypted link required.
		 */
		kLevel2 = 2,
		/**
		 * @brief Authorized + authenticated + encrypted link required.
		 */
		kLevel3 = 3,
		/**
		 * @brief Secure Connections authenticated + encrypted.
		 */
		kLevel4 = 4,
	};

	/**
	 * @brief Cached security configuration parameters.
	 */
	struct SecurityParameters {
		/**
		 * @brief IO capability used to select pairing method.
		 */
		IoCapability io_capability = IoCapability::kNoInputNoOutput;
		/**
		 * @brief Authentication requirement bitfield.
		 */
		AuthenticationRequirement authentication = AuthenticationRequirement::kBonding;
		/**
		 * @brief Minimum encryption key size (7..16).
		 */
		uint8_t min_encryption_key_size = 7;
		/**
		 * @brief Maximum encryption key size (7..16).
		 */
		uint8_t max_encryption_key_size = 16;
		/**
		 * @brief Whether the device is bondable.
		 */
		bool bondable = true;
		/**
		 * @brief Whether to allow only LE Secure Connections.
		 */
		bool secure_connections_only = false;
		/**
		 * @brief Required security level for the GATT client (0 disables).
		 *
		 * This maps to gatt_client_set_required_security_level(level).
		 * Typical values are kLevel0 (off) or kLevel2 (authenticated/encrypted).
		 */
		GattClientSecurityLevel gatt_client_required_security_level = GattClientSecurityLevel::kLevel0;
		/**
		 * @brief Fixed passkey configuration (0 disables).
		 */
		uint32_t fixed_passkey = 0;
		/**
		 * @brief Role used for fixed passkey display/input.
		 */
		FixedPasskeyRole fixed_passkey_role = FixedPasskeyRole::kNone;
	};

	/**
	 * @brief Security Manager event callback interface.
	 *
	 * Event data references are only valid during the callback.
	 */
	struct EventHandler {
		/**
		 * @brief Called when "Just Works" confirmation is requested.
		 */
		virtual void OnJustWorksRequest(ConnectionHandle) const {}
		/**
		 * @brief Called when numeric comparison is requested.
		 */
		virtual void OnNumericComparisonRequest(ConnectionHandle, uint32_t) const {}
		/**
		 * @brief Called when the stack requests passkey display.
		 */
		virtual void OnPasskeyDisplay(ConnectionHandle, uint32_t) const {}
		/**
		 * @brief Called when the stack requests passkey input.
		 */
		virtual void OnPasskeyInput(ConnectionHandle) const {}
		/**
		 * @brief Called when pairing completes.
		 *
		 * @param status High-level pairing status.
		 * @param status_code Stack-specific status value.
		 */
		virtual void OnPairingComplete(ConnectionHandle,
									   PairingStatus,
									   uint8_t /*status_code*/) const {}
		/**
		 * @brief Called when re-encryption completes.
		 */
		virtual void OnReencryptionComplete(ConnectionHandle, uint8_t /*status*/) const {}
		/**
		 * @brief Called when authorization is required.
		 */
		virtual void OnAuthorizationRequest(ConnectionHandle) const {}
		/**
		 * @brief Called when an authorization decision is available.
		 */
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

	/**
	 * @brief Apply a full configuration object at once.
	 *
	 * Marks the SecurityManager as configured; the BLE layer applies the settings
	 * to the platform when security is enabled.
	 */
	BleError Configure(const SecurityParameters& params);

	/**
	 * @brief Set the local IO capability.
	 */
	BleError SetIoCapability(IoCapability capability);
	/**
	 * @brief Set authentication requirement bitfield.
	 */
	BleError SetAuthenticationRequirements(AuthenticationRequirement auth);
	/**
	 * @brief Set minimum and maximum encryption key sizes.
	 */
	BleError SetEncryptionKeySizeRange(uint8_t min_key_size, uint8_t max_key_size);
	/**
	 * @brief Enable or disable bonding.
	 */
	BleError SetBondable(bool bondable);
	/**
	 * @brief Enable or disable Secure Connections only mode.
	 */
	BleError SetSecureConnectionsOnly(bool enabled);
	/**
	 * @brief Set required security level for GATT client operations.
	 */
	BleError SetGattClientRequiredSecurityLevel(GattClientSecurityLevel level);
	/**
	 * @brief Configure fixed passkey for display role.
	 */
	BleError SetFixedPasskeyDisplay(uint32_t passkey);
	/**
	 * @brief Configure fixed passkey for keyboard role.
	 */
	BleError SetFixedPasskeyKeyboard(uint32_t passkey);
	/**
	 * @brief Clear fixed passkey configuration.
	 */
	BleError ClearFixedPasskey();

	/**
	 * @brief Get the cached security parameters.
	 */
	[[nodiscard]] SecurityParameters GetSecurityParameters() const {
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

	/**
	 * @brief Validate that the current configuration can satisfy requirements.
	 *
	 * This check is platform-dependent and should be called before enabling BLE
	 * when the GATT database requires specific security guarantees.
	 */
	bool ValidateConfiguration(bool authentication_required,
							   bool authorization_required,
							   bool encryption_required) const;

	// -----------------------------------------------------------------
	// Event handlers
	// -----------------------------------------------------------------

	/**
	 * @brief Add an event handler (stored as a pointer).
	 */
	void AddEventHandler(const EventHandler& handler);
	/**
	 * @brief Remove an event handler.
	 */
	bool RemoveEventHandler(const EventHandler& handler);
	/**
	 * @brief Add an event handler pointer (no ownership).
	 */
	void AddEventHandler(const EventHandler* handler);
	/**
	 * @brief Remove an event handler pointer.
	 */
	bool RemoveEventHandler(const EventHandler* handler);
	/**
	 * @brief Remove all registered event handlers.
	 */
	void ClearEventHandlers();
	/**
	 * @brief Get the number of registered event handlers.
	 */
	[[nodiscard]] size_t GetEventHandlerCount() const {
		return handlers_.size();
	}
	// -----------------------------------------------------------------
	// Pairing / authorization responses
	// -----------------------------------------------------------------

	/**
	 * @brief Confirm a "Just Works" pairing request.
	 */
	BleError ConfirmJustWorks(ConnectionHandle con_handle);
	/**
	 * @brief Accept or reject a numeric comparison pairing request.
	 */
	BleError ConfirmNumericComparison(ConnectionHandle con_handle, bool accept);
	/**
	 * @brief Provide a passkey for keyboard input role.
	 */
	BleError ProvidePasskey(ConnectionHandle con_handle, uint32_t passkey);
	/**
	 * @brief Initiate pairing as a client or server.
	 */
	BleError RequestPairing(ConnectionHandle con_handle);
	/**
	 * @brief Provide an authorization decision for the given connection.
	 */
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
	return static_cast<SecurityManager::AuthenticationRequirement>(static_cast<uint8_t>(lhs) |
																   static_cast<uint8_t>(rhs));
}

constexpr SecurityManager::AuthenticationRequirement
operator&(SecurityManager::AuthenticationRequirement lhs,
		  SecurityManager::AuthenticationRequirement rhs) {
	return static_cast<SecurityManager::AuthenticationRequirement>(static_cast<uint8_t>(lhs) &
																   static_cast<uint8_t>(rhs));
}

constexpr SecurityManager::AuthenticationRequirement
operator^(SecurityManager::AuthenticationRequirement lhs,
		  SecurityManager::AuthenticationRequirement rhs) {
	return static_cast<SecurityManager::AuthenticationRequirement>(static_cast<uint8_t>(lhs) ^
																   static_cast<uint8_t>(rhs));
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

#endif	// ELEC_C7222_BLE_SECURITY_MANAGER_H_
