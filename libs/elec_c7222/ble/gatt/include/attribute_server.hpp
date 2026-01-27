#ifndef ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_
#define ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_

#include <cstdint>
#include <list>

#include "ble_error.hpp"
#include "non_copyable.hpp"
#include "service.hpp"
#include "uuid.hpp"

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
 * \note This implementation supports a single client connection at a time.
 * If more than one client attempts to connect, the server will accept all of them
 * but use only the last connection handle in its responses. So, it is not recommended
 * for use cases requiring multiple concurrent connections.
 *
 * ---
 * ### Responsibilities
 *
 * - **ATT DB parsing:** Platform-specific code converts the ATT database blob
 *   into `Attribute` objects, then `InitServices()` builds `Service` and
 *   `Characteristic` objects in discovery order.
 * - **ATT routing:** Dispatches ATT read/write requests to the correct
 *   `Characteristic` or service-level `Attribute` handlers.
 * - **HCI event fan-out:** Forwards ATT-related HCI events (e.g. indication
 *   completion, can-send-now) to characteristics so they can drive
 *   notifications/indications.
 *
 * ---
 * ### Security Queries (Planning Before Connections)
 *
 * The server now exposes aggregate security checks across all parsed services:
 * - `HasServicesRequiringAuthentication()`
 * - `HasServicesRequiringAuthorization()`
 *
 * These helpers are intended to guide SecurityManager enablement and
 * configuration before connections are established.
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
 * The class itself is platform-agnostic; the BTstack binding and ATT DB parsing
 * live in the platform implementation (`platform/rpi_pico/attribute_server.cpp`).
 *
 * ---
 * ### RPi Pico W (BTstack) Implementation
 *
 * On RP2040/Pico W, `Init()` treats the context pointer as the BTstack
 * ATT DB blob (att_db.h/att_db.c) and:
 * - Registers BTstack callbacks via `att_server_init()`.
 * - Parses the ATT DB into `Attribute` objects and calls `InitServices()`.
 *
 * The context pointer is cached the first time `Init()` is called. Subsequent
 * calls reuse the stored context pointer so the ATT DB remains stable.
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
 * ### Design Assumptions
 *
 * - The platform provides an immutable ATT database blob whose lifetime
 *   exceeds the `AttributeServer` instance lifetime.
 * - Parsed static attributes point into the ATT DB memory and must not outlive it.
 * - Services/characteristics are rebuilt from the ATT DB each time `Init()` runs.
 *
 * ---
 * ### Internal/Reserved APIs (do not call from application code)
 *
 * The following methods exist to integrate with the BLE stack and platform glue.
 * They are not part of the public application API and should be considered
 * reserved for internal use:
 * - `InitServices()` (parses service/characteristic structure)
 * - `ReadAttribute()` / `WriteAttribute()` (ATT callbacks)
 * - `DispatchBleHciPacket()` (HCI event fan-out)
 */
class AttributeServer : public NonCopyableNonMovable {
   public:
	/// \name Types and Configuration
	///@{
	struct ReadResult {
		/// @brief Number of bytes read on success.
		uint16_t bytes = 0;
		/// @brief ATT error to return when ok == false.
		BleError error = BleError::kSuccess;
		/// @brief True when read succeeded and bytes is valid.
		bool ok = true;
	};
	///@}

	/// \name Construction and Lifetime
	///@{
	/**
	 * @brief Get the singleton instance.
	 *
	 * The server is a process-wide singleton because BTstack exposes a single
	 * global ATT server for the device.
	 */
	static AttributeServer* GetInstance() {
		if(instance_ == nullptr) {
			instance_ = new AttributeServer();
			assert(instance_ != nullptr &&
				   "Failed to allocate AttributeServer singleton instance");
		}
		return instance_;
	}
	///@}

	/// \name Initialization and State
	///@{
	/**
	 * @brief Initialize the ATT server from a platform context.
	 *
	 * On the Pico W BLE stack, the context is the BTstack ATT database blob
	 * (att_db.h). Other platforms may treat the context differently.
	 *
	 * When called the first time, the context pointer is cached in the instance.
	 * The Pico W implementation parses the ATT DB and registers BTstack
	 * read/write callbacks. Subsequent calls reuse the cached context.
	 *
	 * RPi Pico (BTstack) steps:
	 * 1. Cache the ATT DB pointer if not already stored.
	 * 2. Parse the ATT DB into `Attribute` objects.
	 * 3. Call `InitServices()` to build services/characteristics.
	 * 4. Register BTstack callbacks via `att_server_init()`.
	 * 5. Mark the server initialized.
	 *
	 * @param context Platform-specific context pointer.
	 * @return BleError::kSuccess on success.
	 */
	BleError Init(const void* context);

