#ifndef ELEC_C7222_BLE_GATT_SERVICE_HPP_
#define ELEC_C7222_BLE_GATT_SERVICE_HPP_

#include "characteristic.hpp"
#include "uuid.hpp"

#include <iosfwd>
#include <list>
#include <vector>

namespace c7222 {

/**
 * @class Service
 * @brief Represents a GATT Service with its characteristics and included services.
 *
 * A GATT Service consists of:
 * 1. Service Declaration attribute (mandatory) - defines the service UUID
 * 2. Characteristics (mandatory) - the main data containers within the service
 * 3. Included Services (optional) - references to other services
 *
 * The Service Declaration contains:
 * - 2 or 16 bytes: Service UUID (16-bit or 128-bit)
 *
 * Services are identified by their UUID and organize related characteristics
 * into logical groups. Clients discover services first, then characteristics.
 *
 * ---
 * ### Construction / Parsing
 *
 * There are two common ways to create a Service:
 * 1. **Manual construction:** Use the constructor, then add characteristics
 *    via `AddCharacteristic()` or `CreateCharacteristic()`.
 * 2. **Parse from attributes:** Use `ParseFromAttributes()` to extract services
 *    from an ordered attribute list. The extracted attributes are removed from
 *    the list and moved into the resulting Service/Characteristic instances.
 *    Callbacks are not copied; register callbacks after parsing.
 *
 * ---
 * ### Included Service Model
 *
 * Included services are represented by:
 * - A list of `Service` objects (`included_services_`) for higher-level access.
 * - A list of Included Service Declaration attributes (`included_service_declarations_`)
 *   for the ATT database representation.
 *
 * When adding an included service with `AddIncludedService()`, this class
 * creates a declaration attribute that contains:
 * - Start handle (16-bit LE)
 * - End handle (16-bit LE)
 * - Included service UUID (2 or 16 bytes)
 *
 * The current implementation estimates the end handle based on the included
 * service's declaration handle and characteristic count. If you require exact
 * handle ranges, ensure handles are assigned consistently in the ATT DB.
 *
 * ---
 * ### Connection Handle Propagation
 *
 * `SetConnectionHandle()` stores the connection handle on the service and
 * forwards it to all characteristics and included services. This enables
 * notification/indication delivery via the characteristic update APIs.
 *
 * ---
 * ### Security Queries (Planning Before Connections)
 *
 * Services expose security query helpers that aggregate characteristic
 * requirements across the service:
 * - `HasCharacteristicsRequiringAuthentication()`
 * - `HasCharacteristicsRequiringAuthorization()`
 *
 * These are intended to help higher-level components (e.g., the attribute
 * server or BLE facade) decide whether a SecurityManager must be enabled and
 * how it should be configured before connections are established.
 *
 * ---
 * ### RPi Pico (BTstack) Integration
 *
 * On the Pico W BLE stack, the GATT database is compiled into a binary
 * Attribute DB (typically `att_db.h`/`att_db.c`). The AttributeServer parses
 * that binary blob into `Attribute` objects and then calls
 * `Service::ParseFromAttributes()` to build services in discovery order.
 *
 * Important implications:
 * - Parsed static attributes keep pointers into the DB blob; the DB memory
 *   must remain valid for the lifetime of the `Attribute`/`Service`.
 * - Parsing moves the attribute objects; callbacks are not copied. Install
 *   callbacks after parsing as needed.
 *
 * ---
 * ### Internal/Reserved APIs (do not call from application code)
 *
 * The following methods exist to integrate with the BLE stack and platform glue.
 * They are not part of the public application API and should be considered
 * reserved for internal use:
 * - `ParseFromAttributes()` (ATT DB parsing)
 * - `FindServiceAttributeByHandle()` (server routing helpers)
 *
 * ---
 * ### Typical Usage
 *
 * ```cpp
 * // Manual construction (server-side)
 * c7222::Service svc(c7222::Uuid(0x180F));  // Battery Service
 * auto& level = svc.CreateCharacteristic(c7222::Uuid(0x2A19),
 *                                        static_cast<uint8_t>(c7222::Characteristic::Properties::kRead),
 *                                        0 ); // value handle is 0
 * level.SetValue(&initial_level, sizeof(initial_level));
 * // Parsing from a compiled ATT DB (Pico W)
 * auto* server = c7222::AttributeServer::GetInstance();
 * server->Init(att_db);
 * c7222::Service* parsed = server->FindServiceByUuid(c7222::Uuid(0x180F));
 * ``` 
 * @note Services can be either Primary or Secondary(defined by UUID type).
 * @note A service must contain at least one characteristic to be considered valid.
 */
class Service : public MovableOnly {
   public:
	/// \name Types and Configuration
	///@{
	/**
	 * @brief Service type indicators.
	 * 
	 * Services can be Primary (standalone) or Secondary (included in other services).
	 * The type is determined by the service declaration UUID type in the ATT DB.
	 */
	enum class ServiceType : uint8_t {
		/** @brief Primary Service - top-level service discoverable by clients. */
		kPrimary = 0,
		/** @brief Secondary Service - included in other services, not directly discoverable. */
		kSecondary = 1
	};
	///@}

