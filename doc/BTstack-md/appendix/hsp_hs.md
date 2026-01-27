
# HSP Headset API {#sec:hsp_hs_api}


**src/classic/hsp_hs.h** 


    
    /**
     * @brief Set up HSP HS.
     * @param rfcomm_channel_nr
     */
    void hsp_hs_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Create HSP Headset (HS) SDP service record. 
     * @param service Empty buffer in which a new service record will be stored.
     * @param rfcomm_channel_nr
     * @param name
     * @param have_remote_audio_control 
     */
    void hsp_hs_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint8_t have_remote_audio_control);
    
    /**
     * @brief Register packet handler to receive HSP HS events.
     *
     * The HSP HS event has type HCI_EVENT_HSP_META with following subtypes:                      
     * - HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE
     * - HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE
     * - HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE    
     * - HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE 
     * - HSP_SUBEVENT_RING                         
     * - HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED      
     * - HSP_SUBEVENT_SPEAKER_GAIN_CHANGED         
     * - HSP_SUBEVENT_AG_INDICATION     
     *
     * @param callback 
     */
    void hsp_hs_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to HSP Audio Gateway.
     *
     * Perform SDP query for an RFCOMM service on a remote device, 
     * and establish an RFCOMM connection if such service is found. Reception of the  
     * HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully established. 
     *
     * @param bd_addr
     */
    void hsp_hs_connect(bd_addr_t bd_addr);
    
    /**
     * @brief Disconnect from HSP Audio Gateway
     *
     * Releases the RFCOMM channel. Reception of the  
     * HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_hs_disconnect(void);
    
    
    /**
     * @brief Send button press action. Toggle establish/release of audio connection. 
     */
    void hsp_hs_send_button_press(void);
    
    /**
     * @brief Triger establishing audio connection.
     * 
     * Reception of the HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE with status 0
     * indicates if the audio connection is successfully established. 
     * @param bd_addr
     */
    void hsp_hs_establish_audio_connection(void);
    
    /**
     * @brief Trigger releasing audio connection.
     *
     * Reception of the HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_hs_release_audio_connection(void);
    
    /**
     * @brief Set microphone gain. 
     * 
     * The new gain value will be confirmed by the HSP Audio Gateway. 
     * A HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED event will be received.
     * @param gain Valid range: [0,15]
     */
    void hsp_hs_set_microphone_gain(uint8_t gain);
    
    /**
     * @brief Set speaker gain. 
     * 
     * The new gain value will be confirmed by the HSP Audio Gateway. 
     * A HSP_SUBEVENT_SPEAKER_GAIN_CHANGED event will be received.
     * @param gain - valid range: [0,15]
     */
    void hsp_hs_set_speaker_gain(uint8_t gain);
    
    
    
    /**
     * @brief Enable custom indications.
     * 
     * Custom indications are disabled by default. 
     * When enabled, custom indications are received via the HSP_SUBEVENT_AG_INDICATION.
     * @param enable
     */
    void hsp_hs_enable_custom_indications(int enable);
    
    /**
     * @brief Send answer to custom indication.
     *
     * On HSP_SUBEVENT_AG_INDICATION, the client needs to respond
     * with this function with the result to the custom indication
     * @param result 
     */
    int hsp_hs_send_result(const char * result);
    
    /**
     * @brief Set packet types used for incoming SCO connection requests
     * @param common single packet_types: SCO_PACKET_TYPES_*
     */
    void hsp_hs_set_sco_packet_types(uint16_t packet_types);
    
    /**
     * @brief De-Init HSP AG
     */
    void hsp_hs_deinit(void);
    
