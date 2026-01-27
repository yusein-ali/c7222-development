
# SLIP encoder/decoder API {#sec:btstack_slip_api}


**src/btstack_slip.h** 


    
    // ENCODER
    
    /**
     * @brief Initialise SLIP encoder with data
     * @param data
     * @param len
     */
    void btstack_slip_encoder_start(const uint8_t * data, uint16_t len);
    
    /**
     * @brief Check if encoder has data ready
     * @return True if data ready
     */
    int  btstack_slip_encoder_has_data(void);
    
    /** 
     * @brief Get next byte from encoder 
     * @return Next bytes from encoder
     */
    uint8_t btstack_slip_encoder_get_byte(void);
    
    // DECODER
    
    /**
     * @brief Initialise SLIP decoder with buffer
     * @param buffer to store received data
     * @param max_size of buffer
     */
    void btstack_slip_decoder_init(uint8_t * buffer, uint16_t max_size);
    
    /**
     * @brief Process received byte
     * @param input
     */
    
    void btstack_slip_decoder_process(uint8_t input);
    
    /**
     * @brief Get size of decoded frame
     * @return size of frame. Size = 0 => frame not complete
     */
    
    uint16_t btstack_slip_decoder_frame_size(void);
    
