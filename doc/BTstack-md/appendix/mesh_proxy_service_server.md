
# Mesh Proxy Service Server API {#sec:mesh_proxy_service_server_api}


**src/mesh/gatt-service/mesh_proxy_service_server.h** 


    
    /**
     * @brief Init Mesh Proxy Service Server with ATT DB
     */
    void mesh_proxy_service_server_init(void);
    
    /**
     * @brief Send a Proxy PDU message containing proxy PDU from a proxy Server to a proxy Client.
     * @param con_handle 
     * @param proxy_pdu 
     * @param proxy_pdu_size max lenght MESH_PROV_MAX_PROXY_PDU
     */
    void mesh_proxy_service_server_send_proxy_pdu(uint16_t con_handle, const uint8_t * proxy_pdu, uint16_t proxy_pdu_size);
    
    /**
     * @brief Register callback for the PB-GATT.
     * @param callback
     */
    void mesh_proxy_service_server_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Request can send now event to send PDU
     * Generates an MESH_SUBEVENT_CAN_SEND_NOW subevent
     * @param con_handle
     */
    void mesh_proxy_service_server_request_can_send_now(hci_con_handle_t con_handle);
