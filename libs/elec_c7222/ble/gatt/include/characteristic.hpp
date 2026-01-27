#ifndef ELEC_C7222_BLE_GATT_CHARACTERISTIC_HPP_
#define ELEC_C7222_BLE_GATT_CHARACTERISTIC_HPP_

#include <functional>
#include <iosfwd>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "attribute.hpp"
#include "ble_error.hpp"
#include "uuid.hpp"

namespace c7222 {

/**
 * @class Characteristic
 * @brief Represents a GATT Characteristic with its declaration, value, and descriptors.
 *
 * A GATT Characteristic consists of:
 * 1. Characteristic Declaration attribute (mandatory)
 * 2. Characteristic Value attribute (mandatory)
 * 3. Client Characteristic Configuration Descriptor - CCCD (optional, for
 * notifications/indications)
 * 4. Characteristic User Description (optional)
 * 5. Additional descriptors (optional)
 *
 * The Characteristic Declaration contains:
 * - 1 byte: Properties (Read, Write, Notify, Indicate, etc.)
 * - 2 bytes: Value handle (little-endian, points to the Value attribute)
 * - 2 or 16 bytes: Characteristic UUID (16-bit or 128-bit)
 *
 * Characteristics are identified by their UUID and are the primary data containers
 * in a GATT Service.
 *
 * ---
 * ### Construction / Parsing
 *
 * There are two common ways to create a Characteristic:
 * 1. **Manual construction:** Use the constructor with UUID, properties, and handles.
 *    This is useful when you are assembling attributes in code.
 * 2. **Parse from attributes:** Use `ParseFromAttributes()` to extract the first
 *    characteristic from an ordered attribute list (Declaration, Value, then Descriptors).
 *    The extracted attributes are removed from the list and moved into the new
 *    Characteristic; no attribute rebuilding is performed. Callbacks are not copied.
 *
 * ---
 * ### Event Handling Model
 *
 * The class provides event callbacks via `EventHandlers`. These are invoked when:
 * - A client writes to the CCCD or SCCD (enabling/disabling notifications,
 *   indications, or broadcasts).
 * - A client reads or writes the characteristic value (`OnRead`, `OnWrite`).
 * - An indication completes (`OnConfirmationComplete`), which is driven by HCI events.
 *
 * The application must:
 * - Register handlers with `AddEventHandler()`.
 * - Ensure the handler instances outlive the Characteristic (stored as pointers).
 * - Feed HCI events into `DispatchBleHciPacket()` so indication completion and
 *   flow control (ATT_EVENT_CAN_SEND_NOW) are processed.
 * - Set a valid connection handle via `SetConnectionHandle()` to allow
 *   notifications/indications to be transmitted when values update.
 *
 * ---
 * ### RPi Pico (BTstack) Integration
 *
 * On the Pico W BLE stack, the GATT database is compiled into a binary
 * Attribute DB (typically `att_db.h`/`att_db.c`). The AttributeServer
 * parses that binary blob into `Attribute` objects. You can then call
 * `ParseFromAttributes()`
 * to peel one characteristic at a time from the ordered list.
 *
 * Important implications:
 * - Parsed static attributes keep pointers into the DB blob; the DB memory
 *   must remain valid for the lifetime of the `Attribute`/`Characteristic`.
 * - Parsing moves the attribute objects; callbacks are not copied, so register
 *   callbacks after parsing as needed.
 *
 * ---
 * ### Descriptor Model (what, when, and storage)
 *
 * This implementation mirrors the Bluetooth Core Spec descriptor types and
 * chooses dynamic vs static storage based on whether the value must change at
 * runtime or be written by a client.
 *
 * - CCCD (0x2902, Client Characteristic Configuration Descriptor)
 *   - Required when `Notify` or `Indicate` is set in the characteristic
 *     properties (Vol 3, Part G, 3.3.3.3).
 *   - Created lazily by `EnableCCCD()` or `SetCCCDValue()`; initial value
 *     is {0x00, 0x00}.
 *   - Properties: `kRead | kWrite | kDynamic`.
 *   - Dynamic because clients enable/disable notifications/indications at
 *     runtime, and writes must be accepted via the write callback.
 *
 * - SCCD (0x2903, Server Characteristic Configuration Descriptor)
 *   - Required when `Broadcast` is set in the characteristic properties
 *     (Vol 3, Part G, 3.3.3.4).
 *   - Created lazily by `EnableSCCD()` or `SetSCCDValue()`; initial value
 *     is {0x00, 0x00}.
 *   - Properties: `kRead | kWrite | kDynamic`.
 *   - Dynamic because clients can toggle broadcast behavior at runtime.
 *
 * - Characteristic Extended Properties (0x2900)
 *   - Used when `ExtendedProperties` is set (Vol 3, Part G, 3.3.3.1).
 *   - Created by `EnableExtendedProperties()` or `SetExtendedPropertiesValue()`.
 *   - Properties: `kRead` only (read-only metadata).
 *   - Static because the server defines these bits; clients only read them.
 *
 * - Characteristic User Description (0x2901)
 *   - Optional, user-facing label for UI/diagnostics (Vol 3, Part G, 3.3.3.2).
 *   - Created by `SetUserDescription()` or when `AddDescriptor()` sees UUID 0x2901.
 *   - Properties: `kRead` only in this implementation.
 *   - Static because it is server-supplied text that does not change at runtime.
 *
 * - Additional/custom descriptors
 *   - Created with `AddDescriptor()`.
 *   - Storage is dynamic only if `Attribute::Properties::kDynamic` is set;
 *     otherwise values are stored as static data.
 *   - This lets application-specific descriptors choose mutability and
 *     permissions explicitly.
 *
 * ---
 * ### Value Attribute Updates (read/write flow)
 *
 * The value attribute is the primary data payload for the characteristic.
 * This implementation uses internal read/write callbacks by default:
 *
 * - Client read flow:
 *   1. ATT read hits the value attribute and invokes `HandleValueRead()`.
 *   2. `OnRead` handlers fire first, giving the app a chance to call `SetValue()`
 *      (dynamic only) to refresh data.
 *   3. The current stored value is copied into the ATT response buffer.
 *
 * - Client write flow:
 *   1. ATT write hits the value attribute and invokes `HandleValueWrite()`.
 *   2. The implementation validates permissions and copies the bytes into
 *      the value attribute (dynamic storage only).
 *   3. `OnWrite` handlers are called after the value is stored.
 *
 * - Server-initiated updates:
 *   - Call `SetValue()` to update the stored value, then call `UpdateValue()`
 *     to send notifications or indications if enabled.
 *   - If both notification and indication bits are set, the implementation
 *     sends an indication and ignores notifications.
 *
 * Important: if the application replaces the value attribute callbacks via
 * `SetReadCallback()` or `SetWriteCallback()`, the default `HandleValueRead()`
 * and `HandleValueWrite()` are bypassed. In that case, EventHandlers are not
 * invoked automatically and the application is responsible for storage and
 * side effects.
 *
 * ---
 * ### Security Queries (Planning Before Connections)
 *
 * This class now provides higher-level security query helpers that map directly
 * to the configured SecurityLevel bits on the value attribute:
 * - Authentication queries: `ReadRequiresAuthentication()`,
 *   `WriteRequiresAuthentication()`, `RequiresAuthentication()`
 * - Authorization queries: `ReadRequiresAuthorization()`,
 *   `WriteRequiresAuthorization()`, `RequiresAuthorization()`
 *
 * These helpers are intended to support service- and server-level security
 * planning (for example, deciding whether a SecurityManager must be enabled)
 * before connections are established.
 *
 * ---
 * ### Internal/Reserved APIs (do not call from application code)
 *
 * The following methods exist to integrate with the BLE stack and platform glue.
 * They are not part of the public application API and should be considered
 * reserved for internal use:
 * - `DispatchBleHciPacket()` (HCI event dispatch/flow control)
 * - `HandleAttributeRead()` / `HandleAttributeWrite()` (ATT read/write handlers)
 * - `DispatchEvent()` (internal event fan-out)
 * - `UpdateValue()` (platform transport of notifications/indications)
 * - `HandleCccdWrite()` / `HandleSccdWrite()` (descriptor write handling)
 * - `HandleValueRead()` / `HandleValueWrite()` (value attribute handlers)
 * - Attribute factories for standard declarations/descriptors
 *
 * @note Only one of Notify or Indicate should be set; both require CCCD descriptor.
 * @note Read callbacks are required for dynamic read attributes.
 * @note Write callbacks are required for write-enabled attributes.
 */
class Characteristic final : public MovableOnly {
   public:
	/// \name Types and Configuration Enums
	/// Type definitions and configuration enums for characteristics.
	///@{
	/**
	 * @brief GATT Characteristic Properties as defined in Bluetooth Core Spec.
	 *
	 * These properties appear in the first byte of the Characteristic Declaration.
	 * They define which operations are permitted on the characteristic value.
	 *
	 * @note As per Bluetooth Core Specification Vol 3, Part G, Section 3.3.1.1
	 */
	enum class Properties : uint8_t {
		/** @brief No properties set. */
		kNone = 0x00,
		/** @brief Characteristic supports broadcast. */
		kBroadcast = 0x01,
		/** @brief Characteristic value can be read. */
		kRead = 0x02,
		/** @brief Characteristic value can be written without response. */
		kWriteWithoutResponse = 0x04,
		/** @brief Characteristic value can be written with response. */
		kWrite = 0x08,
		/** @brief Characteristic supports notifications. */
		kNotify = 0x10,
		/** @brief Characteristic supports indications. */
		kIndicate = 0x20,
		/** @brief Characteristic supports authenticated signed writes. */
		kAuthenticatedSignedWrites = 0x40,
		/** @brief Characteristic has extended properties descriptor. */
		kExtendedProperties = 0x80
	};

