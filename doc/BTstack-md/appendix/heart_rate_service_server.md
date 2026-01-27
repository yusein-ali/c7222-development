
# Heart Rate Service Server API {#sec:heart_rate_service_server_api}


**src/ble/gatt-service/heart_rate_service_server.h** 


    
    typedef enum {
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_OTHER = 0,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_CHEST,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_WRIST,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_FINGER,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_HAND,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_EAR_LOBE,
    	HEART_RATE_SERVICE_BODY_SENSOR_LOCATION_FOOT
    } heart_rate_service_body_sensor_location_t;
    
    typedef enum {
    	HEART_RATE_SERVICE_SENSOR_CONTACT_UNKNOWN = 0,
    	HEART_RATE_SERVICE_SENSOR_CONTACT_UNSUPPORTED,
    	HEART_RATE_SERVICE_SENSOR_CONTACT_NO_CONTACT,
    	HEART_RATE_SERVICE_SENSOR_CONTACT_HAVE_CONTACT
    } heart_rate_service_sensor_contact_status_t;
    
    
    /**
     * @brief Init Battery Service Server with ATT DB
     * @param body_sensor_location
     * @param energy_expended_supported
     */
    void heart_rate_service_server_init(heart_rate_service_body_sensor_location_t body_sensor_location, int energy_expended_supported);
    
    
    /**
     * @brief Add Energy Expended to the internal accumulator.
     * @param energy_expended_kJ   energy expended in kilo Joules since the last update
     */
    void heart_rate_service_add_energy_expended(uint16_t energy_expended_kJ);
    
    /**
     * @brief Update heart rate (unit: beats per minute)
     * @note triggers notifications if subscribed
     * @param heart_rate_bpm 		beats per minute
     * @param contact    
     * @param rr_interval_count 
     * @param rr_intervals      resolution in 1/1024 seconds
     */
    void heart_rate_service_server_update_heart_rate_values(uint16_t heart_rate_bpm, 
    	heart_rate_service_sensor_contact_status_t contact, int rr_interval_count, uint16_t * rr_intervals);
    
