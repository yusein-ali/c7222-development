#ifndef ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_
#define ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_

#include <cstddef>
#include <cstdint>
#include <list>

#include "ble_error.hpp"
#include "non_copyable.hpp"
#include "service.hpp"

namespace c7222 {

/**
 * @class AttributeServer
 * @brief Encapsulates the ATT attribute server for the Pico W BLE stack.
 *
 * This class parses the compiled ATT database into GATT Service/Characteristic
 * objects and routes ATT read/write requests to the appropriate handlers.
 * It also forwards HCI ATT events to characteristics for indications and
 * flow-control callbacks.
 *
 * ---
 * ### Responsibilities
 *
 * - **ATT DB parsing:** Converts the BTstack ATT database blob into `Attribute`
 *   objects, then builds `Service` and `Characteristic` objects in discovery order.
 * - **ATT routing:** Dispatches ATT read/write requests to the correct
 *   `Characteristic` or service-level `Attribute` handlers.
 * - **HCI event fan-out:** Forwards ATT-related HCI events (e.g. indication
 *   completion, can-send-now) to characteristics so they can drive
 *   notifications/indications.
 *
 * ---
 * ### BTstack Integration Details
 *
 * BTstack exposes the ATT server through a C API:
 * - `att_server_init(att_db, read_cb, write_cb)` registers the database and
 *   callbacks used when a client reads/writes an attribute.
 * - Read callbacks return a byte count on success or an ATT error code.
 * - Write callbacks return 0 on success or an ATT error code.
 *
 * This class wraps that model as follows:
 * - **Read:** `ReadAttribute()` locates the target `Attribute`/`Characteristic`,
 *   checks permissions, then calls `Characteristic::HandleAttributeRead()` or
 *   `Attribute::InvokeReadCallback()`. ATT error codes are mapped through
 *   `BleError` and returned to BTstack via the platform glue.
 * - **Write:** `WriteAttribute()` routes writes to
 *   `Characteristic::HandleAttributeWrite()` or `Attribute::InvokeWriteCallback()`,
 *   enforcing ATT write permissions. Errors are mapped back to BTstack.
 * - **Events:** `DispatchBleHciPacket()` fans out HCI events to all
 *   characteristics so indication completion and `ATT_EVENT_CAN_SEND_NOW`
 *   are handled in one place.
 *
 * The class itself is platform-agnostic; the BTstack binding lives in the
 * platform implementation (`platform/rpi_pico/attribute_server.cpp`).
 *
 * ---
 * ### RPi Pico W (BTstack) Implementation
 *
 * On RP2040/Pico W, `Init()` calls the platform hook which:
 * - Registers BTstack callbacks via `att_server_init()`.
 * - Uses the BTstack ATT DB (typically `att_db.h`/`att_db.c`) as the source
 *   of truth for attribute layout and static values.
 *
 * The ATT DB blob must remain valid for the lifetime of the server because
 * parsed static attributes point directly into the DB memory.
 *
 * ---
 * ### Typical Usage
 *
 * ```cpp
 * auto* server = c7222::AttributeServer::GetInstance();
 * server->Init(att_db);  // att_db from BTstack-generated files
 *
 * // Route connection handle for notifications/indications
 * server->SetConnectionHandle(connection_handle);
 *
 * // Look up a characteristic and install handlers
 * auto* ch = server->FindCharacteristicByUuid(c7222::Uuid(0x2A19));
 * if(ch != nullptr) {
 *     ch->AddEventHandler(my_handler);
 * }
 * ```
 *
 * ---
 * ### Internal/Reserved APIs (do not call from application code)
 *
 * The following methods exist to integrate with the BLE stack and platform glue.
 * They are not part of the public application API and should be considered
 * reserved for internal use:
 * - `ReadAttribute()` / `WriteAttribute()` (ATT callbacks)
 * - `DispatchBleHciPacket()` (HCI event fan-out)
 */
class AttributeServer : public NonCopyableNonMovable {
   public:
	/// \name Types and Configuration
	///@{
	struct ReadResult {
		uint16_t bytes = 0;
		BleError error = BleError::kSuccess;
		bool ok = true;
	};
	///@}