	/**
	 * @brief Client Characteristic Configuration Descriptor (CCCD) bit values.
	 *
	 * CCCD allows clients to configure notifications and indications for a characteristic.
	 *
	 * @note As per Bluetooth Core Spec Vol 3, Part G, Section 3.3.3.3
	 */
	enum class CCCDProperties : uint16_t {
		/** @brief Neither notifications nor indications enabled. */
		kNone = 0x0000,
		/** @brief Notifications enabled. */
		kNotifications = 0x0001,
		/** @brief Indications enabled. */
		kIndications = 0x0002
	};

	/**
	 * @brief Server Characteristic Configuration Descriptor (SCCD) bit values.
	 *
	 * SCCD allows clients to configure broadcasts for a characteristic.
	 *
	 * @note As per Bluetooth Core Spec Vol 3, Part G, Section 3.3.3.4
	 */
	enum class SCCDProperties : uint16_t {
		/** @brief Broadcasts disabled. */
		kNone = 0x0000,
		/** @brief Broadcasts enabled. */
		kBroadcasts = 0x0001
	};

	/**
	 * @brief Characteristic Extended Properties bit values.
	 *
	 * Extended Properties descriptor defines additional characteristic properties.
	 *
	 * @note As per Bluetooth Core Spec Vol 3, Part G, Section 3.3.3.1
	 */
	enum class ExtendedProperties : uint16_t {
		/** @brief No extended properties. */
		kNone = 0x0000,
		/** @brief Reliable Write enabled. */
		kReliableWrite = 0x0001,
		/** @brief Writable Auxiliaries enabled. */
		kWritableAuxiliaries = 0x0002
	};

	/**
	 * @brief Security level requirements for read/write operations.
	 *
	 * These levels define encryption and authentication requirements for accessing
	 * the characteristic value. The 2-bit encoding (Bit0 and Bit1) maps to
	 * BTstack's ATT_PROPERTY_READ/WRITE_PERMISSION_BIT_0 and BIT_1.
	 *
	 * Security levels are enforced by the BLE stack and determine when a client
	 * can successfully read or write the characteristic.
	 *
	 * @note As per Bluetooth Core Specification and BTstack att_db.h
	 */
	enum class SecurityLevel : uint8_t {
		/**
		 * @brief No security required (open access).
		 * Anyone can read/write without pairing or encryption.
		 * Bit pattern: 00b
		 */
		kNone = 0,

		/**
		 * @brief Unauthenticated pairing with encryption required.
		 * Client must pair and establish encrypted link (Just Works or Numeric Comparison).
		 * No MITM protection required.
		 * Bit pattern: 01b
		 */
		kEncryptionRequired = 1,

		/**
		 * @brief Authenticated pairing with encryption required.
		 * Client must pair using authenticated method with MITM protection
		 * (Passkey Entry, Out of Band) and establish encrypted link.
		 * Bit pattern: 10b
		 */
		kAuthenticationRequired = 2,

		/**
		 * @brief Authenticated pairing with encryption AND authorization required.
		 * Highest security level: authenticated pairing plus application-level
		 * authorization check. Application must approve access even after successful
		 * authentication and encryption.
		 * Bit pattern: 11b
		 */
		kAuthorizationRequired = 3
	};

	/**
	 * @brief Event IDs for Characteristic-related ATT events.
	 */
	enum class EventId : uint8_t {
		/** @brief ATT Handle Value Indication Complete (confirmation received or timeout). */
		kHandleValueIndicationComplete,
		/** @brief Generic ATT event (extensible for future events). */
		kAttEventEnd
	};

	/**
	 * @brief Characteristic event handler structure.
	 *
	 * This structure contains callback functions for various characteristic events
	 * related to client configuration changes, value access, and confirmations.
	 *
	 * These callbacks are fired by:
	 * - CCCD/SCCD writes (client enables/disables updates or broadcasts).
	 * - Attribute read/write on the value attribute.
	 * - HCI indication completion events passed via DispatchBleHciPacket().
	 *
	 * \note These Handlers are called before the data of the Attributes are updated!
	 */
	struct EventHandler {
		/**
		 * @brief Called when notifications or indications are enabled by a client.
		 *
		 * This callback is invoked when a client writes to the CCCD to enable
		 * updates for this characteristic.
		 * The CCCD value has already been written when this fires.
		 *
		 * @param is_indication true if indications are enabled, false if notifications are enabled
		 */
		virtual void OnUpdatesEnabled(bool is_indication) {
			(void)is_indication;
		}
		/**
		 * @brief Called when notifications or indications are disabled by a client.
		 *
		 * This callback is invoked when a client writes to the CCCD to disable
		 * updates for this characteristic (or writes 0x0000).
		 * The CCCD value has already been written when this fires.
		 */
		virtual void OnUpdatesDisabled() {
		}

