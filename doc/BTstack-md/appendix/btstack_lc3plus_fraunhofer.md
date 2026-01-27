
# Adapter for Fraunhofer LC3plus Coddec API {#sec:btstack_lc3plus_fraunhofer_api}


**src/btstack_lc3plus_fraunhofer.h** : only uses suitable subset for lc3 testing


    
    typedef struct {
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
        uint16_t                        samples_per_frame;
        uint32_t                        sample_rate;
        // decoder must be 4-byte aligned
        uint8_t                         decoder[LC3PLUS_DEC_MAX_SIZE];
    } btstack_lc3plus_fraunhofer_decoder_t;
    
    typedef struct {
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
        uint32_t                        sample_rate;
        // encoder must be 4-byte aligned
        uint8_t                         encoder[LC3PLUS_ENC_MAX_SIZE];
    } btstack_lc3plus_fraunhofer_encoder_t;
    
    /**
     * Init LC3 Decoder Instance
     * @param context for Fraunhofer LC3plus decoder
     */
    const btstack_lc3_decoder_t * btstack_lc3plus_fraunhofer_decoder_init_instance(btstack_lc3plus_fraunhofer_decoder_t * context);
    
    /**
     * Init LC3 Encoder Instance
     * @param context for Fraunhofer LC3plus encoder
     */
    const btstack_lc3_encoder_t * btstack_lc3plus_fraunhofer_encoder_init_instance(btstack_lc3plus_fraunhofer_encoder_t * context);
    