	/// \name Construction and Parsing
	///@{
	/**
	 * @brief Construct a new Service.
	 * Creates the service with the given UUID and type.
	 * 
	 * @param uuid The UUID identifying this service
	 * @param service_type Type of service (Primary or Secondary)
	 * @param declaration_handle Optional handle for the Declaration attribute (used in DB)
	 */
	explicit Service(const Uuid& uuid, ServiceType service_type = ServiceType::kPrimary,
		uint16_t declaration_handle = 0);

	/**
	 * @brief Construct a Service by moving parsed attributes.
	 *
	 * This constructor takes ownership of the ATT DB attributes and avoids
	 * rebuilding them. The declaration attribute must be a Primary or Secondary
	 * Service Declaration with a 16-bit or 128-bit service UUID value payload.
	 * Handles are preserved from the moved attributes.
	 *
	 * @param declaration_attr Service Declaration attribute (moved)
	 * @param included_service_declarations Included Service Declaration attributes (moved)
	 * @param characteristics Characteristics parsed from the same attribute block (moved)
	 */
	explicit Service(Attribute&& declaration_attr,
					 std::vector<Attribute>&& included_service_declarations,
					 std::list<Characteristic>&& characteristics);

	/**
	 * @brief Move constructor.
	 * Transfers ownership of all internal characteristics and attributes.
	 */
	Service(Service&& other) noexcept = default;

	/**
	 * @brief Move assignment operator.
	 * Transfers ownership of all internal characteristics and attributes.
	 */
	Service& operator=(Service&& other) noexcept = default;

	/**
	 * \brief Deleted default constructor.
	 * A service must have a UUID and type specified.
	 */
	Service() = delete;
	/**
	 * @brief Destructor.
	 * Cleans up all managed characteristics and included services.
	 */
	~Service() override = default;

	/**
	 * @brief Parse Services from an ordered attribute list.
	 *
	 * This function scans for Service Declaration attributes, then consumes
	 * each service block until the next declaration (or end of list). It parses
	 * characteristics one-by-one using Characteristic::ParseFromAttributes.
	 *
	 * @param attributes Attribute list to consume (modified in place)
	 * @return Parsed Services in discovery order
	 *
	 * @note Parsed attributes are moved into each Service instance
	 * @note Expects a well-formed, ordered ATT DB; characteristic parsing is asserted
	 */
	static std::list<Service> ParseFromAttributes(std::list<Attribute>& attributes);
	///@}

	/// \name Accessors and Lookup
	///@{

	/**
	 * @brief Get the UUID of this service.
	 * @return Reference to the service's UUID
	 */
	[[nodiscard]] const Uuid& GetUuid() const { return uuid_; }

	/**
	 * @brief Get the service type (Primary or Secondary).
	 * @return ServiceType of this service
	 */
	[[nodiscard]] ServiceType GetServiceType() const { return service_type_; }

	/**
	 * @brief Get the handle of the Declaration attribute.
	 * @return Declaration attribute handle
	 */
	[[nodiscard]] uint16_t GetDeclarationHandle() const { return declaration_attr_.GetHandle(); }

	/**
	 * @brief Get the number of characteristics in this service.
	 * @return Count of characteristics
	 */
	[[nodiscard]] size_t GetCharacteristicCount() const { return characteristics_.size(); }