		/**
		 * @brief Called when an indication transaction completes.
		 *
		 * This callback is invoked when the indication process finishes, regardless of outcome.
		 * The status parameter indicates whether the confirmation was successfully received,
		 * timed out, or the connection was lost.
		 * Fired after the HCI ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE is received
		 * and parsed in DispatchBleHciPacket().
		 *
		 * @param status 0 for success, non-zero for error (timeout, disconnect, etc.)
		 * @note Only applicable for characteristics with Indication property.
		 * @note This provides additional status information beyond OnConfirmationReceived.
		 */
		virtual void OnIndicationComplete(uint8_t status) {
			(void)status;
		}

		/**
		 * @brief Called when broadcasts are enabled by a client.
		 *
		 * This callback is invoked when a client writes to the SCCD to enable
		 * broadcasts for this characteristic.
		 * The SCCD value has already been written when this fires.
		 */
		virtual void OnBroadcastEnabled() {
		}

		/**
		 * @brief Called when broadcasts are disabled by a client.
		 *
		 * This callback is invoked when a client writes to the SCCD to disable
		 * broadcasts for this characteristic (or writes 0x0000).
		 * The SCCD value has already been written when this fires.
		 */
		virtual void OnBroadcastDisabled() {
		}

		/**
		 * @brief Called when a read operation is performed on this characteristic.
		 *
		 * This callback is invoked when a client attempts to read the characteristic value.
		 * This is a notification callback from the default read handler.
		 * Use this to update the characteristic value (via SetValue()) before
		 * it is copied into the read response.
		 */
		virtual void OnRead() {
		}

		/**
		 * @brief Called when a write operation is performed on this characteristic.
		 *
		 * This callback is invoked when a client writes data to the characteristic value.
		 * This is a notification callback from the default write handler with
		 * the written data provided for information. The data has already been
		 * stored in the value attribute (dynamic storage).
		 *
		 * @param data The data written by the client
		 */
		virtual void OnWrite(const std::vector<uint8_t>& data) {
			(void)data;
		}

		virtual void OnConfirmationReceived(bool status) {
			(void)status;
		}
	private:
		/**
		 * @brief Virtual destructor for the EventHandlers interface.
		 *
		 * This ensures proper cleanup of derived classes when deleted through a base class pointer.
		 * It provides a default no-op implementation and allows implementing classes
		 * to define their own destructor behavior if needed.
		 */
		virtual ~EventHandler() = default;
	};
	///@}

	/// \name Construction and Parsing
	/// APIs for building or parsing characteristics.
	///@{
	/**
	 * @brief Construct a new Characteristic with declaration data.
	 * Creates the characteristic with proper declaration attribute.
	 * The value handle is typically declaration_handle + 1.
	 * @param uuid The UUID identifying this characteristic
	 * @param properties Characteristic properties bitfield (combination of Properties enum flags)
	 * @param value_handle Handle of the Value attribute (typically declaration_handle + 1)
	 * @param declaration_handle Optional handle for the Declaration attribute (used in DB)
	 */
	explicit Characteristic(const Uuid& uuid,
							uint8_t properties,
							uint16_t value_handle,
							uint16_t declaration_handle = 0);

	/**
	 * @brief Construct a Characteristic by moving parsed attributes.
	 *
	 * This constructor takes ownership of the declaration/value/descriptor
	 * attributes produced during parsing and avoids rebuilding them. Handles
	 * are preserved from the moved attributes.
	 *
	 * @param decl_attribute Characteristic Declaration attribute (moved)
	 * @param value_attr Value attribute (moved)
	 * @param descriptor_attrs Descriptor attributes (moved)
	 */
	explicit Characteristic(Attribute&& decl_attribute,
							Attribute&& value_attr,
							std::list<Attribute>&& descriptor_attrs);

	/**
	 * @brief Move constructor.
	 * Transfers ownership of all internal attributes and descriptors.
	 */
	Characteristic(Characteristic&& other) noexcept;

	/**
	 * @brief Move assignment operator.
	 * Transfers ownership of all internal attributes and descriptors.
	 */
	Characteristic& operator=(Characteristic&& other) noexcept;
	/**
	 * \brief Construct a new Characteristic object (deleted).
	 * Prevents default construction without parameters.
	 */
	Characteristic() = delete;
	/**
	 * @brief Destructor.
	 * Cleans up all managed attributes and descriptors.
	 */
	~Characteristic() override = default;

	/**
	 * @brief Parse the first characteristic from a list of attributes.
	 *
	 * This function scans the attribute list for the first Characteristic
	 * Declaration attribute, then groups the following attributes (value and
	 * descriptors) into a single Characteristic.
	 *
	 * Implementation details:
	 * - Iterates the list in order; the first declaration starts a new characteristic.
	 * - Parses declaration value as: properties (1), value handle (2, LE), UUID (2 or 16).
	 * - Matches the value attribute by handle within the extracted slice.
	 * - Consumes following descriptor attributes until the next declaration or service.
	 * - Copies value bytes and descriptor bytes into the new Characteristic; callbacks are not copied.
	 * - Recognizes CCCD and User Description explicitly; other descriptors are added as custom.
	 *
	 * Assumptions:
	 * - Attributes are ordered (Declaration, Value, Descriptors).
	 * - Declaration value length is 5 (16-bit UUID) or 19 (128-bit UUID).
	 * - Value handle in the declaration refers to the value attribute in the list.
	 * - Descriptor attributes follow the value attribute contiguously.
	 * - Parsing stops at the next characteristic declaration or service declaration.
	 *
	 * @param attributes Attribute list to parse; the extracted attributes are removed
	 * @return Parsed Characteristic if a declaration was found; std::nullopt otherwise
	 *
	 * @note Attributes must be in proper order: Declaration, Value, Descriptors
	 * @note Stops parsing when the next characteristic declaration or service declaration is found
	 * @note Value/descriptor callbacks are not copied from the source attributes
	 * @note The parsed attributes are moved into the resulting Characteristic
	 * @note The extracted attributes are removed even if parsing fails
	 */
	static std::optional<Characteristic>
	ParseFromAttributes(std::list<Attribute>& attributes);
	///@}

	/// \name Core Accessors
	/// Accessors for identifiers, handles, and validity checks.
	///@{

	/**
	 * @brief Get the UUID of this characteristic.
	 * @return Reference to the characteristic's UUID
	 */
	[[nodiscard]] const Uuid& GetUuid() const {
		return uuid_;
	}

	/**
	 * @brief Get the properties of this characteristic.
	 * @return Bitmask of characteristic properties (Read, Write, Notify, etc.)
	 */
	[[nodiscard]] Properties GetProperties() const {
		return properties_;
	}

	/**
	 * @brief Get the handle of the Value attribute.
	 * @return Value attribute handle (typically declaration_handle + 1)
	 */
	[[nodiscard]] uint16_t GetValueHandle() const {
		return value_attr_.GetHandle();
	}

	/**
	 * @brief Get the handle of the Declaration attribute.
	 * @return Declaration attribute handle
	 */
	[[nodiscard]] uint16_t GetDeclarationHandle() const {
		return declaration_attr_.GetHandle();
	}

