/**
 * @file attribute.hpp
 * @brief GATT attribute wrapper for BTstack ATT fields.
 */
#ifndef ELEC_C7222_BLE_GATT_ATTRIBUTE_H_
#define ELEC_C7222_BLE_GATT_ATTRIBUTE_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <vector>
#include "uuid.hpp"
#include "non_copyable.hpp"
#include "ble_error.hpp"

namespace c7222 {

/**
 * @brief ATT attribute wrapper with BTstack-compatible fields.
 *
 * This class mirrors a single ATT database entry as used by the RP2040
 * Pico W BLE stack (BTstack). The fields align with BTstack's binary
 * ATT DB layout parsed in `ParseAttributesFromDb`, so the `properties_`
 * bitmask and `handle_` values can be populated directly from the stack.
 *
 * Value storage model:
 * - Static attributes (no `kDynamic` flag): keep a pointer/size pair into
 *   the ATT DB blob without copying. This matches BTstack, which treats
 *   these values as immutable, fixed-at-compile-time data. Avoiding a copy
 *   saves RAM and preserves the DB as the single source of truth.
 *   - Storage: `static_value_ptr_` + `static_value_size_` point into the
 *     ATT DB image parsed by `ParseAttributesFromDb()`.
 *   - Mutability: immutable at runtime; `SetValue()` rejects writes.
 *   - Access: `GetValueData()` returns the DB pointer; `GetValueSize()`
 *     returns the DB length.
 *   - Optional override: `SetStaticValue()` copies into
 *     `static_value_storage_` and repoints `static_value_ptr_`, allowing
 *     a controlled, owned static value without toggling `kDynamic`.
 * - Dynamic attributes (`kDynamic` set): keep a mutable `std::vector` that
 *   can be updated at runtime. Dynamic attributes rely on callbacks and
 *   writable storage; copying into a vector provides ownership and resize
 *   capability that a DB-backed pointer cannot offer.
 *   - Storage: `dynamic_value_` owns the bytes and can grow/shrink.
 *   - Mutability: `SetValue()` and write callbacks may update contents.
 *   - Access: `GetValueData()` returns the vector data pointer; size is
 *     `dynamic_value_.size()`.
 *   - Writes: `InvokeWriteCallback()` rejects writes if no write callback
 *     is installed (dynamic attributes are expected to be callback-backed).
 *
 * Consequence: callers cannot change static attribute payloads via
 * `SetValue()`; writes are rejected unless a dynamic attribute and a write
 * callback are provided.
 *
 * Construction:
 * - Provide a `Uuid` plus a properties bitmask; an optional handle can be
 *   supplied (otherwise it defaults to 0 until assigned).
 * - Provide a `Uuid`, properties, and initial value buffer for static data.
 *
 * Setting flags:
 * - Flags are stored as a `uint16_t` bitmask. Use the `Properties` enum
 *   constants and the provided bitwise operators to combine them.
 * - The `kUuid128` flag is synchronized automatically based on the `Uuid`
 *   when constructing or calling `SetProperties()`.
 *
 * Callback architecture:
 * - Read callbacks return `uint16_t`: byte count on success, or ATT error
 *   code (cast from BleError) on failure. This allows reads to dynamically
 *   determine how much data to return.
 * - Write callbacks return `BleError`: type-safe error codes ensure correct
 *   error handling without manual conversions. Returns BleError::kSuccess
 *   on successful write operations.
 * - Platform-specific implementations in platform/rpi_pico/ handle BTstack
 *   integration; core logic remains platform-independent.
 *
 * Example:
 * @code
 * // Create readable/writable attribute
 * uint16_t props = Attribute::Properties::kRead | Attribute::Properties::kWrite;
 * Attribute attr(Uuid(0x2A37), props);
 *
 * // Set write callback with BleError return type
 * attr.SetWriteCallback([](uint16_t offset, const uint8_t* data, uint16_t size) {
 *     // Process write...
 *     return BleError::kSuccess;  // Type-safe error handling
 * });
 *
 * // Set read callback with uint16_t return (byte count or error)
 * attr.SetReadCallback([](uint16_t offset, uint8_t* buffer, uint16_t buffer_size) {
 *     // Fill buffer...
 *     return bytes_read;  // Return number of bytes read
 * });
 * @endcode
 *
 * Dynamic attributes:
 * - If `kDynamic` is set, the attribute is expected to use callbacks rather
 *   than a fixed in-DB value. `InvokeWriteCallback()` will reject writes
 *   if no write callback is installed, returning BleError::kAttErrorWriteNotPermitted.
 *
 * Thread safety:
 * - Not thread-safe. External synchronization required for concurrent access.
 */
class Attribute : public MovableOnly {
   public:
	/**
	 * @brief Attribute flags with direct BTstack ATT_PROPERTY_* mapping.
	 *
	 * These bits are stored in the ATT DB entry flags field. The layout
	 * matches the BTstack `att_db.c` format used by the Pico W stack.
	 */
	enum class Properties : std::uint16_t {
		/** @brief No properties set. */
		kNone = 0x0000,
		/**
		 * @brief Write permission bit 0 (LSB of a 2-bit write security level).
		 * Combined with `kWritePermissionBit1` by the stack.
		 */
		kWritePermissionBit0 = 0x0001,
		/** @brief Attribute can be read via ATT Read Request. */
		kRead = 0x0002,
		/** @brief Attribute can be written via Write Command (no response). */
		kWriteWithoutResponse = 0x0004,
		/** @brief Attribute can be written via Write Request (with response). */
		kWrite = 0x0008,
		/**
		 * @brief Write permission bit 1 (MSB of a 2-bit write security level).
		 * Combined with `kWritePermissionBit0` by the stack.
		 */
		kWritePermissionBit1 = 0x0010,
		/** @brief Read requires Secure Connections (SC) per BTstack flags. */
		kReadPermissionSc = 0x0020,
		/** @brief Attribute supports Authenticated Signed Write. */
		kAuthenticatedSignedWrite = 0x0040,
		/** @brief Write requires Secure Connections (SC) per BTstack flags. */
		kWritePermissionSc = 0x0080,
		/**
		 * @brief Value is dynamic (handled by callbacks, not fixed DB storage).
		 *
		 * The local `value_` may be used as a cache, but when no write callback
		 * is installed, `InvokeWriteCallback()` rejects writes for dynamic attrs.
		 */
		kDynamic = 0x0100,
		/**
		 * @brief Entry uses a 128-bit UUID in the ATT DB.
		 *
		 * `Attribute` keeps this bit in sync with the `Uuid` type.
		 */
		kUuid128 = 0x0200,
		/**
		 * @brief Read permission bit 0 (LSB of a 2-bit read security level).
		 * Combined with `kReadPermissionBit1` by the stack.
		 */
		kReadPermissionBit0 = 0x0400,
		/**
		 * @brief Read permission bit 1 (MSB of a 2-bit read security level).
		 * Combined with `kReadPermissionBit0` by the stack.
		 */
		kReadPermissionBit1 = 0x0800,
		/**
		 * @brief Mask for encryption key size requirement (bits 12-15).
		 *
		 * The stack encodes the required key size in the upper nibble.
		 */
		kEncryptionKeySizeMask = 0xF000
	};

