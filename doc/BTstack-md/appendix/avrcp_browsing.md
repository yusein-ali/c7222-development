
# AVRCP Browsing API {#sec:avrcp_browsing_api}


**src/classic/avrcp_browsing.h** 


    /**
     * @brief Set up AVRCP Browsing service
     */
    void avrcp_browsing_init(void);
    
    /**
     * @brief Register callback for the AVRCP Browsing Controller client. 
     * @param callback
     */
    void avrcp_browsing_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief   Connect to AVRCP Browsing service on a remote device, emits AVRCP_SUBEVENT_BROWSING_CONNECTION_ESTABLISHED with status
     * @param   remote_addr
     * @param   ertm_buffer
     * @param   ertm_buffer_size
     * @param   ertm_config
     * @param   avrcp_browsing_cid  outgoing parameter, valid if status == ERROR_CODE_SUCCESS
     * @return status     
     */
    uint8_t avrcp_browsing_connect(bd_addr_t remote_addr, uint8_t * ertm_buffer, uint32_t ertm_buffer_size, l2cap_ertm_config_t * ertm_config, uint16_t * avrcp_browsing_cid);
    
    /**
     * @brief Configure incoming connection for Browsing Service.
     * @param avrcp_browsing_cid
     * @param ertm_buffer
     * @param ertm_buffer_size
     * @param ertm_config
     * @return status
     */
    uint8_t avrcp_browsing_configure_incoming_connection(uint16_t avrcp_browsing_cid, uint8_t * ertm_buffer, uint32_t ertm_buffer_size, l2cap_ertm_config_t * ertm_config);
    
    /**
     * @brief Decline incoming connection Browsing Service.
     * @param avrcp_browsing_cid
     * @return status
     */
    uint8_t avrcp_browsing_decline_incoming_connection(uint16_t avrcp_browsing_cid);
    
    /**
     * @brief   Disconnect from AVRCP Browsing service
     * @param   avrcp_browsing_cid
     * @return status
     */
    uint8_t avrcp_browsing_disconnect(uint16_t avrcp_browsing_cid);
    
    /**
     * @brief De-Init AVRCP Browsing
     */
    void avrcp_browsing_deinit(void);
    
