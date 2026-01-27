
# Audio Interface API {#sec:btstack_audio_api}


**src/btstack_audio.h** : 
Abstraction layer for 16-bit audio playback and recording within BTstack.

Most embedded implementations, e.g. the one for ESP32, use a single I2S interface which
requires that the sample rate is the same for sink and source roles


    
    typedef struct {
        btstack_time_us_t timestamp;
    } btstack_audio_context_t;
    
    typedef struct {
    
        /**
         * @brief Setup audio codec for specified samplerate and number of channels
         * @param Channels (1=mono, 2=stereo)
         * @param Sample rate
         * @param Playback callback with sample buffer, number of samples, and, if available playback time of first sample
         * @return 0 on success
         */
        int (*init)(uint8_t channels,
                    uint32_t samplerate, 
                    void (*playback) (int16_t *  buffer,
                                      uint16_t num_samples,
                                      const btstack_audio_context_t * timeinfo));
    
        /**
         * @brief Get the current playback sample rate, may differ from the
         *        specified sample rate
         */
        uint32_t (*get_samplerate)(void);
    
        /**
         * @brief Set volume
         * @param Volume 0..127
         */
        void (*set_volume)(uint8_t volume);
    
        /**
         * @brief Start stream
         */
        void (*start_stream)(void);
    
        /** 
         * @brief Stop stream
         */
        void (*stop_stream)(void);
    
        /**
         * @brief Close audio codec
         */
        void (*close)(void);
    
    } btstack_audio_sink_t;
    
    
    typedef struct {
    
        /**
         * @brief Setup audio codec for specified samplerate and number of channels
         * @param Channels (1=mono, 2=stereo)
         * @param Sample rate
         * @param Recording callback with sample buffer, number of samples, and, if available recording time of first sample
         * @return 0 on success
         */
        int (*init)(uint8_t channels,
                    uint32_t samplerate, 
                    void (*recording)(const int16_t * buffer,
                                      uint16_t num_samples,
                                      const btstack_audio_context_t * timeinfo));
    
        /**
         * @brief Get the current recording sample rate, may differ from the
         *        specified sameple rate
         */
        uint32_t (*get_samplerate)(void);
    
        /**
         * @brief Set Gain
         * @param Gain 0..127
         */
        void (*set_gain)(uint8_t gain);
    
        /**
         * @brief Start stream
         */
        void (*start_stream)(void);
    
        /** 
         * @brief Stop stream
         */
        void (*stop_stream)(void);
    
        /**
         * @brief Close audio codec
         */
        void (*close)(void);
    
    } btstack_audio_source_t;
    
    
    /**
     * @brief Get BTstack Audio Sink Instance
     * @return btstack_audio_sink implementation
     */
    const btstack_audio_sink_t * btstack_audio_sink_get_instance(void);
    
    /**
     * @brief Get BTstack Audio Source Instance
     * @return btstack_audio_source implementation
     */
    const btstack_audio_source_t * btstack_audio_source_get_instance(void);
    
    
    /**
     * @brief Get BTstack Audio Sink Instance
     * @param btstack_audio_sink implementation
     */
    void btstack_audio_sink_set_instance(const btstack_audio_sink_t * audio_sink_impl);
    
    /**
     * @brief Get BTstack Audio Source Instance
     * @param btstack_audio_source implementation
     */
    void btstack_audio_source_set_instance(const btstack_audio_source_t * audio_source_impl);
    
    
    // common implementations
    const btstack_audio_sink_t *   btstack_audio_portaudio_sink_get_instance(void);
    const btstack_audio_source_t * btstack_audio_portaudio_source_get_instance(void);
    
    const btstack_audio_sink_t *   btstack_audio_alsa_sink_get_instance(void);
    
    const btstack_audio_sink_t *   btstack_audio_embedded_sink_get_instance(void);
    const btstack_audio_source_t * btstack_audio_embedded_source_get_instance(void);
    
    const btstack_audio_sink_t *    btstack_audio_esp32_sink_get_instance(void);
    const btstack_audio_source_t *  btstack_audio_esp32_source_get_instance(void);
    
    // platform-specific extension
    void btstack_audio_portaudio_sink_set_device(const char * device_name);
    void btstack_audio_portaudio_source_set_device(const char * device_name);
    
