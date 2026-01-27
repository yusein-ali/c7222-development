
# HID Service Client - HIDS Host API {#sec:hids_host_api}


**src/ble/gatt-service/hids_host.h** 


    
    /**
     * @brief Initialize HID Service Host. The HID Descriptor storage is shared between all connections.
     *
     * @param hid_descriptor_storage
     * @param hid_descriptor_storage_len
     */
    void hids_host_init(uint8_t * hid_descriptor_storage, uint16_t hid_descriptor_storage_len);
    
    /**
     * @brief Connect to HID Services of remote device. Event GATTSERVICE_SUBEVENT_HID_SERVICE_CONNECTED will be emitted
     * after all remote HID services and characteristics are found, and notifications for all input reports are enabled.
     * Status code can be ERROR_CODE_SUCCES if at least one HID service is found, otherwise either ATT errors or 
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no service or report map or report are found.
     * It also contains the number of individual HIDS Services. 
     *
     * Connection state is stored in hids_host_t objects provided via memory pool
     * Max number of connections is limited by MAX_NR_HIDS_HOSTS unless HAVE_MALLOC is used
     *
     * @param con_handle
     * @param packet_handler
     * @param protocol_mode see hid_protocol_mode_t in btstack_hid.h
     * @param hids_cid (out) to use for other functions
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client
     *         associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED.
     */
    uint8_t hids_host_connect(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler, hid_protocol_mode_t protocol_mode, uint16_t * hids_cid);
    
    /**
     * @brief Send HID report. Event GATTSERVICE_SUBEVENT_HID_REPORT_WRITTEN is emitted.
     *
     * @param hids_cid
     * @param report_id
     * @param report_type
     * @param report
     * @param report_len
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, 
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found, or
     * ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE if report length exceeds MTU.
     */
    uint8_t hids_host_send_write_report(uint16_t hids_cid, uint8_t report_id, hid_report_type_t report_type, const uint8_t * report, uint8_t report_len);
    
    /**
     * @brief Get HID report. Event GATTSERVICE_SUBEVENT_HID_REPORT is emitted.
     *
     * @param hids_cid
     * @param report_id
     * @param report_type
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, 
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found, or
     * ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE if report length exceeds MTU.
     */
    uint8_t hids_host_send_get_report(uint16_t hids_cid, uint8_t report_id, hid_report_type_t report_type);
    
    /**
     * @brief Get HID Information. Event GATTSERVICE_SUBEVENT_HID_INFORMATION is emitted.
     *
     * @param hids_cid
     * @param service_index
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, or
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found.
     */
    uint8_t hids_host_get_hid_information(uint16_t hids_cid, uint8_t service_index);
    
    /**
     * @brief Get Protocol Mode. Event GATTSERVICE_SUBEVENT_HID_PROTOCOL_MODE is emitted.
     *
     * @param hids_cid
     * @param service_index
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, or
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found.
     */
    uint8_t hids_host_get_protocol_mode(uint16_t hids_cid, uint8_t service_index);
    
    /**
     * @brief Set Protocol Mode.
     *
     * @param hids_cid
     * @param service_index
     * @param protocol_mode
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, or
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found.
     */
    uint8_t hids_host_send_set_protocol_mode(uint16_t hids_cid, uint8_t service_index, hid_protocol_mode_t protocol_mode);
    
    /**
     * @brief Send Suspend to remote HID service.
     *
     * @param hids_cid
     * @param service_index
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, or
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found.
     */
    uint8_t hids_host_send_suspend(uint16_t hids_cid, uint8_t service_index);
    
    /**
     * @brief Send Exit Suspend to remote HID service.
     *
     * @param hids_cid
     * @param service_index
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, 
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state, or
     * ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no report with given type and ID is found.
     */
    uint8_t hids_host_send_exit_suspend(uint16_t hids_cid, uint8_t service_index);
    
    /**
     * @brief Enable all notifications. Event GATTSERVICE_SUBEVENT_HID_SERVICE_REPORTS_NOTIFICATION reports current configuration.
     *
     * @param hids_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, or
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state.
     */
    uint8_t hids_host_enable_notifications(uint16_t hids_cid);
    
    /**
     * @brief Disable all notifications. Event GATTSERVICE_SUBEVENT_HID_SERVICE_REPORTS_NOTIFICATION reports current configuration.
     *
     * @param hids_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, or
     * ERROR_CODE_COMMAND_DISALLOWED if client is in wrong state.
     */
    uint8_t hids_host_disable_notifications(uint16_t hids_cid);
    
    /**
     * @brief Disconnect from HID Service.
     *
     * @param hids_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER
     */
    uint8_t hids_host_disconnect(uint16_t hids_cid);
    
    /*
     * @brief Get descriptor data. For services in boot mode without a Report Map, a default HID Descriptor for Keyboard/Mouse is provided.
     *
     * @param hid_cid
     * @return data
     */
    const uint8_t * hids_host_descriptor_storage_get_descriptor_data(uint16_t hids_cid, uint8_t service_index);
    
    /*
     * @brief Get descriptor length
     *
     * @param hid_cid
     * @return length
     */
    uint16_t hids_host_descriptor_storage_get_descriptor_len(uint16_t hids_cid, uint8_t service_index);
    
    /**
     * @brief De-initialize HID Service Client. 
     *
     */
    void hids_host_deinit(void);
    
