
# SDP Client RFCOMM API {#sec:sdp_client_rfcomm_api}


**src/classic/sdp_client_rfcomm.h** 


    
    /** 
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 16-bit UUID anywhere.
     * @note calls sdp_service_search_pattern_for_uuid16 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid16
     */
     uint8_t sdp_client_query_rfcomm_channel_and_name_for_uuid(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /**
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 16-bit UUID in its ServiceClassIDList
     * @note calls sdp_service_search_pattern_for_uuid16 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid16
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_service_class_uuid(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /**
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 128-bit UUID anywhere
     * @note calls sdp_service_search_pattern_for_uuid128 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid128
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_uuid128(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * uuid128);
    
    /** 
     * @brief Searches SDP records on a remote device for RFCOMM services with a given service search pattern.
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_search_pattern(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_serviceSearchPattern);
    
