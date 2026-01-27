
# Device ID Server API {#sec:device_id_server_api}


**src/classic/device_id_server.h** : 
Create Device ID SDP Records.



    
    /**
     * @brief Create SDP record for Device ID service
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param vendor_id_source usually DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH or DEVICE_ID_VENDOR_ID_SOURCE_USB
     * @param vendor_id
     * @param product_it
     * @param version
     */
    void device_id_create_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t vendor_id_source, uint16_t vendor_id, uint16_t product_id, uint16_t version);
    
