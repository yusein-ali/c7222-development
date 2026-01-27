
# HID Device API {#sec:hid_device_api}


**src/classic/hid_device.h** 


    
    typedef struct {
        uint16_t        hid_device_subclass;
        uint8_t         hid_country_code;
        uint8_t         hid_virtual_cable;
        uint8_t         hid_remote_wake;
        uint8_t         hid_reconnect_initiate;
        bool            hid_normally_connectable;
        bool            hid_boot_device;
        uint16_t        hid_ssr_host_max_latency; 
        uint16_t        hid_ssr_host_min_timeout;
        uint16_t        hid_supervision_timeout;
        const uint8_t * hid_descriptor;
        uint16_t        hid_descriptor_size;
        // ServiceName, or NULL
        const char *    device_name;
    } hid_sdp_record_t;
    
    /**
     * @brief Create HID Device SDP service record. 
     * @param service Empty buffer in which a new service record will be stored.
     * @param have_remote_audio_control 
     * @param service
     * @param service_record_handle
     * @param params
     */
    void hid_create_sdp_record(uint8_t * service, uint32_t service_record_handle, const hid_sdp_record_t * params);
    
    /**
     * @brief Set up HID Device 
     * @param boot_protocol_mode_supported
     * @param hid_descriptor_len
     * @param hid_descriptor
     */
    void hid_device_init(bool boot_protocol_mode_supported, uint16_t hid_descriptor_len, const uint8_t * hid_descriptor);
    
    /**
     * @brief Register callback for the HID Device client. 
     * @param callback
     */
    void hid_device_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Register get report callback for the HID Device client. 
     * @param callback
     */
    void hid_device_register_report_request_callback(int (*callback)(uint16_t hid_cid, hid_report_type_t report_type, uint16_t report_id, int * out_report_size, uint8_t * out_report));
    
    /**
     * @brief Register set report callback for the HID Device client. 
     * @param callback
     */
    void hid_device_register_set_report_callback(void (*callback)(uint16_t hid_cid, hid_report_type_t report_type, int report_size, uint8_t * report));
    
    /**
     * @brief Register callback to receive report data for the HID Device client. 
     * @param callback
     */
    void hid_device_register_report_data_callback(void (*callback)(uint16_t cid, hid_report_type_t report_type, uint16_t report_id, int report_size, uint8_t * report));
    
    /*
     * @brief Create HID connection to HID Host
     * @param addr
     * @param hid_cid to use for other commands
     * @result status
     */
    uint8_t hid_device_connect(bd_addr_t addr, uint16_t * hid_cid);
    
    /*
     * @brief Disconnect from HID Host
     * @param hid_cid
     */
    void hid_device_disconnect(uint16_t hid_cid);
    
    /**
     * @brief Request can send now event to send HID Report
     * Generates an HID_SUBEVENT_CAN_SEND_NOW subevent
     * @param hid_cid
     */
    void hid_device_request_can_send_now_event(uint16_t hid_cid);
    
    /**
     * @brief Send HID message on interrupt channel
     * @param hid_cid
     */
    void hid_device_send_interrupt_message(uint16_t hid_cid, const uint8_t * message, uint16_t message_len);
    
    /**
     * @brief Send HID message on control channel
     * @param hid_cid
     */
    void hid_device_send_control_message(uint16_t hid_cid, const uint8_t * message, uint16_t message_len);
    
    /**
     * @brief Retutn 1 if boot protocol mode active
     * @param hid_cid
     */
    int hid_device_in_boot_protocol_mode(uint16_t hid_cid);
    
    /**
     * @brief De-Init HID Device
     */
    void hid_device_deinit(void);
    
