
# HFP Audio Encoder API {#sec:hfp_codec_api}


**src/classic/hfp_codec.h** : @brief Create SCO packet with H2 Synchronization Header and encoded audio samples


    
    typedef struct hfp_codec hfp_codec_t;
    
    #ifdef ENABLE_HFP_WIDE_BAND_SPEECH
    /**
     * @brief Initialize HFP Audio Codec for mSBC
     * @param hfp_codec
     * @param msbc_encoder_context for msbc encoder
     * @return status
     */
    void hfp_codec_init_msbc_with_codec(hfp_codec_t * hfp_codec, const btstack_sbc_encoder_t * sbc_encoder, void * sbc_encoder_context);
    
    /**
     * @brief Initialize HFP Audio Codec for mSBC
     * @deprecated Please use hfp_codec_init_msbc_with_codec
     * @param hfp_codec
     * @param msbc_encoder_context for msbc encoder
     * @return status
     */
    void hfp_codec_init_msbc(hfp_codec_t * hfp_codec, btstack_sbc_encoder_state_t * msbc_encoder_context);
    #endif
    
    #ifdef ENABLE_HFP_SUPER_WIDE_BAND_SPEECH
    /**
     * @brief Initialize HFP Audio Codec for LC3-SWB
     * @param hfp_codec
     * @param lc3_encoder
     * @param lc3_encoder_context for lc3_encoder
     * @return status
     */
    void hfp_codec_init_lc3_swb(hfp_codec_t * hfp_codec, const btstack_lc3_encoder_t * lc3_encoder, void * lc3_encoder_context);
    #endif
    
    /**
     * @brief Get number of audio samples per HFP SCO frame
     * @param hfp_codec
     * @return num audio samples per 7.5ms SCO packet
     */
    uint16_t hfp_codec_num_audio_samples_per_frame(const hfp_codec_t * hfp_codec);
    
    /**
     * @brief Checks if next frame can be encoded
     * @param hfp_codec
     */
    bool hfp_codec_can_encode_audio_frame_now(const hfp_codec_t * hfp_codec);
    
    /**
     * Get number of bytes ready for sending
     * @param hfp_codec
     * @return num bytes ready for current packet
     */
    uint16_t hfp_codec_num_bytes_available(const hfp_codec_t * hfp_codec);
    
    /**
     * Encode audio samples for HFP SCO packet
     * @param hfp_codec
     * @param pcm_samples - complete audio frame of hfp_msbc_num_audio_samples_per_frame int16 samples
     */
    void hfp_codec_encode_audio_frame(hfp_codec_t * hfp_codec, int16_t * pcm_samples);
    
    /**
     * Read from stream into SCO packet buffer
     * @param hfp_codec
     * @param buffer to store stream
     * @param size num bytes to read from stream
     */
    void hfp_codec_read_from_stream(hfp_codec_t * hfp_codec, uint8_t * buffer, uint16_t size);
    
    /**
     * @param hfp_codec
     */
    void hfp_codec_deinit(hfp_codec_t * hfp_codec);
    
