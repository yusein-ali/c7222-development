
# Cycling Speed and Cadence Service Server API {#sec:cycling_speed_and_cadence_service_server_api}


**src/ble/gatt-service/cycling_speed_and_cadence_service_server.h** 


    
    typedef enum {
    	CSC_SERVICE_SENSOR_LOCATION_OTHER = 0,
    	CSC_SERVICE_SENSOR_LOCATION_TOP_OF_SHOE,
    	CSC_SERVICE_SENSOR_LOCATION_IN_SHOE,
    	CSC_SERVICE_SENSOR_LOCATION_HIP,
    	CSC_SERVICE_SENSOR_LOCATION_FRONT_WHEEL,
    	CSC_SERVICE_SENSOR_LOCATION_LEFT_CRANK,
    	CSC_SERVICE_SENSOR_LOCATION_RIGHT_CRANK,
    	CSC_SERVICE_SENSOR_LOCATION_LEFT_PEDAL,
    	CSC_SERVICE_SENSOR_LOCATION_RIGHT_PEDAL,
    	CSC_SERVICE_SENSOR_LOCATION_FRONT_HUB,
    	CSC_SERVICE_SENSOR_LOCATION_REAR_DROPOUT,
    	CSC_SERVICE_SENSOR_LOCATION_CHAINSTAY,
    	CSC_SERVICE_SENSOR_LOCATION_REAR_WHEEL,
    	CSC_SERVICE_SENSOR_LOCATION_REAR_HUB,
    	CSC_SERVICE_SENSOR_LOCATION_CHEST,
    	CSC_SERVICE_SENSOR_LOCATION_SPIDER,
    	CSC_SERVICE_SENSOR_LOCATION_CHAIN_RING,
    	CSC_SERVICE_SENSOR_LOCATION_RESERVED
    } cycling_speed_and_cadence_sensor_location_t;
    
    typedef enum {
    	CSC_FLAG_WHEEL_REVOLUTION_DATA_SUPPORTED = 0,
    	CSC_FLAG_CRANK_REVOLUTION_DATA_SUPPORTED,
    	CSC_FLAG_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED
    } csc_feature_flag_bit_t;
    
    typedef enum {
    	CSC_OPCODE_IDLE = 0,
    	CSC_OPCODE_SET_CUMULATIVE_VALUE = 1,
    	CSC_OPCODE_START_SENSOR_CALIBRATION,
    	CSC_OPCODE_UPDATE_SENSOR_LOCATION,
    	CSC_OPCODE_REQUEST_SUPPORTED_SENSOR_LOCATIONS,
    	CSC_OPCODE_RESPONSE_CODE = 16
    } csc_opcode_t;
    
    /**
     * @brief Init Server with ATT DB
     */
    void cycling_speed_and_cadence_service_server_init(uint32_t supported_sensor_locations, 
    	uint8_t multiple_sensor_locations_supported, uint8_t wheel_revolution_data_supported, uint8_t crank_revolution_data_supported);
    
    /**
     * @brief Update heart rate (unit: beats per minute)
     * @note triggers notifications if subscribed
     */
    void cycling_speed_and_cadence_service_server_update_values(int32_t wheel_revolutions, uint16_t last_wheel_event_time, uint16_t crank_revolutions, uint16_t last_crank_event_time);
    
