
# Nordic SPP Service Server API {#sec:nordic_spp_service_server_api}


**src/ble/gatt-service/nordic_spp_service_server.h** 


    
    /**
     * @text The Nordic SPP Service is implementation of the Nordic SPP-like profile.
     *
     * To use with your application, add `#import <nordic_spp_service.gatt>` to your .gatt file
     * and call all functions below. All strings and blobs need to stay valid after calling the functions.
     */
    
    /**
     * @brief Init Nordic SPP Service Server with ATT DB
     * @param packet_handler for events and tx data from peer as RFCOMM_DATA_PACKET
     */
    void nordic_spp_service_server_init(btstack_packet_handler_t packet_handler);
    
    /** 
     * @brief Queue send request. When called, one packet can be send via nordic_spp_service_send below
     * @param request
     * @param con_handle
     */
    void nordic_spp_service_server_request_can_send_now(btstack_context_callback_registration_t * request, hci_con_handle_t con_handle);
    
    /**
     * @brief Send data
     * @param con_handle
     * @param data
     * @param size
     */
    int nordic_spp_service_server_send(hci_con_handle_t con_handle, const uint8_t * data, uint16_t size);
    
