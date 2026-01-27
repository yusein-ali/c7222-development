
# Network Interface API {#sec:btstack_network_api}


**src/btstack_network.h** 


    
    /**
     * @brief Initialize network interface
     * @param send_packet_callback
     */
    void btstack_network_init(void (*send_packet_callback)(const uint8_t * packet, uint16_t size));
    
    /**
     * @brief Bring up network interfacd
     * @param network_address
     * @return 0 if ok
     */
    int  btstack_network_up(bd_addr_t network_address);
    
    /**
     * @brief Shut down network interfacd
     * @param network_address
     * @return 0 if ok
     */
    int  btstack_network_down(void);
    
    /** 
     * @brief Receive packet on network interface, e.g., forward packet to TCP/IP stack 
     * @param packet
     * @param size
     */
    void btstack_network_process_packet(const uint8_t * packet, uint16_t size);
    
    /** 
     * @brief Notify network interface that packet from send_packet_callback was sent and the next packet can be delivered.
     */
    void btstack_network_packet_sent(void);
    
    /**
     * @brief Get network name after network was activated
     * @note e.g. tapX on Linux, might not be useful on all platforms
     * @return network name
     */
    const char * btstack_network_get_name(void);
    
