#include "security_event_handler.hpp"

void SecurityEventHandler::OnJustWorksRequest(c7222::ConnectionHandle con_handle) const {
	std::printf("[SM] Just Works request: handle=0x%04x\n", con_handle);
	if(security_manager_ != nullptr) {
		// Minimal policy: auto-confirm Just Works pairing.
		(void) security_manager_->ConfirmJustWorks(con_handle);
	}
}

void SecurityEventHandler::OnNumericComparisonRequest(c7222::ConnectionHandle con_handle,
														 uint32_t number) const {
	std::printf("[SM] Numeric comparison: handle=0x%04x number=%lu\n",
			   con_handle,
			   static_cast<unsigned long>(number));
	if(security_manager_ != nullptr) {
		// Minimal policy: auto-confirm the displayed number.
		(void) security_manager_->ConfirmNumericComparison(con_handle, true);
	}
}

void SecurityEventHandler::OnPasskeyDisplay(c7222::ConnectionHandle con_handle,
												uint32_t passkey) const {
	std::printf("[SM] Passkey display: handle=0x%04x passkey=%06lu\n",
			   con_handle,
			   static_cast<unsigned long>(passkey));
}

void SecurityEventHandler::OnPasskeyInput(c7222::ConnectionHandle con_handle) const {
	std::printf("[SM] Passkey input requested: handle=0x%04x\n", con_handle);
	if(security_manager_ != nullptr) {
		// Minimal policy: provide a fixed passkey.
		(void) security_manager_->ProvidePasskey(con_handle, 123456);
	}
}

void SecurityEventHandler::OnPairingComplete(c7222::ConnectionHandle con_handle,
												 c7222::SecurityManager::PairingStatus status,
												 uint8_t status_code) const {
	// Report final pairing outcome for debugging.
	std::printf("[SM] Pairing complete: handle=0x%04x status=%u code=0x%02x\n",
			   con_handle,
			   static_cast<unsigned>(status),
			   static_cast<unsigned>(status_code));
}

void SecurityEventHandler::OnReencryptionComplete(c7222::ConnectionHandle con_handle,
													  uint8_t status) const {
	// Re-encryption happens when restoring link security.
	std::printf("[SM] Re-encryption complete: handle=0x%04x status=0x%02x\n",
			   con_handle,
			   static_cast<unsigned>(status));
}

void SecurityEventHandler::OnAuthorizationRequest(c7222::ConnectionHandle con_handle) const {
	std::printf("[SM] Authorization request: handle=0x%04x\n", con_handle);
	if(security_manager_ != nullptr) {
		// Minimal policy: grant authorization.
		(void) security_manager_->SetAuthorization(
			con_handle,
			c7222::SecurityManager::AuthorizationResult::kGranted);
	}
}

void SecurityEventHandler::OnAuthorizationResult(
	c7222::ConnectionHandle con_handle, c7222::SecurityManager::AuthorizationResult result) const {
	// Log the authorization result reported by the stack.
	std::printf("[SM] Authorization result: handle=0x%04x result=%u\n",
			   con_handle,
			   static_cast<unsigned>(result));
}

