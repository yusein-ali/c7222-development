
# TX Power Service Server API {#sec:tx_power_service_server_api}


**src/ble/gatt-service/tx_power_service_server.h** 


    
    /**
     * @brief Init TX Power Service Server with ATT DB
     * @param tx_power_level
     */
    void tx_power_service_server_init(int8_t tx_power_level);
    
    /**
     * @brief Update TX power level
     * @param tx_power_level_dBm range [-100,20]
     */
    void tx_power_service_server_set_level(int8_t tx_power_level_dBm);
    
