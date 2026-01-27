
# ATT Database Engine API {#sec:att_db_api}


**src/ble/att_db.h** 


    
    // map ATT ERROR CODES on to att_read_callback length
    #define ATT_READ_ERROR_CODE_OFFSET       0xfe00u
    
    // custom BTstack ATT Response Pending for att_read_callback
    #define ATT_READ_RESPONSE_PENDING                 0xffffu
    
    // internally used to signal write response pending
    // To ask ATT Server to defer the write response, you can return ATT_ERROR_WRITE_RESPONSE_PENDING in your att_write_callback
    #define ATT_INTERNAL_WRITE_RESPONSE_PENDING       0xfffeu
    
    /**
     * @brief ATT Client Read Callback for Dynamic Data
     * - if buffer == NULL, don't copy data, just return size of value
     * - if buffer != NULL, copy data and return number bytes copied
     *
     * If ENABLE_ATT_DELAYED_READ_RESPONSE is defined, you may return ATT_READ_RESPONSE_PENDING if data isn't available yet
     * and call att_server_response_ready to re-trigger the callback.
     *
     * @param con_handle of hci le connection
     * @param attribute_handle to be read
     * @param offset defines start of attribute value
     * @param buffer 
     * @param buffer_size
     * @return size of value if buffer is NULL, otherwise number of bytes copied
     */
    typedef uint16_t (*att_read_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    
    /**
     * @brief ATT Client Write Callback for Dynamic Data
     * Each Prepared Write Request triggers a callback with transaction mode ATT_TRANSACTION_MODE_ACTIVE.
     * On Execute Write, the callback will be called with ATT_TRANSACTION_MODE_VALIDATE and allows to validate all queued writes and return an application error.
     * If none of the registered callbacks return an error for ATT_TRANSACTION_MODE_VALIDATE and the callback will be called with ATT_TRANSACTION_MODE_EXECUTE.
     * Otherwise, all callbacks will be called with ATT_TRANSACTION_MODE_CANCEL.
     *
     * If the additional validation step is not needed, just return 0 for all callbacks with transaction mode ATT_TRANSACTION_MODE_VALIDATE.
     *
     * If ENABLE_ATT_DELAYED_READ_RESPONSE is defined, you may return ATT_ERROR_WRITE_RESPONSE_PENDING if data isn't available yet
     * and call att_server_response_ready to re-trigger the callback.
     *
     * @param con_handle of hci le connection
     * @param attribute_handle to be written
     * @param transaction - ATT_TRANSACTION_MODE_NONE for regular writes. For prepared writes: ATT_TRANSACTION_MODE_ACTIVE, ATT_TRANSACTION_MODE_VALIDATE, ATT_TRANSACTION_MODE_EXECUTE, ATT_TRANSACTION_MODE_CANCEL
     * @param offset into the value - used for queued writes and long attributes
     * @param buffer 
     * @param buffer_size
     * @param signature used for signed write commmands
     * @return 0 if write was ok, ATT_ERROR_PREPARE_QUEUE_FULL if no space in queue, ATT_ERROR_INVALID_OFFSET if offset is larger than max buffer
     */
    typedef int (*att_write_callback_t)(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
    
    // Read & Write Callbacks for handle range
    typedef struct att_service_handler {
        btstack_linked_item_t * item;
        att_read_callback_t read_callback;
        att_write_callback_t write_callback;
        btstack_packet_handler_t packet_handler;
        uint16_t start_handle;
        uint16_t end_handle;
        uint8_t flags;
    } att_service_handler_t;
    
    // MARK: ATT Operations
    
    /**
     * @brief setup ATT database
     * @param db
     */
    void att_set_db(uint8_t const * db);
    
    /*
     * @brief set callback for read of dynamic attributes
     * @param callback
     */
    void att_set_read_callback(att_read_callback_t callback);
    
    /**
     * @brief set callback for write of dynamic attributes
     * @param callback
     */
    void att_set_write_callback(att_write_callback_t callback);
    
    /**
     * @brief debug helper, dump ATT database to stdout using log_info
     */
    void att_dump_attributes(void);
    
    /**
     * @brief process ATT request against database and put response into response buffer
     * @param att_connection used for mtu and security properties
     * @param request_buffer, request_len: ATT request from clinet
     * @param response_buffer for result
     * @return len of data in response buffer. 0 = no response, 
     *          ATT_READ_RESPONSE_PENDING if it was returned at least once for dynamic data (requires ENABLE_ATT_DELAYED_READ_RESPONSE)
     */
    uint16_t att_handle_request(att_connection_t * att_connection,
                                uint8_t * request_buffer,
                                uint16_t request_len,
                                uint8_t * response_buffer);
    
    /**
     * @brief setup value notification in response buffer for a given handle and value
     * @param att_connection
     * @param attribute_handle
     * @param value
     * @param value_len
     * @param response_buffer for notification
     */
    uint16_t att_prepare_handle_value_notification(att_connection_t * att_connection,
                                                   uint16_t attribute_handle,
                                                   const uint8_t *value,
                                                   uint16_t value_len, 
                                                   uint8_t * response_buffer);
    
    /**
     * @brief setup value notification in response buffer for multiple handles and values
     * @param att_connection
     * @param attribute_handle
     * @param value
     * @param value_len
     * @param response_buffer for notification
     */
    uint16_t att_prepare_handle_value_multiple_notification(att_connection_t * att_connection,
                                                            uint8_t num_attributes,
                                                            const uint16_t * attribute_handles,
                                                            const uint8_t ** values_data,
                                                            const uint16_t * values_len,
                                                            uint8_t * response_buffer);
    
    /**
     * @brief setup value indication in response buffer for a given handle and value
     * @param att_connection
     * @param attribute_handle
     * @param value
     * @param value_len
     * @param response_buffer for indication
     */
    uint16_t att_prepare_handle_value_indication(att_connection_t * att_connection,
                                                 uint16_t attribute_handle,
                                                 const uint8_t *value,
                                                 uint16_t value_len, 
                                                 uint8_t * response_buffer);
    
    /**
     * @brief transcation queue of prepared writes, e.g., after disconnect
     * @return att_connection
     */
    void att_clear_transaction_queue(att_connection_t * att_connection);
    
    // att_read_callback helpers for a various data types
    
    /**
     * @brief Handle read of blob like data for att_read_callback
     * @param blob of data
     * @param blob_size of blob
     * @param offset from att_read_callback
     * @param buffer from att_read_callback
     * @param buffer_size from att_read_callback
     * @return value size for buffer == 0 and num bytes copied otherwise
     */
    uint16_t att_read_callback_handle_blob(const uint8_t * blob, uint16_t blob_size, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    
    /**
     * @brief Handle read of little endian unsigned 32 bit value for att_read_callback
     * @param value
     * @param offset from att_read_callback
     * @param buffer from att_read_callback
     * @param buffer_size from att_read_callback
     * @return value size for buffer == 0 and num bytes copied otherwise
     */
    uint16_t att_read_callback_handle_little_endian_32(uint32_t value, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    
    /**
     * @brief Handle read of little endian unsigned 16 bit value for att_read_callback
     * @param value
     * @param offset from att_read_callback
     * @param buffer from att_read_callback
     * @param buffer_size from att_read_callback
     * @return value size for buffer == 0 and num bytes copied otherwise
     */
    uint16_t att_read_callback_handle_little_endian_16(uint16_t value, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    
    /**
     * @brief Handle read of single byte for att_read_callback
     * @param blob of data
     * @param blob_size of blob
     * @param offset from att_read_callback
     * @param buffer from att_read_callback
     * @param buffer_size from att_read_callback
     * @return value size for buffer == 0 and num bytes copied otherwise
     */
    uint16_t att_read_callback_handle_byte(uint8_t value, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
    
    
    // experimental client API
    /**
     * @brief Get UUID for handle
     * @param attribute_handle
     * @return 0 if not found
     */
    uint16_t att_uuid_for_handle(uint16_t attribute_handle);
    
    /**
     * @brief Get const value for handle
     * @param attribute_handle
     * @param out_value_len  output variable that hold value len
     * @return value 
     */
    
    const uint8_t * gatt_server_get_const_value_for_handle(uint16_t attribute_handle, uint16_t * out_value_len);
    
    // experimental GATT Server API
    
    /**
     * @brief Get handle range for primary or secondary service.
     * @param uuid16
     * @param start_handle
     * @param end_handle
     * @return false if not found
     */
    bool gatt_server_get_handle_range_for_service_with_uuid16(uint16_t uuid16, uint16_t * start_handle, uint16_t * end_handle);
    
    /**
     * @brief Get handle range for included service.
     * @param start_handle
     * @param end_handle
     * @param uuid16
     * @param out_included_service_handle
     * @param out_included_service_start_handle
     * @param out_included_service_end_handle
     * @return false if not found
     */
    bool gatt_server_get_included_service_with_uuid16(uint16_t start_handle, uint16_t end_handle, uint16_t uuid16, 
        uint16_t * out_included_service_handle, uint16_t * out_included_service_start_handle, uint16_t * out_included_service_end_handle);
    
    /**
     * @brief Get value handle for characteristic.
     * @param start_handle
     * @param end_handle
     * @param uuid16
     * @return 0 if not found
     */
    uint16_t gatt_server_get_value_handle_for_characteristic_with_uuid16(uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
    
    /**
     * @brief Get descriptor handle for characteristic.
     * @param start_handle
     * @param end_handle
     * @param characteristic_uuid16
     * @param descriptor_uuid16
     * @return 0 if not found
     */
    uint16_t gatt_server_get_descriptor_handle_for_characteristic_with_uuid16(uint16_t start_handle, uint16_t end_handle, uint16_t characteristic_uuid16, uint16_t descriptor_uuid16);
    
    /**
     * @brief Get client configuration handle for characteristic.
     * @param start_handle
     * @param end_handle
     * @param characteristic_uuid16
     * @return 0 if not found
     */
    uint16_t gatt_server_get_client_configuration_handle_for_characteristic_with_uuid16(uint16_t start_handle, uint16_t end_handle, uint16_t characteristic_uuid16);
    
    /**
     * @brief Get server configuration handle for characteristic.
     * @param start_handle
     * @param end_handle
     * @param characteristic_uuid16
     * @param descriptor_uuid16
     * @return 0 if not found
     */
    uint16_t gatt_server_get_server_configuration_handle_for_characteristic_with_uuid16(uint16_t start_handle, uint16_t end_handle, uint16_t characteristic_uuid16);
    
    
    /**
     * @brief Get handle range for primary service.
     * @param uuid128
     * @param start_handle
     * @param end_handle
     * @return false if not found
     */
    bool gatt_server_get_handle_range_for_service_with_uuid128(const uint8_t * uuid128, uint16_t * start_handle, uint16_t * end_handle);
    
    /**
     * @brief Get value handle.
     * @param start_handle
     * @param end_handle
     * @param uuid128
     * @return 0 if not found
     */
    uint16_t gatt_server_get_value_handle_for_characteristic_with_uuid128(uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /**
     * @brief Get client configuration handle.
     * @param start_handle
     * @param end_handle
     * @param uuid128
     * @return 0 if not found
     */
    uint16_t gatt_server_get_client_configuration_handle_for_characteristic_with_uuid128(uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /**
     * @brief Get Database Hash provided via GATT Database Hash characteristic
     * @note Used by att_server to discard stored CCCD values for bonded devices if hash has changed
     * @param hash
     * @return true if hash is available
     */
    bool gatt_server_get_database_hash(uint8_t * hash);
    
