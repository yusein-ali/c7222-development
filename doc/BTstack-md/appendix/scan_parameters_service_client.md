
# Scan Parameters Service Client API {#sec:scan_parameters_service_client_api}


**src/ble/gatt-service/scan_parameters_service_client.h** 


    
    /**
     * @brief Initialize Scan Parameters Service. 
     */
    void scan_parameters_service_client_init(void);
    
    /**
     * @brief Set Scan Parameters Service. It will update all connected devices.
     * @param scan_interval
     * @param scan_window
     */
    void scan_parameters_service_client_set(uint16_t scan_interval, uint16_t scan_window);
    
    /**
     * @brief Connect to Scan Parameters Service of remote device. 
     *
     * The GATTSERVICE_SUBEVENT_SCAN_PARAMETERS_SERVICE_CONNECTED event completes the request. 
     * Its status is set to ERROR_CODE_SUCCESS if remote service and SCAN_INTERVAL_WINDOW characteristic are found. 
     * *
     * Other status codes of this event:
     * - GATT_CLIENT_IN_WRONG_STATE: client in wrong state
     * - ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE: service or characteristic not found
     * - ATT errors, see bluetooth.h
     *
     * @param con_handle
     * @param packet_handler
     * @param sps_connection
     * @param sps_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if client with con_handle not found
     */
    uint8_t scan_parameters_service_client_connect(
            hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler,
            sps_client_connection_t * sps_connection,
            uint16_t * sps_cid);
    
    /**
     * @brief Disconnect from Scan Parameters Service.
     * @param scan_parameters_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if client with con_handle is not found
     */
    uint8_t scan_parameters_service_client_disconnect(uint16_t scan_parameters_cid);
    
    
    /**
     * @brief De-initialize Scan Parameters Service. 
     */
    void scan_parameters_service_client_deinit(void);
    
