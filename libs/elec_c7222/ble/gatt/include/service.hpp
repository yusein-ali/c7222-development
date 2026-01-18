#ifndef ELEC_C7222_BLE_GATT_SERVICE_HPP_
#define ELEC_C7222_BLE_GATT_SERVICE_HPP_

#include "characteristic.hpp"
#include "uuid.hpp"
#include "ble_error.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <list>
#include <memory>
#include <string>
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
 * @note Services can be either Primary or Secondary (defined by UUID type)
 * @note A service must contain at least one characteristic
 */
class Service : public MovableOnly{
public:
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
	~Service() = default;

	/**
	 * @brief Parse Services from an ordered attribute list.
	 *
	 * This function scans for Service Declaration attributes, then consumes
	 * each service block until the next declaration (or end of list). It parses
	 * characteristics one-by-one using Characteristic::ParseFromAttributes.
	 *
	 * @param attributes Attribute list to consume (modified in place)
	 * @return Parsed Services in discovery order
	 */
	static std::list<Service> ParseFromAttributes(std::list<Attribute>& attributes);

	// ========== Accessors ==========

	/**
	 * @brief Get the UUID of this service.
	 * @return Reference to the service's UUID
	 */
	const Uuid& GetUuid() const { return uuid_; }

	/**
	 * @brief Get the service type (Primary or Secondary).
	 * @return ServiceType of this service
	 */
	ServiceType GetServiceType() const { return service_type_; }

	/**
	 * @brief Get the handle of the Declaration attribute.
	 * @return Declaration attribute handle
	 */
	uint16_t GetDeclarationHandle() const { return declaration_attr_.GetHandle(); }

	/**
	 * @brief Get the number of characteristics in this service.
	 * @return Count of characteristics
	 */
	size_t GetCharacteristicCount() const { return characteristics_.size(); }

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
	const Characteristic& GetCharacteristic(size_t index) const;

	/**
	 * @brief Get a characteristic by UUID.
	 * @param uuid UUID to search for
	 * @return Pointer to the characteristic if found, nullptr otherwise
	 */
	Characteristic* FindCharacteristicByUuid(const Uuid& uuid);

	/**
	 * @brief Get a characteristic by UUID (const version).
	 * @param uuid UUID to search for
	 * @return Const pointer to the characteristic if found, nullptr otherwise
	 */
	const Characteristic* FindCharacteristicByUuid(const Uuid& uuid) const;

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
	const Characteristic* FindCharacteristicByHandle(uint16_t handle) const;

	/**
	 * @brief Get the number of included services.
	 * @return Count of included services
	 */
	size_t GetIncludedServiceCount() const { return included_services_.size(); }

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
	const Service& GetIncludedService(size_t index) const;

	/**
	 * @brief Get the number of included service declaration attributes.
	 * @return Count of included service declarations
	 */
	size_t GetIncludedServiceDeclarationCount() const { return included_service_declarations_.size(); }

	/**
	 * @brief Check if this service is valid.
	 * @return true if UUID is valid, handles are non-zero, and has at least one characteristic
	 */
	bool IsValid() const;

	/**
	 * @brief Check if this service uses 128-bit UUID.
	 * @return true if service UUID is 128-bit
	 */
	bool Uses128BitUuid() const { return uuid_.Is128Bit(); }

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
	uint16_t GetConnectionHandle() const { return connection_handle_; }

	// ========== Characteristic Management ==========

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

	// ========== Included Service Management ==========

	/**
	 * @brief Add an included service reference.
	 * 
	 * Adds a reference to another service that is included in this service.
	 * This creates an Included Service Declaration attribute in the ATT DB.
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

	// ========== Iteration ==========

	/**
	 * @brief Get iterator to first characteristic.
	 * @return Iterator to begin of characteristics vector
	 */
	auto begin() { return characteristics_.begin(); }

	/**
	 * @brief Get iterator to end of characteristics.
	 * @return Iterator to end of characteristics vector
	 */
	auto end() { return characteristics_.end(); }

	/**
	 * @brief Get const iterator to first characteristic.
	 * @return Const iterator to begin of characteristics vector
	 */
	auto begin() const { return characteristics_.begin(); }

	/**
	 * @brief Get const iterator to end of characteristics.
	 * @return Const iterator to end of characteristics vector
	 */
	auto end() const { return characteristics_.end(); }

	/**
	 * @brief Get const iterator to first characteristic.
	 * @return Const iterator to begin of characteristics vector
	 */
	auto cbegin() const { return characteristics_.cbegin(); }

	/**
	 * @brief Get const iterator to end of characteristics.
	 * @return Const iterator to end of characteristics vector
	 */
	auto cend() const { return characteristics_.cend(); }

	// ========== Stream Output ==========

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

private:
	/**
	 * @brief Service UUID identifying this service.
	 */
	Uuid uuid_;

	/**
	 * @brief Type of this service (Primary or Secondary).
	 */
	ServiceType service_type_;

	/**
	 * @brief Service Declaration attribute.
	 */
	Attribute declaration_attr_;

	/**
	 * @brief Last known connection handle for this service.
	 */
	uint16_t connection_handle_ = 0;

	/**
	 * @brief Collection of characteristics in this service.
	 */
	std::vector<Characteristic> characteristics_;

	/**
	 * @brief Collection of included services.
	 */
	std::vector<Service> included_services_;

	/**
	 * @brief Included Service Declaration attributes.
	 * Each declaration describes an included service reference.
	 */
	std::vector<Attribute> included_service_declarations_;

};

}  // namespace c7222

#endif  // ELEC_C7222_BLE_GATT_SERVICE_HPP_
