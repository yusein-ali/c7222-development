
# Lienar Resampling API {#sec:btstack_resample_api}


**src/btstack_resample.h** : 
Linear resampling for 16-bit audio code samples using 16 bit/16 bit fixed point math.



    
    /**
     * @brief Init resample context
     * @param num_channels
     * @return btstack_audio implementation
     */
    void btstack_resample_init(btstack_resample_t * context, int num_channels);
    
    /**
     * @brief Set resampling factor
     * @param factor as fixed point value, identity is 0x10000
     */
    void btstack_resample_set_factor(btstack_resample_t * context, uint32_t factor);
    
    /**
     * @brief Process block of input samples
     * @note size of output buffer is not checked
     * @param input_buffer
     * @param num_frames
     * @param output_buffer
     * @return number destination frames
     */
    uint16_t btstack_resample_block(btstack_resample_t * context, const int16_t * input_buffer, uint32_t num_frames, int16_t * output_buffer);
    