	/**
	 * @brief Standard GATT Attribute Type UUIDs.
	 * 
	 * These are well-known 16-bit UUIDs defined by the Bluetooth specification
	 * to identify common attribute types used in GATT services,
	 * characteristic-related attributes and descriptors.
	 * 
	 * @note As per Bluetooth Assigned Numbers
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

	/**
	 * @brief Callback type for reading attribute value.
	 * Parameters: offset, destination buffer, buffer size
	 * Returns: number of bytes read, or ATT error code if > 0xfe00
	 */
	using ReadCallback = std::function<uint16_t(uint16_t offset, uint8_t* buffer, uint16_t buffer_size)>;

	/**
	 * @brief Callback type for writing attribute value.
	 * Parameters: offset, source data, data size
	 * Returns: BleError code (BleError::kSuccess = success)
	 */
	using WriteCallback = std::function<BleError(uint16_t offset, const uint8_t* data, uint16_t size)>;

	// ========== Static Helper Functions for Attribute Type Checking ==========

	/**
	 * @brief Check if an attribute is a Primary Service Declaration.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches the Primary Service Declaration type
	 */
	static bool IsPrimaryServiceDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a Secondary Service Declaration.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches the Secondary Service Declaration type
	 */
	static bool IsSecondaryServiceDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is any Service Declaration (Primary or Secondary).
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches either service declaration type
	 */
	static bool IsServiceDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is an Included Service Declaration.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches the Included Service Declaration type
	 */
	static bool IsIncludedServiceDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a Characteristic Declaration.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches the Characteristic Declaration type
	 */
	static bool IsCharacteristicDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a Service Declaration (primary or secondary).
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches a primary or secondary service type
	 */
	static bool IsServiceDeclaration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a Client Characteristic Configuration Descriptor.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches CCCD type
	 */
	static bool IsClientCharacteristicConfiguration(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a Characteristic User Description.
	 * @param attr Attribute to check
	 * @return true if the attribute UUID matches User Description type
	 */
	static bool IsCharacteristicUserDescription(const Attribute& attr);

	/**
	 * @brief Check if an attribute is a descriptor (not declaration or value).
	 * @param attr Attribute to check
	 * @return true if the attribute is a known descriptor type
	 */
	static bool IsDescriptor(const Attribute& attr);

	Attribute() = default;
	
	/** @brief Move constructor. */
	Attribute(Attribute&&) noexcept = default;

	/** @brief Move assignment operator. */
	Attribute& operator=(Attribute&&) noexcept = default;

	/**
	 * @brief Construct an attribute with a UUID and properties.
	 */
	Attribute(const Uuid& uuid, uint16_t properties, uint16_t handle = 0);
	/**
	 * @brief Construct an attribute with a UUID, properties, and value data.
	 * @details Static attributes store a pointer/size into the ATT DB buffer;
	 * dynamic attributes copy the data into owned storage so it can be mutated.
	 */
	Attribute(const Uuid& uuid, uint16_t properties, const uint8_t* data, size_t size, uint16_t handle = 0);

	uint16_t GetHandle() const {
		return handle_;
	}

	void SetHandle(uint16_t handle) {
		handle_ = handle;
	}

	const Uuid& GetUuid() const {
		return uuid_;
	}

	bool IsUuid128() const {
		return uuid_.Is128Bit();
	}

	/**
	 * @brief Check if this attribute has a valid handle and UUID.
	 */
	bool IsValid() const {
		return handle_ != 0 && uuid_.IsValid();
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

	/**
	 * @brief Get the properties bitmask of the attribute.
	 * @return Properties bitmask
	 */
	uint16_t GetProperties() const {
		return properties_;
	}

	/**
	 * @brief Set the properties bitmask of the attribute.
	 * @param properties New properties bitmask
	 */
	void SetProperties(uint16_t properties) {
		properties_ = properties;
		UpdateUuidProperty();
	}

	/**
	 * @brief Get value as vector (for compatibility).
	 * Note: Returns empty for static attributes because they are DB-backed.
	 */
	const std::vector<uint8_t>& GetDynamicValue() const {
		return dynamic_value_;
	}

	/**
	 * @brief Get pointer to attribute value data.
	 * @return Pointer to value data, or nullptr if no value
	 * @note Static attributes return the ATT DB pointer; dynamic attributes
	 * return the owned vector data pointer.
	 */
	const uint8_t* GetValueData() const {
		if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
			return dynamic_value_.empty() ? nullptr : dynamic_value_.data();
		} else {
			return static_value_ptr_;
		}
	}

	/**
	 * @brief Get size of attribute value.
	 * @return Size of value in bytes
	 * @note Mirrors the storage choice: DB size for static, vector size for dynamic.
	 */
	size_t GetValueSize() const {
		if((properties_ & static_cast<uint16_t>(Properties::kDynamic)) != 0) {
			return dynamic_value_.size();
		} else {
			return static_value_size_;
		}
	}

	/**
	 * @brief Set attribute value.
	 * Only allowed for dynamic attributes; static attributes are immutable
	 * because their bytes reside in the ATT DB image.
	 * @param data Pointer to value data
	 * @param size Size of value in bytes
	 * @return true if value was set, false if rejected (static attribute)
	 */
	bool SetValue(const uint8_t* data, size_t size);

	/**
	 * @brief Set attribute value for static attributes.
	 * Copies the data into owned storage and updates the static value pointer.
	 * @param data Pointer to value data
	 * @param size Size of value in bytes
	 * @return true if value was set, false if rejected (dynamic attribute)
	 */
	bool SetStaticValue(const uint8_t* data, size_t size);

	/**
	 * @brief Set attribute value from rvalue vector (move semantics).
	 * Only allowed for dynamic attributes.
	 * @param data Rvalue reference to vector (ownership transferred via move)
	 * @return true if value was set, false if rejected (static attribute)
	 */
	bool SetValue(std::vector<uint8_t>&& data);

	/**
	 * @brief Set attribute value from lvalue vector (copy semantics).
	 * Only allowed for dynamic attributes.
	 * @param data Const reference to vector (copied into internal storage)
	 * @return true if value was set, false if rejected (static attribute)
	 */
	bool SetValue(const std::vector<uint8_t>& data);

	/**
	 * @brief Set attribute value for static attributes from vector (copy semantics).
	 * @param data Const reference to vector (copied into static storage)
	 * @return true if value was set, false if rejected (dynamic attribute)
	 */
	bool SetStaticValue(const std::vector<uint8_t>& data);

	/**
	 * @brief Set attribute value from a typed value (generic template).
	 * Converts any trivial type to bytes and stores as the attribute value.
	 * Only allowed for dynamic attributes.
	 * @tparam T Must be a trivial type (primitive types, arrays, small structs)
	 * @param value The value to store
	 * @return true if value was set, false if rejected (static attribute)
	 * @note Uses binary representation; for endian-sensitive types, ensure consistency
	 * @example SetValue<uint16_t>(0x1234) stores as bytes {0x34, 0x12} (little-endian)
	 */
	template<typename T>
	bool SetValue(const T& value) {
		static_assert(std::is_trivial<T>::value,
			"T must be a trivial type for binary conversion");
		return SetValue(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
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
	 * @note Platform-specific implementation required in platform dependent attribute.cpp
	 */
	uint16_t InvokeReadCallback(uint16_t offset, uint8_t* buffer, uint16_t buffer_size) const;

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
	 * @return BleError code (BleError::kSuccess = success)
	 * @note Platform-independent implementation in src/attribute.cpp
	 */
	BleError InvokeWriteCallback(uint16_t offset, const uint8_t* data, uint16_t size);

	/**
	 * @brief Stream insertion operator for Attribute.
	 * Outputs the attribute handle, UUID, and parsed properties flags.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Attribute& attr);

   private:
	/**
	 * @brief Update the kUuid128 property flag based on the UUID type.
	 *
	 * Synchronizes the Properties::kUuid128 bit with the actual UUID size.
	 * Called automatically during construction and when properties are updated.
	 * This ensures the properties bitmask always reflects whether a 128-bit
	 * UUID is in use, matching BTstack's ATT database expectations.
	 *
	 * @note Private helper method - automatically maintains consistency between
	 *       uuid_ and the kUuid128 flag in properties_.
	 */
	void UpdateUuidProperty() {
		if(uuid_.Is128Bit()) {
			properties_ |= static_cast<uint16_t>(Properties::kUuid128);
		} else {
			properties_ &= ~static_cast<uint16_t>(Properties::kUuid128);
		}
	}

	// ========== Core Attribute Identity ==========

	/**
	 * @brief Attribute UUID (16-bit or 128-bit).
	 *
	 * Identifies the attribute type per Bluetooth specifications.
	 * Can be a standard 16-bit UUID (e.g., 0x2A37 for Heart Rate)
	 * or a custom 128-bit UUID. The kUuid128 property flag is
	 * automatically synchronized with this field via UpdateUuidProperty().
	 */
	Uuid uuid_{};

	/**
	 * @brief ATT attribute handle (0 when unassigned).
	 *
	 * Unique identifier within the ATT database. Assigned by the GATT
	 * server during service registration. Handle 0 indicates an unassigned
	 * or temporary attribute. Handles are typically assigned sequentially
	 * starting from 1.
	 */
	uint16_t handle_ = 0;

	/**
	 * @brief Raw BTstack properties bitmask (ATT DB flags).
	 *
	 * Combines Properties enum values defining attribute capabilities:
	 * - Access permissions (kRead, kWrite, kWriteWithoutResponse)
	 * - Security requirements (kReadPermissionBit0/1, kWritePermissionBit0/1)
	 * - Secure Connections (kReadPermissionSc, kWritePermissionSc)
	 * - Storage type (kDynamic for callback-based, static for DB-backed)
	 * - UUID size (kUuid128, auto-managed by UpdateUuidProperty())
	 *
	 * Directly matches BTstack's ATT database flag encoding.
	 */
	uint16_t properties_ = 0;

	// ========== Static Attribute Value Storage ==========

	/**
	 * @brief Pointer to static value in ATT DB (nullptr for dynamic attributes).
	 *
	 * For static attributes (Properties::kDynamic NOT set):
	 * - Points directly to read-only data in the compiled ATT database
	 * - Avoids memory duplication - DB is the single source of truth
	 * - Value cannot be modified at runtime (immutable by design)
	 *
	 * For dynamic attributes:
	 * - Always nullptr
	 * - Value stored in dynamic_value_ vector instead
	 *
	 * @note This pointer is NOT owned by Attribute unless SetStaticValue()
	 *       is used, in which case it points into static_value_storage_.
	 */
	const uint8_t* static_value_ptr_ = nullptr;

	/**
	 * @brief Size of static value in bytes (0 for dynamic attributes).
	 *
	 * For static attributes: size of data referenced by static_value_ptr_.
	 * For dynamic attributes: always 0 (use dynamic_value_.size() instead).
	 *
	 * Cached from ATT DB parsing to avoid recomputing size on each access.
	 */
	size_t static_value_size_ = 0;

	/**
	 * @brief Owned storage for static values set at runtime.
	 *
	 * Used by SetStaticValue(); DB-backed static attributes keep a pointer
	 * into the ATT DB image and leave this vector empty.
	 */
	std::vector<uint8_t> static_value_storage_{};

	// ========== Dynamic Attribute Value Storage ==========

	/**
	 * @brief Dynamic attribute value storage (empty for static attributes).
	 *
	 * For dynamic attributes (Properties::kDynamic set):
	 * - Owned, mutable storage for attribute value
	 * - Can be updated via SetValue() methods
	 * - Automatically resizes to accommodate new data
	 * - Used when attribute value changes at runtime
	 *
	 * For static attributes:
	 * - Remains empty (size 0)
	 * - SetValue() operations are rejected
	 *
	 * @note Memory overhead only applies to dynamic attributes. Static
	 *       attributes keep this vector empty to save RAM.
	 */
	std::vector<uint8_t> dynamic_value_{};

	// ========== Callback Functions ==========

	/**
	 * @brief Optional read callback for dynamic value retrieval.
	 *
	 * Function signature: uint16_t(uint16_t offset, uint8_t* buffer, uint16_t buffer_size)
	 *
	 * Called when a remote BLE client reads this attribute via ATT Read Request.
	 * The callback must:
	 * - Fill the buffer with attribute data starting at the specified offset
	 * - Return the number of bytes written to buffer on success
	 * - Return an ATT error code (>= 0xFE00, typically cast from BleError) on failure
	 *
	 * If no callback is set, InvokeReadCallback() returns either the stored
	 * static/dynamic value or an error code for unauthorized access.
	 *
	 * @note Typically used for dynamic attributes where value is computed
	 *       on-demand rather than stored.
	 */
	ReadCallback read_callback_{};

	/**
	 * @brief Optional write callback for dynamic value updates.
	 *
	 * Function signature: BleError(uint16_t offset, const uint8_t* data, uint16_t size)
	 *
	 * Called when a remote BLE client writes to this attribute via ATT Write Request.
	 * The callback must:
	 * - Process the write data starting at the specified offset
	 * - Return BleError::kSuccess if write is accepted
	 * - Return appropriate BleError code on failure (e.g., kAttErrorWriteNotPermitted)
	 *
	 * If no callback is set for a dynamic attribute, InvokeWriteCallback()
	 * returns BleError::kAttErrorWriteNotPermitted. For static attributes,
	 * writes are always rejected as the value is immutable DB data.
	 *
	 * Type safety: BleError return type ensures compile-time correctness
	 * and eliminates manual uint16_t conversions for error codes.
	 *
	 * @note Required for writable dynamic attributes. Without this callback,
	 *       all write attempts will fail with permission error.
	 */
	WriteCallback write_callback_{};
};

/**
 * @brief Parse a BTstack ATT database into Attribute objects.
 * @param db Pointer to ATT DB starting with the version byte.
 * @return Parsed attributes in database order.
 */
std::list<Attribute> ParseAttributesFromDb(const uint8_t* db);

/**
 * @brief Stream insertion operator for Attribute.
 * Outputs the attribute handle, UUID, and parsed properties flags.
 */
std::ostream& operator<<(std::ostream& os, const Attribute& attr);

// Bitwise operators for Attribute::Properties enum
// These operators enable convenient property flag manipulation:
//   uint16_t props = Attribute::Properties::kRead | Attribute::Properties::kWrite;

/**
 * @brief Bitwise OR for two Properties values.
 * @return Result as uint16_t
 */
constexpr uint16_t operator|(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise AND for two Properties values.
 * @return Result as uint16_t
 */
constexpr uint16_t operator&(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise XOR for two Properties values.
 * @return Result as uint16_t
 */
constexpr uint16_t operator^(Attribute::Properties lhs, Attribute::Properties rhs) {
	return static_cast<uint16_t>(lhs) ^ static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise NOT for a Properties value.
 * @return Complement as uint16_t
 */
constexpr uint16_t operator~(Attribute::Properties value) {
	return static_cast<uint16_t>(~static_cast<uint16_t>(value));
}

/**
 * @brief Bitwise OR assignment: uint16_t |= Properties.
 * @return Reference to lhs after operation
 */
constexpr uint16_t operator|=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = static_cast<uint16_t>(lhs | static_cast<uint16_t>(rhs));
	return lhs;
}

/**
 * @brief Bitwise AND assignment: uint16_t &= Properties.
 * @return Reference to lhs after operation
 */
constexpr uint16_t operator&=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = static_cast<uint16_t>(lhs & static_cast<uint16_t>(rhs));
	return lhs;
}

/**
 * @brief Bitwise XOR assignment: uint16_t ^= Properties.
 * @return Reference to lhs after operation
 */
constexpr uint16_t operator^=(uint16_t& lhs, Attribute::Properties rhs) {
	lhs = static_cast<uint16_t>(lhs ^ static_cast<uint16_t>(rhs));
	return lhs;
}

/**
 * @brief Bitwise OR: uint16_t | Properties.
 * Enables mixing uint16_t with Properties in either operand order.
 */
constexpr uint16_t operator|(uint16_t lhs, Attribute::Properties rhs) {
	return lhs | static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise AND: uint16_t & Properties.
 */
constexpr uint16_t operator&(uint16_t lhs, Attribute::Properties rhs) {
	return lhs & static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise XOR: uint16_t ^ Properties.
 */
constexpr uint16_t operator^(uint16_t lhs, Attribute::Properties rhs) {
	return lhs ^ static_cast<uint16_t>(rhs);
}

/**
 * @brief Bitwise OR: Properties | uint16_t.
 */
constexpr uint16_t operator|(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) | rhs;
}

/**
 * @brief Bitwise AND: Properties & uint16_t.
 */
constexpr uint16_t operator&(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) & rhs;
}

/**
 * @brief Bitwise XOR: Properties ^ uint16_t.
 */
constexpr uint16_t operator^(Attribute::Properties lhs, uint16_t rhs) {
	return static_cast<uint16_t>(lhs) ^ rhs;
}

}  // namespace c7222

#endif	// ELEC_C7222_BLE_GATT_ATTRIBUTE_H_
