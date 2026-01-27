
# Battery Service Server API {#sec:battery_service_server_api}


**src/ble/gatt-service/battery_service_server.h** 


    
    /**
     * @brief Init Battery Service Server with ATT DB
     * @param battery_value in range 0-100
     */
    void battery_service_server_init(uint8_t battery_value);
    
    /**
     * @brief Update battery value
     * @note triggers notifications if subscribed
     * @param battery_value in range 0-100
     */
    void battery_service_server_set_battery_value(uint8_t battery_value);
    
