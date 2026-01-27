
# LC3 Interface API {#sec:btstack_lc3_api}


**src/btstack_lc3.h** : 
Interface for LC3 implementations



    
    typedef enum {
        BTSTACK_LC3_FRAME_DURATION_10000US,
        BTSTACK_LC3_FRAME_DURATION_7500US
    } btstack_lc3_frame_duration_t;
    
    typedef struct {
    
        /**
         * Configure Decoder
         * @param context
         * @param sample_rate
         * @param frame_duration
         * @param octets_per_frame
         * @return status
         */
        uint8_t (*configure)(void * context, uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration, uint16_t octets_per_frame);
    
        /**
         * Decode LC3 Frame into signed 16-bit samples
         * @param context
         * @param bytes
         * @param BFI Bad Frame Indication flags
         * @param pcm_out buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param BEC_detect Bit Error Detected flag
         * @return status
         */
         uint8_t (*decode_signed_16)(void * context, const uint8_t *bytes, uint8_t BFI,
                           int16_t* pcm_out, uint16_t stride, uint8_t * BEC_detect);
    
        /**
         * Decode LC3 Frame into signed 24-bit samples, sign-extended to 32-bit
         * @param context
         * @param bytes
         * @param BFI Bad Frame Indication flags
         * @param pcm_out buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param BEC_detect Bit Error Detected flag
         * @return status
         */
        uint8_t (*decode_signed_24)(void * context, const uint8_t *bytes, uint8_t BFI,
                                    int32_t* pcm_out, uint16_t stride, uint8_t * BEC_detect);
    
    } btstack_lc3_decoder_t;
    
    typedef struct {
        /**
         * Configure Decoder
         * @param context
         * @param sample_rate
         * @param frame_duration
         * @param octets_per_frame
         * @return status
         */
        uint8_t (*configure)(void * context, uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration, uint16_t octets_per_frame);
    
        /**
         * Encode LC3 Frame with 16-bit signed PCM samples
         * @param context
         * @param pcm_in buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param bytes
         * @return status
         */
        uint8_t (*encode_signed_16)(void * context, const int16_t* pcm_in, uint16_t stride, uint8_t *bytes);
    
        /**
         * Encode LC3 Frame with 24-bit signed PCM samples, sign-extended to 32 bit
         * @param context
         * @param pcm_in buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param bytes
         * @return status
         */
        uint8_t (*encode_signed_24)(void * context, const int32_t* pcm_in, uint16_t stride, uint8_t *bytes);
    
    } btstack_lc3_encoder_t;
    
    /**
     * @brief Map enum to ISO Interval in us
     * @param frame_duration enum
     * @return frame_duratoin in us or 0 for invalid frame_duration enum
     */
    uint16_t btstack_lc3_frame_duration_in_us(btstack_lc3_frame_duration_t frame_duration);
    
    /**
     * @bbrief Calculate number of samples per ISO Interval
     * @param sample_rate
     * @param frame_duration
     * @return
     */
    uint16_t btstack_lc3_samples_per_frame(uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration);
    