	/**
	 * @brief Get the list of characteristics in this service.
	 * @return Reference to the characteristic list
	 */
	[[nodiscard]] const std::list<Characteristic>& GetCharacteristics() const {
		return characteristics_;
	}
	/**
	 * \brief Get the Characteristics object
	 * 
	 * \return std::list<Characteristic>& 
	 */
	std::list<Characteristic>& GetCharacteristics() {
		return characteristics_;
	}

	/**
	 * @brief Get a characteristic by index.
	 * @param index Index of the characteristic (0 to GetCharacteristicCount()-1)
	 * @return Reference to the characteristic, throws std::out_of_range if invalid
	 */
	Characteristic& GetCharacteristic(size_t index);

	/**
	 * @brief Get a characteristic by index (const version).
	 * @param index Index of the characteristic (0 to GetCharacteristicCount()-1)
	 * @return Const reference to the characteristic, throws std::out_of_range if invalid
	 */
	[[nodiscard]] const Characteristic& GetCharacteristic(size_t index) const;

	/**
	 * @brief Get a characteristic by UUID.
	 * @param uuid UUID to search for
	 * @return Pointer to the characteristic if found, nullptr otherwise
	 */
	Characteristic* FindCharacteristicByUuid(const Uuid& uuid);

	/**
	 * @brief Find characteristics that include all specified properties.
	 *
	 * @param properties Bitfield of properties to match (logical AND filter)
	 * @return List of matching characteristic pointers (may be empty)
	 */
	[[nodiscard]] std::list<Characteristic*> FindCharacteristicsByProperties(Characteristic::Properties properties) const;

	/**
	 * @brief Find characteristics that use dynamically assigned value handles.
	 *
	 * Dynamic characteristics have a value handle of 0 until the stack assigns one.
	 * @return List of characteristics with dynamic handles (may be empty)
	 */
	[[nodiscard]] std::list<Characteristic*> FindCharacteristicsDynamic() const;

	///@}

	/// \name Security Queries
	///@{
	/**
	 * @brief Check whether any characteristic requires authentication.
	 *
	 * This is useful during service construction/registration to determine
	 * whether the service contains security-sensitive characteristics.
	 */
	[[nodiscard]] bool HasCharacteristicsRequiringAuthentication() const;

	/**
	 * @brief Check whether any characteristic requires authorization.
	 *
	 * Authorization implies authentication and typically requires an
	 * application-level decision.
	 */
	[[nodiscard]] bool HasCharacteristicsRequiringAuthorization() const;
	///@}

	/// \name Convenience Queries
	///@{
	/**
	 * @brief Find characteristics that are writable.
	 *
	 * Matches characteristics advertising the Write property.
	 * @return List of writable characteristic pointers (may be empty)
	 */
	[[nodiscard]] std::list<Characteristic*> FindCharacteristicsWritable() const {
		return FindCharacteristicsByProperties(Characteristic::Properties::kWrite);
	}

	/**
	 * @brief Find characteristics that are readable.
	 *
	 * Matches characteristics advertising the Read property.
	 * @return List of readable characteristic pointers (may be empty)
	 */
	[[nodiscard]] std::list<Characteristic*> FindCharacteristicsReadable() const {
		return FindCharacteristicsByProperties(Characteristic::Properties::kRead);
	}

	/**
	 * @brief Find characteristics that can notify or indicate.
	 *
	 * Matches characteristics with Notify and/or Indicate properties set.
	 * @return List of notifiable or indicatable characteristic pointers (may be empty)
	 */
	[[nodiscard]] std::list<Characteristic*> FindCharacteristicsNotifiableOrIndicatable() const {
		return FindCharacteristicsByProperties(Characteristic::Properties::kNotify | Characteristic::Properties::kIndicate);
	}

	/**
	 * @brief Get a characteristic by UUID (const version).
	 * @param uuid UUID to search for
	 * @return Const pointer to the characteristic if found, nullptr otherwise
	 */
	[[nodiscard]] const Characteristic* FindCharacteristicByUuid(const Uuid& uuid) const;

	/**
	 * @brief Get a characteristic by handle.
	 * @param handle Attribute handle (declaration or value)
	 * @return Pointer to the characteristic if found, nullptr otherwise
	 */
	Characteristic* FindCharacteristicByHandle(uint16_t handle);

