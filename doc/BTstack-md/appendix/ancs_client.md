
# ANCS Client API {#sec:ancs_client_api}


**src/ble/gatt-service/ancs_client.h** 


    
    void ancs_client_init(void);
    void ancs_client_register_callback(btstack_packet_handler_t callback);
    const char * ancs_client_attribute_name_for_id(uint16_t id);
    
