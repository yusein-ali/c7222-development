
# Battery Service Server v1.1 API {#sec:battery_service_v1_server_api}


**src/ble/gatt-service/battery_service_v1_server.h** 


    
    /**
     * @brief Init Battery Service Server with ATT DB
     */
    void battery_service_v1_server_init(void);
    
    /**
     * @brief Register Battery Service
     * @param service
     * @param connections array of battery_service_v1_server_connection_t for storage
     * @param connection_max_num
     * @param out_service_id
     */
    void battery_service_v1_server_register(battery_service_v1_t * service, battery_service_v1_server_connection_t * connections, uint8_t connection_max_num,  uint16_t * out_service_id);
    
    /**
     * @brief Deregister Battery Service Server
     * @param service
     */
    void battery_service_v1_server_deregister(battery_service_v1_t * service);
    
    /**
     * @brief Set callback for broadcast updates.
     * @param callback
     */
    void battery_service_v1_server_set_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Update battery level
     * @note Triggers notification if subscribed
     * @param service
     * @param battery_level in range 0-100
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_battery_level(battery_service_v1_t * service,uint8_t battery_level);
    
    /**
     * @brief Update battery level status
     * @note Triggers notification or indication if subscribed
     * @param service
     * @param battery_level_status in range 0-100
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_battery_level_status(battery_service_v1_t * service, const battery_level_status_t * battery_level_status);
    
    /**
     * @brief Update battery estimated service date as days elapsed since the Epoch (Jan 1, 1970)
     * @note Triggers notification or indication if subscribed
     * @param service
     * @param estimated_service_date_days 
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_estimated_service_date_days(battery_service_v1_t * service, uint32_t estimated_service_date_days);
    
    /**
     * @brief Update battery critcal status flags
     * @note Triggers indication if subscribed
     * @param service
     * @param critcal_status_flags
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_critcal_status_flags(battery_service_v1_t * service, uint8_t critcal_status_flags);
    
    /**
     * @brief Update battery energy status
     * @note Triggers notification or indication if subscribed
     * @param service
     * @param energy_status
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_energy_status(battery_service_v1_t * service, const battery_energy_status_t * energy_status);
    
    /**
     * @brief Update battery time status
     * @note Triggers notification or indication if subscribed
     * @param service
     * @param time_status
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_time_status(battery_service_v1_t * service, const battery_time_status_t * time_status);
    
    /**
     * @brief Update battery health status
     * @note Triggers notification or indication if subscribed
     * @param service
     * @param health_status
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_health_status(battery_service_v1_t * service, const battery_health_status_t * health_status);
    
    /**
     * @brief Update battery health information
     * @note Triggers indication if subscribed
     * @param service
     * @param health_information
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_health_information(battery_service_v1_t * service, const battery_health_information_t * health_information);
    
    /**
     * @brief Update battery information
     * @note Triggers indication if subscribed
     * @param service
     * @param information
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_information(battery_service_v1_t * service, const battery_information_t * information);
    
    /**
     * @brief Update manufacturer name 
     * @note Triggers indication if subscribed
     * @param service
     * @param manufacturer_name
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_manufacturer_name(battery_service_v1_t * service, const char * manufacturer_name);
    
    /**
     * @brief Update model_number 
     * @note Triggers indication if subscribed
     * @param service
     * @param model number
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_model_number(battery_service_v1_t * service, const char * model_number);
    
    /**
     * @brief Update serial_number 
     * @note Triggers indication if subscribed
     * @param service
     * @param serial number
     * @return ERROR_CODE_SUCCESS if value is valid, otherwise ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE
     */
    uint8_t battery_service_v1_server_set_serial_number(battery_service_v1_t * service, const char * serial_number);
    
    /**
     * @brief Get Advertisement Data for all active Characteristic Broadcasts
     * @param adv_interval
     * @param adv_buffer
     * @param adv_size
     * @return
     */
    uint16_t battery_service_v1_server_get_broadcast_advertisement(uint16_t adv_interval, uint8_t * adv_buffer, uint16_t adv_size);
    
    /**
     * @brief Get Advertisement Data for single active Characteristic Broadcast
     * @param adv_interval
     * @param adv_buffer
     * @param adv_size
     * @return
     */
    uint16_t battery_service_v1_server_get_broadcast_advertisement_single(battery_service_v1_t * service, uint16_t adv_interval, uint8_t * adv_buffer, uint16_t adv_size);
    
    void battery_service_v1_server_deinit(void);
    
