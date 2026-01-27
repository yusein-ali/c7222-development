
# L2CAP API {#sec:l2cap_api}


**src/l2cap.h** : 
Logical Link Control and Adaption Protocol 



    
    //
    // PSM numbers from https://www.bluetooth.com/specifications/assigned-numbers/logical-link-control 
    //
    #define PSM_SDP           BLUETOOTH_PROTOCOL_SDP
    #define PSM_RFCOMM        BLUETOOTH_PROTOCOL_RFCOMM
    #define PSM_BNEP          BLUETOOTH_PROTOCOL_BNEP
    // @TODO: scrape PSMs Bluetooth SIG site and put in bluetooth_psm.h or bluetooth_l2cap.h
    #define PSM_HID_CONTROL   0x11
    #define PSM_HID_INTERRUPT 0x13
    #define PSM_ATT           0x1f
    #define PSM_IPSP          0x23
    
    /** 
     * @brief Set up L2CAP and register L2CAP with HCI layer.
     */
    void l2cap_init(void);
    
    /**
     * @brief Add event packet handler for LE Connection Parameter Update events
     */
    void l2cap_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Remove event packet handler.
     */
    void l2cap_remove_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /** 
     * @brief Get max MTU for Classic connections based on btstack configuration
     */
    uint16_t l2cap_max_mtu(void);
    
    /** 
     * @brief Get max MTU for LE connections based on btstack configuration
     */
    uint16_t l2cap_max_le_mtu(void);
    
    /**
    * @brief Set the max MTU for LE connections, if not set l2cap_max_mtu() will be used.
    */
    void l2cap_set_max_le_mtu(uint16_t max_mtu);
    
    /** 
     * @brief Creates L2CAP channel to the PSM of a remote device with baseband address. A new baseband connection will be initiated if necessary.
     * @param packet_handler
     * @param address
     * @param psm
     * @param mtu
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu, uint16_t * out_local_cid);
    
    /** 
     * @brief Disconnects L2CAP channel with given identifier. 
     * @param local_cid
     * @return status ERROR_CODE_SUCCESS if successful or L2CAP_LOCAL_CID_DOES_NOT_EXIST
     */
    uint8_t l2cap_disconnect(uint16_t local_cid);
    
    /** 
     * @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier. 
     */
    uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);
    
    /** 
     * @brief Sends L2CAP data packet to the channel with given identifier.
     * @note For channel in credit-based flow control mode, data needs to stay valid until .. event
     * @param local_cid
     * @param data to send
     * @param len of data
     * @return status
     */
    uint8_t l2cap_send(uint16_t local_cid, const uint8_t *data, uint16_t len);
    
    /** 
     * @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler. 
     * @param packet_handler
     * @param psm
     * @param mtu
     * @param security_level
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t l2cap_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu, gap_security_level_t security_level);
    
    /** 
     * @brief Unregisters L2CAP service with given PSM.
     */
    uint8_t l2cap_unregister_service(uint16_t psm);
    
    /** 
     * @brief Accepts incoming L2CAP connection.
     */
    void l2cap_accept_connection(uint16_t local_cid);
    
    /** 
     * @brief Deny incoming L2CAP connection.
     */
    void l2cap_decline_connection(uint16_t local_cid);
    
    /** 
     * @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
     * @return true if packet can be sent
     */
    bool l2cap_can_send_packet_now(uint16_t local_cid);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_request_can_send_now_event(uint16_t local_cid);
    
    /** 
     * @brief Reserve outgoing buffer
     * @note Only for L2CAP Basic Mode Channels
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     */
    void l2cap_reserve_packet_buffer(void);
    
    /** 
     * @brief Get outgoing buffer and prepare data.
     * @note Only for L2CAP Basic Mode Channels
     */
    uint8_t *l2cap_get_outgoing_buffer(void);
    
    /** 
     * @brief Send L2CAP packet prepared in outgoing buffer to channel
     * @note Only for L2CAP Basic Mode Channels
     */
    uint8_t l2cap_send_prepared(uint16_t local_cid, uint16_t len);
    
    /** 
     * @brief Release outgoing buffer (only needed if l2cap_send_prepared is not called)
     * @note Only for L2CAP Basic Mode Channels
     */
    void l2cap_release_packet_buffer(void);
    
    //
    // Connection-Oriented Channels in Enhanced Retransmission Mode - ERTM
    //
    
    /**
     * @brief Creates L2CAP channel to the PSM of a remote device with baseband address using Enhanced Retransmission Mode.
     *        A new baseband connection will be initiated if necessary.
     * @param packet_handler
     * @param address
     * @param psm
     * @param ertm_config
     * @param buffer to store reassembled rx packet, out-of-order packets and unacknowledged outgoing packets with their tretransmission timers
     * @param size of buffer
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm,
                                      l2cap_ertm_config_t * ertm_contig, uint8_t * buffer, uint32_t size, uint16_t * out_local_cid);
    
    /**
     * @brief Accepts incoming L2CAP connection for Enhanced Retransmission Mode
     * @param local_cid
     * @param ertm_config
     * @param buffer to store reassembled rx packet, out-of-order packets and unacknowledged outgoing packets with their tretransmission timers
     * @param size of buffer
     * @return status
     */
    uint8_t l2cap_ertm_accept_connection(uint16_t local_cid, l2cap_ertm_config_t * ertm_contig, uint8_t * buffer, uint32_t size);
    
    /**
     * @brief Deny incoming incoming L2CAP connection for Enhanced Retransmission Mode
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_decline_connection(uint16_t local_cid);
    
    /**
     * @brief ERTM Set channel as busy.
     * @note Can be cleared by l2cap_ertm_set_ready
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_set_busy(uint16_t local_cid);
    
    /**
     * @brief ERTM Set channel as ready
     * @note Used after l2cap_ertm_set_busy
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_set_ready(uint16_t local_cid);
    
    
    //
    // L2CAP Connection-Oriented Channels in LE Credit-Based Flow-Control Mode - CBM
    //
    
    /**
     * @brief Register L2CAP service in LE Credit-Based Flow-Control Mode
     * @note MTU and initial credits are specified in l2cap_cbm_accept_connection(..) call
     * @param packet_handler
     * @param psm
     * @param security_level
     */
    uint8_t l2cap_cbm_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, gap_security_level_t security_level);
    
    /**
     * @brief Unregister L2CAP service in LE Credit-Based Flow-Control Mode
     * @param psm
     */
    
    uint8_t l2cap_cbm_unregister_service(uint16_t psm);
    
    /*
     * @brief Accept incoming connection LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param receive_buffer        buffer used for reassembly of L2CAP LE Information Frames into service data unit (SDU) with given MTU
     * @param receive_buffer_size   buffer size equals MTU
     * @param initial_credits       Number of initial credits provided to peer or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     */
    
    uint8_t l2cap_cbm_accept_connection(uint16_t local_cid, uint8_t * receive_sdu_buffer, uint16_t mtu, uint16_t initial_credits);
    
    /** 
     * @brief Deecline connection in LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param result                result, see L2CAP_CBM_CONNECTION_RESULT_SUCCESS in bluetooth.h
     */
    
    uint8_t l2cap_cbm_decline_connection(uint16_t local_cid, uint16_t result);
    
    /**
     * @brief Create outgoing channel in LE Credit-Based Flow-Control Mode
     * @param packet_handler        Packet handler for this connection
     * @param con_handle            HCI Connection Handle, LE transport
     * @param psm                   Service PSM to connect to
     * @param receive_buffer        buffer used for reassembly of L2CAP LE Information Frames into service data unit (SDU) with given MTU
     * @param receive_buffer_size   buffer size equals MTU
     * @param initial_credits       Number of initial credits provided to peer or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param security_level        Minimum required security level
     * @param out_local_cid         L2CAP LE Channel Identifier is stored here
     */
    uint8_t l2cap_cbm_create_channel(btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle,
        uint16_t psm, uint8_t * receive_sdu_buffer, uint16_t mtu, uint16_t initial_credits, gap_security_level_t security_level,
        uint16_t * out_local_cid);
    
    /**
     * @brief Provide credits for channel in LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param credits               Number additional credits for peer
     */
    uint8_t l2cap_cbm_provide_credits(uint16_t local_cid, uint16_t credits);
    
    /**
     * @brief Returns the number of credits provided by peer
     * @param local_cid
     * @return number of credits
     */
    uint16_t l2cap_cbm_available_credits(uint16_t local_cid);
    
    //
    // L2CAP Connection-Oriented Channels in Enhanced Credit-Based Flow-Control Mode - ECBM
    //
    
    /**
     * @brief Register L2CAP service in Enhanced Credit-Based Flow-Control Mode
     * @note MTU and initial credits are specified in l2cap_enhanced_accept_connection(..) call
     * @param packet_handler
     * @param psm
     * @param min_remote_mtu
     * @param security_level
     * @oaram authorization_required
     * @return status
     */
    uint8_t l2cap_ecbm_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t min_remote_mtu,
                                        gap_security_level_t security_level, bool authorization_required);
    
    /**
     * @brief Unregister L2CAP service in Enhanced Credit-Based Flow-Control Mode
     * @param psm
     * @return status
     */
    
    uint8_t l2cap_ecbm_unregister_service(uint16_t psm);
    
    /**
     * @brief Set Minimal MPS for channel in Enhanced Credit-Based Flow-Control Mode
     * @param mps_min
     */
    void l2cap_ecbm_mps_set_min(uint16_t mps_min);
    
    /**
     * @brief Set Minimal MPS for channel in Enhanced Credit-Based Flow-Control Mode
     * @param mps_max
     */
    void l2cap_ecbm_mps_set_max(uint16_t mps_max);
    
    /**
     * @brief Create outgoing channel in Enhanced Credit-Based Flow-Control Mode
     * @note receive_buffer points to an array of receive buffers with num_channels elements
     * @note out_local_cid points to an array where CID is stored with num_channel elements
     * @param packet_handler        Packet handler for this connection
     * @param con_handle            HCI Connection Handle
     * @param security_level        Minimum required security level
     * @param psm                   Service PSM to connect to
     * @param num_channels          number of channels to create
     * @param initial_credits       Number of initial credits provided to peer per channel or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param receive_buffer_size   buffer size equals MTU
     * @param receive_buffers       Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @param out_local_cids        Array of L2CAP Channel Identifiers is stored here on success
     * @return status
     */
    uint8_t l2cap_ecbm_create_channels(btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle,
                                           gap_security_level_t security_level,
                                           uint16_t psm, uint8_t num_channels, uint16_t initial_credits, uint16_t receive_buffer_size,
                                           uint8_t ** receive_buffers, uint16_t * out_local_cids);
    
    /**
     * @brief  Accept incoming connection Enhanced Credit-Based Flow-Control Mode
     * @param local_cid            from L2CAP_EVENT_INCOMING_DATA_CONNECTION
     * @param num_channels
     * @param initial_credits      Number of initial credits provided to peer per channel or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param receive_buffer_size
     * @param receive_buffers      Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @param out_local_cids       Array of L2CAP Channel Identifiers is stored here on success
     * @return status
     */
    uint8_t l2cap_ecbm_accept_channels(uint16_t local_cid, uint8_t num_channels, uint16_t initial_credits,
                                                uint16_t receive_buffer_size, uint8_t ** receive_buffers, uint16_t * out_local_cids);
    /**
     * @brief Decline connection in Enhanced Credit-Based Flow-Control Mode
     * @param local_cid           from L2CAP_EVENT_INCOMING_DATA_CONNECTION
     * @param result              See L2CAP_ECBM_CONNECTION_RESULT_ALL_SUCCESS in bluetooth.h
     * @return status
     */
    uint8_t l2cap_ecbm_decline_channels(uint16_t local_cid, uint16_t result);
    
    /**
     * @brief Provide credits for channel in Enhanced Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param credits               Number additional credits for peer
     * @return status
     */
    uint8_t l2cap_ecbm_provide_credits(uint16_t local_cid, uint16_t credits);
    
    /**
     * @brief Request emission of L2CAP_EVENT_ECBM_CAN_SEND_NOW as soon as possible
     * @note L2CAP_EVENT_ECBM_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param local_cid             L2CAP Channel Identifier
     * @return status
     */
    uint8_t l2cap_ecbm_request_can_send_now_event(uint16_t local_cid);
    
    /**
     * @brief Reconfigure MPS/MTU of local channels
     * @param num_cids
     * @param local_cids            array of local_cids to reconfigure
     * @param receive_buffer_size   buffer size equals MTU
     * @param receive_buffers       Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @return status
     */
    uint8_t l2cap_ecbm_reconfigure_channels(uint8_t num_cids, uint16_t * local_cids, int16_t receive_buffer_size, uint8_t ** receive_buffers);
    
    /**
     * @brief Trigger pending connection responses after pairing completed
     * @note Must be called after receiving an SM_PAIRING_COMPLETE event, will be removed eventually
     * @param con_handle
     */
    void l2cap_ecbm_trigger_pending_connection_responses(hci_con_handle_t con_handle);
    
    /**
     * @brief Returns the number of outgoing credits provided by peer
     * @param local_cid
     * @return number of credits
     */
    uint16_t l2cap_ecbm_available_credits(uint16_t local_cid);
    
    /**
     * @brief De-Init L2CAP
     */
    void l2cap_deinit(void);
    
