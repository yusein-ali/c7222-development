
# base64 Decoder API {#sec:btstack_base64_decoder_api}


**src/btstack_base64_decoder.h** 


    
    #define BTSTACK_BASE64_DECODER_MORE     -1
    #define BTSTACK_BASE64_DECODER_COMPLETE -2
    #define BTSTACK_BASE64_DECODER_INVALID  -3
    #define BTSTACK_BASE64_DECODER_FULL     -4
    
    /**
     * @brief Initialize base64 decoder
     * @param context
     */
    void btstack_base64_decoder_init(btstack_base64_decoder_t * context);
    
    /**
     * @brief Decode single byte
     * @brief context
     * @return value, or BTSTACK_BASE64_DECODER_MORE, BTSTACK_BASE64_DECODER_COMPLETE, BTSTACK_BASE64_DECODER_INVALID
     */
    int  btstack_base64_decoder_process_byte(btstack_base64_decoder_t * context, uint8_t c);
    
    /**
     * @brief Decode block
     * @brief input_data base64 encoded message
     * @brief input_size
     * @breif output_buffer
     * @brief output_max_size
     * @return output_size or BTSTACK_BASE64_DECODER_INVALID, BTSTACK_BASE64_DECODER_FULL
     */
    int btstack_base64_decoder_process_block(const uint8_t * input_data, uint32_t input_size, uint8_t * output_buffer, uint32_t output_max_size);
    