	/**
	 * @brief Check if this characteristic is valid.
	 * @return true if UUID is valid and handles are non-zero
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Check if this characteristic matches the given UUID and handle.
	 * @param uuid UUID to compare
	 * @param handle Attribute handle (declaration or value)
	 * @return true if both UUID matches and handle matches declaration or value
	 */
	[[nodiscard]] bool IsThisCharacteristic(const Uuid& uuid, uint16_t handle) const;

	/**
	 * @brief Check if this characteristic matches the given UUID.
	 * @param uuid UUID to compare
	 * @return true if UUID matches
	 */
	[[nodiscard]] bool IsThisCharacteristic(const Uuid& uuid) const;

	/**
	 * @brief Check if this characteristic owns the given handle.
	 * @param handle Attribute handle (declaration or value)
	 * @return true if handle matches declaration, value, or descriptor
	 */
	[[nodiscard]] bool HasHandle(uint16_t handle) const;
	///@}

	/// \name Capability Checks
	/// Helpers to query characteristic capability flags.
	///@{

	/**
	 * @brief Check if characteristic supports broadcasts.
	 * @return true if kBroadcast property is set
	 */
	[[nodiscard]] bool HasBroadcast() const {
		return (static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kBroadcast)) !=
			   0;
	}

	/**
	 * @brief Check if characteristic supports read operations.
	 * @return true if kRead property is set
	 */
	[[nodiscard]] bool CanRead() const {
		return (static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kRead)) != 0;
	}

	/**
	 * @brief Check if characteristic supports write without response.
	 * @return true if kWriteWithoutResponse property is set
	 */
	[[nodiscard]] bool CanWriteWithoutResponse() const {
		return (static_cast<uint8_t>(properties_) &
				static_cast<uint8_t>(Properties::kWriteWithoutResponse)) != 0;
	}

	/**
	 * @brief Check if characteristic supports write operations (with response).
	 * @return true if kWrite property is set
	 */
	[[nodiscard]] bool CanWrite() const {
		return (static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kWrite)) != 0;
	}

	/**
	 * @brief Check if characteristic supports notifications.
	 * @return true if kNotify property is set
	 */
	[[nodiscard]] bool HasNotifications() const {
		return (static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kNotify)) != 0;
	}

	/**
	 * @brief Check if characteristic supports indications.
	 * @return true if kIndicate property is set
	 */
	[[nodiscard]] bool HasIndications() const {
		return (static_cast<uint8_t>(properties_) & static_cast<uint8_t>(Properties::kIndicate)) !=
			   0;
	}

	/**
	 * @brief Check if characteristic supports authenticated signed writes.
	 * @return true if kAuthenticatedSignedWrites property is set
	 */
	[[nodiscard]] bool CanSignedWrite() const {
		return (static_cast<uint8_t>(properties_) &
				static_cast<uint8_t>(Properties::kAuthenticatedSignedWrites)) != 0;
	}

	/**
	 * @brief Check if characteristic properties flag has extended properties capability.
	 * @return true if kExtendedProperties property bit is set
	 */
	[[nodiscard]] bool HasExtendedPropertiesCapability() const {
		return (static_cast<uint8_t>(properties_) &
				static_cast<uint8_t>(Properties::kExtendedProperties)) != 0;
	}
	///@}

	/// \name Security Requirement Checks
	/// Helpers to inspect configured security requirements.
	///@{
	/**
	 * @brief Check if characteristic has read permission bits set (requires authorization).
	 * Read permission bits 0 and 1 indicate security level requirements.
	 * @return true if read permission bits (kReadPermissionBit0/1) are set
	 */
	[[nodiscard]] bool ReadHasSecurityRequirement() const {
		uint16_t props = value_attr_.GetProperties();
		return (props & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit0)) != 0 ||
			   (props & static_cast<uint16_t>(Attribute::Properties::kReadPermissionBit1)) != 0;
	}

	/**
	 * @brief Check if characteristic has write permission bits set (requires authorization).
	 * Write permission bits 0 and 1 indicate security level requirements.
	 * @return true if write permission bits (kWritePermissionBit0/1) are set
	 */
	[[nodiscard]] bool WriteHasSecurityRequirement() const {
		uint16_t props = value_attr_.GetProperties();
		return (props & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit0)) != 0 ||
			   (props & static_cast<uint16_t>(Attribute::Properties::kWritePermissionBit1)) != 0;
	}

	/**
	 * @brief Check if characteristic read requires Secure Connections (SC).
	 * @return true if kReadPermissionSc is set
	 */
	[[nodiscard]] bool ReadRequiresSC() const {
		return (value_attr_.GetProperties() &
				static_cast<uint16_t>(Attribute::Properties::kReadPermissionSc)) != 0;
	}

	/**
	 * @brief Check if characteristic write requires Secure Connections (SC).
	 * @return true if kWritePermissionSc is set
	 */
	[[nodiscard]] bool WriteRequiresSC() const {
		return (value_attr_.GetProperties() &
				static_cast<uint16_t>(Attribute::Properties::kWritePermissionSc)) != 0;
	}

	/**
	 * @brief Get the required encryption key size for this characteristic.
	 * Returns the encryption key size requirement encoded in bits 12-15.
	 * @return Encryption key size (0-16 bytes), or 0 if no requirement
	 */
	[[nodiscard]] uint16_t GetEncryptionKeySize() const {
		return (value_attr_.GetProperties() &
				static_cast<uint16_t>(Attribute::Properties::kEncryptionKeySizeMask)) >>
			   12;
	}

	/**
	 * @brief Check whether reads require authenticated pairing (MITM).
	 *
	 * This returns true when the configured read security level is
	 * kAuthenticationRequired or kAuthorizationRequired.
	 */
	[[nodiscard]] bool ReadRequiresAuthentication() const;

	/**
	 * @brief Check whether writes require authenticated pairing (MITM).
	 *
	 * This returns true when the configured write security level is
	 * kAuthenticationRequired or kAuthorizationRequired.
	 */
	[[nodiscard]] bool WriteRequiresAuthentication() const;

	/**
	 * @brief Check whether reads require application-level authorization.
	 *
	 * Authorization is the highest level and implies authentication.
	 */
	[[nodiscard]] bool ReadRequiresAuthorization() const;

	/**
	 * @brief Check whether writes require application-level authorization.
	 *
	 * Authorization is the highest level and implies authentication.
	 */
	[[nodiscard]] bool WriteRequiresAuthorization() const;

	/**
	 * @brief Check whether any access requires authentication.
	 *
	 * Returns true if either read or write requires authentication.
	 */
	[[nodiscard]] bool RequiresAuthentication() const;

	/**
	 * @brief Check whether any access requires authorization.
	 *
	 * Returns true if either read or write requires authorization.
	 */
	[[nodiscard]] bool RequiresAuthorization() const;
	///@}

	/// \name Security Configuration
	/// APIs to configure required security levels.
	///@{

	/**
	 * @brief Set read security level requirement for this characteristic.
	 *
	 * Configures the security level that clients must meet to read this characteristic.
	 * This sets the appropriate ATT_PROPERTY_READ_PERMISSION_BIT_0 and BIT_1 flags
	 * on the value attribute.
	 *
	 * @param level Security level (None, Encryption, Authentication, or Authorization)
	 *
	 * @example
	 * // Require authenticated pairing for reads
	 * characteristic.SetReadSecurityLevel(Characteristic::SecurityLevel::kAuthenticationRequired);
	 */
	void SetReadSecurityLevel(SecurityLevel level);

	/**
	 * @brief Set write security level requirement for this characteristic.
	 *
	 * Configures the security level that clients must meet to write this characteristic.
	 * This sets the appropriate ATT_PROPERTY_WRITE_PERMISSION_BIT_0 and BIT_1 flags
	 * on the value attribute.
	 *
	 * @param level Security level (None, Encryption, Authentication, or Authorization)
	 *
	 * @example
	 * // Require encryption for writes
	 * characteristic.SetWriteSecurityLevel(Characteristic::SecurityLevel::kEncryptionRequired);
	 */
	void SetWriteSecurityLevel(SecurityLevel level);

	/**
	 * @brief Set Secure Connections requirement for reads.
	 *
	 * When enabled, reads require LE Secure Connections pairing (Bluetooth 4.2+).
	 * This provides enhanced security with ECDH key agreement.
	 * Sets ATT_PROPERTY_READ_PERMISSION_SC flag.
	 *
	 * @param required true to require Secure Connections, false to allow legacy pairing
	 */
	void SetReadRequiresSecureConnections(bool required);

	/**
	 * @brief Set Secure Connections requirement for writes.
	 *
	 * When enabled, writes require LE Secure Connections pairing (Bluetooth 4.2+).
	 * This provides enhanced security with ECDH key agreement.
	 * Sets ATT_PROPERTY_WRITE_PERMISSION_SC flag.
	 *
	 * @param required true to require Secure Connections, false to allow legacy pairing
	 */
	void SetWriteRequiresSecureConnections(bool required);

	/**
	 * @brief Set minimum encryption key size requirement.
	 *
	 * Configures the minimum encryption key size (in bytes) that clients must use.
	 * Valid range is 7-16 bytes per Bluetooth specification.
	 * Sets bits 12-15 of the attribute properties (ATT_PROPERTY_ENCRYPTION_KEY_SIZE_MASK).
	 *
	 * @param key_size Minimum key size in bytes (7-16), or 0 for no requirement
	 * @note Most implementations use 16 bytes (128-bit) for maximum security
	 */
	void SetEncryptionKeySize(uint8_t key_size);

	/**
	 * @brief Get current read security level.
	 * @return SecurityLevel extracted from read permission bits
	 */
	[[nodiscard]] SecurityLevel GetReadSecurityLevel() const;

	/**
	 * @brief Get current write security level.
	 * @return SecurityLevel extracted from write permission bits
	 */
	[[nodiscard]] SecurityLevel GetWriteSecurityLevel() const;
	///@}

	/// \name Permission Evaluation
	/// Evaluate access permissions for a given connection state.
	///@{

	/**
	 * @brief Check if read is permitted given the connection security state.
	 *
	 * Evaluates whether a client with the given security state is allowed to read
	 * this characteristic based on the configured read security level and
	 * Secure Connections requirement.
	 *
	 * @param authorized true if client is authorized (passed app-level authorization)
	 * @param authenticated true if client has authenticated pairing
	 * @return true if read is permitted, false if security requirements not met
	 *
	 * @note Secure Connections requirement is checked separately and not included
	 *       in this logic (caller must verify SC separately if needed)
	 *
	 * @example
	 * if (characteristic.IsReadPermitted(user_authorized, is_authenticated)) {
	 *     // Allow read operation
	 * }
	 */
	[[nodiscard]] bool IsReadPermitted(bool authorized, bool authenticated) const;

	/**
	 * @brief Check if write is permitted given the connection security state.
	 *
	 * Evaluates whether a client with the given security state is allowed to write
	 * this characteristic based on the configured write security level and
	 * Secure Connections requirement.
	 *
	 * @param authorized true if client is authorized (passed app-level authorization)
	 * @param authenticated true if client has authenticated pairing
	 * @return true if write is permitted, false if security requirements not met
	 *
	 * @note Secure Connections requirement is checked separately and not included
	 *       in this logic (caller must verify SC separately if needed)
	 *
	 * @example
	 * if (characteristic.IsWritePermitted(user_authorized, is_authenticated)) {
	 *     // Allow write operation
	 * }
	 */
	[[nodiscard]] bool IsWritePermitted(bool authorized, bool authenticated) const;

	/**
	 * @brief Check if characteristic has 128-bit UUID.
	 * @return true if kUuid128 is set in value attribute
	 */
	[[nodiscard]] bool Uses128BitUuid() const {
		return (value_attr_.GetProperties() &
				static_cast<uint16_t>(Attribute::Properties::kUuid128)) != 0;
	}

	/**
	 * @brief Check if characteristic value is dynamic (can change).
	 * @return true if kDynamic property is set
	 */
	[[nodiscard]] bool IsDynamic() const {
		return (value_attr_.GetProperties() &
				static_cast<uint16_t>(Attribute::Properties::kDynamic)) != 0;
	}
	///@}

	/// \name Value Access
	/// Read the current stored characteristic value.
	///@{

	/**
	 * @brief Get the current value data pointer.
	 * For dynamic values, this may change after SetValue() calls.
	 * @return Pointer to value data, or nullptr if no value set
	 */
	[[nodiscard]] const uint8_t* GetValueData() const;

	/**
	 * @brief Get the size of the current value.
	 * @return Size in bytes of the characteristic value
	 */
	[[nodiscard]] size_t GetValueSize() const;

	/**
	 * @brief Get the complete value as a vector.
	 * @return Vector copy of the characteristic value
	 */
	[[nodiscard]] std::vector<uint8_t> GetValueAsVector() const;
	///@}

	/// \name Value Updates
	/// Update stored values and trigger update flow.
	///@{

	/**
	 * @brief Set the characteristic value from raw bytes (pointer + size).
	 * Only allowed for dynamic characteristics.
	 * @param data Pointer to value data (copied into storage)
	 * @param size Size of the data in bytes
	 * @return true if value was set, false if rejected (e.g., static characteristic)
	 */
	bool SetValue(const uint8_t* data, size_t size);

	/**
	 * @brief Set the characteristic value from an rvalue vector (move semantics).
	 * Only allowed for dynamic characteristics.
	 * @param data Rvalue reference to vector (moved into storage)
	 * @return true if value was set, false if rejected (e.g., static characteristic)
	 */
	bool SetValue(std::vector<uint8_t>&& data);

	/**
	 * @brief Set the characteristic value from an lvalue vector (copy semantics).
	 * Only allowed for dynamic characteristics.
	 * @param data Const reference to vector (copied into storage)
	 * @return true if value was set, false if rejected (e.g., static characteristic)
	 */
	bool SetValue(const std::vector<uint8_t>& data);

	/**
	 * @brief Set the characteristic value from a typed value (generic template).
	 * Converts any trivial type to bytes and stores as the value.
	 * Only allowed for dynamic characteristics.
	 * @tparam T Must be a trivial type (primitive types, arrays, small structs)
	 * @param value The value to store
	 * @return true if value was set, false if rejected (e.g., static characteristic)
	 * @note Uses binary representation; for endian-sensitive types, ensure consistency
	 * @example SetValue<uint16_t>(0x1234) stores as bytes {0x34, 0x12} (little-endian)
	 */
	template <typename T>
	bool SetValue(const T& value) {
		static_assert(std::is_trivial<T>::value, "T must be a trivial type for binary conversion");
		return SetValue(reinterpret_cast<const uint8_t*>(&value), sizeof(T));
	}
	///@}

	/// \name Descriptor Management
	/// Create and inspect standard and custom descriptors.
	///@{

	/**
	 * @brief Enable Client Characteristic Configuration Descriptor (CCCD).
	 * Required for characteristics with Notify or Indicate properties.
	 * Initializes CCCD with value {0x00, 0x00} (notifications and indications disabled).
	 * @return Reference to the CCCD Attribute for further configuration
	 */
	Attribute& EnableCCCD();

	/**
	 * @brief Check if CCCD descriptor is present.
	 * @return true if CCCD has been enabled
	 */
	[[nodiscard]] bool HasCCCD() const {
		return cccd_ != nullptr;
	}

	/**
	 * @brief Check if notifications are enabled via CCCD.
	 * @return true if CCCD is present and notifications bit is set
	 */
	[[nodiscard]] bool IsNotificationsEnabled() const;

	/**
	 * @brief Check if indications are enabled via CCCD.
	 * @return true if CCCD is present and indications bit is set
	 */
	[[nodiscard]] bool IsIndicationsEnabled() const;

	/**
	 * @brief Get the CCCD descriptor.
	 * @return Pointer to CCCD Attribute, or nullptr if not enabled
	 */
	Attribute* GetCCCD() {
		return cccd_.get();
	}

	/**
	 * @brief Get the CCCD descriptor (const version).
	 * @return Const pointer to CCCD Attribute, or nullptr if not enabled
	 */
	[[nodiscard]] const Attribute* GetCCCD() const {
		return cccd_.get();
	}

	/**
	 * @brief Set CCCD configuration value.
	 * Enables or configures notifications and/or indications.
	 * Automatically creates CCCD descriptor if not present.
	 * @param config CCCD configuration flags
	 * @return Reference to the CCCD Attribute
	 */
	Attribute& SetCCCDValue(CCCDProperties config);

	/**
	 * @brief Enable Server Characteristic Configuration Descriptor (SCCD).
	 * Required for characteristics with Broadcast property.
	 * Initializes SCCD with value {0x00, 0x00} (broadcasts disabled).
	 * @return Reference to the SCCD Attribute for further configuration
	 */
	Attribute& EnableSCCD();

	/**
	 * @brief Check if SCCD descriptor is present.
	 * @return true if SCCD has been enabled
	 */
	[[nodiscard]] bool HasSCCD() const {
		return sccd_ != nullptr;
	}

	/**
	 * @brief Check if broadcasts are enabled via SCCD.
	 * @return true if SCCD is present and broadcasts bit is set
	 */
	[[nodiscard]] bool IsBroadcastEnabled() const;

	/**
	 * @brief Get the SCCD descriptor.
	 * @return Pointer to SCCD Attribute, or nullptr if not enabled
	 */
	Attribute* GetSCCD() {
		return sccd_.get();
	}

	/**
	 * @brief Get the SCCD descriptor (const version).
	 * @return Const pointer to SCCD Attribute, or nullptr if not enabled
	 */
	[[nodiscard]] const Attribute* GetSCCD() const {
		return sccd_.get();
	}

	/**
	 * @brief Set SCCD configuration value.
	 * Enables or disables broadcasts.
	 * Automatically creates SCCD descriptor if not present.
	 * @param config SCCD configuration flags
	 * @return Reference to the SCCD Attribute
	 */
	Attribute& SetSCCDValue(SCCDProperties config);

	/**
	 * @brief Enable Characteristic Extended Properties Descriptor.
	 * Defines additional characteristic properties beyond the standard properties byte.
	 * Initializes with value {0x00, 0x00} (no extended properties).
	 * @return Reference to the Extended Properties Attribute for further configuration
	 */
	Attribute& EnableExtendedProperties();

	/**
	 * @brief Check if Extended Properties descriptor is present.
	 * @return true if Extended Properties has been enabled
	 */
	[[nodiscard]] bool HasExtendedProperties() const {
		return extended_properties_ != nullptr;
	}

	/**
	 * @brief Get the Extended Properties descriptor.
	 * @return Pointer to Extended Properties Attribute, or nullptr if not enabled
	 */
	Attribute* GetExtendedProperties() {
		return extended_properties_.get();
	}

	/**
	 * @brief Get the Extended Properties descriptor (const version).
	 * @return Const pointer to Extended Properties Attribute, or nullptr if not enabled
	 */
	[[nodiscard]] const Attribute* GetExtendedProperties() const {
		return extended_properties_.get();
	}

	/**
	 * @brief Set Extended Properties configuration value.
	 * Configures reliable write and writable auxiliaries.
	 * Automatically creates Extended Properties descriptor if not present.
	 * @param config Extended properties flags
	 * @return Reference to the Extended Properties Attribute
	 */
	Attribute& SetExtendedPropertiesValue(ExtendedProperties config);

	/**
	 * @brief Set the Characteristic User Description.
	 * Creates or updates the user-readable description of this characteristic.
	 * @param description Human-readable description string
	 * @return Reference to the User Description Attribute for further configuration
	 */
	Attribute& SetUserDescription(const std::string& description);

	/**
	 * @brief Check if User Description descriptor is present.
	 * @return true if user description has been set
	 */
	[[nodiscard]] bool HasUserDescription() const {
		return user_description_ != nullptr;
	}

	/**
	 * @brief Get the User Description descriptor.
	 * @return Pointer to User Description Attribute, or nullptr if not set
	 */
	Attribute* GetUserDescription() {
		return user_description_.get();
	}

	/**
	 * @brief Get the User Description descriptor (const version).
	 * @return Const pointer to User Description Attribute, or nullptr if not set
	 */
	[[nodiscard]] const Attribute* GetUserDescription() const {
		return user_description_.get();
	}

	/**
	 * @brief Add a custom descriptor to this characteristic.
	 * Allows extension with application-specific descriptors beyond CCCD and User Description.
	 * @param uuid UUID of the descriptor
	 * @param properties Descriptor properties (typically Read and/or Write)
	 * @param value Initial value of the descriptor
	 * @param handle Optional handle for the descriptor (used in DB)
	 * @return Reference to the newly added Attribute for callback configuration
	 */
	Attribute& AddDescriptor(const Uuid& uuid,
							 Attribute::Properties properties,
							 const std::vector<uint8_t>& value,
							 uint16_t handle = 0);

	/**
	 * @brief Get count of custom descriptors (excluding CCCD and User Description).
	 * @return Number of additional descriptors
	 */
	[[nodiscard]] size_t GetDescriptorCount() const {
		return descriptors_.size();
	}

	/**
	 * @brief Get a custom descriptor by index.
	 * @param index Index of the descriptor (0 to GetDescriptorCount()-1)
	 * @return Pointer to the Attribute, or nullptr if index out of range
	 */
	Attribute* GetDescriptor(size_t index);

	/**
	 * @brief Get a custom descriptor by index (const version).
	 * @param index Index of the descriptor (0 to GetDescriptorCount()-1)
	 * @return Const pointer to the Attribute, or nullptr if index out of range
	 */
	[[nodiscard]] const Attribute* GetDescriptor(size_t index) const;
	///@}

	/// \name Attribute Access
	/// Access underlying Attribute objects.
	///@{

	/**
	 * @brief Get the Declaration Attribute.
	 * The declaration attribute contains properties, value handle, and UUID.
	 * @return Reference to the Declaration Attribute
	 */
	Attribute& GetDeclarationAttribute() {
		return declaration_attr_;
	}

	/**
	 * @brief Get the Declaration Attribute (const version).
	 * @return Const reference to the Declaration Attribute
	 */
	[[nodiscard]] const Attribute& GetDeclarationAttribute() const {
		return declaration_attr_;
	}

	/**
	 * @brief Get the Value Attribute.
	 * The value attribute holds the actual characteristic data.
	 * @return Reference to the Value Attribute
	 */
	Attribute& GetValueAttribute() {
		return value_attr_;
	}

	/**
	 * @brief Get the Value Attribute (const version).
	 * @return Const reference to the Value Attribute
	 */
	[[nodiscard]] const Attribute& GetValueAttribute() const {
		return value_attr_;
	}
	///@}

	/// \name Event Handler Management
	/// Register and manage characteristic event handlers.
	///@{

	/**
	 * @brief Register an event handler for this characteristic.
	 *
	 * The handler is stored as a pointer; it must outlive the Characteristic instance.
	 * Multiple handlers can be registered and will be called in the order they were added.
	 *
	 * @param handler Reference to the EventHandlers structure to register
	 */
	void AddEventHandler(EventHandler& handler);

	/**
	 * @brief Unregister an event handler from this characteristic.
	 *
	 * @param handler Reference to the EventHandlers structure to unregister
	 * @return true if the handler was found and removed, false otherwise
	 *
	 * @note Handlers are compared by pointer address. If multiple identical handlers were added,
	 * only the first is removed.
	 * @note Removing a handler that was not registered has no effect.
	 * @note Do not remove handlers while iterating through the handler list (e.g. during event
	 * dispatch). It is NOT safe to remove handlers from within event callbacks.
	 * @note The handler instance is not deleted by this method. Therefore, the caller must delete
	 * the object if it was dynamically allocated.
	 */
	bool RemoveEventHandler(const EventHandler& handler);

	/**
	 * @brief Clear all registered event handlers for this characteristic.
	 *
	 * @note The handler instances are not deleted by this method. Therefore, the caller must delete
	 * the objects if they were dynamically allocated.
	 */
	void ClearEventHandlers();

	/**
	 * @brief Get the list of registered event handlers.
	 * @return List of pointers to registered EventHandlers structures
	 */
	[[nodiscard]] std::list<EventHandler*> GetEventHandlers() const {
		return event_handlers_;
	}
	///@}

	/// \name Connection Handle Management
	/// Manage the active connection handle used for updates.
	///@{

	/**
	 * @brief Set the connection handle for this characteristic.
	 *
	 * The connection handle is set when a device connects and the characteristic's
	 * security requirements are satisfied. Used by UpdateValue to send notifications/indications.
	 *
	 * @param connection_handle The connection handle (0 is invalid/disconnected)
	 */
	void SetConnectionHandle(uint16_t connection_handle) {
		connection_handle_ = connection_handle;
	}

	/**
	 * @brief Get the current connection handle.
	 * @return The connection handle, or 0 if disconnected/invalid
	 */
	[[nodiscard]] uint16_t GetConnectionHandle() const {
		return connection_handle_;
	}
	///@}

	/// \name Stack Dispatch (Internal)
	/// BLE stack entry points reserved for internal use.
	///@{

	/**
	 * @brief Dispatch BLE HCI packet to the appropriate event handler.
	 *
	 * This must be fed HCI events so that indication completion and
	 * ATT_EVENT_CAN_SEND_NOW flow control are handled.
	 *
	 * @param packet_type The type of the HCI packet
	 * @param packet_data Pointer to the packet data
	 * @param packet_data_size Size of the packet data
	 * @return BleError indicating success or failure
	 * @note Internal use only (platform BLE stack dispatch).
	 */
	virtual BleError DispatchBleHciPacket(uint8_t packet_type,
										  const uint8_t* packet_data,
										  uint16_t packet_data_size);
	/**
	 * @brief Attribute read handler for BLE stack callbacks.
	 * @note Internal use only (ATT read handler).
	 */
	uint16_t HandleAttributeRead(uint16_t attribute_handle,
								 uint16_t offset,
								 uint8_t* buffer,
								 uint16_t buffer_size);
	/**
	 * @brief Attribute write handler for BLE stack callbacks.
	 * @note Internal use only (ATT write handler).
	 */
	BleError HandleAttributeWrite(uint16_t attribute_handle,
								  uint16_t offset,
								  const uint8_t* data,
								  uint16_t size);
	///@}

   protected:
	/// \name Internal Event Fan-out
	/// Internal event dispatch to registered handlers.
	///@{
	/**
	 * @brief Dispatch event to registered event handlers.
	 *
	 * @param event_id The event identifier
	 * @param event_data Pointer to the event data
	 * @param event_data_size Size of the event data
	 * @return BleError indicating success or failure
	 * @note Internal use only (event fan-out from stack dispatch).
	 */
	BleError DispatchEvent(EventId event_id,
								   const uint8_t* event_data,
								   uint16_t event_data_size);
	///@}

	/// \name Internal Updates
	/// Internal update/notification transport.
	///@{
	/**
	 * @brief Update the characteristic value and send notification/indication if enabled.
	 *
	 * This is called internally by SetValue functions to handle platform-specific
	 * transmission logic. The function checks CCCD configuration and sends the appropriate
	 * update to the connected client based on enabled notifications/indications.
	 *
	 * Logic:
	 * - If neither notifications nor indications are enabled: no transmission
	 * - If only notifications are enabled: sends notification
	 * - If only indications are enabled: sends indication
	 * - If both are enabled: sends indication and ignores notification
	 *
	 * @return BleError indicating success or failure
	 *
	 * @note Only executes if connection_handle_ is valid (non-zero)
	 * @note Platform-specific implementation handles actual transmission
	 * @note Internal use only (called from SetValue() and BLE stack flow control).
	 */
	virtual BleError UpdateValue();
	///@}

   private:
	// Core characteristic data
	Uuid uuid_;					  ///< Characteristic UUID
	Properties properties_;		  ///< Read, Write, Notify, Indicate, etc.
	uint16_t connection_handle_;  ///< Current connection handle (0 if disconnected)
	bool notification_pending_;	 ///< True if notification/indication is pending due to full buffers

	// Required attributes
	Attribute declaration_attr_;  ///< Characteristic Declaration attribute
	Attribute value_attr_;		  ///< Characteristic Value attribute

	// Optional descriptors
	std::unique_ptr<Attribute> cccd_;  ///< Client Characteristic Configuration Descriptor
	std::unique_ptr<Attribute> sccd_;  ///< Server Characteristic Configuration Descriptor
	std::unique_ptr<Attribute> extended_properties_;  ///< Extended Properties descriptor
	std::unique_ptr<Attribute> user_description_;	  ///< User Description descriptor
	std::list<Attribute> descriptors_;				  ///< Additional custom descriptors

	/// \name Internal Attribute Handlers
	/// Internal ATT read/write helpers.
	///@{
	// Internal helpers for descriptor write handling
	/**
	 * @brief CCCD write handler used by the stack dispatcher.
	 * @note Internal use only.
	 */
	BleError HandleCccdWrite(uint16_t offset, const uint8_t* data, uint16_t size) const;
	/**
	 * @brief SCCD write handler used by the stack dispatcher.
	 * @note Internal use only.
	 */
	BleError HandleSccdWrite(uint16_t offset, const uint8_t* data, uint16_t size) const;

	// Internal helpers for value attribute read/write handling
	/**
	 * @brief Value read handler used by the stack dispatcher.
	 * @note Internal use only.
	 */
	uint16_t HandleValueRead(uint16_t offset, uint8_t* buffer, uint16_t buffer_size);
	/**
	 * @brief Value write handler used by the stack dispatcher.
	 * @note Internal use only.
	 */
	BleError HandleValueWrite(uint16_t offset, const uint8_t* data, uint16_t size);
	///@}

	// Event handlers
	std::list<EventHandler*> event_handlers_;  ///< Registered event handlers

	void RebindInternalCallbacks();

	/**
	 * @brief Stream insertion operator for Characteristic.
	 * Outputs the characteristic UUID, properties, security requirements, and descriptors.
	 */
	friend std::ostream& operator<<(std::ostream& os, const Characteristic& characteristic);
};

