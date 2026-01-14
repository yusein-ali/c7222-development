/**
 * @file uuid.hpp
 * @brief GATT UUID wrapper.
 */
#ifndef ELEC_C7222_BLE_GATT_UUID_H_
#define ELEC_C7222_BLE_GATT_UUID_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <ostream>

namespace c7222 {

/**
 * @brief UUID storage for 16-bit and 128-bit UUIDs.
 *
 * Example (16-bit construction and access):
 * @code
 * c7222::Uuid uuid16(0x180D);
 * uint16_t short_id = uuid16.Get16Bit();
 * @endcode
 *
 * Example (128-bit construction and access):
 * @code
 * std::array<uint8_t, 16> bytes = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC,
 *                                  0xDE, 0xF0, 0x11, 0x22, 0x33, 0x44,
 *                                  0x55, 0x66, 0x77, 0x88};
 * c7222::Uuid uuid128(bytes);
 * const auto& raw = uuid128.Get128Bit();
 * @endcode
 */
class Uuid {
   public:
	/**
	 * @brief UUID representation type.
	 */
	enum class Type : uint8_t {
		/** @brief Uninitialized or invalid UUID. */
		Invalid = 0,
		/** @brief 16-bit UUID stored in the first two bytes. */
		k16Bit = 1,
		/** @brief 128-bit UUID stored in the full array. */
		k128Bit = 2
	};
	/** @brief Constructs a 16-bit UUID with value 0x0000. */
	Uuid() = default;

	/** @brief Copy-constructs a UUID. */
	Uuid(const Uuid&) = default;

	/**
	 * @brief Constructs a 16-bit UUID from a numeric value.
	 * @param uuid16 16-bit UUID value.
	 */
	explicit Uuid(uint16_t uuid16) : type_(Type::k16Bit) {
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&uuid16);
		std::fill(uuid_.begin(), uuid_.end(), 0);
		std::copy(ptr, ptr + sizeof(uuid16), uuid_.begin());
	}

	/**
	 * @brief Constructs a UUID from a raw byte buffer.
	 * @param uuid Pointer to 2 or 16 bytes of UUID data.
	 * @param size Size of the buffer in bytes (must be 2 or 16).
	 */
	Uuid(const uint8_t* uuid, size_t size) {
		switch(size) {
			case 2:
				type_ = Type::k16Bit;
				std::copy(uuid, uuid + 2, uuid_.begin());
			case 16:
				type_ = Type::k128Bit;
				std::copy(uuid, uuid + 16, uuid_.begin());
				break;
			default:
				assert(false && "UUID can be either 16 bits or 128 bits");
		}
	}

	/**
	 * @brief Constructs a 128-bit UUID from an array.
	 * @param uuid 128-bit UUID bytes.
	 */
	explicit Uuid(const std::array<uint8_t, 16>& uuid) : type_(Type::k128Bit), uuid_(uuid) {}

	/**
	 * @brief Constructs a 16-bit UUID from an array.
	 * @param uuid 16-bit UUID bytes.
	 */
	explicit Uuid(const std::array<uint8_t, 2>& uuid) : type_(Type::k16Bit) {
		std::copy(uuid.begin(), uuid.end(), uuid_.begin());
	}

	/**
	 * @brief Returns the UUID type.
	 */
	Type type() const {
		return type_;
	}

	/**
	 * @brief Returns true if this UUID is 16-bit.
	 */
	bool Is16Bit() const {
		return type_ == Type::k16Bit;
	}

	/**
	 * @brief Returns true if this UUID is 128-bit.
	 */
	bool Is128Bit() const {
		return type_ == Type::k128Bit;
	}

	/**
	 * @brief Returns the 16-bit UUID value.
	 * @return 16-bit UUID value.
	 */
	uint16_t Get16Bit() const {
		assert(type_ == Type::k16Bit && "UUID is not 16-bit");
		uint16_t ret;
		uint8_t* ptr = reinterpret_cast<uint8_t*>(&ret);
		std::copy(uuid_.begin(), uuid_.begin() + 2, ptr);
		return ret;
	}

	/**
	 * @brief Returns the 128-bit UUID bytes.
	 * @return 128-bit UUID byte array.
	 */
	const std::array<uint8_t, 16>& Get128Bit() const {
		assert(type_ == Type::k128Bit && "UUID is not 128-bit");
		return uuid_;
	}

	/**
	 * @brief Returns the raw UUID storage pointer.
	 * @return Pointer to the internal UUID byte array.
	 */
	const uint8_t* data() const {
		return uuid_.data();
	}

	/**
	 * @brief Converts a 16-bit UUID to a 128-bit Bluetooth base UUID.
	 * @param uuid16 16-bit UUID to expand.
	 * @return 128-bit UUID using the Bluetooth base UUID format.
	 */
	static Uuid Convert16To128(const Uuid& uuid16) {
		assert(uuid16.type_ == Type::k16Bit && "Input UUID is not 16-bit");
		std::array<uint8_t, 16> uuid128 = {0x00,
										   0x00,
										   0x00,
										   0x00,
										   0x00,
										   0x00,
										   0x10,
										   0x00,
										   0x80,
										   0x00,
										   0x00,
										   0x80,
										   0x5F,
										   0x9B,
										   0x34,
										   0xFB};
		const uint8_t* short_uuid = uuid16.data();
		uuid128[2] = (short_uuid[1]) & 0xFF;
		uuid128[3] = (short_uuid[0]) & 0xFF;
		return Uuid(uuid128);
	}

	/**
	 * @brief Prints the UUID to a stream.
	 *
	 * For 16-bit UUIDs the output is 0xNNNN. For 128-bit UUIDs the output is
	 * the standard 8-4-4-4-12 hex format.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid);

   private:
	/** @brief UUID representation type. */
	Type type_ = Type::k16Bit;
	/** @brief UUID storage (first two bytes used for 16-bit). */
	std::array<uint8_t, 16> uuid_{};
};

std::ostream& operator<<(std::ostream& os, const Uuid& uuid);

}  // namespace c7222

#endif	// ELEC_C7222_BLE_GATT_UUID_H_
