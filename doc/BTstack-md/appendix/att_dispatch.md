
# ATT Dispatch API {#sec:att_dispatch_api}


**src/ble/att_dispatch.h** : 
Dispatcher for independent implementation of ATT client and server.



    
    /**
     * @brief Register packet handler for ATT Client PDUs
     * @param packet_handler
     */
    void att_dispatch_register_client(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Register packet handler for ATT Server PDUs
     * @param packet_handler
     */
    void att_dispatch_register_server(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Register L2CAP ATT Service with fixed PSM
     * @note requires ENABLE_GATT_OVER_CLASSIC
     */
    void att_dispatch_classic_register_service(void);
    
    /**
     * @brief Create L2CAP ATT Connection
     * @note ENABLE_GATT_OVER_CLASSIC
     * @param address
     * @param l2cap_psm
     */
    uint8_t att_dispatch_classic_connect(bd_addr_t address, uint16_t l2cap_psm, uint16_t *out_cid);
    
    /**
     * @brief can send packet for client
     * @param con_handle
     */
    bool att_dispatch_client_can_send_now(hci_con_handle_t con_handle);
    
    /**
     * @brief can send packet for server
     * @param con_handle
     */
    bool att_dispatch_server_can_send_now(hci_con_handle_t con_handle);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for client
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_client_request_can_send_now_event(hci_con_handle_t con_handle);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for server
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_server_request_can_send_now_event(hci_con_handle_t con_handle);
    
    /** 
    * @brief Used for propogating a updated MTU from att_server to gatt_client
    * @param con_handle
    * @param mtu
    */
    void att_dispatch_server_mtu_exchanged(hci_con_handle_t con_handle, uint16_t new_mtu);
    
    /**
    * @brief Used for propogating a updated MTU from gatt_client to att_server
    * @param con_handle
    * @param mtu
    */
    void att_dispatch_client_mtu_exchanged(hci_con_handle_t con_handle, uint16_t new_mtu);
    