	/**
	 * @brief Check whether the server was initialized.
	 */
	[[nodiscard]] bool IsInitialized() const {
		return initialized_;
	}

	/**
	 * @brief Get the stored platform context pointer.
	 *
	 * On Pico W this is the ATT DB blob pointer; other platforms may store
	 * different context data.
	 */
	[[nodiscard]] const void* GetContext() const {
		return context_;
	}

	/**
	 * @brief Check whether a platform context has been stored.
	 */
	[[nodiscard]] bool HasContext() const {
		return context_ != nullptr;
	}
	///@}

	/// \name Service and Characteristic Lookup
	///@{
	/**
	 * @brief Get the number of parsed services.
	 *
	 * Returns the count of services parsed from the ATT DB in discovery order.
	 */
	[[nodiscard]] size_t GetServiceCount() const {
		return services_.size();
	}


	/**
	 * @brief Get a service by index.
	 *
	 * @param index Service index in discovery order.
	 * @return Reference to the service.
	 */
	Service& GetService(size_t index);

	/**
	 * @brief Get the list of services.
	 *
	 * @return Const reference to the service list.
	 */
	[[nodiscard]] const std::list<Service>& GetServices() const {
		return services_;
	}

	/**
	 * @brief Get the list of services.
	 *
	 * @return Reference to the service list.
	 */
	std::list<Service>& GetServices() {
		return services_;
	}
	
	/**
	 * @brief Get a service by index (const version).
	 *
	 * @param index Service index in discovery order.
	 * @return Const reference to the service.
	 */
	[[nodiscard]] const Service& GetService(size_t index) const;

	/**
	 * @brief Find a service by UUID.
	 *
	 * @param uuid Service UUID to search for.
	 * @return Pointer to the service, or nullptr if not found.
	 */
	Service* FindServiceByUuid(const Uuid& uuid);

	/**
	 * @brief Find a service by UUID (const version).
	 *
	 * @param uuid Service UUID to search for.
	 * @return Const pointer to the service, or nullptr if not found.
	 */
	[[nodiscard]] const Service* FindServiceByUuid(const Uuid& uuid) const;

	/**
	 * @brief Check whether any service contains characteristics requiring authentication.
	 *
	 * This aggregates the per-service checks and is intended to guide
	 * SecurityManager configuration before connections are established.
	 */
	[[nodiscard]] bool HasServicesRequiringAuthentication() const;

	/**
	 * @brief Check whether any service contains characteristics requiring authorization.
	 *
	 * Authorization implies authentication and typically requires
	 * application-level approval.
	 */
	[[nodiscard]] bool HasServicesRequiringAuthorization() const;

	/**
	 * @brief Find characteristics by UUID.
	 *
	 * Multiple characteristics can share a UUID (e.g., replicated instances),
	 * so this returns all matches in discovery order.
	 *
	 * @param uuid Characteristic UUID to search for.
	 * @return List of matching characteristics (may be empty).
	 */
	std::list<Characteristic*> FindCharacteristicByUuid(const Uuid& uuid);

	/**
	 * @brief Find characteristics by UUID (const version).
	 *
	 * Multiple characteristics can share a UUID (e.g., replicated instances),
	 * so this returns all matches in discovery order.
	 *
	 * @param uuid Characteristic UUID to search for.
	 * @return List of matching characteristics (may be empty).
	 */
	[[nodiscard]] std::list<const Characteristic*> FindCharacteristicByUuid(const Uuid& uuid) const;

	/**
	 * @brief Find a characteristic by attribute handle.
	 *
	 * @param handle ATT handle to search for (value, declaration, or descriptor).
	 * @return Pointer to the characteristic, or nullptr if not found.
	 */
	Characteristic* FindCharacteristicByHandle(uint16_t handle);

	/**
	 * @brief Find a characteristic by attribute handle (const version).
	 *
	 * @param handle ATT handle to search for (value, declaration, or descriptor).
	 * @return Const pointer to the characteristic, or nullptr if not found.
	 */
	[[nodiscard]] const Characteristic* FindCharacteristicByHandle(uint16_t handle) const;
	///@}

	/// \name Iteration
	///@{
	/**
	 * @brief Get iterator to first service.
	 * @return Iterator to begin of services list
	 */
	auto begin() {
		return services_.begin();
	}

	/**
	 * @brief Get iterator to end of services.
	 * @return Iterator to end of services list
	 */
	auto end() {
		return services_.end();
	}

	/**
	 * @brief Get const iterator to first service.
	 * @return Const iterator to begin of services list
	 */
	[[nodiscard]] auto begin() const {
		return services_.begin();
	}

	/**
	 * @brief Get const iterator to end of services.
	 * @return Const iterator to end of services list
	 */
	[[nodiscard]] auto end() const {
		return services_.end();
	}

