#ifndef EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_
#define EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_

#include <cstdint>
#include <cstdio>

#include "security_manager.hpp"


/**
 * Minimal security event handler for the GATT server example.
 *
 * This keeps behavior explicit during debugging and provides simple
 * default decisions for pairing/authorization flows.
 */
class SecurityEventHandler final : public c7222::SecurityManager::EventHandler {
   public:
	explicit SecurityEventHandler(c7222::SecurityManager* security_manager)
		: security_manager_(security_manager) {}
	SecurityEventHandler() = default;

	void SetSecurityManager(c7222::SecurityManager* security_manager) {
		security_manager_ = security_manager;
	}

	void OnJustWorksRequest(c7222::ConnectionHandle con_handle) const override;
	void OnNumericComparisonRequest(c7222::ConnectionHandle con_handle, uint32_t number) const override;
	void OnPasskeyDisplay(c7222::ConnectionHandle con_handle, uint32_t passkey) const override;
	void OnPasskeyInput(c7222::ConnectionHandle con_handle) const override;
	void OnPairingComplete(c7222::ConnectionHandle con_handle,
						   c7222::SecurityManager::PairingStatus status,
						   uint8_t status_code) const override;
	void OnReencryptionComplete(c7222::ConnectionHandle con_handle, uint8_t status) const override;
	void OnAuthorizationRequest(c7222::ConnectionHandle con_handle) const override;
	void OnAuthorizationResult(c7222::ConnectionHandle con_handle,
							   c7222::SecurityManager::AuthorizationResult result) const override;

   private:
	c7222::SecurityManager* security_manager_ = nullptr;
};


#endif  // EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_

