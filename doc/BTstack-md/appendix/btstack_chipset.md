
# Chipset Driver API {#sec:btstack_chipset_api}


**src/btstack_chipset.h** : 
The API implements custom chipset initialization and support of proprietary extensions
to set UART baud rate, Bluetooth Address, and similar.



    
    typedef struct {
        /**
         * chipset driver name
         */
        const char * name;
    
        /**
         * init driver
         * allows to reset init script index
         * @param transport_config
         */
        void (*init)(const void * transport_config);
    
        /**
         * support custom init sequences after RESET command
         * @param  hci_cmd_buffer to store generated command
         * @return result see btstack_chipset_result_t
         */
        btstack_chipset_result_t (*next_command)(uint8_t * hci_cmd_buffer); 
    
        /**
         * provide UART Baud Rate change command.
         * @param baudrate
         * @param hci_cmd_buffer to store generated command
         */
        void (*set_baudrate_command)(uint32_t baudrate, uint8_t *hci_cmd_buffer); 
        
        /** provide Set BD Addr command
         * @param baudrate
         * @param hci_cmd_buffer to store generated command
         */
        void (*set_bd_addr_command)(bd_addr_t addr, uint8_t *hci_cmd_buffer); 
    
    } btstack_chipset_t;
    
