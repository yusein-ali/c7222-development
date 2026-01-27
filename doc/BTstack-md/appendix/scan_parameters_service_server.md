
# Scan Parameters Service Server API {#sec:scan_parameters_service_server_api}


**src/ble/gatt-service/scan_parameters_service_server.h** 


    
    /**
     * @text The Scan Parameters Service enables a remote GATT Client to store the LE scan parameters it is using locally. These parameters can be utilized by the application to optimize power consumption and/or reconnection latency.
     *
     * To use with your application, add `#import <scan_parameters_service.gatt>` to your .gatt file
     * and call all functions below. All strings and blobs need to stay valid after calling the functions.
     */
    
    /**
     * @brief Init Scan Parameters Service Server with ATT DB
     * @param packet_handler 
     */
    void scan_parameters_service_server_init(btstack_packet_handler_t packet_handler);
    
    /** 
     * @brief Request scan parameters from Scan Parameters Client.
     */
    void scan_parameters_service_server_request_scan_parameters(void);
    