// ========== Bitwise operators for Characteristic::Properties ==========

/**
 * @brief Bitwise OR operator for Characteristic::Properties.
 * @param lhs Left-hand side operand
 * @param rhs Right-hand side operand
 * @return Combined properties bitmask
 */
constexpr Characteristic::Properties operator|(Characteristic::Properties lhs,
											   Characteristic::Properties rhs) noexcept {
	return static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) |
												   static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise AND operator for Characteristic::Properties.
 * @param lhs Left-hand side operand
 * @param rhs Right-hand side operand
 * @return Intersection of properties bitmask
 */
constexpr Characteristic::Properties operator&(Characteristic::Properties lhs,
											   Characteristic::Properties rhs) noexcept {
	return static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) &
												   static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise XOR operator for Characteristic::Properties.
 * @param lhs Left-hand side operand
 * @param rhs Right-hand side operand
 * @return XOR of properties bitmask
 */
constexpr Characteristic::Properties operator^(Characteristic::Properties lhs,
											   Characteristic::Properties rhs) noexcept {
	return static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) ^
												   static_cast<uint8_t>(rhs));
}

/**
 * @brief Bitwise NOT operator for Characteristic::Properties.
 * @param prop Properties to invert
 * @return Inverted properties bitmask
 */
constexpr Characteristic::Properties operator~(Characteristic::Properties prop) noexcept {
	return static_cast<Characteristic::Properties>(~static_cast<uint8_t>(prop));
}

