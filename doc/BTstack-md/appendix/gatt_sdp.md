
# GATT SDP API {#sec:gatt_sdp_api}


**src/classic/gatt_sdp.h** 


    
    /** 
     * @brief Creates SDP record forG ATT service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param gatt_start_handle
     * @param gatt_end_handle
     */
    void gatt_create_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t gatt_start_handle, uint16_t gatt_end_handle);
    
