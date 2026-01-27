
# SDP Client API {#sec:sdp_client_api}


**src/classic/sdp_client.h** 


    
    typedef struct de_state {
        uint8_t  in_state_GET_DE_HEADER_LENGTH ;
        uint32_t addon_header_bytes;
        uint32_t de_size;
        uint32_t de_offset;
    } de_state_t; 
    
    void de_state_init(de_state_t * state);
    int  de_state_size(uint8_t eventByte, de_state_t *de_state);
    
    /**
     * @brief SDP Client Init
     */
    void sdp_client_init(void);
    
    /** 
     * @brief Checks if the SDP Client is ready
     * @deprecated Please use sdp_client_register_query_callback instead
     * @return true when no query is active
     */
    bool sdp_client_ready(void);
    
    /**
     * @brief Requests a callback, when the SDP Client is ready and can be used
     * @note The callback might happens before sdp_client_register_query_callback has returned
     * @param callback_registration
     */
    uint8_t sdp_client_register_query_callback(btstack_context_callback_registration_t * callback_registration);
    
    /** 
     * @brief Queries the SDP service of the remote device given a service search pattern and a list of attribute IDs. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @param callback for attributes values and done event
     * @param remote address
     * @param des_service_search_pattern 
     * @param des_attribute_id_list
     */
    uint8_t sdp_client_query(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern, const uint8_t * des_attribute_id_list);
    
    /*
     * @brief Searches SDP records on a remote device for all services with a given UUID.
     * @note calls sdp_client_query with service search pattern based on uuid16
     */
    uint8_t sdp_client_query_uuid16(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /*
     * @brief Searches SDP records on a remote device for all services with a given UUID.
     * @note calls sdp_client_query with service search pattern based on uuid128
     */
    uint8_t sdp_client_query_uuid128(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t* uuid128);
    
    
    /** 
     * @brief Retrieves all attribute IDs of a SDP record specified by its service record handle and a list of attribute IDs. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
     * @param callback for attributes values and done event 
     * @param remote address
     * @param search_service_record_handle 
     * @param des_attributeIDList
     */
    uint8_t sdp_client_service_attribute_search(btstack_packet_handler_t callback, bd_addr_t remote, uint32_t search_service_record_handle, const uint8_t * des_attributeIDList);
    
    /** 
     * @brief Query the list of SDP records that match a given service search pattern. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
     * @param callback for attributes values and done event
     * @param remote address
     * @param des_service_search_pattern 
     */
    uint8_t sdp_client_service_search(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern);
    
    #ifdef ENABLE_SDP_EXTRA_QUERIES
    void sdp_client_parse_service_record_handle_list(uint8_t* packet, uint16_t total_count, uint16_t current_count);
    #endif
    
    /**
     * @brief De-Init SDP Client
     */
    void sdp_client_deinit(void);
    
