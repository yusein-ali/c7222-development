#include "attribute_server.hpp"

#include <btstack.h>

namespace c7222 {
namespace {

uint16_t att_read_callback(hci_con_handle_t connection_handle,
						   uint16_t attribute_handle,
						   uint16_t offset,
						   uint8_t* buffer,
						   uint16_t buffer_size) {
	(void) connection_handle;
	auto* server = AttributeServer::GetInstance();
	if(server == nullptr) {
		return ATT_ERROR_UNLIKELY_ERROR;
	}

	const AttributeServer::ReadResult result =
		server->ReadAttribute(attribute_handle, offset, buffer, buffer_size);
	if(result.ok) {
		return result.bytes;
	}

	uint8_t bt_error = ATT_ERROR_UNLIKELY_ERROR;
	if(btstack_map::ToBtStack(result.error, bt_error)) {
		return bt_error;
	}
	return ATT_ERROR_UNLIKELY_ERROR;
}

int att_write_callback(hci_con_handle_t connection_handle,
					   uint16_t attribute_handle,
					   uint16_t transaction_mode,
					   uint16_t offset,
					   uint8_t* buffer,
					   uint16_t buffer_size) {
	(void) connection_handle;
	(void) transaction_mode;
	auto* server = AttributeServer::GetInstance();
	if(server == nullptr) {
		return ATT_ERROR_UNLIKELY_ERROR;
	}

	const BleError status =
		server->WriteAttribute(attribute_handle, offset, buffer, buffer_size);
	if(status == BleError::kSuccess) {
		return 0;
	}

	uint8_t bt_error = ATT_ERROR_UNLIKELY_ERROR;
	if(btstack_map::ToBtStack(status, bt_error)) {
		return bt_error;
	}
	return ATT_ERROR_UNLIKELY_ERROR;
}

}  // namespace

BleError AttributeServer::InitPlatform(const uint8_t* att_db) {
	att_server_init(att_db, att_read_callback, att_write_callback);
	return BleError::kSuccess;
}

}  // namespace c7222
