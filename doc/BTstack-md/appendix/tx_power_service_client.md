
# Immediate Alert Service Client API {#sec:tx_power_service_client_api}


**src/ble/gatt-service/tx_power_service_client.h** 


    
        
    /**
     * @brief Initialize Immediate Alert Service. 
     */
    void tx_power_service_client_init(void);
    
     /**
     * @brief Connect to a TX Power Service instance of remote device. The client will automatically register for notifications.
     * The TX Power level state is received via GATTSERVICE_SUBEVENT_TXPS_CLIENT_TX_POWER_LEVEL event.
     *
     * Event GATTSERVICE_SUBEVENT_TXPS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param iac_connection
     * @param iac_characteristics_storage           storage for characteristics
     * @param iac_characteristics_num               == TX_POWER_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t tx_power_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             txps_client_connection_t * iac_connection,
             gatt_service_client_characteristic_t * iac_characteristics_storage, uint8_t iac_characteristics_num,
             uint16_t * txps_cid
    );
    
    /**
     * @note Trigger read of TX Power Level characteristic.
     * @param txps_cid
     * @return status
     */
    uint8_t tx_power_service_client_read_tx_power_level(uint16_t txps_cid);
    
    /**
     * @brief Disconnect.
     * @param txps_cid
     * @return status
     */
    uint8_t tx_power_service_client_disconnect(uint16_t txps_cid);
    
    /**
     * @brief De-i  nitialize Immediate Alert Service.
     */
    void tx_power_service_client_deinit(void);
    
