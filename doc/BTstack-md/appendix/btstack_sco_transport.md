
# SCO Transport API {#sec:btstack_sco_transport_api}


**src/btstack_sco_transport.h** : 
Hardware abstraction for PCM/I2S Inteface used for HSP/HFP profiles.



    
    typedef struct {
    
        /**
         * register packet handler for SCO HCI packets
         */
        void (*register_packet_handler)(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
    
        /**
         * open transport
         * @param con_handle of SCO connection
         * @param sco_format
         */
        void (*open)(hci_con_handle_t con_handle, sco_format_t sco_format);
    
        /**
         * send SCO packet
         */
        void (*send_packet)(const uint8_t *buffer, uint16_t length);
    
        /**
         * close transport
         * @param con_handle of SCO connection
         */
        void (*close)(hci_con_handle_t con_handle);
    
    } btstack_sco_transport_t;
    
