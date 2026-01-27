
# HCI Transport API {#sec:hci_transport_api}


**src/hci_transport.h** : 
The API allows BTstack to use different transport interfaces.



    
    /* HCI packet types */
    typedef struct {
        /**
         * transport name 
         */
        const char * name;
    
        /**
         * init transport
         * @param transport_config
         */
        void   (*init) (const void *transport_config);
    
        /**
         * open transport connection
         */
        int    (*open)(void);
    
        /**
         * close transport connection
         */
        int    (*close)(void);
    
        /**
         * register packet handler for HCI packets: ACL, SCO, and Events
         */
        void   (*register_packet_handler)(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
    
        /**
         * support async transport layers, e.g. IRQ driven without buffers
         */
        int    (*can_send_packet_now)(uint8_t packet_type);
    
        /**
         * send packet
         */
        int    (*send_packet)(uint8_t packet_type, uint8_t *packet, int size);
    
        /**
         * extension for UART transport implementations
         */
        int    (*set_baudrate)(uint32_t baudrate);
    
        /**
         * extension for UART H5 on CSR: reset BCSP/H5 Link
         */
        void   (*reset_link)(void);
    
        /**
         * extension for USB transport implementations: config SCO connections
         */
        void   (*set_sco_config)(uint16_t voice_setting, int num_connections);
    
    } hci_transport_t;
    
    typedef enum {
        HCI_TRANSPORT_CONFIG_UART,
        HCI_TRANSPORT_CONFIG_USB,
        HCI_TRANSPORT_CONFIG_LINUX,
    } hci_transport_config_type_t;
    
    typedef struct {
        hci_transport_config_type_t type;
    } hci_transport_config_t;
    
    typedef struct {
        hci_transport_config_type_t type; // == HCI_TRANSPORT_CONFIG_UART
        uint32_t   baudrate_init;         // initial baud rate
        uint32_t   baudrate_main;         // = 0: same as initial baudrate
        int        flowcontrol;           //
        const char *device_name;
        int        parity;                // see btstack_uart.h BTSTACK_UART_PARITY
    } hci_transport_config_uart_t;
    
    typedef struct {
        hci_transport_config_type_t type; // == HCI_TRANSPORT_CONFIG_LINUX
        int device_id;                    // e.g. 0 for HCI0
    } hci_transport_config_linux_t;
    
