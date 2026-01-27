
# PBAP Client API {#sec:pbap_client_api}


**src/classic/pbap_client.h** 


    
    /**
     * Setup PhoneBook Access Client
     */
    void pbap_client_init(void);
    
    /**
     * @brief Create PBAP connection to a Phone Book Server (PSE) server on a remote device.
     * If the server requires authentication, a PBAP_SUBEVENT_AUTHENTICATION_REQUEST is emitted, which
     * can be answered with pbap_authentication_password(..).
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event,
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     *
     * This function allows for multiple parallel connections.
     *
     * @param client storage for connection state. Must stay valid until connection closes
     * @param l2cap_ertm_config
     * @param l2cap_ertm_buffer_size
     * @param l2cap_ertm_buffer
     * @param handler
     * @param addr
     * @param out_cid to use for further commands
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_MEMORY_ALLOC_FAILED if PBAP or GOEP connection already exists.
     */
    
    uint8_t pbap_client_connect(pbap_client_t * client, l2cap_ertm_config_t *l2cap_ertm_config, uint8_t *l2cap_ertm_buffer,
                                uint16_t l2cap_ertm_buffer_size, btstack_packet_handler_t handler, bd_addr_t addr, uint16_t * out_cid);
    
    /**
     * @brief Create PBAP connection to a Phone Book Server (PSE) server on a remote device. 
     * If the server requires authentication, a PBAP_SUBEVENT_AUTHENTICATION_REQUEST is emitted, which
     * can be answered with pbap_authentication_password(..).
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event, 
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     *
     * This function uses a single pbap_client_t and l2cap ertm buffer instance and can only be used for a single connection.
     * Fur multiple parallel connections, use pbap_client_connect.
     *
     * @param handler 
     * @param addr
     * @param out_cid to use for further commands
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_MEMORY_ALLOC_FAILED if PBAP or GOEP connection already exists.  
     */
    uint8_t pbap_connect(btstack_packet_handler_t handler, bd_addr_t addr, uint16_t * out_cid);
    
    /**
     * Create SDP Record for Phonebook Access Client
     * @param service
     * @param service_record_handle
     * @param service_name
     */
    void pbap_client_create_sdp_record(uint8_t *service, uint32_t service_record_handle, const char *service_name);
    
    /**
     * @brief Provide password for OBEX Authentication after receiving PBAP_SUBEVENT_AUTHENTICATION_REQUEST.
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event, 
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     * 
     * @param pbap_cid
     * @param password (null terminated string) - not copied, needs to stay valid until connection completed
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_authentication_password(uint16_t pbap_cid, const char * password);
    
    /** 
     * @brief Disconnects PBAP connection with given identifier.
     * Event PBAP_SUBEVENT_CONNECTION_CLOSED indicates that PBAP connection is closed.
     *
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_disconnect(uint16_t pbap_cid);
    
    /** 
     * @brief Set current folder on PSE. The status is reported via PBAP_SUBEVENT_OPERATION_COMPLETED event. 
     * 
     * @param pbap_cid
     * @param path - note: path is not copied
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_phonebook(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Set vCard Selector for get/pull phonebook. No event is emitted.
     * 
     * @param pbap_cid
     * @param vcard_selector - combination of PBAP_PROPERTY_MASK_* properties
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_vcard_selector(uint16_t pbap_cid, uint32_t vcard_selector);
    
    /**
     * @brief Set vCard Selector for get/pull phonebook. No event is emitted.
     * 
     * @param pbap_cid
     * @param vcard_selector_operator - PBAP_VCARD_SELECTOR_OPERATOR_OR (default) or PBAP_VCARD_SELECTOR_OPERATOR_AND
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_vcard_selector_operator(uint16_t pbap_cid, int vcard_selector_operator);
    
    /**
     * @brief Set Property Selector. No event is emitted.
     * @param pbap_cid
     * @param property_selector - combination of PBAP_PROPERTY_MASK_* properties
     * @return
     */
    uint8_t pbap_set_property_selector(uint16_t pbap_cid, uint32_t property_selector);
    
    /**
     * @brief Set number of items returned by pull phonebook.
     * @param pbap_cid
     * @param max_list_count
     * @return
     */
    uint8_t pbap_set_max_list_count(uint16_t pbap_cid, uint16_t max_list_count);
    
    /**
     * @bbrief Set start offset for pull phonebook
     * @param pbap_cid
     * @param list_start_offset
     * @return
     */
    uint8_t pbap_set_list_start_offset(uint16_t pbap_cid, uint16_t list_start_offset);
    
    /**
     * @bbrief Set order for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param order
     * @return
     */
    uint8_t pbap_set_order(uint16_t pbap_cid, uint8_t order);
    
    /**
     * @bbrief Set search property for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param search_property
     * @return
     */
    uint8_t pbap_set_search_property(uint16_t pbap_cid, uint8_t search_property);
    
    /**
     * @bbrief Set search property for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param search_value
     * @return
     */
    uint8_t pbap_set_search_value(uint16_t pbap_cid, const char * search_value);
    
    /**
     * @brief Get size of phone book from PSE. The result is reported via PBAP_SUBEVENT_PHONEBOOK_SIZE event. 
     * @note This function uses the PullPhoneBook command
     *
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_get_phonebook_size(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull phone book from PSE. The result is reported via registered packet handler (see pbap_connect function),
     * with packet type set to PBAP_DATA_PACKET. Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the phone book. 
     * 
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_phonebook(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Get size of phone book from PSE. The result is reported via PBAP_SUBEVENT_PHONEBOOK_SIZE event.
     *
     * @note This function uses the PullvCardListing command
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_get_vcard_listing_size(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull vCard listing. vCard data is emitted via PBAP_SUBEVENT_CARD_RESULT event. 
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of vCard listing.
     *
     * @param pbap_cid
     * @param path
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_vcard_listing(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull vCard entry. The result is reported via callback (see pbap_connect function),
     * with packet type set to PBAP_DATA_PACKET.
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the vCard entry.
     * 
     * @param pbap_cid
     * @param path
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_vcard_entry(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Lookup contact(s) by phone number. vCard data is emitted via PBAP_SUBEVENT_CARD_RESULT event. 
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the lookup.
     *
     * @param pbap_cid
     * @param phone_number
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_lookup_by_number(uint16_t pbap_cid, const char * phone_number);
    
    /**
     * @brief Abort current operation. No event is emitted.
     * 
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_abort(uint16_t pbap_cid);
    
    
    /**
     * @brief Set flow control mode - default is off. No event is emitted.
     * @note When enabled, pbap_next_packet needs to be called after a packet was processed to receive the next one
     *
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_flow_control_mode(uint16_t pbap_cid, int enable);
        
    /**
     * @brief Trigger next packet from PSE when Flow Control Mode is enabled.
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_next_packet(uint16_t pbap_cid);
    
    /**
     * @brief De-Init PBAP Client
     */
    void pbap_client_deinit(void);
    
