
# HCI Logging API {#sec:hci_dump_api}


**src/hci_dump.h** : 
Dump HCI trace as BlueZ's hcidump format, Apple's PacketLogger, or stdout.



    
    typedef enum {
        HCI_DUMP_INVALID = 0,
        HCI_DUMP_BLUEZ,
        HCI_DUMP_PACKETLOGGER,
        HCI_DUMP_BTSNOOP,
    } hci_dump_format_t;
    
    typedef struct {
        // reset output, called if max packets is reached, to limit file size
        void (*reset)(void);
        // log packet
        void (*log_packet)(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len);
        // log message
        void (*log_message)(int log_level, const char * format, va_list argptr);
    #ifdef __AVR__ \
        // log message - AVR
        void (*log_message_P)(int log_level, PGM_P * format, va_list argptr);
    #endif
    } hci_dump_t;
    
    /**
     * @brief Init HCI Dump
     * @param hci_dump_impl - platform-specific implementation
     */
    void hci_dump_init(const hci_dump_t * hci_dump_impl);
    
    /**
     * @brief Enable packet logging
     * @param enabled default: true
     */
    void hci_dump_enable_packet_log(bool enabled);
    
    /**
     * @brief
     */
    void hci_dump_enable_log_level(int log_level, int enable);
    
    /**
     * @brief Set max number of packets - output file might be truncated
     * @param packets - number of packets to log, -1 for unlimited
     */
    void hci_dump_set_max_packets(int packets);
    
    /**
     * @brief Dump Packet
     * @param packet_type
     * @param in is 1 for incoming, 0 for outoing
     * @param packet
     * @param len
     */
    void hci_dump_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len);
    
    /**
     * @brief Dump Message
     * @param log_level
     * @param format
     */
    void hci_dump_log(int log_level, const char * format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 2, 3)))
    #endif
    ;
    
    #ifdef __AVR__
    /*
     * @brief Dump Message using format string stored in PGM memory (allows to save RAM)
     * @param log_level
     * @param format
     */
    void hci_dump_log_P(int log_level, PGM_P format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 2, 3)))
    #endif
    ;
    #endif
    
    /**
     * @brief Dump internal BTstack event
     * @note only logged if ENABLE_LOG_BTSTACK_EVENTS is defined
     * @param packet
     * @param len
     */
    void hci_dump_btstack_event(const uint8_t *packet, uint16_t len);
    
    /**
     * @brief Setup header for PacketLogger format
     * @param buffer
     * @param tv_sec
     * @param tv_us
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_packetlogger(uint8_t * buffer, uint32_t tv_sec, uint32_t tv_us, uint8_t packet_type, uint8_t in, uint16_t len);
    
    /**
     * @brief Setup header for BLUEZ (hcidump) format
     * @param buffer
     * @param tv_sec
     * @param tv_us
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_bluez(uint8_t * buffer, uint32_t tv_sec, uint32_t tv_us, uint8_t packet_type, uint8_t in, uint16_t len);
    
    /**
     * @brief Setup header for BT Snoop format
     * @param buffer
     * @param ts_usec_high upper 32-bit of 64-bit microsecond timestamp
     * @param ts_usec_low  lower 2-bit of 64-bit microsecond timestamp
     * @param cumulative_drops since last packet was recorded
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_btsnoop(uint8_t * buffer, uint32_t ts_usec_high, uint32_t ts_usec_low, uint32_t cumulative_drops, uint8_t packet_type, uint8_t in, uint16_t len);
    
