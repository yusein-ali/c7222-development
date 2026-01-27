
# AVRCP Browsing Target API {#sec:avrcp_browsing_target_api}


**src/classic/avrcp_browsing_target.h** 


    
    /**
     * @brief Set up AVRCP Browsing Target client.
     */
    void avrcp_browsing_target_init(void);
    
    /**
     * @brief Register callback for the AVRCP Browsing Target client to receive events:
     * - AVRCP_SUBEVENT_BROWSING_SEARCH
     * - AVRCP_SUBEVENT_BROWSING_GET_FOLDER_ITEMS
     * - AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS
     * - AVRCP_SUBEVENT_BROWSING_SET_BROWSED_PLAYER
     * - AVRCP_SUBEVENT_BROWSING_CHANGE_PATH
     * - AVRCP_SUBEVENT_BROWSING_GET_ITEM_ATTRIBUTES
     *  
     * @param callback
     */
    void avrcp_browsing_target_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Accept set browsed player
     * @param browsing_cid
     * @param uid_counter
     * @param browsed_player_id
     * @param response
     * @param response_size
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_accept_set_browsed_player(uint16_t browsing_cid, uint16_t uid_counter, uint16_t browsed_player_id, uint8_t * response, uint16_t response_len);
    
    /**
     * @brief Reject set browsed player
     * @param browsing_cid
     * @param status
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_reject_set_browsed_player(uint16_t browsing_cid, avrcp_status_code_t status);
    
    /**
     * @brief Send answer to get folder items query on event AVRCP_SUBEVENT_BROWSING_GET_FOLDER_ITEMS. The first byte of this event defines the scope of the query, see avrcp_browsing_scope_t.
     * @param browsing_cid
     * @param uid_counter
     * @param attr_list
     * @param attr_list_size
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_folder_items_response(uint16_t browsing_cid, uint16_t uid_counter, uint8_t * attr_list, uint16_t attr_list_size, uint16_t num_items);
    
    /**
     * @brief Send answer to get total number of items query on event AVRCP_SUBEVENT_BROWSING_GET_TOTAL_NUM_ITEMS. The first byte of this event defines the scope of the query, see avrcp_browsing_scope_t.
     * @param browsing_cid
     * @param uid_counter
     * @param total_num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_total_num_items_response(uint16_t browsing_cid, uint16_t uid_counter, uint32_t total_num_items);
    
    /**
     * @brief Send answer to change path command on event AVRCP_SUBEVENT_BROWSING_CHANGE_PATH.
     * @param browsing_cid
     * @param status
     * @param num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_change_path_response(uint16_t browsing_cid, avrcp_status_code_t status, uint32_t num_items);
    
    /**
     * @brief Send answer to get item attribute response on event AVRCP_SUBEVENT_BROWSING_GET_ITEM_ATTRIBUTES.
     * @param browsing_cid
     * @param status
     * @param attr_list
     * @param attr_list_size
     * @param num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_item_attributes_response(uint16_t browsing_cid, avrcp_status_code_t status, uint8_t * attr_list, uint16_t attr_list_size, uint8_t num_items);
    
    /**
     * @brief Send answer to search query on event AVRCP_SUBEVENT_BROWSING_SEARCH.
     * @param browsing_cid
     * @param uid_counter
     * @param total_num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_search_response(uint16_t browsing_cid, avrcp_status_code_t status, uint16_t uid_counter, uint32_t num_items);
    
    /**
     * @brief De-Init AVRCP Browsing Target
     */
    void avrcp_browsing_target_deinit(void);
    
