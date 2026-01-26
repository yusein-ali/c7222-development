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
#include <ostream>

#include <bluetooth_gatt.h>  // For BTstack defined UUIDs
namespace c7222 {

/**
 * @brief UUID storage for 16-bit and 128-bit UUIDs.
 *
 * The first two bytes of `uuid_` are used for 16-bit UUIDs and the full
 * 16-byte array is used for 128-bit UUIDs. On RP2040, 16-bit UUIDs are
 * stored in little-endian order to match the BTstack ATT DB layout.
 * For 128-bit UUIDs, the byte order matches the standard UUID string
 * format shown by the stream operator.
 *
 * The class also defines common 16-bit GATT attribute type UUIDs and
 * helper predicates to recognize declaration/descriptor UUIDs.
 *
 * @note The UUID class provides type-safe and endian-aware access to
 *       16-bit and 128-bit UUID values, ensuring compatibility with
 *       Bluetooth Low Energy (BLE) specifications and BTstack's
 *       attribute database layout.
 *
 * @note You can find BTstack defined UUIDs in "bluetooth_gatt.h" file in ".pico-sdk/sdk/2.2.0/lib/btstack/src/bluetooth_gatt.h"
 *
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

	/**
	 * @brief Standard GATT Attribute Type UUIDs (16-bit).
	 *
	 * These values identify service/characteristic declarations and common
	 * descriptors as defined by Bluetooth Assigned Numbers.
	 */
	enum class AttributeType : uint16_t {
		/** @brief Primary Service Declaration attribute (0x2800). */
		kPrimaryServiceDeclaration = 0x2800,
		/** @brief Secondary Service Declaration attribute (0x2801). */
		kSecondaryServiceDeclaration = 0x2801,
		/** @brief Included Service Declaration attribute (0x2802). */
		kIncludedServiceDeclaration = 0x2802,
		/** @brief Characteristic Declaration attribute (0x2803). */
		kCharacteristicDeclaration = 0x2803,
		/** @brief Server Characteristic Configuration Descriptor (0x2903). */
		kServerCharacteristicConfiguration = 0x2903,
		/** @brief Client Characteristic Configuration Descriptor (0x2902). */
		kClientCharacteristicConfiguration = 0x2902,
		/** @brief Characteristic User Description Descriptor (0x2901). */
		kCharacteristicUserDescription = 0x2901,
		/** @brief Characteristic Extended Properties Descriptor (0x2900). */
		kCharacteristicExtendedProperties = 0x2900,
		/** @brief Characteristic Presentation Format Descriptor (0x2904). */
		kCharacteristicPresentationFormat = 0x2904,
		/** @brief Characteristic Aggregate Format Descriptor (0x2905). */
		kCharacteristicAggregateFormat = 0x2905
	};
	/** @brief Constructs an invalid/empty UUID. */
	Uuid() = default;

	/** @brief Copy-constructs a UUID. */
	Uuid(const Uuid&) = default;

	/** @brief Move-constructs a UUID. */
	Uuid(Uuid&&) noexcept = default;

	/** @brief Copy assignment operator. */
	Uuid& operator=(const Uuid&) = default;

	/** @brief Move assignment operator. */
	Uuid& operator=(Uuid&&) noexcept = default;

	/**
	 * @brief Constructs a 16-bit UUID from a numeric value.
	 * @param uuid16 16-bit UUID value (stored in little-endian order).
	 */
	explicit Uuid(uint16_t uuid16) : type_(Type::k16Bit) {
		auto* ptr = reinterpret_cast<uint8_t*>(&uuid16);
		std::fill(uuid_.begin(), uuid_.end(), 0);
		std::copy(ptr, ptr + sizeof(uuid16), uuid_.begin());
	}

	/**
	 * @brief Constructs a UUID from a raw byte buffer.
	 * @param uuid Pointer to 2 or 16 bytes of UUID data.
	 * @param size Size of the buffer in bytes (must be 2 or 16).
	 *
	 * For 16-bit UUIDs, the buffer is interpreted as little-endian.
	 * For 128-bit UUIDs, the buffer is interpreted in standard byte order.
	 */
	Uuid(const uint8_t* uuid, size_t size);

	/**
	 * @brief Constructs a 128-bit UUID from an array.
	 * @param uuid 128-bit UUID bytes in standard order.
	 */
	explicit Uuid(const std::array<uint8_t, 16>& uuid) : type_(Type::k128Bit), uuid_(uuid) {}

	/**
	 * @brief Constructs a 16-bit UUID from an array.
	 * @param uuid 16-bit UUID bytes in little-endian order.
	 */
	explicit Uuid(const std::array<uint8_t, 2>& uuid) : type_(Type::k16Bit) {
		std::copy(uuid.begin(), uuid.end(), uuid_.begin());
	}

	/**
	 * @brief Returns the UUID type.
	 */
	[[nodiscard]] Type type() const {
		return type_;
	}

	/**
	 * @brief Returns true if this UUID is 16-bit.
	 */
	[[nodiscard]] bool Is16Bit() const {
		return type_ == Type::k16Bit;
	}

	/**
	 * @brief Returns true if this UUID is 128-bit.
	 */
	[[nodiscard]] bool Is128Bit() const;

