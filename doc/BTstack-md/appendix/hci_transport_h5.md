
# HCI Transport H5 API {#sec:hci_transport_h5_api}


**src/hci_transport_h5.h** 


    
    /*
     * @brief Setup H5 instance with btstack_uart implementation that supports SLIP frames
     * @param uart_driver to use
     */
    const hci_transport_t * hci_transport_h5_instance(const btstack_uart_t * uart_driver);
    
    /*
     * @brief Enable H5 Low Power Mode: enter sleep mode after x ms of inactivity
     * @param inactivity_timeout_ms or 0 for off
     */
    void hci_transport_h5_set_auto_sleep(uint16_t inactivity_timeout_ms);
    
    /*
     * @brief Enable BSCP mode H5, by enabling event parity
     * @deprecated Parity can be enabled in UART driver configuration
     */
    void hci_transport_h5_enable_bcsp_mode(void);
    
