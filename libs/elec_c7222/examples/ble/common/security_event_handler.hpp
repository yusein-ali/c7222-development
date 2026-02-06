/**
 * @file security_event_handler.hpp
 * @brief Minimal Security Manager event handler for BLE examples.
 *
 * This header declares a small helper class that implements
 * `c7222::SecurityManager::EventHandler` and prints Security Manager events
 * to the console. It is used in BLE examples to observe pairing, passkey, and
 * authorization flows with minimal policy decisions.
 *
 * The class holds an optional `SecurityManager` pointer so it can confirm
 * pairing operations and apply simple authorization decisions in response to
 * events. Without this pointer, the handler can still log events but cannot
 * drive the pairing flow forward.
 */
#ifndef EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_
#define EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_

#include <cstdint>
#include <cstdio>

#include "security_manager.hpp"


/**
 * @brief Minimal security event handler for BLE examples.
 *
 * The implementation intentionally keeps policies simple and explicit to make
 * debugging easier in a teaching environment.
 *
 * The stored `SecurityManager` instance is used to:
 * - Confirm Just Works pairing requests.
 * - Confirm numeric comparison requests.
 * - Provide a fixed passkey for input requests.
 * - Grant authorization on request.
 */
class SecurityEventHandler final : public c7222::SecurityManager::EventHandler {
   public:
	/**
	 * @brief Construct with an optional SecurityManager pointer.
	 *
	 * @param security_manager Security manager used for confirmations and authorization.
	 */
	explicit SecurityEventHandler(c7222::SecurityManager* security_manager)
		: security_manager_(security_manager) {}
	/// Default constructor (manager can be set later).
	SecurityEventHandler() = default;

	/**
	 * @brief Attach a SecurityManager after construction.
	 */
	void SetSecurityManager(c7222::SecurityManager* security_manager) {
		security_manager_ = security_manager;
	}

	/**
	 * @brief Handle "Just Works" pairing request.
	 *
	 * Logs the event and auto-confirms if a SecurityManager is available.
	 */
	void OnJustWorksRequest(c7222::ConnectionHandle con_handle) const override;
	/**
	 * @brief Handle numeric comparison pairing request.
	 *
	 * Logs the number and auto-confirms if a SecurityManager is available.
	 */
	void OnNumericComparisonRequest(c7222::ConnectionHandle con_handle, uint32_t number) const override;
	/**
	 * @brief Handle passkey display event.
	 *
	 * Logs the passkey for user confirmation.
	 */
	void OnPasskeyDisplay(c7222::ConnectionHandle con_handle, uint32_t passkey) const override;
	/**
	 * @brief Handle passkey input request.
	 *
	 * Logs the request and provides a fixed passkey (123456) in the example
	 * implementation.
	 */
	void OnPasskeyInput(c7222::ConnectionHandle con_handle) const override;
	/**
	 * @brief Handle pairing completion event.
	 *
	 * Logs the final pairing status and status code.
	 */
	void OnPairingComplete(c7222::ConnectionHandle con_handle,
						   c7222::SecurityManager::PairingStatus status,
						   uint8_t status_code) const override;
	/**
	 * @brief Handle reencryption completion event.
	 *
	 * Logs status for debugging link security changes.
	 */
	void OnReencryptionComplete(c7222::ConnectionHandle con_handle, uint8_t status) const override;
	/**
	 * @brief Handle authorization request for a connection.
	 *
	 * Logs the request and grants authorization in the example implementation.
	 */
	void OnAuthorizationRequest(c7222::ConnectionHandle con_handle) const override;
	/**
	 * @brief Handle authorization result event.
	 *
	 * Logs the authorization outcome.
	 */
	void OnAuthorizationResult(c7222::ConnectionHandle con_handle,
							   c7222::SecurityManager::AuthorizationResult result) const override;

   private:
	c7222::SecurityManager* security_manager_ = nullptr;
};


#endif  // EXAMPLES_BLE_GATT_SERVER_SECURITY_EVENT_HANDLER_HPP_
