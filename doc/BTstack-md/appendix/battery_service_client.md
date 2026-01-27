
# Battery Service Client API {#sec:battery_service_client_api}


**src/ble/gatt-service/battery_service_client.h** 


    
        
    /**
     * @brief Initialize Battery Service. 
     */
    void battery_service_client_init(void);
    
    /**
     * @brief Connect to Battery Services of remote device. The client will try to register for notifications. 
     * If notifications are not supported by remote Battery Service, the client will poll battery level
     * If poll_interval_ms is 0, polling is disabled, and only notifications will be received.
     * In either case, the battery level is received via GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL event.
     * The battery level is reported as percentage, i.e. 100 = full and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * For manual polling, see battery_service_client_read_battery_level below.
     *
     * Event GATTSERVICE_SUBEVENT_BATTERY_SERVICE_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no battery service is found, or ATT errors (see bluetooth.h). 
     * This event event also returns number of battery instances found on remote server, as well as poll bitmap that indicates which indexes 
     * of services require polling, i.e. they do not support notification on battery level change,
     *
     * @param con_handle
     * @param packet_handler
     * @param poll_interval_ms or 0 to disable polling
     * @param battery_service_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED 
     */
    uint8_t battery_service_client_connect(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler, uint32_t poll_interval_ms, uint16_t * battery_service_cid);
    
    /**
     * @brief Read battery level for service with given index. Event GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL is 
     * received with battery level (unit is in percentage, i.e. 100 = full). The battery level is valid if the ATTT status 
     * is equal to ATT_ERROR_SUCCESS, see ATT errors (see bluetooth.h) for other values.
     * @param battery_service_cid
     * @param service_index
     * @return status
     */
    uint8_t battery_service_client_read_battery_level(uint16_t battery_service_cid, uint8_t service_index);
    
    /**
     * @brief Disconnect from Battery Service.
     * @param battery_service_cid
     * @return status
     */
    uint8_t battery_service_client_disconnect(uint16_t battery_service_cid);
    
    /**
     * @brief De-initialize Battery Service. 
     */
    void battery_service_client_deinit(void);
    