/**
 * @brief Bitwise OR assignment operator for Characteristic::Properties.
 * @param lhs Left-hand side operand (modified)
 * @param rhs Right-hand side operand
 * @return Reference to modified lhs
 */
constexpr Characteristic::Properties& operator|=(Characteristic::Properties& lhs,
												 Characteristic::Properties rhs) noexcept {
	lhs = static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) |
												  static_cast<uint8_t>(rhs));
	return lhs;
}

/**
 * @brief Bitwise AND assignment operator for Characteristic::Properties.
 * @param lhs Left-hand side operand (modified)
 * @param rhs Right-hand side operand
 * @return Reference to modified lhs
 */
constexpr Characteristic::Properties& operator&=(Characteristic::Properties& lhs,
												 Characteristic::Properties rhs) noexcept {
	lhs = static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) &
												  static_cast<uint8_t>(rhs));
	return lhs;
}

/**
 * @brief Bitwise XOR assignment operator for Characteristic::Properties.
 * @param lhs Left-hand side operand (modified)
 * @param rhs Right-hand side operand
 * @return Reference to modified lhs
 */
constexpr Characteristic::Properties& operator^=(Characteristic::Properties& lhs,
												 Characteristic::Properties rhs) noexcept {
	lhs = static_cast<Characteristic::Properties>(static_cast<uint8_t>(lhs) ^
												  static_cast<uint8_t>(rhs));
	return lhs;
}

