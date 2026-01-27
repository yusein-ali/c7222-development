
# UART API {#sec:btstack_uart_api}


**src/btstack_uart.h** : 
Common types for UART transports



    
    typedef struct {
        /**
         * init transport
         * @param uart_config
         */
        int (*init)(const btstack_uart_config_t * uart_config);
    
        /**
         * open transport connection
         */
        int (*open)(void);
    
        /**
         * close transport connection
         */
        int (*close)(void);
    
        /**
         * set callback for block received. NULL disables callback
         */
        void (*set_block_received)(void (*block_handler)(void));
    
        /**
         * set callback for sent. NULL disables callback
         */
        void (*set_block_sent)(void (*block_handler)(void));
    
        /**
         * set baudrate
         */
        int (*set_baudrate)(uint32_t baudrate);
    
        /**
         * set parity
         */
        int  (*set_parity)(int parity);
    
        /**
         * set flowcontrol
         */
        int  (*set_flowcontrol)(int flowcontrol);
    
        /**
         * receive block
         */
        void (*receive_block)(uint8_t *buffer, uint16_t len);
    
        /**
         * send block
         */
        void (*send_block)(const uint8_t *buffer, uint16_t length);
    
    
        /** Support for different Sleep Modes in TI's H4 eHCILL and in H5 - can be set to NULL if not used */
    
        /**
         * query supported wakeup mechanisms
         * @return supported_sleep_modes mask
         */
         int (*get_supported_sleep_modes)(void);
    
        /**
         * set UART sleep mode - allows to turn off UART and it's clocks to save energy
         * Supported sleep modes:
         * - off: UART active, RTS low if receive_block was called and block not read yet
         * - RTS high, wake on CTS: RTS should be high. On CTS pulse, UART gets enabled again and RTS goes to low
         * - RTS low, wake on RX: data on RX will trigger UART enable, bytes might get lost
         */
        void (*set_sleep)(btstack_uart_sleep_mode_t sleep_mode);
    
        /** 
         * set wakeup handler - needed to notify hci transport of wakeup requests by Bluetooth controller
         * Called upon CTS pulse or RX data. See sleep modes.
         */
        void (*set_wakeup_handler)(void (*wakeup_handler)(void));
    
    
        /** Support for HCI H5 Transport Mode - can be set to NULL for H4 */
    
        /**
         * H5/SLIP only: set callback for frame received. NULL disables callback
         */
        void (*set_frame_received)(void (*frame_handler)(uint16_t frame_size));
    
        /**
         * H5/SLIP only: set callback for frame sent. NULL disables callback
         */
        void (*set_frame_sent)(void (*block_handler)(void));
    
        /**
         * H5/SLIP only: receive SLIP frame
         */
        void (*receive_frame)(uint8_t *buffer, uint16_t len);
    
        /**
         * H5/SLIP only: send SLIP frame
         */
        void (*send_frame)(const uint8_t *buffer, uint16_t length);
    
    } btstack_uart_t;
    
