
# Device Information Service Client API {#sec:device_information_service_client_api}


**src/ble/gatt-service/device_information_service_client.h** 


    
    /**
     * @brief Initialize Device Information Service. 
     */
    void device_information_service_client_init(void);
    
    /**
     * @brief Query Device Information Service. The client will query the remote service and emit events: 
     *
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MANUFACTURER_NAME
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MODEL_NUMBER     
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SERIAL_NUMBER    
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_HARDWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_FIRMWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SOFTWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SYSTEM_ID        
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_IEEE_REGULATORY_CERTIFICATION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_PNP_ID 
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_UDI_FOR_MEDICAL_DEVICES  
     *
     * Event GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_DONE is received when all queries are done, of if service was not found.
     * The status field of this event indicated ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @return status ERROR_CODE_SUCCESS on success, otherwise GATT_CLIENT_IN_WRONG_STATE if query is already in progress
     */
    uint8_t device_information_service_client_query(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler);
    
    
    /**
     * @brief De-initialize Device Information Service. 
     */
    void device_information_service_client_deinit(void);
    
