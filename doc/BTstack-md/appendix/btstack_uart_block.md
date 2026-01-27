
# UART Block API {#sec:btstack_uart_block_api}


**src/btstack_uart_block.h** : 
Compatibility layer for ports that use btstack_uart_block_t 



    
    typedef btstack_uart_t btstack_uart_block_t;
    
    // existing block-only implementations
    const btstack_uart_block_t * btstack_uart_block_windows_instance(void);
    const btstack_uart_block_t * btstack_uart_block_embedded_instance(void);
    const btstack_uart_block_t * btstack_uart_block_freertos_instance(void);
    
    // mapper for extended implementation
    static inline const btstack_uart_block_t * btstack_uart_block_posix_instance(void){
        return (const btstack_uart_block_t *) btstack_uart_posix_instance();
    }
    /* API_STOP */
    
    #if defined __cplusplus
    }
    #endif
    
    #endif
