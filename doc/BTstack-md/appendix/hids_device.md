
# HID Service Server API {#sec:hids_device_api}


**src/ble/gatt-service/hids_device.h** 


    
    typedef struct {
        uint16_t value_handle;
        uint16_t client_configuration_handle;
        uint16_t client_configuration_value;
    
        hid_report_type_t type;
        uint8_t id;
        uint8_t size;
    } hids_device_report_t;
    
    /**
     * @text Implementation of the GATT HIDS Device
     * To use with your application, add '#import <hids.gatt>' to your .gatt file
     */
    
    /**
     * @brief Set up HIDS Device with single INPUT, OUTPUT and FEATURE report 
     */
    void hids_device_init(uint8_t hid_country_code, const uint8_t * hid_descriptor, uint16_t hid_descriptor_size);
    
    /**
     * @brief Set up HIDS Device for multiple instances of INPUT, OUTPUT and FEATURE reports
     */
    void hids_device_init_with_storage(uint8_t hid_country_code, const uint8_t * hid_descriptor, uint16_t hid_descriptor_size, 
        uint16_t num_reports, hids_device_report_t * report_storage);
    
    /**
     * @brief Register callback for the HIDS Device client.
     * @param callback
     */
    void hids_device_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Register report callback for Get Report operation
     * @param callback
     */
    void hids_device_register_get_report_callback(void (*callback)(hci_con_handle_t con_handle, hid_report_type_t report_type, uint16_t report_id, uint16_t max_report_size, uint8_t * out_report));
    
    /**
     * @brief Request can send now event to send HID Report
     * Generates an HIDS_SUBEVENT_CAN_SEND_NOW subevent
     * @param hid_cid
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t hids_device_request_can_send_now_event(hci_con_handle_t con_handle);
    
    /**
     * @brief Send HID Input Report for Report ID
     * @param con_handle
     * @param report_id
     * @param report
     * @param report_len
     * @returns status
     */
    uint8_t hids_device_send_input_report_for_id(hci_con_handle_t con_handle, uint16_t report_id, const uint8_t * report, uint16_t report_len);
    
    /**
     * @brief Send HID Input Report for first Input Report
     * @param con_handle
     * @param report
     * @param report_len
     * @returns status
     */
    uint8_t hids_device_send_input_report(hci_con_handle_t con_handle, const uint8_t * report, uint16_t report_len);
    
    /**
     * @brief Send HID Boot Mouse Input Report
     * @param con_handle
     * @param report
     * @param report_len
     * @returns status
     */
    uint8_t hids_device_send_boot_mouse_input_report(hci_con_handle_t con_handle, const uint8_t * report, uint16_t report_len);
    
    /**
     * @brief Send HID Boot Mouse Input Report
     * @param con_handle
     * @param report
     * @param report_len
     * @returns status
     */
    uint8_t hids_device_send_boot_keyboard_input_report(hci_con_handle_t con_handle, const uint8_t * report, uint16_t report_len);
    
