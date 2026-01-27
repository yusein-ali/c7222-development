
# GATT Client API {#sec:gatt_client_api}


**src/ble/gatt_client.h** 


    
    typedef struct {
        uint16_t start_group_handle;
        uint16_t end_group_handle;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_service_t;
    
    typedef struct {
        uint16_t start_handle;
        uint16_t value_handle;
        uint16_t end_handle;
        uint16_t properties;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_characteristic_t;
    
    typedef struct {
        uint16_t handle;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_characteristic_descriptor_t;
    
    /** 
     * @brief Set up GATT client.
     */
    void gatt_client_init(void);
    
    /**
     * @brief Set minimum required security level for GATT Client
     * @note  The Bluetooth specification makes the GATT Server responsible to check for security.
     *        This allows an attacker to spoof an existing device with a GATT Servers, but skip the authentication part.
     *        If your application is exchanging sensitive data with a remote device, you would need to manually check
     *        the security level before sending/receive such data.
     *        With level > 0, the GATT Client triggers authentication for all GATT Requests and defers any exchange
     *        until the required security level is established.
     *        gatt_client_request_can_write_without_response_event does not trigger authentication
     *        gatt_client_request_to_write_without_response does not trigger authentication
     *  @pram level, default LEVEL_0 (no encryption required)
     */
    void gatt_client_set_required_security_level(gap_security_level_t level);
    
    /**
     * @brief Connect to remote GATT Server over Classic (BR/EDR) Connection
     *        GATT_EVENT_CONNECTED with status and con_handle for other API functions
     *        is emitted on connection complete.
     * @note requires ENABLE_GATT_OVER_CLASSIC.
     * @param addr
     * @return status
     */
    uint8_t gatt_client_classic_connect(btstack_packet_handler_t callback, bd_addr_t addr);
    
    /**
     * @brief Disconnect o Classic (BR/EDR) connection to a remote GATT Server
     * @note requires ENABLE_GATT_OVER_CLASSIC
     * @param con_handle
     * @return status
     */
    uint8_t gatt_client_classic_disconnect(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Setup Enhanced LE Bearer with up to 5 channels on existing LE connection
     * @param callback for GATT_EVENT_CONNECTED and GATT_EVENT_DISCONNECTED events
     * @param con_handle
     * @param num_channels
     * @param storage_buffer for L2CAP connection
     * @param storage_size - each channel requires (2 * ATT MTU) + 10 bytes
     * @return
     */
    uint8_t gatt_client_le_enhanced_connect(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint8_t num_channels, uint8_t * storage_buffer, uint16_t storage_size);
    
    /**
     * @brief MTU is available after the first query has completed. If status is equal to ERROR_CODE_SUCCESS, it returns the real value, 
     * otherwise the default value ATT_DEFAULT_MTU (see bluetooth.h). 
     * @param  con_handle   
     * @param  mtu or 0 in case of error
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if MTU is not exchanged and MTU auto-exchange is disabled
     *                ERROR_CODE_SUCCESS                                        on success
     */
    uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu);
    
    /**
     * @brief Sets whether a MTU Exchange Request shall be automatically send before the 
     * first attribute read request is send. Default is enabled.
     * @param enabled   
     */
    void gatt_client_mtu_enable_auto_negotiation(uint8_t enabled);
    
    /**
     * @brief Sends a MTU Exchange Request, this allows for the client to exchange MTU 
     * when gatt_client_mtu_enable_auto_negotiation is disabled.
     * @param  callback   
     * @param  con_handle
     */
    void gatt_client_send_mtu_negotiation(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Returns 1 if the GATT client is ready to receive a query. It is used with daemon. 
     * @param  con_handle
     * @return is_ready_status     0 - if no GATT client for con_handle is found, or is not ready, otherwise 1
     */
    int gatt_client_is_ready(hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers all primary services. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_primary_services(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers all secondary services. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_secondary_services(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param callback   
     * @param con_handle
     * @param uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times.
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param callback
     * @param con_handle
     * @param uuid16
     * @param service_id    - context provided to callback in events
     * @param connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_primary_services_by_uuid16_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                         uint16_t uuid16, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param callback   
     * @param con_handle
     * @param uuid128
     * @param service_id    - context provided to callback in events
     * @param connection_id - context provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid128_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                          const uint8_t * uuid128, uint16_t service_id, uint16_t connection_id);
    
    /** 
     * @brief Finds included services within the specified service. 
     * For each found included service a GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT event will be emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * Information about included service type (primary/secondary) can be retrieved either by sending 
     * an ATT find information request for the returned start group handle 
     * (returning the handle and the UUID for primary or secondary service) or by comparing the service 
     * to the list of all primary services. 
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_find_included_services_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service);
    
    /**
     * @brief Finds included services within the specified service.
     * For each found included service a GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * Information about included service type (primary/secondary) can be retrieved either by sending
     * an ATT find information request for the returned start group handle
     * (returning the handle and the UUID for primary or secondary service) or by comparing the service
     * to the list of all primary services.
     * @param  callback
     * @param  con_handle
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @param  service_id
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_find_included_services_for_service_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                        gatt_client_service_t * service, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Discovers all characteristics within the specified service. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will be emited. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service);
    
    /**
     * @brief Discovers all characteristics within the specified service.
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will be emited.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback
     * @param  con_handle
     * @param  service
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_characteristics_for_service_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service,
                                                                          uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief The following four functions are used to discover all characteristics within 
     * the specified service or handle range, and return those that match the given UUID. 
     * 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service, uint16_t uuid16);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service, const uint8_t * uuid128);
    
    /** 
     * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. 
     * For each found descriptor a GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT event will be emitted.
     * 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    
    /**
     * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic.
     * For each found descriptor a GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT event will be emitted.
     *
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_characteristic_descriptors_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                         gatt_client_characteristic_t * characteristic, uint16_t service_id, uint16_t connection_it);
    
    /** 
     * @brief Reads the characteristic value using the characteristic's value handle. 
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /** 
     * @brief Reads the characteristic value using the characteristic's value handle. 
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle);
    
    /**
     * @brief Reads the characteristic value using the characteristic's value handle.
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_value_of_characteristic_using_value_handle_with_context(btstack_packet_handler_t callback,
                                                                                     hci_con_handle_t con_handle,
                                                                                     uint16_t value_handle,
                                                                                     uint16_t service_id,
                                                                                     uint16_t connection_id);
    
    /**
     * @brief Reads the characteric value of all characteristics with the uuid. 
     * For each characteristic value found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
    
    /**
     * @brief Reads the characteric value of all characteristics with the uuid. 
     * For each characteristic value found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle);
    
    /**
     * @brief Reads the long characteristic value using the characteristic's value handle.
     * The value will be returned in several blobs.
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_context(btstack_packet_handler_t callback,
                                                                                          hci_con_handle_t con_handle, uint16_t value_handle,
                                                                                          uint16_t service_id, uint16_t connection_id);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  offset 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t offset);
    
    /*
     * @brief Read multiple characteristic values.
     * The all results are emitted via single GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event, 
     * followed by the GATT_EVENT_QUERY_COMPLETE event, which marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  num_value_handles 
     * @param  value_handles list of handles 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
    
    /*
     * @brief Read multiple varaible characteristic values. Only supported over LE Enhanced Bearer
     * The all results are emitted via single GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event,
     * followed by the GATT_EVENT_QUERY_COMPLETE event, which marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  num_value_handles
     * @param  value_handles list of handles
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_multiple_variable_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle without 
     * an acknowledgment that the write was successfully performed.
     * @param  con_handle   
     * @param  value_handle
     * @param  value_length
     * @param  value is copied on success and does not need to be retained
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                BTSTACK_ACL_BUFFERS_FULL   , if L2CAP cannot send, there are no free ACL slots
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the authenticated characteristic value using the characteristic's value handle 
     * without an acknowledgment that the write was successfully performed.
     * @note GATT_EVENT_QUERY_COMPLETE is emitted with ATT_ERROR_SUCCESS for success,
     * or ATT_ERROR_BONDING_INFORMATION_MISSING if there is no bonding information stored.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  message_len
     * @param  message is not copied, make sure memory is accessible until write is done
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                      uint16_t message_len, uint8_t * message);
    
    /** 
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed, if the event's att_status field is set to 
     * ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic value using the characteristic's value handle.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed, if the event's att_status field is set to
     * ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_value_of_characteristic_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                                   uint16_t value_length, uint8_t * value, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                                        uint16_t value_length, uint8_t * value, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes of the long characteristic value using the characteristic's value handle. 
     * It uses server response to validate that the write was correctly received. 
     * The GATT_EVENT_QUERY_COMPLETE EVENT marks the end of write.
     * The write is successfully performed, if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes). 
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Reads the characteristic descriptor using its handle. 
     * If the characteristic descriptor is found, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor);
    
    /** 
     * @brief Reads the characteristic descriptor using its handle. 
     * If the characteristic descriptor is found, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  offset
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the client characteristic configuration of the specified characteristic. 
     * It is used to subscribe for notifications or indications of the characteristic value. 
     * For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
     * resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  configuration                                                    GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED     if configuring notification, but characteristic has no notification property set
     *                GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED       if configuring indication, but characteristic has no indication property set
     *                ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE         if configuration is invalid
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);
    
    /**
     * @brief Writes the client characteristic configuration of the specified characteristic.
     * It is used to subscribe for notifications or indications of the characteristic value.
     * For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
     * resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  configuration                                                    GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED     if configuring notification, but characteristic has no notification property set
     *                GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED       if configuring indication, but characteristic has no indication property set
     *                ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE         if configuration is invalid
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_client_characteristic_configuration_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                               gatt_client_characteristic_t * characteristic, uint16_t configuration, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Register for changes to the Service Changed and Database Hash Characteristics of the remote GATT Service
     * *
     * When configured, GATT_EVENT_QUERY_COMPLETE event is emitted
     * If supported, the Database Hash is read as well
     *
     * Requires ENABLE_GATT_CLIENT_CACHING
     *
     * @param callback
     */
    void gatt_client_add_service_changed_handler(btstack_packet_callback_registration_t * callback);
    
    /**
     * @brief Remove callback for service changes
     *
     * Requires ENABLE_GATT_CLIENT_CACHING
     *
     * @param callback
     */
    void gatt_client_remove_service_changed_handler(btstack_packet_callback_registration_t * callback);
    
    /**
     * @brief Register for notifications and indications of a characteristic enabled by 
     * the gatt_client_write_client_characteristic_configuration function.
     * @param notification struct used to store registration
     * @param callback
     * @param con_handle or GATT_CLIENT_ANY_CONNECTION to receive updates from all connected devices
     * @param characteristic or NULL to receive updates for all characteristics
     */
    void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification, btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /**
     * @brief Stop listening to characteristic value updates registered with
     * the gatt_client_listen_for_characteristic_value_updates function.
     * @param notification struct used in gatt_client_listen_for_characteristic_value_updates
     */
    void gatt_client_stop_listening_for_characteristic_value_updates(gatt_client_notification_t * notification);
    
    /**
     * @brief Register for notifications and indications of characteristic in a service
     * the gatt_client_write_client_characteristic_configuration function.
     * @param notification struct used to store registration
     * @param callback
     * @param con_handle or GATT_CLIENT_ANY_CONNECTION to receive updates from all connected devices
     * @param service
     * @param end_handle
     * @param service_id    - context provided to callback in events
     * @param connection_id - contest provided to callback in events
     */
    void gatt_client_listen_for_service_characteristic_value_updates(gatt_client_service_notification_t * notification,
                                                                     btstack_packet_handler_t callback,
                                                                     hci_con_handle_t con_handle,
                                                                     gatt_client_service_t * service,
                                                                     uint16_t service_id,
                                                                     uint16_t connection_id);
    
    /**
     * @brief Stop listening to characteristic value updates for registered service with
     * the gatt_client_listen_for_characteristic_value_updates function.
     * @param notification struct used in gatt_client_listen_for_characteristic_value_updates
     */
    void gatt_client_stop_listening_for_service_characteristic_value_updates(gatt_client_service_notification_t * notification);
    
    
    /**
     * @brief Transactional write. It can be called as many times as it is needed to write the characteristics within the same transaction. 
     * Call the gatt_client_execute_write function to commit the transaction.
     * @param  callback   
     * @param  con_handle
     * @param  attribute_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     */
    uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /**
     * @brief Commit transactional write. GATT_EVENT_QUERY_COMPLETE is received.
     * @param  callback   
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Abort transactional write. GATT_EVENT_QUERY_COMPLETE is received.
     * @param  callback   
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when regular gatt query can be sent
     * @note callback might happen during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection handle unknown,
     *       - ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if GATT client memory exceeded, or
     *       - ERROR_CODE_COMMAND_DISALLOWED if GATT client already registered callback
     */
    uint8_t gatt_client_request_to_send_gatt_query(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Remove queued callback for regular gatt queries, to be used on disconnect for example
     * @param callback_registration
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok
     */
    uint8_t gatt_client_remove_gatt_query(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when writing characteristic value without response is possible
     * @note callback might happen during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t gatt_client_request_to_write_without_response(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    
    // the following functions are marked as deprecated and will be removed eventually
    /**
     * @brief Requests GATT_EVENT_CAN_WRITE_WITHOUT_RESPONSE that guarantees
     * a single successful gatt_client_write_value_of_characteristic_without_response call.
     * @deprecated please use gatt_client_request_to_write_without_response instead
     * @param  callback
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_request_can_write_without_response_event(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    #ifdef ENABLE_GATT_CLIENT_CACHING
    /**
     * @brief Get GATT Database Hash for remote device
     * @param con_handle
     * @return database hash or NULL if not available
     */
    const uint8_t * gatt_client_get_database_hash(hci_con_handle_t con_handle);
    
    /**
     * @brief Get next cache id for remote device. This allows to enumerate all GATT Service Client requets
     * @param con_handle
     * @return
     */
    uint16_t gatt_client_get_next_cache_id(hci_con_handle_t con_handle);
    #endif
    
    /**
     * @brief Map ATT Error Code to (extended) Error Codes
     * @param att_error_code
     * @return
     */
    uint8_t gatt_client_att_status_to_error_code(uint8_t att_error_code);
    
