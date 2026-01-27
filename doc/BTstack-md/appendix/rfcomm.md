
# RFCOMM API {#sec:rfcomm_api}


**src/classic/rfcomm.h** 


    
    /** 
     * @brief Set up RFCOMM.
     */
    void rfcomm_init(void);
    
    /** 
     * @brief Set security level required for incoming connections, need to be called before registering services.
     * @deprecated use gap_set_security_level instead
     */
    void rfcomm_set_required_security_level(gap_security_level_t security_level);
    
    /* 
     * @brief Create RFCOMM connection to a given server channel on a remote deivce.
     * This channel will automatically provide enough credits to the remote side.
     * @param addr
     * @param server_channel
     * @param out_cid
     * @result status
     */
    uint8_t rfcomm_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint16_t * out_cid);
    
    /* 
     * @brief Create RFCOMM connection to a given server channel on a remote deivce.
     * This channel will use explicit credit management. During channel establishment, an initial  amount of credits is provided.
     * @param addr
     * @param server_channel
     * @param initial_credits
     * @param out_cid
     * @result status
     */
    uint8_t rfcomm_create_channel_with_initial_credits(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint8_t initial_credits, uint16_t * out_cid);
    
    /** 
     * @brief Disconnects RFCOMM channel with given identifier.
     * @return status
     */
    uint8_t rfcomm_disconnect(uint16_t rfcomm_cid);
    
    /** 
     * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler.
     * This channel provides credits automatically to the remote side -> no flow control
     * @param packet handler for all channels of this service
     * @param channel 
     * @param max_frame_size
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t rfcomm_register_service(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size);
    
    /** 
     * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler. 
     * This channel will use explicit credit management. During channel establishment, an initial amount of credits is provided.
     * @param packet handler for all channels of this service
     * @param channel 
     * @param max_frame_size
     * @param initial_credits
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t rfcomm_register_service_with_initial_credits(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size, uint8_t initial_credits);
    
    /** 
     * @brief Unregister RFCOMM service.
     */
    void rfcomm_unregister_service(uint8_t service_channel);
    
    /** 
     * @brief Accepts incoming RFCOMM connection.
     * @return status
     */
    uint8_t rfcomm_accept_connection(uint16_t rfcomm_cid);
    
    /** 
     * @brief Deny incoming RFCOMM connection.
     * @return status
     */
    uint8_t rfcomm_decline_connection(uint16_t rfcomm_cid);
    
    /** 
     * @brief Grant more incoming credits to the remote side for the given RFCOMM channel identifier.
     * @return status
     */
    uint8_t rfcomm_grant_credits(uint16_t rfcomm_cid, uint8_t credits);
    
    /** 
     * @brief Checks if RFCOMM can send packet. 
     * @param rfcomm_cid
     * @result true if can send now
     */
    bool rfcomm_can_send_packet_now(uint16_t rfcomm_cid);
    
    /** 
     * @brief Request emission of RFCOMM_EVENT_CAN_SEND_NOW as soon as possible
     * @note RFCOMM_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param rfcomm_cid
     * @prarm status
     */
    uint8_t rfcomm_request_can_send_now_event(uint16_t rfcomm_cid);
    
    /** 
     * @brief Sends RFCOMM data packet to the RFCOMM channel with given identifier.
     * @param rfcomm_cid
     * @return status
     */
    uint8_t rfcomm_send(uint16_t rfcomm_cid, uint8_t *data, uint16_t len);
    
    /** 
     * @brief Sends Local Line Status, see LINE_STATUS_..
     * @param rfcomm_cid
     * @param line_status
     * @return status
     */
    uint8_t rfcomm_send_local_line_status(uint16_t rfcomm_cid, uint8_t line_status);
    
    /** 
     * @brief Send local modem status. see MODEM_STAUS_..
     * @param rfcomm_cid
     * @param modem_status
     * @return status
     */
    uint8_t rfcomm_send_modem_status(uint16_t rfcomm_cid, uint8_t modem_status);
    
    /** 
     * @brief Configure remote port 
     * @param rfcomm_cid
     * @param baud_rate
     * @param data_bits
     * @param stop_bits
     * @param parity
     * @param flow_control
     * @return status
     */
    uint8_t rfcomm_send_port_configuration(uint16_t rfcomm_cid, rpn_baud_t baud_rate, rpn_data_bits_t data_bits, rpn_stop_bits_t stop_bits, rpn_parity_t parity, uint8_t flow_control);
    
    /** 
     * @brief Query remote port 
     * @param rfcomm_cid
     * @return status
     */
    uint8_t rfcomm_query_port_configuration(uint16_t rfcomm_cid);
    
    /** 
     * @brief Query max frame size
     * @param rfcomm_cid
     * @return max frame size
     */
    uint16_t rfcomm_get_max_frame_size(uint16_t rfcomm_cid);
    
    /** 
     * @brief Reserve packet buffer to allow to create RFCOMM packet in place
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     *
     * if (rfcomm_can_send_packet_now(cid)){
     *     rfcomm_reserve_packet_buffer();
     *     uint8_t * buffer = rfcomm_get_outgoing_buffer();
     *     uint16_t buffer_size = rfcomm_get_max_frame_size(cid);
     *     .. setup data in buffer with len ..
     *     rfcomm_send_prepared(cid, len)
     * }
     */
    void rfcomm_reserve_packet_buffer(void);
    
    /**
     * @brief Get outgoing buffer
     * @return pointer to outgoing rfcomm buffer
     */
    uint8_t * rfcomm_get_outgoing_buffer(void);
    
    /**
     * @brief Send packet prepared in outgoing buffer
     * @note This releases the outgoing rfcomm buffer
     * @param rfcomm_cid
     * @param len
     * @return status
     */
    uint8_t rfcomm_send_prepared(uint16_t rfcomm_cid, uint16_t len);
    
    /**
     * @brief Release outgoing buffer in case rfcomm_send_prepared was not called
    */
    void rfcomm_release_packet_buffer(void);
    
    /**
     * @brief Enable L2CAP ERTM mode for RFCOMM. request callback is used to provide ERTM buffer. released callback returns buffer
     *
     * @note on request_callback, the app must set the ertm_config, buffer, size fields to enable ERTM for the current connection
     * @note if buffer is not set, BASIC mode will be used instead
     *
     * @note released_callback provides ertm_id from earlier request to match request and release
     *
     * @param request_callback
     * @param released_callback
     */
    void rfcomm_enable_l2cap_ertm(void request_callback(rfcomm_ertm_request_t * request), void released_callback(uint16_t ertm_id));
    
    
    // Event getters for RFCOMM_EVENT_PORT_CONFIGURATION
    
    /**
     * @brief Get field rfcomm_cid from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return rfcomm_cid
     */
    static inline uint16_t rfcomm_event_port_configuration_get_rfcomm_cid(const uint8_t * event){
        return little_endian_read_16(event, 2);
    }
    
    /**
     * @brief Get field local from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return remote - false for local port, true for remote port
     */
    static inline bool rfcomm_event_port_configuration_get_remote(const uint8_t * event){
        return event[4] != 0;
    }
    
    /**
     * @brief Get field baud_rate from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return baud_rate
     */
    
    static inline rpn_baud_t rfcomm_event_port_configuration_get_baud_rate(const uint8_t * event){
        return (rpn_baud_t) event[5];
    }
    
    /**
     * @brief Get field data_bits from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return data_bits
     */
    
    static inline rpn_data_bits_t rfcomm_event_port_configuration_get_data_bits(const uint8_t * event){
        return (rpn_data_bits_t) (event[6] & 3);
    }
    /**
     * @brief Get field stop_bits from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return stop_bits
     */
    static inline rpn_stop_bits_t rfcomm_event_port_configuration_get_stop_bits(const uint8_t * event){
        return (rpn_stop_bits_t) ((event[6] >> 2) & 1);
    }
    
    /**
     * @brief Get field parity from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return parity
     */
    static inline rpn_parity_t rfcomm_event_port_configuration_get_parity(const uint8_t * event){
        return (rpn_parity_t) ((event[6] >> 3) & 7);
    }
    
    /**
     * @brief Get field flow_control from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return flow_control
     */
    
    static inline uint8_t rfcomm_event_port_configuration_get_flow_control(const uint8_t * event){
        return event[7] & 0x3f;
    }
    
    /**
     * @brief Get field xon from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return xon
     */
    static inline uint8_t rfcomm_event_port_configuration_get_xon(const uint8_t * event){
        return event[8];
    }
    
    /**
     * @brief Get field xoff from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return xoff
     */
    static inline uint8_t rfcomm_event_port_configuration_get_xoff(const uint8_t * event){
        return event[9];
    }
    
    /**
     * @brief De-Init RFCOMM
     */
    void rfcomm_deinit(void);
    
