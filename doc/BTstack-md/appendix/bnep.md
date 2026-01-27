
# BNEP API {#sec:bnep_api}


**src/classic/bnep.h** 


    
    /**
     * @brief Set up BNEP.
     */
    void bnep_init(void);
    
    /**
     * @brief Check if a data packet can be send out.
     */
    int bnep_can_send_packet_now(uint16_t bnep_cid);
    
    /** 
     * @brief Request emission of BNEP_CAN_SEND_NOW as soon as possible
     * @note BNEP_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param bnep_cid
     */
    void bnep_request_can_send_now_event(uint16_t bnep_cid);
    
    /**
     * @brief Send a data packet.
     */
    int bnep_send(uint16_t bnep_cid, uint8_t *packet, uint16_t len);
    
    /**
     * @brief Set the network protocol filter.
     */
    int bnep_set_net_type_filter(uint16_t bnep_cid, bnep_net_filter_t *filter, uint16_t len);
    
    /**
     * @brief Set the multicast address filter.
     */
    int bnep_set_multicast_filter(uint16_t bnep_cid, bnep_multi_filter_t *filter, uint16_t len);
    
    /**
     * @brief Set security level required for incoming connections, need to be called before registering services.
     * @deprecated use gap_set_security_level instead
     */
    void bnep_set_required_security_level(gap_security_level_t security_level);
    
    /**
     * @brief Creates BNEP connection (channel) to a given server on a remote device with baseband address. A new baseband connection will be initiated if necessary. 
     */
    int bnep_connect(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint16_t l2cap_psm, uint16_t uuid_src, uint16_t uuid_dest);
    
    /**
     * @brief Disconnects BNEP channel with given identifier. 
     */
    void bnep_disconnect(bd_addr_t addr);
    
    /**
     * @brief Registers BNEP service, set a maximum frame size and assigns a packet handler. On embedded systems, use NULL for connection parameter. 
     */
    uint8_t bnep_register_service(btstack_packet_handler_t packet_handler, uint16_t service_uuid, uint16_t max_frame_size);
    
    /**
     * @brief Unregister BNEP service.
     */
    void bnep_unregister_service(uint16_t service_uuid);
    
    /**
     * @brief De-Init BNEP
     */
    void bnep_deinit(void);
    
