/**
 * @file attribute.hpp
 * @brief GATT attribute wrapper for BTstack ATT fields.
 */
#ifndef ELEC_C7222_BLE_GATT_ATTRIBUTE_H_
#define ELEC_C7222_BLE_GATT_ATTRIBUTE_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>
#include "uuid.hpp"

namespace c7222 {

/**
 * @brief ATT attribute wrapper with BTstack-compatible fields.
 */
class Attribute {
   public:
	/**
	 * @brief Attribute flags with direct BTstack ATT_PROPERTY_* mapping.
	 */
	enum class Properties : std::uint16_t {
		kNone = 0x0000,
		kWritePermissionBit0 = 0x0001,
		kRead = 0x0002,
		kWriteWithoutResponse = 0x0004,
		kWrite = 0x0008,
		kWritePermissionBit1 = 0x0010,
		kReadPermissionSc = 0x0020,
		kAuthenticatedSignedWrite = 0x0040,
		kWritePermissionSc = 0x0080,
		kDynamic = 0x0100,
		kUuid128 = 0x0200,
		kReadPermissionBit0 = 0x0400,
		kReadPermissionBit1 = 0x0800,
		kEncryptionKeySizeMask = 0xF000
	};

	/**
	 * @brief Callback type for reading attribute value.
	 * Parameters: offset, destination buffer, buffer size
	 * Returns: number of bytes read, or ATT error code if > 0xfe00
	 */
	using ReadCallback = std::function<uint16_t(uint16_t offset, uint8_t* buffer, uint16_t buffer_size)>;

	/**
	 * @brief Callback type for writing attribute value.
	 * Parameters: offset, source data, data size
	 * Returns: ATT error code (0 = success)
	 */
	using WriteCallback = std::function<uint16_t(uint16_t offset, const uint8_t* data, uint16_t size)>;

	Attribute() = default;
	
	/**
	 * @brief Construct an attribute with a UUID and properties.
	 */
	Attribute(const Uuid& uuid, uint16_t properties, uint16_t handle = 0)
		: uuid_(uuid), properties_(properties), handle_(handle) {
		UpdateUuidProperty();
	}

	/**
	 * @brief Construct an attribute with a UUID, properties, and value data.
	 */
	Attribute(const Uuid& uuid, uint16_t properties, const uint8_t* data, size_t size, uint16_t handle = 0)
		: uuid_(uuid), properties_(properties), handle_(handle) {
		UpdateUuidProperty();
		if(data != nullptr && size > 0) {
			value_.assign(data, data + size);
		}
	}

	uint16_t handle() const {
		return handle_;
	}

	void SetHandle(uint16_t handle) {
		handle_ = handle;
	}

	const Uuid& uuid() const {
		return uuid_;
	}

	bool IsUuid128() const {
		return uuid_.Is128Bit();
	}

	/**
	 * @brief Check if this attribute matches a UUID.
	 * @param uuid UUID to compare
	 * @return true if UUIDs match
	 */
	bool IsThisAttribute(const Uuid& uuid) const {
		return uuid_ == uuid;
	}

	/**
	 * @brief Check if this attribute matches a handle.
	 * @param handle Attribute handle to compare
	 * @return true if handles match and handle != 0
	 */
	bool IsThisAttribute(uint16_t handle) const {
		return handle != 0 && handle_ == handle;
	}

	/**
	 * @brief Check if this attribute matches both UUID and handle.
	 * @param uuid UUID to compare
	 * @param handle Attribute handle to compare
	 * @return true if both UUID and handle match
	 */
	bool IsThisAttribute(const Uuid& uuid, uint16_t handle) const {
		return uuid_ == uuid && handle_ == handle;
	}

	uint16_t properties() const {
		return properties_;
	}

	void SetProperties(uint16_t properties) {
		properties_ = properties;
		UpdateUuidProperty();
	}

	const std::vector<uint8_t>& value() const {
		return value_;
	}

	const uint8_t* GetValueData() const {
		return value_.empty() ? nullptr : value_.data();
	}

	size_t GetValueSize() const {
		return value_.size();
	}

