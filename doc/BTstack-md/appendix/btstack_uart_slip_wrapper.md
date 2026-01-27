
# UART SLIP Wrapper API {#sec:btstack_uart_slip_wrapper_api}


**src/btstack_uart_slip_wrapper.h** : 
Compatibility layer to use new H5 implementation with btstack_uart.h implementations without SLIP support.
Using this compatibility layer caused increased processing as it uses single byte UART reads.

If you're using H5, please consider implement the H5/SLIP functions in your btstack_uart.h or hal_uart_dma.h implementation.



    
    /**
     * @brief Initialize SLIP wrapper for existing btstack_uart_block_t instance without SLIP support
     * @param uart_block_without_slip
     * @return btstack_uart_t instance with SLIP support for use with hci_trasnport_h5
     */
    const btstack_uart_t * btstack_uart_slip_wrapper_instance(const btstack_uart_t * uart_without_slip);
    
