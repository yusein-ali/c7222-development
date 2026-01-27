
# EM9304 SPI API {#sec:btstack_em9304_spi_api}


**src/btstack_em9304_spi.h** : 
BTstack's Hardware Abstraction Layer for EM9304 connected via SPI with additional RDY Interrupt line.



    
    #include <stdint.h>
    typedef struct {
    
        /**
         * @brief Open SPI
         */
        int (*open)(void);
    
        /**
         * @brief Close SPI
         */
        int (*close)(void);
    
        /**
         * @brief Check if full duplex operation via transceive is supported
         * @return 1 if supported
         */
        int  (*get_fullduplex_support)(void);
    
        /**
         * @brief Set callback for RDY
         * @param callback or NULL to disable callback
         */
        void (*set_ready_callback)(void (*callback)(void));
    
        /**
         * @brief Set callback for transfer complete
         * @param callback
         */
        void (*set_transfer_done_callback)(void (*callback)(void));
    
        /**
         * @brief Set Chip Selet
         * @param enable
         */
        void (*set_chip_select)(int enable);
    
        /**
         * @brief Poll READY state
         */
        int (*get_ready)(void);
    
        /**
         * @brief Transmit and Receive bytes via SPI
         * @param tx_data buffer to transmit
         * @param rx_data buffer to receive into
         * @param len 
         */
        void (*transceive)(const uint8_t * tx_data, uint8_t * rx_data, uint16_t len);
    
        /**
         * @brief Transmit bytes via SPI
         * @param tx_data buffer to transmit
         * @param len 
         */
        void (*transmit)(const uint8_t * tx_data, uint16_t len);
    
        /**
         * @brief Receive bytes via SPI
         * @param rx_data buffer to receive into
         * @param len 
         */
        void (*receive)(uint8_t * rx_data, uint16_t len);
    
    } btstack_em9304_spi_t;
    
    /**
     * @brief Get EM9304 SPI instance
     */
    const btstack_em9304_spi_t * btstack_em9304_spi_embedded_instance(void);
    