	/// \name Construction and Lifetime
	///@{
	/**
	 * @brief Get the singleton instance.
	 */
	static AttributeServer* GetInstance() {
		if(instance_ == nullptr) {
			instance_ = new AttributeServer();
		}
		return instance_;
	}
	///@}

	/// \name Initialization and State
	///@{
	/**
	 * @brief Initialize the ATT server from a compiled ATT DB.
	 *
	 * @param att_db Pointer to BTstack ATT database blob (att_db.h).
	 * @return BleError::kSuccess on success.
	 */
	BleError Init(const uint8_t* att_db);

	/**
	 * @brief Check whether the server was initialized.
	 */
	bool IsInitialized() const { return initialized_; }
	///@}

	/// \name Service and Characteristic Lookup
	///@{
	/**
	 * @brief Get the number of parsed services.
	 */
	size_t GetServiceCount() const { return services_.size(); }

	/**
	 * @brief Get a service by index.
	 */
	Service& GetService(size_t index);

	/**
	 * @brief Get a service by index (const version).
	 */
	const Service& GetService(size_t index) const;

	/**
	 * @brief Find a service by UUID.
	 */
	Service* FindServiceByUuid(const Uuid& uuid);

	/**
	 * @brief Find a service by UUID (const version).
	 */
	const Service* FindServiceByUuid(const Uuid& uuid) const;

	/**
	 * @brief Find a characteristic by UUID.
	 */
	Characteristic* FindCharacteristicByUuid(const Uuid& uuid);

	/**
	 * @brief Find a characteristic by UUID (const version).
	 */
	const Characteristic* FindCharacteristicByUuid(const Uuid& uuid) const;

	/**
	 * @brief Find a characteristic by attribute handle.
	 */
	Characteristic* FindCharacteristicByHandle(uint16_t handle);

	/**
	 * @brief Find a characteristic by attribute handle (const version).
	 */
	const Characteristic* FindCharacteristicByHandle(uint16_t handle) const;
	///@}

	/// \name Connection and Event Routing
	///@{
	/**
	 * @brief Set the active connection handle for all characteristics.
	 */
	void SetConnectionHandle(uint16_t connection_handle);

	/**
	 * @brief Get the current connection handle.
	 */
	uint16_t GetConnectionHandle() const { return connection_handle_; }

	/**
	 * @brief Dispatch HCI ATT events to all characteristics.
	 */
	BleError DispatchBleHciPacket(uint8_t packet_type,
								  const uint8_t* packet_data,
								  uint16_t packet_data_size);
	///@}

	/// \name ATT Callbacks (Internal Use)
	///@{
	/**
	 * @brief Handle an ATT read request (internal use).
	 */
	ReadResult ReadAttribute(uint16_t attribute_handle,
							 uint16_t offset,
							 uint8_t* buffer,
							 uint16_t buffer_size);

	/**
	 * @brief Handle an ATT write request (internal use).
	 */
	BleError WriteAttribute(uint16_t attribute_handle,
							uint16_t offset,
							const uint8_t* data,
							uint16_t size);
	///@}

   private:
	/// \name Construction and Platform Hooks
	///@{
	AttributeServer() = default;
	~AttributeServer() = default;

	BleError InitPlatform(const uint8_t* att_db);
	void ParseDatabase(const uint8_t* att_db);
	///@}

	/// \name Internal Lookup Helpers
	///@{
	Attribute* FindServiceAttributeByHandle(uint16_t handle);
	const Attribute* FindServiceAttributeByHandle(uint16_t handle) const;
	Attribute* FindAttributeByHandle(uint16_t handle);
	const Attribute* FindAttributeByHandle(uint16_t handle) const;

	static bool IsAttErrorCode(uint16_t value, BleError& out_error);
	///@}

	/// \name State
	///@{
	/// @brief Parsed GATT services in discovery order.
	std::list<Service> services_{};
	/// @brief Active connection handle (0 when disconnected).
	uint16_t connection_handle_ = 0;
	/// @brief True after Init() successfully parsed and bound the ATT DB.
	bool initialized_ = false;
	///@}

	/// \name Singleton Storage
	///@{
	/// @brief Singleton instance pointer.
	static AttributeServer* instance_;
	///@}
};

}  // namespace c7222

#endif  // ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_
