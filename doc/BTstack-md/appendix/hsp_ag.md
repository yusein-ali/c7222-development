
# HSP Audio Gateway API {#sec:hsp_ag_api}


**src/classic/hsp_ag.h** 


    
    /**
     * @brief Set up HSP AG.
     * @param rfcomm_channel_nr
     */
    void hsp_ag_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Create HSP Audio Gateway (AG) SDP service record. 
     * @param service Empty buffer in which a new service record will be stored.
     * @param service_record_handle
     * @param rfcomm_channel_nr
     * @param name
     */
    void hsp_ag_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name);
    
    /**
     * @brief Register packet handler to receive HSP AG events.
    
     * The HSP AG event has type HCI_EVENT_HSP_META with following subtypes:  
     * - HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE
     * - HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE
     * - HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE    
     * - HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE                       
     * - HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED      
     * - HSP_SUBEVENT_SPEAKER_GAIN_CHANGED         
     * - HSP_SUBEVENT_HS_COMMAND      
     *
     * @param callback 
     */
    void hsp_ag_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to HSP Headset.
     *
     * Perform SDP query for an RFCOMM service on a remote device, 
     * and establish an RFCOMM connection if such service is found. Reception of the  
     * HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully established. 
     *
     * @param bd_addr
     */
    void hsp_ag_connect(bd_addr_t bd_addr);
    
    /**
     * @brief Disconnect from HSP Headset
     *
     * Reception of the HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_ag_disconnect(void);
    
    
    /**
     * @brief Establish audio connection.
     * 
     * Reception of the HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE with status 0
     * indicates if the audio connection is successfully established. 
     * @param bd_addr
     */
    void hsp_ag_establish_audio_connection(void);
    
    /**
     * @brief Release audio connection.
     *
     * Reception of the HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_ag_release_audio_connection(void);
    
    /**
     * @brief Set microphone gain. 
     * @param gain Valid range: [0,15]
     */
    void hsp_ag_set_microphone_gain(uint8_t gain);
    
    /**
     * @brief Set speaker gain. 
     * @param gain Valid range: [0,15]
     */
    void hsp_ag_set_speaker_gain(uint8_t gain);
    
    /**
     * @brief Start ringing because of incoming call.
     */
    void hsp_ag_start_ringing(void);
    
    /**
     * @brief Stop ringing (e.g. call was terminated).
     */
    void hsp_ag_stop_ringing(void);
    
    /**
     * @brief Enable custom AT commands.
     * 
     * Custom commands are disabled by default. 
     * When enabled, custom AT commands are received via the HSP_SUBEVENT_HS_COMMAND.
     * @param enable
     */
    void hsp_ag_enable_custom_commands(int enable);
    
    /**
     * @brief Send a custom AT command to HSP Headset.
     *
     * On HSP_SUBEVENT_AG_INDICATION, the client needs to respond
     * with this function with the result to the custom command.
     * @param result 
     */
    int hsp_ag_send_result(char * result);
    
    /**
     * @brief Set packet types used for outgoing SCO connection requests
     * @param common single packet_types: SCO_PACKET_TYPES_*
     */
    void hsp_ag_set_sco_packet_types(uint16_t packet_types);
    
    /**
     * @brief De-Init HSP AG
     */
    void hsp_ag_deinit(void);
    