	/**
	 * @brief Get const iterator to first service.
	 * @return Const iterator to begin of services list
	 */
	[[nodiscard]] auto cbegin() const {
		return services_.cbegin();
	}

	/**
	 * @brief Get const iterator to end of services.
	 * @return Const iterator to end of services list
	 */
	[[nodiscard]] auto cend() const {
		return services_.cend();
	}
	///@}

	/// \name Connection and Event Routing
	///@{
	/**
	 * @brief Set the active connection handle for all characteristics.
	 *
	 * This value is propagated into each service/characteristic so that
	 * notifications/indications can be sent from `Characteristic::UpdateValue()`.
	 */
	void SetConnectionHandle(uint16_t connection_handle);

	/**
	 * @brief Get the current connection handle.
	 *
	 * Returns 0 when disconnected or not set.
	 */
	[[nodiscard]] uint16_t GetConnectionHandle() const {
		return connection_handle_;
	}

	void SetDisconnected();
	/**
	 * @brief Check whether a connection handle is set.
	 */
	[[nodiscard]] bool HasConnectionHandle() const {
		return connection_handle_ != 0;
	}

	/**
	 * @brief Queries whether a client is connected to the server.
	 *
	 * An attribute server is connected if it has a non-zero connection handle.
	 */
	[[nodiscard]] bool IsConnected() const {
		return connection_handle_ != 0;
	}

	/**
	 * @brief Dispatch HCI ATT events to all characteristics.
	 *
	 * This should be called from the BLE packet handler so that ATT indication
	 * completion and can-send-now flow control are handled by characteristics.
	 */
	BleError DispatchBleHciPacket(uint8_t packet_type,
								  const uint8_t* packet_data,
								  uint16_t packet_data_size);
	///@}

	/// \name ATT Callbacks (Internal Use)
	///@{
	/**
	 * @brief Handle an ATT read request (internal use).
	 *
	 * Used by the platform binding to service BTstack read callbacks.
	 */
	ReadResult ReadAttribute(uint16_t attribute_handle,
							 uint16_t offset,
							 uint8_t* buffer,
							 uint16_t buffer_size);

	/**
	 * @brief Handle an ATT write request (internal use).
	 *
	 * Used by the platform binding to service BTstack write callbacks.
	 */
	BleError
	WriteAttribute(uint16_t attribute_handle, uint16_t offset, const uint8_t* data, uint16_t size);
	///@}

	/// \name Stream Output
	///@{
	/**
	 * @brief Stream insertion operator for attribute server information.
	 *
	 * Outputs summary details including:
	 * - Initialization state
	 * - Service count
	 * - Connection handle
	 */
	friend std::ostream& operator<<(std::ostream& os, const AttributeServer& server);
	///@}

	/// \name Service Parsing Helpers
	///@{
	/**
	 * @brief Initialize services from a parsed attribute list.
	 *
	 * This method consumes the ordered attribute list to construct services
	 * and characteristics in discovery order. The list is modified in place
	 * and may be emptied by the parsing process.
	 *
	 * @param attributes Parsed attributes in DB order.
	 */
	void InitServices(std::list<Attribute>& attributes);
	///@}

   private:
	/// \name Construction and Platform Hooks
	///@{
	AttributeServer() = default;
	~AttributeServer() = default;
	///@}

	/// \name Internal Lookup Helpers
	///@{
	/**
	 * @brief Find a service-level attribute by handle.
	 *
	 * Searches the service declaration and included service declarations.
	 */
	Attribute* FindServiceAttributeByHandle(uint16_t handle);
	/**
	 * @brief Find a service-level attribute by handle (const version).
	 */
	[[nodiscard]] const Attribute* FindServiceAttributeByHandle(uint16_t handle) const;
	/**
	 * @brief Find any attribute by handle across services and characteristics.
	 */
	Attribute* FindAttributeByHandle(uint16_t handle);
	/**
	 * @brief Find any attribute by handle across services and characteristics (const version).
	 */
	[[nodiscard]] const Attribute* FindAttributeByHandle(uint16_t handle) const;

	/**
	 * @brief Check whether a value represents an ATT error code.
	 *
	 * Used to interpret BTstack-style return values from read callbacks.
	 */
	static bool IsAttErrorCode(uint16_t value, BleError& out_error);
	///@}

	/// \name State
	///@{
	/// @brief Parsed GATT services in discovery order.
	std::list<Service> services_;
	/// @brief Platform-specific context pointer (e.g., ATT DB blob on Pico W).
	const void* context_ = nullptr;
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

#endif	// ELEC_C7222_BLE_GATT_ATTRIBUTE_SERVER_HPP_