	/**
	 * @brief Get a characteristic by handle (const version).
	 * @param handle Attribute handle (declaration or value)
	 * @return Const pointer to the characteristic if found, nullptr otherwise
	 */
	[[nodiscard]] const Characteristic* FindCharacteristicByHandle(uint16_t handle) const;

	/**
	 * @brief Find a service declaration or included service declaration by handle.
	 * @param handle Attribute handle to search for
	 * @return Pointer to the attribute if found, nullptr otherwise
	 */
	Attribute* FindServiceAttributeByHandle(uint16_t handle);

	/**
	 * @brief Find a service declaration or included service declaration by handle (const version).
	 * @param handle Attribute handle to search for
	 * @return Const pointer to the attribute if found, nullptr otherwise
	 */
	[[nodiscard]] const Attribute* FindServiceAttributeByHandle(uint16_t handle) const;
	///@}

	/// \name Additional Lookup and State
	///@{
	/**
	 * @brief Get the number of included services.
	 * @return Count of included services
	 */
	[[nodiscard]] size_t GetIncludedServiceCount() const { return included_services_.size(); }

	/**
	 * @brief Get an included service by index.
	 * @param index Index of the included service
	 * @return Reference to the included service, throws std::out_of_range if invalid
	 */
	Service& GetIncludedService(size_t index);

	/**
	 * @brief Get an included service by index (const version).
	 * @param index Index of the included service
	 * @return Const reference to the included service, throws std::out_of_range if invalid
	 */
	[[nodiscard]] const Service& GetIncludedService(size_t index) const;

	/**
	 * @brief Get the number of included service declaration attributes.
	 * @return Count of included service declarations
	 */
	[[nodiscard]] size_t GetIncludedServiceDeclarationCount() const { return included_service_declarations_.size(); }

	/**
	 * @brief Check if this service is valid.
	 * @return true if UUID is valid, handles are non-zero, and has at least one characteristic
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Check if this service uses 128-bit UUID.
	 * @return true if service UUID is 128-bit
	 */
	[[nodiscard]] bool Uses128BitUuid() const { return uuid_.Is128Bit(); }

	/**
	 * @brief Set the connection handle for all characteristics in this service.
	 *
	 * @param connection_handle The connection handle (0 is invalid/disconnected)
	 */
	void SetConnectionHandle(uint16_t connection_handle);

	/**
	 * @brief Get the current connection handle for this service.
	 * @return The connection handle, or 0 if disconnected/invalid
	 */
	[[nodiscard]] uint16_t GetConnectionHandle() const { return connection_handle_; }
	///@}

	/// \name Characteristic Management
	///@{

	/**
	 * @brief Add a characteristic to this service.
	 * 
	 * Adds an existing characteristic to the service. The characteristic
	 * is moved into the service, transferring ownership.
	 * 
	 * @param characteristic The characteristic to add (will be moved)
	 * @return Reference to the added characteristic
	 */
	Characteristic& AddCharacteristic(Characteristic&& characteristic);

	/**
	 * @brief Create and add a new characteristic to this service.
	 *
	 * Creates a new characteristic with the given parameters and adds it to the service.
	 * The caller is responsible for assigning handles if the ATT DB requires fixed
	 * handle values; otherwise handles may remain 0 until assigned by the stack.
	 * 
	 * @param uuid UUID for the characteristic
	 * @param properties Characteristic properties bitfield
	 * @param value_handle Handle of the Value attribute
	 * @param declaration_handle Optional handle for the Declaration attribute
	 * @return Reference to the newly added characteristic
	 */
	Characteristic& CreateCharacteristic(const Uuid& uuid, uint8_t properties,
		uint16_t value_handle, uint16_t declaration_handle = 0);

	/**
	 * @brief Remove a characteristic by index.
	 * 
	 * @param index Index of the characteristic to remove
	 * @return true if removed successfully, false if index out of range
	 */
	bool RemoveCharacteristic(size_t index);

	/**
	 * @brief Remove all characteristics from this service.
	 */
	void ClearCharacteristics();
	///@}

	/// \name Included Service Management
	///@{

