
# GATT Service Client API {#sec:gatt_service_client_api}


**src/ble/gatt_service_client.h** 


    
    
    /**
     * @brief Initialize GATT Service Client infrastructure
     */
    void gatt_service_client_init(void);
    
    /**
     * @brief Register new GATT Service Client with list of Characteristic UUID16s
     * @param client
     * @param connection_and_notification_handler packet handler that receives events: GATT_EVENT_NOTIFICATION, GATT_EVENT_IDENTIFICATION and two HCI_EVENT_GATTSERVICE_META subevents: GATTSERVICE_SUBEVENT_CLIENT_CONNECTED/DISCONNECTED
     * @param characteristic_uuid16s
     * @param characteristic_uuid16s_num
     */
    void gatt_service_client_register_client_with_uuid16s(gatt_service_client_t *client, btstack_packet_handler_t connection_and_notification_handler,
                                             const uint16_t *characteristic_uuid16s, uint16_t characteristic_uuid16s_num);
    
    /**
     * @brief Register new GATT Service Client with list of Characteristic UUID128s
     * @param client
     * @param connection_and_notification_handler packet handler that receives events: GATT_EVENT_NOTIFICATION, GATT_EVENT_IDENTIFICATION and two HCI_EVENT_GATTSERVICE_META subevents: GATTSERVICE_SUBEVENT_CLIENT_CONNECTED/DISCONNECTED
     * @param characteristic_uuid128s
     * @param characteristic_uuid128s_num
     */
    
    void gatt_service_client_register_client_with_uuid128s(gatt_service_client_t *client, btstack_packet_handler_t packet_handler,
                                             const uuid128_t *characteristic_uuid128s, uint16_t characteristic_uuid128s_num);
    
    /**
     * @brief Get Characteristic UUID16 for given Characteristic index
     *
     * @param client
     * @param characteristic_index
     * @return uuid16 or 0 if index out of range
     */
    uint16_t gatt_service_client_characteristic_uuid16_for_index(const gatt_service_client_t * client, uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic UUID128 for given Characteristic index
     *
     * @param client
     * @param characteristic_index
     * @return uuid128 or NULL if index out of range
     */
    const uuid128_t * gatt_service_client_characteristic_uuid128_for_index(const gatt_service_client_t * client, uint8_t characteristic_index);
    
    /**
     * @brief Unregister GATT Service Client
     * @param client
     */
    void gatt_service_client_unregister_client(gatt_service_client_t * client);
    
    /**
     * @brief Connect to the first instance of Primary GATT Service with UUID16
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid16
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_primary_service_with_uuid16(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                            gatt_service_client_connection_t *connection,
                                                            uint16_t service_uuid16,
                                                            gatt_service_client_characteristic_t *characteristics,
                                                            uint8_t characteristics_num);
    
    /**
     * @brief Connect to the n-th instance of Primary GATT Service with UUID16
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid128
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_primary_service_with_uuid128(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                                    gatt_service_client_connection_t *connection,
                                                                    const uuid128_t * service_uuid128,
                                                                    gatt_service_client_characteristic_t *characteristics,
                                                                    uint8_t characteristics_num);
    
    /**
     * @brief Connect to the Secondary GATT Service with given handle range
     *
     * UUID16 and Service Index are stored for GATT Service Client user only
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid16
     * @param service_start_handle
     * @param service_end_handle
     * @param service_index
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_secondary_service_with_uuid16(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                              gatt_service_client_connection_t *connection,
                                                              uint16_t service_uuid16, uint8_t service_index,
                                                              uint16_t service_start_handle, uint16_t service_end_handle,
                                                              gatt_service_client_characteristic_t *characteristics,
                                                              uint8_t characteristics_num);
    
    /**
     * @brief Disconnect service client
     * @param client
     * @param connection
     * @return
     */
    uint8_t gatt_service_client_disconnect(gatt_service_client_connection_t *connection);
    
    /**
     * @brief Check if Characteristic is available and can be queried
     * @param client
     * @param connection
     * @param characteristic_index
     * @return ERROR_CODE_SUCCESS if ready, ERROR_CODE_COMMAND_DISALLOWED or ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE otherwise
     */
    uint8_t gatt_service_client_can_query_characteristic(const gatt_service_client_connection_t *connection,
                                                 uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic Value Handle for given Characteristic index
     * *
     * @param connection
     * @param characteristic_index
     * @return
     */
    uint16_t gatt_service_client_characteristic_value_handle_for_index(const gatt_service_client_connection_t *connection,
                                                                       uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic index Handle for given Characteristic Value Handle
     * *
     * @param connection
     * @param value_handle
     * @return index of characteristic in initial list or GATT_SERVICE_CLIENT_INVALID_INDEX
     */
    uint8_t gatt_service_client_characteristic_index_for_value_handle(const gatt_service_client_connection_t *connection,
                                                                      uint16_t value_handle);
    
    /**
     * @brief Get connection id
     * @param client
     * @param connection
     * @returns connection_id
     */
    uint16_t gatt_service_client_get_connection_id(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get connection handle
     * @param client
     * @param connection
     * @returns con_handle
     */
    hci_con_handle_t gatt_service_client_get_con_handle(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get service index provided in connect call
     * @param client
     * @param connection
     * @returns connection_id
     */
    uint8_t gatt_service_client_get_service_index(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get remote MTU
     * @param client
     * @param connection
     * @returns MTU or 0 in case of error
     */
    uint16_t gatt_service_client_get_mtu(const gatt_service_client_connection_t *connection);
    
    
    /**
     * @brief Dump characteristic value handles
     * @param client
     * @param connection
     * @param characteristic_names
     */
    void gatt_service_client_dump_characteristic_value_handles(const gatt_service_client_connection_t *connection,
                                                               const char **characteristic_names);
    
    /**
     * Return connection for given CID
     * @param client
     * @param connection_cid
     * @return
     */
    gatt_service_client_connection_t * gatt_service_client_get_connection_for_cid(const gatt_service_client_t *client, uint16_t connection_cid);
    
    /**
     * Substitute subevent ID with the given one, and send the event
     * @param callback
     * @param packet
     * @param size
     * @param subevent_id
     */
    void gatt_service_client_replace_subevent_id_and_emit(btstack_packet_handler_t callback, uint8_t * packet, uint16_t size, uint8_t subevent_id);
    
    /**
     * @brief De-Init
     * @param client
     */
    void gatt_service_client_deinit(void);
    
    
