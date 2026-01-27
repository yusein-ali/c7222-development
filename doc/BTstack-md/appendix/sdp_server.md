
# SDP Server API {#sec:sdp_server_api}


**src/classic/sdp_server.h** 


    
    /** 
     * @brief Set up SDP Server.
     */
    void sdp_init(void);
    
    /**
     * @brief Register Service Record with database using ServiceRecordHandle stored in record
     * @pre AttributeIDs are in ascending order
     * @pre ServiceRecordHandle is first attribute and valid
     * @param record is not copied!
     * @result status
     */
    uint8_t sdp_register_service(const uint8_t * record);
    
    /** 
     * @brief Unregister service record internally.
     */
    void sdp_unregister_service(uint32_t service_record_handle);
    
    /**
     * @brief gets service record handle from record
     * @resutl service record handle or 0
     */
    uint32_t sdp_get_service_record_handle(const uint8_t * record);
    
    /**
     * @brief Finds an unused valid service record handle
     * @result handle
     */
    uint32_t sdp_create_service_record_handle(void);
    
    /**
     * @brief gets record for handle
     * @result record
     */
    
    uint8_t * sdp_get_record_for_handle(uint32_t handle);
    
    /**
     * @brief De-Init SDP Server
     */
    void sdp_deinit(void);
    
