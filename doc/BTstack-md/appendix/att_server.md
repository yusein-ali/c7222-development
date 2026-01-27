
# ATT Server API {#sec:att_server_api}


**src/ble/att_server.h** 


    
    /**
     * @title ATT Server
     *
     * @TLV 'BTC' x
     */
    
    /*
     * @brief setup ATT server
     * @param db attribute database created by compile-gatt.py
     * @param read_callback, see att_db.h, can be NULL
     * @param write_callback, see attl.h, can be NULL
     */
    void att_server_init(uint8_t const * db, att_read_callback_t read_callback, att_write_callback_t write_callback);
    
    /**
     * @brief Enable support for Enhanced ATT bearer
     * @note Requires ENABLE_GATT_OVER_EATT
     * @param num_eatt_bearers
     * @param storage_buffer
     * @param storage_size must be >= num_eatt_bearers * sizeof(att_server_eatt_bearer_t)
     * @return status   ERROR_CODE_SUCCESS
     *                  ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if buffer too small or no entry in l2cap service pool
     *                  L2CAP_SERVICE_ALREADY_REGISTERED if called twice
     */
    uint8_t att_server_eatt_init(uint8_t num_eatt_bearers, uint8_t * storage_buffer, uint16_t storage_size);
    
    /*
     * @brief register packet handler for ATT server events:
     *        - ATT_EVENT_CAN_SEND_NOW
     *        - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
     *        - ATT_EVENT_MTU_EXCHANGE_COMPLETE 
     * @param handler
     */
    void att_server_register_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief register read/write callbacks for specific handle range
     * @param att_service_handler_t
     */
    void att_server_register_service_handler(att_service_handler_t * handler);
    
    /**
     * @brief Request callback when sending is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_register_can_send_now_callback(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Return ATT MTU
     * @param con_handle
     * @return mtu if ok, 0 otherwise
     */
    uint16_t att_server_get_mtu(hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when sending notifcation is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t att_server_request_to_send_notification(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when sending indication is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t att_server_request_to_send_indication(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief notify client about attribute value change
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);
    
    /**
     * @brief notify client about multiple attribute value changes
     * @param con_handle
     * @param num_attributes
     * @param attribute_handles[]
     * @param values_data[]
     * @param values_len[]
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_multiple_notify(hci_con_handle_t con_handle, uint8_t num_attributes,
                                       const uint16_t * attribute_handles, const uint8_t ** values_data, const uint16_t * values_len);
    
    /**
     * @brief indicate value change to client. client is supposed to reply with an indication_response
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_indicate(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);
    
    #ifdef ENABLE_ATT_DELAYED_RESPONSE
    /**
     * @brief response ready - called after returning ATT_READ__RESPONSE_PENDING in an att_read_callback or
     * ATT_ERROR_WRITE_REQUEST_PENDING IN att_write_callback before to trigger callback again and complete the transaction
     * @nore The ATT Server will retry handling the current ATT request
     * @param con_handle
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_response_ready(hci_con_handle_t con_handle);
    #endif
    
    /**
     * De-Init ATT Server 
     */
    void att_server_deinit(void);
    
    // the following functions will be removed soon
    
    /**
     * @brief tests if a notification or indication can be send right now
     * @param con_handle
     * @return 1, if packet can be sent
     */
    int  att_server_can_send_packet_now(hci_con_handle_t con_handle);
    
    /**
     * @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
     * @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_server_request_can_send_now_event(hci_con_handle_t con_handle);
    // end of deprecated functions
    
