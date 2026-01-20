#include "attribute_server.hpp"

#include <btstack.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

namespace c7222 {
namespace {

// Binary parsing helpers
constexpr size_t kEntryHeaderSize = 6;	// Size + Flags + Handle
constexpr size_t kUuid16Size = 2;
constexpr size_t kUuid128Size = 16;
constexpr size_t kValue16Offset = kEntryHeaderSize + kUuid16Size;	 // 8
constexpr size_t kValue128Offset = kEntryHeaderSize + kUuid128Size;	 // 22

uint16_t ReadLe16(const uint8_t* data) {
	return *(reinterpret_cast<const uint16_t*>(data));
}

std::array<uint8_t, 16> ReverseUuid128(const uint8_t* data) {
	std::array<uint8_t, 16> out{};
	for(size_t i = 0; i < out.size(); ++i) {
		out[i] = data[out.size() - 1 - i];
	}
	return out;
}

Attribute ParseEntry(const uint8_t* ptr, uint16_t entry_size) {
	const uint16_t flags = ReadLe16(ptr + 2);
	const uint16_t handle = ReadLe16(ptr + 4);
	const uint8_t* uuid_ptr = ptr + 6;

	if((flags & static_cast<uint16_t>(Attribute::Properties::kUuid128)) != 0) {
		if(entry_size < kValue128Offset) {
			return Attribute();
		}
		const uint8_t* value_ptr = ptr + kValue128Offset;
		const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue128Offset);
		const std::array<uint8_t, 16> uuid_bytes = ReverseUuid128(uuid_ptr);
		return Attribute(Uuid(uuid_bytes), flags, value_ptr, value_len, handle);
	}

	if(entry_size < kValue16Offset) {
		return Attribute();
	}
	const uint16_t uuid16 = ReadLe16(uuid_ptr);
	const uint8_t* value_ptr = ptr + kValue16Offset;
	const uint16_t value_len = static_cast<uint16_t>(entry_size - kValue16Offset);
	return Attribute(Uuid(uuid16), flags, value_ptr, value_len, handle);
}

std::unique_ptr<std::list<Attribute>> ParseAttributesFromDb(const uint8_t* db) {
	auto attributes = std::make_unique<std::list<Attribute>>();

	if(db == nullptr) {
		return attributes;
	}

	const uint8_t* ptr = db + 1;

	while(true) {
		const uint16_t entry_size = ReadLe16(ptr);
		if(entry_size == 0) {
			break;
		}
		if(entry_size < kEntryHeaderSize) {
			break;
		}

		attributes->emplace_back(std::move(ParseEntry(ptr, entry_size)));
		ptr += entry_size;
	}

	return attributes;
}

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

	const BleError status = server->WriteAttribute(attribute_handle, offset, buffer, buffer_size);
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

BleError AttributeServer::Init(const void* context) {
	// Reset runtime state before re-initializing from the platform context.
	services_.clear();
	connection_handle_ = 0;
	initialized_ = false;

	assert(context != nullptr &&
		   "AttributeServer::Init called with null context for"
		   "RPi Pico platform. It must be a pointer to the ATT DB blob.");
	if(context == nullptr) {
		return BleError::kUnspecifiedError;
	}

	// Cache the ATT DB pointer the first time we see it.
	if(context_ == nullptr) {
		context_ = context;
	}

	const auto* att_db = static_cast<const uint8_t*>(context_);
	// Parse the BTstack ATT DB into Attribute objects and build services.
	auto attributes = ParseAttributesFromDb(att_db);
	if(attributes) {
		InitServices(*attributes);
	}

	// Ensure L2CAP/SM are initialized before registering the ATT server.
	l2cap_init();
	sm_init();

	// Register ATT read/write callbacks with BTstack using the ATT DB blob.
	att_server_init(att_db, att_read_callback, att_write_callback);
	initialized_ = true;
	return BleError::kSuccess;
}

}  // namespace c7222
