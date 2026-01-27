
# SPP Server API {#sec:spp_server_api}


**src/classic/spp_server.h** : 
Create SPP SDP Records.



    
    /**
     * @brief Create SDP record for SPP service with official SPP Service Class
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param rfcomm_channel
     * @param name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void spp_create_sdp_record(uint8_t *service, uint32_t service_record_handle, int rfcomm_channel, const char *name);
    
    /**
     * @brief Create SDP record for SPP service with custom service UUID (e.g. for use with Android)
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param service_uuid128 buffer
     * @param rfcomm_channel
     * @param name
     */
    void spp_create_custom_sdp_record(uint8_t *service, uint32_t service_record_handle, const uint8_t * service_uuid128, int rfcomm_channel, const char *name);
    
