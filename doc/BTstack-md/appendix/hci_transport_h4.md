
# HCI Transport H4 API {#sec:hci_transport_h4_api}


**src/hci_transport_h4.h** 


    
    /*
     * @brief Setup H4 instance with btstack_uart implementation
     * @param btstack_uart_block_driver to use
     */
    const hci_transport_t * hci_transport_h4_instance_for_uart(const btstack_uart_t * uart_driver);
    
    /*
     * @brief Setup H4 instance with btstack_uart_block implementation
     * @param btstack_uart_block_driver to use
     * @deprecated use hci_transport_h4_instance_for_uart instead
     */
    const hci_transport_t * hci_transport_h4_instance(const btstack_uart_block_t * uart_driver);
    