	bool SetValue(const uint8_t* data, size_t size) {
		if(data == nullptr || size == 0) {
			value_.clear();
			return true;
		}
		if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) == 0 &&
		   size > value_.size()) {
			return false;
		}
		value_.assign(data, data + size);
		return true;
	}

	/**
	 * @brief Set the read callback for this attribute.
	 * Called when a remote client reads this attribute.
	 * @param callback Function to handle reads
	 */
	void SetReadCallback(ReadCallback callback) {
		read_callback_ = std::move(callback);
	}

	/**
	 * @brief Check if a read callback is registered.
	 * @return true if read callback is set
	 */
	bool HasReadCallback() const {
		return static_cast<bool>(read_callback_);
	}

	/**
	 * @brief Invoke the read callback.
	 * @param offset Offset into the attribute value
	 * @param buffer Destination buffer for data
	 * @param buffer_size Size of destination buffer
	 * @return Number of bytes read, or error code
	 */
	uint16_t InvokeReadCallback(uint16_t offset, uint8_t* buffer, uint16_t buffer_size) const {
		if(read_callback_) {
			return read_callback_(offset, buffer, buffer_size);
		}
		// Default: copy from static value
		if(offset >= value_.size()) {
			return 0;
		}
		uint16_t bytes_to_copy = std::min(static_cast<uint16_t>(value_.size() - offset), buffer_size);
		if(buffer && bytes_to_copy > 0) {
			std::copy(value_.begin() + offset, value_.begin() + offset + bytes_to_copy, buffer);
		}
		return bytes_to_copy;
	}

	/**
	 * @brief Set the write callback for this attribute.
	 * Called when a remote client writes this attribute.
	 * @param callback Function to handle writes
	 */
	void SetWriteCallback(WriteCallback callback) {
		write_callback_ = std::move(callback);
	}

	/**
	 * @brief Check if a write callback is registered.
	 * @return true if write callback is set
	 */
	bool HasWriteCallback() const {
		return static_cast<bool>(write_callback_);
	}

	/**
	 * @brief Invoke the write callback.
	 * @param offset Offset into the attribute value
	 * @param data Source data to write
	 * @param size Number of bytes to write
	 * @return ATT error code (0 = success)
	 */
	uint16_t InvokeWriteCallback(uint16_t offset, const uint8_t* data, uint16_t size) {
		if(write_callback_) {
			return write_callback_(offset, data, size);
		}
		// Default: write to static value if not dynamic
		if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
			return 0x03;  // ATT_ERROR_WRITE_NOT_PERMITTED
		}
		return SetValue(data, size) ? 0 : 0x06;  // ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LENGTH
	}

   private:
	void UpdateUuidProperty() {
		if(uuid_.Is128Bit()) {
			properties_ |= static_cast<uint16_t>(Properties::kUuid128);
		} else {
			properties_ &= ~static_cast<uint16_t>(Properties::kUuid128);
		}
	}

	Uuid uuid_{};
	uint16_t handle_ = 0;  // ATT attribute handle
	uint16_t properties_ = 0;
	std::vector<uint8_t> value_{};
	ReadCallback read_callback_{};
	WriteCallback write_callback_{};
};

/**
 * @brief Parse a BTstack ATT database into Attribute objects.
 * @param db Pointer to ATT DB starting with the version byte.
 * @return Parsed attributes in database order.
 */
std::list<Attribute> ParseAttributesFromDb(const uint8_t* db);

constexpr uint16_t operator|(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator&(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator^(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) ^ static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator~(Attribute::Properties value) {
	return static_cast<uint16_t>(~static_cast<uint16_t>(value));
}
constexpr uint16_t operator|=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = lhs | rhs;
	return lhs;
}
constexpr uint16_t operator&=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = lhs & rhs;
	return lhs;
}
constexpr uint16_t operator^=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = lhs ^ rhs;
	return lhs;
}
constexpr uint16_t operator|(uint16_t lhs, Attribute::Properties rhs) {
	return lhs | static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator&(uint16_t lhs, Attribute::Properties rhs) {
	return lhs & static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator^(uint16_t lhs, Attribute::Properties rhs) {
	return lhs ^ static_cast<uint16_t>(rhs);
}
constexpr uint16_t operator|(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) | rhs;
}
constexpr uint16_t operator&(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) & rhs;
}
constexpr uint16_t operator^(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) ^ rhs;
}

}  // namespace c7222

#endif	// ELEC_C7222_BLE_GATT_ATTRIBUTE_H_
