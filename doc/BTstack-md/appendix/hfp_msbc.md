
# HFP mSBC Encoder API {#sec:hfp_msbc_api}


**src/classic/hfp_msbc.h** 


    
    /**
     *
     */
    void hfp_msbc_init(void);
    
    /**
     *
     */
    int  hfp_msbc_num_audio_samples_per_frame(void);
    
    /**
     *
     */
    int  hfp_msbc_can_encode_audio_frame_now(void);
    
    /**
     * @param pcm_samples - complete audio frame of hfp_msbc_num_audio_samples_per_frame int16 samples
     */
    void hfp_msbc_encode_audio_frame(int16_t * pcm_samples);
    
    /**
     *
     */
    int  hfp_msbc_num_bytes_in_stream(void);
    
    /**
     *
     * @param buffer to store stream
     * @param size num bytes to read from stream
     */
    void hfp_msbc_read_from_stream(uint8_t * buffer, int size);
    
    /**
     * @brief De-Init HFP mSBC Codec
     */
    void hfp_msbc_deinit(void);
    
