
# A2DP Source API {#sec:a2dp_source_api}


**src/classic/a2dp_source.h** : 
Audio/Video Distribution Transport Protocol A2DP Source is a device that streames media data.



    
    /**
     * @brief Create A2DP Source service record. 
     * @param service
     * @param service_record_handle
     * @param supported_features 16-bit bitmap, see AVDTP_SOURCE_SF_* values in avdtp.h
     * @param service_name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param service_provider_name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void a2dp_source_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);
    
    /**
     * @brief Initialize up A2DP Source device.
     */
    void a2dp_source_init(void);
    
    /**
     * @brief Create a stream endpoint of type SOURCE, and register media codec by specifying its capabilities and the default configuration.
     * @param media_type    			See avdtp_media_type_t values in avdtp.h (audio, video or multimedia).
     * @param media_codec_type 			See avdtp_media_codec_type_t values in avdtp.h 
     * @param codec_capabilities        Media codec capabilities as defined in A2DP spec, section 4 - Audio Codec Interoperability Requirements.
     * @param codec_capabilities_len	Media codec capabilities length.
     * @param codec_configuration 		Default media codec configuration.
     * @param codec_configuration_len	Media codec configuration length. 
     *
     * @return local_stream_endpoint 				
     */
    avdtp_stream_endpoint_t * a2dp_source_create_stream_endpoint(avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type,
                                                                 const uint8_t *codec_capabilities, uint16_t codec_capabilities_len,
                                                                 uint8_t * codec_configuration, uint16_t codec_configuration_len);
    
    /**
     *  @brief Unregister stream endpoint and free it's memory
     *  @param stream_endpoint created by a2dp_source_create_stream_endpoint
     */
    void a2dp_source_finalize_stream_endpoint(avdtp_stream_endpoint_t * stream_endpoint);
    
    /**
     * @brief Register callback for the A2DP Source client. It will receive following subevents of HCI_EVENT_A2DP_META HCI event type: 
     * - A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW:			Indicates that the next media packet can be sent.
     *
     * - A2DP_SUBEVENT_SIGNALING_CONNECTION_ESTABLISHED             Received when signaling connection with a remote is established.
     * - A2DP_SUBEVENT_SIGNALING_CONNECTION_RELEASED                Received when signaling connection with a remote is released
     * - A2DP_SUBEVENT_STREAM_ESTABLISHED                           Received when stream to a remote device is established.
     * - A2DP_SUBEVENT_STREAM_STARTED                               Received when stream is started.
     * - A2DP_SUBEVENT_STREAM_SUSPENDED                             Received when stream is paused.
     * - A2DP_SUBEVENT_STREAM_STOPED                                received when stream is aborted or stopped.
     * - A2DP_SUBEVENT_STREAM_RELEASED                              Received when stream is released.
     * - A2DP_SUBEVENT_SIGNALING_DELAY_REPORTING_CAPABILITY         Currently the only capability that is passed to client
     * - A2DP_SUBEVENT_SIGNALING_CAPABILITIES_DONE                  Signals that all capabilities are reported
     * - A2DP_SUBEVENT_SIGNALING_DELAY_REPORT                       Delay report
     * - A2DP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION      SBC configuration
     * - A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW          Signals that a media packet can be sent
     * - A2DP_SUBEVENT_COMMAND_REJECTED                             Command reject
     * @param callback
     */
    void a2dp_source_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Open stream.
     * @param remote_addr
     * @param avdtp_cid 		Assigned A2DP channel identifier used for further A2DP commands.
     */
    uint8_t a2dp_source_establish_stream(bd_addr_t remote_addr, uint16_t *avdtp_cid);
    
        /**
         * @brief Reconfigure stream.
         * @param local_seid	 	  ID assigned to a local stream endpoint
         * @param sampling_frequency  New sampling frequency to use. Cannot be called while stream is active
         */
    uint8_t a2dp_source_reconfigure_stream_sampling_frequency(uint16_t a2dp_cid, uint32_t sampling_frequency);
    
    /**
     * @brief Start stream.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid	 	ID of a local stream endpoint.
     */
    uint8_t a2dp_source_start_stream(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Pause stream.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     */
    uint8_t a2dp_source_pause_stream(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Release stream and disconnect from remote. 
     * @param a2dp_cid 			A2DP channel identifier.
     */
    uint8_t a2dp_source_disconnect(uint16_t a2dp_cid);
    
    /**
     * @brief Request to send a media packet. Packet can be then sent on reception of A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW event.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     */
    void a2dp_source_stream_endpoint_request_can_send_now(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Return maximal media payload size, does not include media header.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @return max_media_payload_size_without_media_header
     */
    int a2dp_max_media_payload_size(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Send media payload.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param marker
     * @param timestamp         in sample rate units
     * @param payload
     * @param payload_size
     * @param marker
     * @return status
     */
    uint8_t a2dp_source_stream_send_media_payload_rtp(uint16_t a2dp_cid, uint8_t local_seid, uint8_t marker, uint32_t timestamp,
                                              uint8_t *payload, uint16_t payload_size);
    
    /**
     * @brief Send media packet
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param packet
     * @param size
     * @return status
     */
    uint8_t a2dp_source_stream_send_media_packet(uint16_t a2dp_cid, uint8_t local_seid, const uint8_t * packet, uint16_t size);
    
    /**
     * @brief Select and configure SBC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     SBC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_sbc(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_sbc_t * configuration);
    
    /**
     * @brief Select and configure MPEG AUDIO endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AUDIO Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_mpeg_audio(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_mpeg_audio_t * configuration);
    
    /**
     * @brief Select and configure MPEG AAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AAC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_mpeg_aac(uint16_t a2dp_cid,  uint8_t local_seid,  uint8_t remote_seid, const avdtp_configuration_mpeg_aac_t * configuration);
    
    /**
     * @brief Select and configure ATRAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     ATRAC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_atrac(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_atrac_t * configuration);
    
    /**
     * @brief Select and configure Non-A2DP endpoint. Bytes 0-3 of codec information contain Vendor ID, bytes 4-5 contain Vendor Specific Codec ID (little endian)
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param media_codec_information
     * @param media_codec_information_len
     * @return status
     */
    uint8_t a2dp_source_set_config_other(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const uint8_t * media_codec_information, uint8_t media_codec_information_len);
    
    /**
     * @brief Register media configuration validator. Can reject insuitable configuration or report stream endpoint as currently busy
     * @note validator has to return AVDTP error codes like: AVDTP_ERROR_CODE_SEP_IN_USE or AVDTP_ERROR_CODE_UNSUPPORTED_CONFIGURATION
     *       the callback receives the media configuration in the same format as the existing A2dP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION
     *       and similar
     * @param callback
     */
    void a2dp_source_register_media_config_validator(uint8_t (*callback)(const avdtp_stream_endpoint_t * stream_endpoint, const uint8_t * event, uint16_t size));
    
    /**
     * @brief De-Init A2DP Source device.
     */
    void a2dp_source_deinit(void);
    
