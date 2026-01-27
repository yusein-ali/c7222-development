
# Immediate Alert Service Client API {#sec:immediate_alert_service_client_api}


**src/ble/gatt-service/immediate_alert_service_client.h** 


    
        
    /**
     * @brief Initialize Immediate Alert Service. 
     */
    void immediate_alert_service_client_init(void);
    
     /**
     * @brief Connect to a Immediate Alert Service instance of remote device. The client will automatically register for notifications.
     * The mute state is received via GATTSERVICE_SUBEVENT_LLS_CLIENT_MUTE event.
     * The mute state can be 0 - off, 1 - on, 2 - disabled and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * Event GATTSERVICE_SUBEVENT_LLS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param iac_connection
     * @param iac_characteristics_storage           storage for characteristics
     * @param iac_characteristics_num               == IMMEDIATE_ALERT_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t immediate_alert_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             ias_client_connection_t * iac_connection,
             uint16_t * ias_cid
    );
    
    uint8_t immediate_alert_service_client_write_alert_level(uint16_t ias_cid, ias_alert_level_t alert_level);
    
    
    /**
     * @brief Disconnect.
     * @param ias_cid
     * @return status
     */
    uint8_t immediate_alert_service_client_disconnect(uint16_t ias_cid);
    
    /**
     * @brief De-initialize Immediate Alert Service. 
     */
    void immediate_alert_service_client_deinit(void);
    