/**
 * @brief Stream output operator for Characteristic::Properties.
 * Outputs a human-readable list of properties (e.g., "Read | Write | Notify").
 * @param os Output stream
 * @param props Properties bitmask
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, Characteristic::Properties props);

/**
 * @brief Stream output operator for Characteristic.
 * Outputs comprehensive information about the characteristic including UUID, properties,
 * handles, value, and all descriptors.
 * @param os Output stream
 * @param characteristic The characteristic to output
 * @return Reference to the output stream
 */
std::ostream& operator<<(std::ostream& os, const Characteristic& characteristic);

// ========== Bitwise operators for Characteristic::CCCDProperties ==========

constexpr Characteristic::CCCDProperties operator|(Characteristic::CCCDProperties lhs,
												   Characteristic::CCCDProperties rhs) noexcept {
	return static_cast<Characteristic::CCCDProperties>(static_cast<uint16_t>(lhs) |
													   static_cast<uint16_t>(rhs));
}

constexpr Characteristic::CCCDProperties operator&(Characteristic::CCCDProperties lhs,
												   Characteristic::CCCDProperties rhs) noexcept {
	return static_cast<Characteristic::CCCDProperties>(static_cast<uint16_t>(lhs) &
													   static_cast<uint16_t>(rhs));
}

