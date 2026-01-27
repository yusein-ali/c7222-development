
# AVRCP Cover Art Client API {#sec:avrcp_cover_art_client_api}


**src/classic/avrcp_cover_art_client.h** 


    
    /**
     * @brief Set up AVRCP Cover Art client
     */
    void avrcp_cover_art_client_init(void);
    
    /**
     * @brief   Connect to AVRCP Cover Art service on a remote device, emits AVRCP_SUBEVENT_COVER_ART_CONNECTION_ESTABLISHED with status
     * @param   packet_handler
     * @param   remote_addr
     * @param   ertm_buffer
     * @param   ertm_buffer_size
     * @param   ertm_config
     * @param   avrcp_cover_art_cid  outgoing parameter, valid if status == ERROR_CODE_SUCCESS
     * @return status     
     */
    uint8_t avrcp_cover_art_client_connect(avrcp_cover_art_client_t *cover_art_client, btstack_packet_handler_t packet_handler,
                                   bd_addr_t remote_addr, uint8_t *ertm_buffer, uint32_t ertm_buffer_size,
                                   const l2cap_ertm_config_t *ertm_config, uint16_t *avrcp_cover_art_cid);
    
    /**
     * @brief Request cover art thumbnail for cover with a given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_linked_thumbnail(uint16_t avrcp_cover_art_cid, const char * image_handle);
    
    /**
     * @brief Request cover art image for given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     *        and given image descriptor
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @param image_descriptor
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_image(uint16_t avrcp_cover_art_cid, const char * image_handle, const char * image_descriptor);
    
    /**
     * @brief Request image properties for given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_image_properties(uint16_t avrcp_cover_art_cid, const char * image_handle);
    
    /**
     * @brief   Disconnect from AVRCP Cover Art service
     * @param   avrcp_cover_art_cid
     * @return status
     */
    uint8_t avrcp_cover_art_client_disconnect(uint16_t avrcp_cover_art_cid);
    
    /**
     * @brief De-Init AVRCP Cover Art Client
     */
    void avrcp_cover_art_client_deinit(void);
    
