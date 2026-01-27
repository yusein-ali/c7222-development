
# Personal Area Network (PAN) API {#sec:pan_api}


**src/classic/pan.h** 


    
    /** 
     * @brief Creates SDP record for PANU BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param name if NULL, the default service name will be assigned
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     */
    void pan_create_panu_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc);
    
    /** 
     * @brief Creates SDP record for GN BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param name if NULL, the default service name will be assigned
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
     * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
     */
    void pan_create_gn_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc, const char *IPv4Subnet,
    	const char *IPv6Subnet);
    
    /** 
     * @brief Creates SDP record for NAP BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param name if NULL, the default service name will be assigned
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     * @param net_access_type type of available network access
     * @param max_net_access_rate based on net_access_type measured in byte/s
     * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
     * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
     */
    void pan_create_nap_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc, net_access_type_t net_access_type,
    	uint32_t max_net_access_rate, const char *IPv4Subnet, const char *IPv6Subnet);
    
