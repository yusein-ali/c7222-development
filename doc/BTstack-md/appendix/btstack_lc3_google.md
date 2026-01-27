
# LC3 Google Adapter API {#sec:btstack_lc3_google_api}


**src/btstack_lc3_google.h** 

    
    typedef struct {
        lc3_decoder_mem_48k_t           decoder_mem;
        lc3_decoder_t                   decoder;    // pointer
        uint32_t                        sample_rate;
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
    } btstack_lc3_decoder_google_t;
    
    typedef struct {
        lc3_encoder_mem_48k_t           encoder_mem;
        lc3_encoder_t                   encoder;    // pointer
        uint32_t                        sample_rate;
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
    } btstack_lc3_encoder_google_t;
    
    /**
     * Init LC3 Decoder Instance
     * @param context for EHIMA LC3 decoder
     */
    const btstack_lc3_decoder_t * btstack_lc3_decoder_google_init_instance(btstack_lc3_decoder_google_t * context);
    
    /**
     * Init LC3 Decoder Instance
     * @param context for EHIMA LC3 decoder
     */
    const btstack_lc3_encoder_t * btstack_lc3_encoder_google_init_instance(btstack_lc3_encoder_google_t * context);
    
