
# Broadcast Audio Scan Service Server (BASS) API {#sec:broadcast_audio_scan_service_server_api}


**src/le-audio/gatt-service/broadcast_audio_scan_service_server.h** : 
@text The Broadcast Audio Scan Service is used by servers to expose their status with respect 
to synchronization to broadcast Audio Streams and associated data, including Broadcast_Codes 
used to decrypt encrypted broadcast Audio Streams. Clients can use the attributes exposed by 
servers to observe and/or request changes in server behavior.

To use with your application, add `#import <broadcast_audio_scan_service.gatt>` to your .gatt file. 


    // memory for list of these structs is allocated by the application
    typedef struct {
        // assigned by client via control point
        bass_source_data_t data;
    
        uint8_t  update_counter;
        uint8_t  source_id; 
        bool     in_use;
    
        le_audio_big_encryption_t big_encryption;
        uint8_t  bad_code[16];
    
        uint16_t bass_receive_state_handle;
        uint16_t bass_receive_state_client_configuration_handle;
        uint16_t bass_receive_state_client_configuration;
    } bass_server_source_t;
    
    typedef struct {
        hci_con_handle_t con_handle;
        uint16_t sources_to_notify;
    
        // used for caching long write
        uint8_t  long_write_buffer[512];
        uint16_t long_write_value_size;
        uint16_t long_write_attribute_handle;
    } bass_server_connection_t;
    
    /**
     * @brief Init Broadcast Audio Scan Service Server with ATT DB
     * @param sources_num
     * @param sources
     * @param clients_num
     * @param clients
     */
    void broadcast_audio_scan_service_server_init(uint8_t const sources_num, bass_server_source_t * sources, uint8_t const clients_num, bass_server_connection_t * clients);
    
    /**
     * @brief Register packet handler to receive events:
     * - LEAUDIO_SUBEVENT_BASS_SERVER_SCAN_STOPPED
     * - LEAUDIO_SUBEVENT_BASS_SERVER_SCAN_STARTED
     * - LEAUDIO_SUBEVENT_BASS_SERVER_BROADCAST_CODE
     * - LEAUDIO_SUBEVENT_BASS_SERVER_SOURCE_ADDED
     * - LEAUDIO_SUBEVENT_BASS_SERVER_SOURCE_MODIFIED
     * - LEAUDIO_SUBEVENT_BASS_SERVER_SOURCE_DELETED
     * @param packet_handler
     */
    void broadcast_audio_scan_service_server_register_packet_handler(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Set PA state of source.
     * @param source_index
     * @param sync_state
     */
    void broadcast_audio_scan_service_server_set_pa_sync_state(uint8_t source_index, le_audio_pa_sync_state_t sync_state);
    
    /**
     * @brief Add source.
     * @param source_data
     * @param source_index
     */
    void broadcast_audio_scan_service_server_add_source(const bass_source_data_t *source_data, uint8_t * source_index);
    
    /**
     * @brief Deinit Broadcast Audio Scan Service Server
     */
    void broadcast_audio_scan_service_server_deinit(void);
    
