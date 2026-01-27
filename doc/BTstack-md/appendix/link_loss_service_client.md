
# Link Loss Service Client API {#sec:link_loss_service_client_api}


**src/ble/gatt-service/link_loss_service_client.h** 


    
        
    /**
     * @brief Initialize Link Loss Service. 
     */
    void link_loss_service_client_init(void);
    
     /**
     * @brief Connect to a Link Loss Service instance of remote device. The client will automatically register for notifications.
     * The mute state is received via GATTSERVICE_SUBEVENT_LLS_CLIENT_MUTE event.
     * The mute state can be 0 - off, 1 - on, 2 - disabeled and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * Event GATTSERVICE_SUBEVENT_LLS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param llc_connection
     * @param llc_characteristics_storage           storage for characteristics
     * @param llc_characteristics_num               == LINK_LOSS_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t link_loss_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             lls_client_connection_t * llc_connection,
             gatt_service_client_characteristic_t * llc_characteristics_storage, uint8_t llc_characteristics_num,
             uint16_t * lls_cid
    );
    
    uint8_t link_loss_service_client_read_alert_level(uint16_t lls_cid);
    uint8_t link_loss_service_client_write_alert_level(uint16_t lls_cid, lls_alert_level_t alert_level);
    
    
    /**
     * @brief Disconnect.
     * @param lls_cid
     * @return status
     */
    uint8_t link_loss_service_client_disconnect(uint16_t lls_cid);
    
    /**
     * @brief De-initialize Link Loss Service. 
     */
    void link_loss_service_client_deinit(void);
    