	/**
	 * @brief Returns true if the UUID has been initialized to 16- or 128-bit.
	 */
	[[nodiscard]] bool IsValid() const {
		return type_ != Type::Invalid;
	}

	/**
	 * @brief Returns the 16-bit UUID value.
	 * @return 16-bit UUID value.
	 */
	[[nodiscard]] uint16_t Get16Bit() const {
		assert(type_ == Type::k16Bit && "UUID is not 16-bit");
		uint16_t ret;
		auto* ptr = reinterpret_cast<uint8_t*>(&ret);
		std::copy(uuid_.begin(), uuid_.begin() + 2, ptr);
		return ret;
	}

	/**
	 * @brief Returns the 128-bit UUID bytes.
	 * @return 128-bit UUID byte array.
	 */
	[[nodiscard]] const std::array<uint8_t, 16>& Get128Bit() const {
		assert(type_ == Type::k128Bit && "UUID is not 128-bit");
		return uuid_;
	}

	/**
	 * @brief Returns the raw UUID storage pointer.
	 * @return Pointer to the internal UUID byte array.
	 *
	 * The byte order matches the storage rules documented above.
	 */
	[[nodiscard]] const uint8_t* data() const {
		return uuid_.data();
	}

	/**
	 * @brief Converts a 16-bit UUID to a 128-bit Bluetooth base UUID.
	 * @param uuid16 16-bit UUID to expand.
	 * @return 128-bit UUID using the Bluetooth base UUID format.
	 */
	static Uuid Convert16To128(const Uuid& uuid16);

	// ========== GATT Attribute Type Helpers ==========
	// These helpers return false for non-16-bit/custom UUIDs.

	/**
	 * @brief Create a Primary Service Declaration UUID (0x2800).
	 */
	static Uuid PrimaryServiceDeclaration();

	/**
	 * @brief Create a Secondary Service Declaration UUID (0x2801).
	 */
	static Uuid SecondaryServiceDeclaration();

	/**
	 * @brief Create an Included Service Declaration UUID (0x2802).
	 */
	static Uuid IncludedServiceDeclaration();

	/**
	 * @brief Create a Characteristic Declaration UUID (0x2803).
	 */
	static Uuid CharacteristicDeclaration();

	/**
	 * @brief Create a Client Characteristic Configuration UUID (0x2902).
	 */
	static Uuid ClientCharacteristicConfiguration();

	/**
	 * @brief Create a Server Characteristic Configuration UUID (0x2903).
	 */
	static Uuid ServerCharacteristicConfiguration();

	/**
	 * @brief Create a Characteristic User Description UUID (0x2901).
	 */
	static Uuid CharacteristicUserDescription();

	/**
	 * @brief Create a Characteristic Extended Properties UUID (0x2900).
	 */
	static Uuid CharacteristicExtendedProperties();

	/**
	 * @brief Create a Characteristic Presentation Format UUID (0x2904).
	 */
	static Uuid CharacteristicPresentationFormat();

	/**
	 * @brief Create a Characteristic Aggregate Format UUID (0x2905).
	 */
	static Uuid CharacteristicAggregateFormat();

	/**
	 * @brief Check if UUID is Primary Service Declaration (0x2800).
	 */
	static bool IsPrimaryServiceDeclaration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Secondary Service Declaration (0x2801).
	 */
	static bool IsSecondaryServiceDeclaration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Included Service Declaration (0x2802).
	 */
	static bool IsIncludedServiceDeclaration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Characteristic Declaration (0x2803).
	 */
	static bool IsCharacteristicDeclaration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is any Service Declaration (Primary or Secondary).
	 */
	static bool IsServiceDeclaration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Client Characteristic Configuration (0x2902).
	 */
	static bool IsClientCharacteristicConfiguration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Server Characteristic Configuration (0x2903).
	 */
	static bool IsServerCharacteristicConfiguration(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Characteristic User Description (0x2901).
	 */
	static bool IsCharacteristicUserDescription(const Uuid& uuid);

	/**
	 * @brief Check if UUID is Characteristic Extended Properties (0x2900).
	 */
	static bool IsCharacteristicExtendedProperties(const Uuid& uuid);

	/**
	 * @brief Check if UUID matches a known descriptor type.
	 */
	static bool IsDescriptor(const Uuid& uuid);

	/**
	 * @brief Equality comparison based on UUID type and value.
	 * @param other UUID to compare against.
	 * @return true when type and value match.
	 */
	bool operator==(const Uuid& other) const;

	/**
	 * @brief Inequality comparison based on UUID type and value.
	 * @param other UUID to compare against.
	 * @return true when type or value differ.
	 */
	bool operator!=(const Uuid& other) const {
		return !(*this == other);
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
	Type type_ = Type::Invalid;
	/** @brief UUID storage (first two bytes used for 16-bit). */
	std::array<uint8_t, 16> uuid_{};
};

/**
 * @brief Prints the UUID to a stream.
 *
 * This is the definition corresponding to the friend declaration above.
 */
std::ostream& operator<<(std::ostream& os, const Uuid& uuid);

}  // namespace c7222

#endif	// ELEC_C7222_BLE_GATT_UUID_H_