// ========== Bitwise operators for Characteristic::SCCDProperties ==========

constexpr Characteristic::SCCDProperties operator|(Characteristic::SCCDProperties lhs,
												   Characteristic::SCCDProperties rhs) noexcept {
	return static_cast<Characteristic::SCCDProperties>(static_cast<uint16_t>(lhs) |
													   static_cast<uint16_t>(rhs));
}

constexpr Characteristic::SCCDProperties operator&(Characteristic::SCCDProperties lhs,
												   Characteristic::SCCDProperties rhs) noexcept {
	return static_cast<Characteristic::SCCDProperties>(static_cast<uint16_t>(lhs) &
													   static_cast<uint16_t>(rhs));
}

// ========== Bitwise operators for Characteristic::ExtendedProperties ==========

constexpr Characteristic::ExtendedProperties
operator|(Characteristic::ExtendedProperties lhs, Characteristic::ExtendedProperties rhs) noexcept {
	return static_cast<Characteristic::ExtendedProperties>(static_cast<uint16_t>(lhs) |
														   static_cast<uint16_t>(rhs));
}

constexpr Characteristic::ExtendedProperties
operator&(Characteristic::ExtendedProperties lhs, Characteristic::ExtendedProperties rhs) noexcept {
	return static_cast<Characteristic::ExtendedProperties>(static_cast<uint16_t>(lhs) &
														   static_cast<uint16_t>(rhs));
}

}  // namespace c7222

#endif	// ELEC_C7222_BLE_GATT_CHARACTERISTIC_HPP_