	/**
	 * @brief Add an included service reference.
	 *
	 * Adds a reference to another service that is included in this service.
	 * This creates an Included Service Declaration attribute in the ATT DB.
	 * The declaration stores the included service start/end handles and UUID.
	 * The end handle is estimated from the included service metadata unless
	 * handles are explicitly assigned in the DB.
	 * 
	 * @param included_service Reference to the service to include
	 * @param declaration_handle Optional handle for the Included Service Declaration
	 */
	void AddIncludedService(const Service& included_service, uint16_t declaration_handle = 0);

	/**
	 * @brief Remove an included service by index.
	 * 
	 * @param index Index of the included service to remove
	 * @return true if removed successfully, false if index out of range
	 */
	bool RemoveIncludedService(size_t index);

	/**
	 * @brief Remove all included services.
	 */
	void ClearIncludedServices();
	///@}

	/// \name Iteration
	///@{

	/**
	 * @brief Get iterator to first characteristic.
	 * @return Iterator to begin of characteristics list
	 */
	auto begin() { return characteristics_.begin(); }

	/**
	 * @brief Get iterator to end of characteristics.
	 * @return Iterator to end of characteristics list
	 */
	auto end() { return characteristics_.end(); }

	/**
	 * @brief Get const iterator to first characteristic.
	 * @return Const iterator to begin of characteristics list
	 */
	[[nodiscard]] auto begin() const { return characteristics_.begin(); }

	/**
	 * @brief Get const iterator to end of characteristics.
	 * @return Const iterator to end of characteristics list
	 */
	[[nodiscard]] auto end() const { return characteristics_.end(); }

	/**
	 * @brief Get const iterator to first characteristic.
	 * @return Const iterator to begin of characteristics list
	 */
	[[nodiscard]] auto cbegin() const { return characteristics_.cbegin(); }

	/**
	 * @brief Get const iterator to end of characteristics.
	 * @return Const iterator to end of characteristics list
	 */
	[[nodiscard]] auto cend() const { return characteristics_.cend(); }
	///@}

	/// \name Stream Output
	///@{

	/**
	 * @brief Stream insertion operator for service information.
	 * 
	 * Outputs service details including:
	 * - Service UUID
	 * - Service type (Primary/Secondary)
	 * - Declaration handle
	 * - Count of characteristics and included services
	 * 
	 * @param os Output stream
	 * @param service Service to output
	 * @return Reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const Service& service);
	///@}

private:
	/**
	 * @brief Service UUID identifying this service.
	 *
	 * This is the 16-bit or 128-bit UUID that appears in the Service Declaration
	 * attribute and is used for service discovery by clients.
	 */
	Uuid uuid_;

	/**
	 * @brief Type of this service (Primary or Secondary).
	 *
	 * Primary services are discoverable by clients. Secondary services are only
	 * reachable via inclusion in other services.
	 */
	ServiceType service_type_;

	/**
	 * @brief Service Declaration attribute.
	 *
	 * Holds the declaration UUID and service UUID value. For parsed services, this
	 * attribute points into the ATT DB blob and should be treated as immutable.
	 */
	Attribute declaration_attr_;

	/**
	 * @brief Last known connection handle for this service.
	 *
	 * Propagated to all characteristics via SetConnectionHandle() so notifications
	 * and indications can be sent when values update.
	 */
	uint16_t connection_handle_ = 0;

	/**
	 * @brief Collection of characteristics in this service.
	 *
	 * The list preserves discovery order without requiring contiguous storage.
	 * Characteristics are move-only and owned by the service.
	 */
	std::list<Characteristic> characteristics_;

	/**
	 * @brief Collection of included services.
	 *
	 * These are full Service objects for higher-level access and handle propagation.
	 * They are independent of the Included Service Declaration attributes.
	 */
	std::list<Service> included_services_;

	/**
	 * @brief Included Service Declaration attributes.
	 * Each declaration describes an included service reference.
	 *
	 * These attributes represent the ATT DB view (start/end handle + UUID). They
	 * are kept separate from the `included_services_` objects to match the ATT
	 * database encoding.
	 */
	std::vector<Attribute> included_service_declarations_;

};

}  // namespace c7222

#endif  // ELEC_C7222_BLE_GATT_SERVICE_HPP_
