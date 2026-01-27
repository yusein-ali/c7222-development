
# Host Controler Interface (HCI) API {#sec:hci_api}


**src/hci.h** 


    
        
    // HCI init and configuration
    
    
    /**
     * @brief Set up HCI. Needs to be called before any other function.
     */
    void hci_init(const hci_transport_t *transport, const void *config);
    
    /**
     * @brief Configure Bluetooth chipset driver. Has to be called before power on, or right after receiving the local version information.
     */
    void hci_set_chipset(const btstack_chipset_t *chipset_driver);
    
    /**
     * @brief Enable custom init for chipset driver to send HCI commands before HCI Reset
     */
    void hci_enable_custom_pre_init(void);
    
    /**
     * @brief Configure Bluetooth hardware control. Has to be called before power on.
     * @[aram hardware_control implementation
     */
    void hci_set_control(const btstack_control_t *hardware_control);
    
    #ifdef HAVE_SCO_TRANSPORT
    /**
     * @brief Set SCO Transport implementation for SCO over PCM mode
     * @param sco_transport that sends SCO over I2S or PCM interface
     */
    void hci_set_sco_transport(const btstack_sco_transport_t *sco_transport);
    #endif
    
    #ifdef ENABLE_CLASSIC
    /**
     * @brief Configure Bluetooth hardware control. Has to be called before power on.
     */
    void hci_set_link_key_db(btstack_link_key_db_t const * link_key_db);
    #endif
    
    /**
     * @brief Set callback for Bluetooth Hardware Error
     */
    void hci_set_hardware_error_callback(void (*fn)(uint8_t error));
    
    /**
     * @brief Set Public BD ADDR - passed on to Bluetooth chipset during init if supported in bt_control_h
     */
    void hci_set_bd_addr(bd_addr_t addr);
    
    /**
     * @brief Configure Voice Setting for use with SCO data in HSP/HFP
     */
    void hci_set_sco_voice_setting(uint16_t voice_setting);
    
    /**
     * @brief Get SCO Voice Setting
     * @return current voice setting
     */
    uint16_t hci_get_sco_voice_setting(void);
    
    /**
     * @brief Set number of ISO packets to buffer for BIS/CIS
     * @param num_packets (default = 1)
     */
    void hci_set_num_iso_packets_to_queue(uint8_t num_packets);
    
    /**
     * @brief Set inquiry mode: standard, with RSSI, with RSSI + Extended Inquiry Results. Has to be called before power on.
     * @param inquriy_mode see bluetooth_defines.h
     */
    void hci_set_inquiry_mode(inquiry_mode_t inquriy_mode);
    
    /**
     * @brief Requests the change of BTstack power mode.
     * @param power_mode
     * @return 0 if success, otherwise error
     */
    int  hci_power_control(HCI_POWER_MODE power_mode);
    
    /**
     * @brief Shutdown HCI
     */
    void hci_close(void);
    
    
    // Callback registration
    
    
    /**
     * @brief Add event packet handler. 
     */
    void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Remove event packet handler.
     */
    void hci_remove_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Registers a packet handler for ACL data. Used by L2CAP
     */
    void hci_register_acl_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief Registers a packet handler for SCO data. Used for HSP and HFP profiles.
     */
    void hci_register_sco_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief Registers a packet handler for ISO data. Used for LE Audio profiles
     */
    void hci_register_iso_packet_handler(btstack_packet_handler_t handler);
    
    // Sending HCI Commands
    
    /** 
     * @brief Check if CMD packet can be sent to controller
     * @return true if command can be sent
     */
    bool hci_can_send_command_packet_now(void);
    
    /**
     * @brief Creates and sends HCI command packets based on a template and a list of parameters. Will return error if outgoing data buffer is occupied.
     * @return status
     */
    uint8_t hci_send_cmd(const hci_cmd_t * cmd, ...);
    
    
    // Sending SCO Packets
    
    /** @brief Get SCO payload length for existing SCO connection and current SCO Voice setting
     *  @note  Using SCO packets of the exact length is required for USB transfer in general and some H4 controllers as well
     *  @param sco_con_handle
     *  @return Length of SCO payload in bytes (not audio frames) incl. 3 byte header
     */
    uint16_t hci_get_sco_packet_length_for_connection(hci_con_handle_t sco_con_handle);
    
    /** @brief Get SCO packet length for one of the existing SCO connections and current SCO Voice setting
     *  @deprecated Please use hci_get_sco_packet_length_for_connection instead
     *  @note  Using SCO packets of the exact length is required for USB transfer
     *  @return Length of SCO packets in bytes (not audio frames) incl. 3 byte header
     */
    uint16_t hci_get_sco_packet_length(void);
    
    /**
     * @brief Request emission of HCI_EVENT_SCO_CAN_SEND_NOW as soon as possible
     * @note HCI_EVENT_SCO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it.
     * @param con_handle
     */
    void hci_request_sco_can_send_now_event_for_con_handle(hci_con_handle_t con_handle);
    
    /**
     * @brief Request emission of HCI_EVENT_SCO_CAN_SEND_NOW as soon as possible
     * @deprecated Please use hci_request_sco_can_send_now_event_for_con_handle instead
     * @note HCI_EVENT_SCO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it. con_handle will
     *       be set to HCI_CON_HANDLE_INVALID in the event
     */
    void hci_request_sco_can_send_now_event(void);
    
    /**
     * @brief Check HCI packet buffer and if SCO packet can be sent to controller
     * @param con_handle
     * @return true if sco packet can be sent
     */
    bool hci_can_send_sco_packet_now_for_con_handle(hci_con_handle_t con_handle);
    
    /**
     * @brief Check HCI packet buffer and if SCO packet can be sent to controller
     * @deprecated Please use hci_can_send_sco_packet_now_for_con_handle instead
     * @return true if sco packet can be sent
     */
    bool hci_can_send_sco_packet_now(void);
    
    /**
     * @brief Check if SCO packet can be sent to controller
     * @return true if sco packet can be sent
     */
    bool hci_can_send_prepared_sco_packet_now(void);
    
    /**
     * @brief Send SCO packet prepared in HCI packet buffer
     */
    uint8_t hci_send_sco_packet_buffer(int size);
    
    /**
     * @brief Request emission of HCI_EVENT_BIS_CAN_SEND_NOW for all BIS as soon as possible
     * @param big_handle
     * @note HCI_EVENT_ISO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     */
    uint8_t hci_request_bis_can_send_now_events(uint8_t big_handle);
    
    /**
     * @brief Request emission of HCI_EVENT_CIS_CAN_SEND_NOW for all outgoing CIS of referenced CIG
     *
     * @note As CIG_IDs are only unique for Central, a CIS Connection Handle is used to identify the CIG.
     *       The group_complete field in HCI_EVENT_CIS_CAN_SEND_NOW can be used to request again.
     *
     * @note HCI_EVENT_CIS_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     *
     * @param cis_con_handle
     */
    uint8_t hci_request_cis_can_send_now_events(hci_con_handle_t cis_con_handle);
    
    /**
     * @brief Send ISO packet prepared in HCI packet buffer
     */
    uint8_t hci_send_iso_packet_buffer(uint16_t size);
    
    /**
     * Reserves outgoing packet buffer.
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     */
    void hci_reserve_packet_buffer(void);
    
    /**
     * Get pointer for outgoing packet buffer
     */
    uint8_t* hci_get_outgoing_packet_buffer(void);
    
    /**
     * Release outgoing packet buffer\
     * @note only called instead of hci_send_prepared
     */
    void hci_release_packet_buffer(void);
    
    /**
    * @brief Sets the master/slave policy
    * @param policy (0: attempt to become master, 1: let connecting device decide)
    */
    void hci_set_master_slave_policy(uint8_t policy);
    
    /**
     * @brief Check if Controller supports BR/EDR (Bluetooth Classic)
     * @return true if supported
     * @note only valid in working state
     */
    bool hci_classic_supported(void);
    
    /**
     * @brief Check if Controller supports LE (Bluetooth Low Energy)
     * @return true if supported
     * @note only valid in working state
     */
    bool hci_le_supported(void);
    
    /**
     * @brief Check if LE Extended Advertising is supported
     * @return true if supported
     */
    bool hci_le_extended_advertising_supported(void);
    
    /** @brief Check if address type corresponds to LE connection
     *  @bparam address_type
     *  @erturn true if LE connection
     */
    bool hci_is_le_connection_type(bd_addr_type_t address_type);
    
    /** @brief Check if address type corresponds to Identity Address
     *  @bparam address_type
     *  @erturn true if LE connection
     */
    bool hci_is_le_identity_address_type(bd_addr_type_t address_type);
    
