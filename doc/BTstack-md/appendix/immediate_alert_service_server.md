
# Immediate Alert Service Server API {#sec:immediate_alert_service_server_api}


**src/ble/gatt-service/immediate_alert_service_server.h** 


    
    /**
     * @brief Init Immediate Alert Service Server with ATT DB
     * @param mute_state
     */
    void immediate_alert_service_server_init(void);
    
    /**
     * @brief Register packet handler
     */
    void immediate_alert_service_server_register_packet_handler(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Set alert level.
     * @param alert_level
     */
    uint8_t immediate_alert_service_server_set_alert_level(const ias_alert_level_t alert_level);
    
    /**
     * @brief Set alert timeout. Default is 0 that equals to no timeout.
     * @param alert_timeout_ms
     */
    uint8_t immediate_alert_service_server_set_alert_timeout(uint32_t alert_timeout_ms);
    
    
    void immediate_alert_service_server_stop_alerting(void);
    
