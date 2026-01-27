
# Link Loss Service Server API {#sec:link_loss_service_server_api}


**src/ble/gatt-service/link_loss_service_server.h** 


    
    /**
     * @brief Init Link Loss Service Server with ATT DB
     * @param mute_state
     */
    void link_loss_service_server_init(void);
    
    /**
     * @brief Register packet handler
     */
    void link_loss_service_server_register_packet_handler(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Set alert level. Default is no alert.
     * @param alert_level
     */
    uint8_t link_loss_service_server_set_alert_level(lls_alert_level_t alert_level);
    
    /**
     * @brief Set alert timeout. Default is 0 that equals to no timeout.
     * @param alert_timeout_ms
     */
    uint8_t link_loss_service_server_set_alert_timeout(uint32_t alert_timeout_ms);
    
    
    void link_loss_service_server_stop_alerting(void);
    
