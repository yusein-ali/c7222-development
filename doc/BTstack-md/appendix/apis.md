
# APIs


## AD Data Parser API {#sec:ad_parser_api}


**ad_parser.h** : 
AD Data (Advertisements and EIR) Parser


    
    typedef struct ad_context {
         const uint8_t * data;
         uint8_t   offset;
         uint8_t   length;
    } ad_context_t;
    
    // Advertising or Scan Response data iterator
    void ad_iterator_init(ad_context_t *context, uint8_t ad_len, const uint8_t * ad_data);
    bool ad_iterator_has_more(const ad_context_t * context);
    void ad_iterator_next(ad_context_t * context);
    
    // Access functions
    uint8_t         ad_iterator_get_data_type(const ad_context_t * context);
    uint8_t         ad_iterator_get_data_len(const ad_context_t * context);
    const uint8_t * ad_iterator_get_data(const ad_context_t * context);
    
    // convenience function on complete advertisements
    bool ad_data_contains_uuid16(uint8_t ad_len, const uint8_t * ad_data, uint16_t uuid16);
    bool ad_data_contains_uuid128(uint8_t ad_len, const uint8_t * ad_data, const uint8_t * uuid128);
    

## ATT Database Engine API {#sec:att_db_api}


**att_db.h** 


    
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
    

## Runtine ATT Database Setup API {#sec:att_db_util_api}


**att_db_util.h** : 
Helper to construct ATT DB at runtime (BTstack GATT Compiler is not used).



    
    /**
     * @brief Init ATT DB storage
     */
    void att_db_util_init(void);
    
    /**
     * @brief Set handle value for next table entry
     * @param handle
     * @note new handle must be >= previous to avoid reusing assigned handles
     */
    void att_db_util_set_next_handle(uint16_t handle);
    
    /**
     * @brief Add primary service for 16-bit UUID
     * @param uuid16
     * @return attribute handle for the new service definition
     */
    uint16_t att_db_util_add_service_uuid16(uint16_t uuid16);
    
    /**
     * @brief Add primary service for 128-bit UUID
     * @param uuid128
     * @return attribute handle for the new service definition
     */
    uint16_t att_db_util_add_service_uuid128(const uint8_t * uuid128);
    
    /**
     * @brief Add secondary service for 16-bit UUID
     * @param uuid16
     * @return attribute handle for the new service definition
     */
    uint16_t att_db_util_add_secondary_service_uuid16(uint16_t uuid16);
    
    /**
     * @brief Add secondary service for 128-bit UUID
     * @param uuid128
     * @return attribute handle for the new service definition
     */
    uint16_t att_db_util_add_secondary_service_uuid128(const uint8_t * uuid128);
    
    /**
     * @brief Add included service with 16-bit UUID
     * @param start_group_handle
     * @param end_group_handle
     * @param uuid16
     * @return attribute handle for the new service definition
     */
    uint16_t att_db_util_add_included_service_uuid16(uint16_t start_group_handle, uint16_t  end_group_handle, uint16_t uuid16);
    
    /**
     * @brief Add Characteristic with 16-bit UUID, properties, and data
     * @param uuid16
     * @param properties        - see ATT_PROPERTY_* in src/bluetooth.h
     * @param read_permissions  - see ATT_SECURITY_* in src/bluetooth.h
     * @param write_permissions - see ATT_SECURITY_* in src/bluetooth.h
     * @param data returned in read operations if ATT_PROPERTY_DYNAMIC is not specified
     * @param data_len
     * @return attribute handle of the new characteristic value declaration
     * @note If properties contains ATT_PROPERTY_NOTIFY or ATT_PROPERTY_INDICATE flags, a Client Configuration Characteristic Descriptor (CCCD)
     *       is created as well. The attribute value handle of the CCCD is the attribute value handle plus 1
     */
    uint16_t att_db_util_add_characteristic_uuid16(uint16_t uuid16, uint16_t properties, uint8_t read_permission, uint8_t write_permission, uint8_t * data, uint16_t data_len);
    
    /**
     * @brief Add Characteristic with 128-bit UUID, properties, and data
     * @param uuid128
     * @param properties        - see ATT_PROPERTY_* in src/bluetooth.h
     * @param read_permissions  - see ATT_SECURITY_* in src/bluetooth.h
     * @param write_permissions - see ATT_SECURITY_* in src/bluetooth.h
     * @param data returned in read operations if ATT_PROPERTY_DYNAMIC is not specified
     * @param data_len
     * @return attribute handle of the new characteristic value declaration
     * @note If properties contains ATT_PROPERTY_NOTIFY or ATT_PROPERTY_INDICATE flags, a Client Configuration Characteristic Descriptor (CCCD)
     *       is created as well. The attribute value handle of the CCCD is the attribute value handle plus 1
     */
    uint16_t att_db_util_add_characteristic_uuid128(const uint8_t * uuid128, uint16_t properties, uint8_t read_permission, uint8_t write_permission, uint8_t * data, uint16_t data_len);
    
    /**
    * @brief Add descriptor with 16-bit UUID, properties, and data
    * @param uuid16
    * @param properties        - see ATT_PROPERTY_* in src/bluetooth.h
    * @param read_permissions  - see ATT_SECURITY_* in src/bluetooth.h
    * @param write_permissions - see ATT_SECURITY_* in src/bluetooth.h
    * @param data returned in read operations if ATT_PROPERTY_DYNAMIC is not specified
    * @param data_len
    * @return attribute handle of the new characteristic descriptor declaration
    */
    uint16_t att_db_util_add_descriptor_uuid16(uint16_t uuid16, uint16_t properties, uint8_t read_permission, uint8_t write_permission, uint8_t * data, uint16_t data_len);
    
    /**
    * @brief Add descriptor with 128-bit UUID, properties, and data
    * @param uuid128
    * @param properties        - see ATT_PROPERTY_* in src/bluetooth.h
    * @param read_permissions  - see ATT_SECURITY_* in src/bluetooth.h
    * @param write_permissions - see ATT_SECURITY_* in src/bluetooth.h
    * @param data returned in read operations if ATT_PROPERTY_DYNAMIC is not specified
    * @param data_len
    * @return attribute handle of the new characteristic descriptor declaration
    */
    uint16_t att_db_util_add_descriptor_uuid128(const uint8_t * uuid128, uint16_t properties, uint8_t read_permission, uint8_t write_permission, uint8_t * data, uint16_t data_len);
    
    /**
     * @brief Get address of constructed ATT DB
     */
    uint8_t * att_db_util_get_address(void);
    
    /**
     * @brief Get size of constructed ATT DB 
     */
    uint16_t att_db_util_get_size(void);
    
    /**
     * @brief Get number of bytes that are included in GATT Database Hash
     */
    uint16_t att_db_util_hash_len(void);
    
    /**
     * @brief init generator for GATT Database Hash
     */
    void att_db_util_hash_init(void);
    
    /**
     * @brief get next byte from generator for GATT Database Hash
     */
    uint8_t att_db_util_hash_get_next(void);
    
    /**
     * @brief Calculate GATT Database Hash using crypto engine
     * @param request
     * @param db_hash
     * @param callback
     * @param callback_arg
     */
    void att_db_util_hash_calc(btstack_crypto_aes128_cmac_t * request, uint8_t * db_hash, void (* callback)(void * arg), void * callback_arg);
    

## ATT Dispatch API {#sec:att_dispatch_api}


**att_dispatch.h** : 
Dispatcher for independent implementation of ATT client and server.



    
    /**
     * @brief Register packet handler for ATT Client PDUs
     * @param packet_handler
     */
    void att_dispatch_register_client(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Register packet handler for ATT Server PDUs
     * @param packet_handler
     */
    void att_dispatch_register_server(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Register L2CAP ATT Service with fixed PSM
     * @note requires ENABLE_GATT_OVER_CLASSIC
     */
    void att_dispatch_classic_register_service(void);
    
    /**
     * @brief Create L2CAP ATT Connection
     * @note ENABLE_GATT_OVER_CLASSIC
     * @param address
     * @param l2cap_psm
     */
    uint8_t att_dispatch_classic_connect(bd_addr_t address, uint16_t l2cap_psm, uint16_t *out_cid);
    
    /**
     * @brief can send packet for client
     * @param con_handle
     */
    bool att_dispatch_client_can_send_now(hci_con_handle_t con_handle);
    
    /**
     * @brief can send packet for server
     * @param con_handle
     */
    bool att_dispatch_server_can_send_now(hci_con_handle_t con_handle);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for client
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_client_request_can_send_now_event(hci_con_handle_t con_handle);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible for server
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_dispatch_server_request_can_send_now_event(hci_con_handle_t con_handle);
    
    /** 
    * @brief Used for propogating a updated MTU from att_server to gatt_client
    * @param con_handle
    * @param mtu
    */
    void att_dispatch_server_mtu_exchanged(hci_con_handle_t con_handle, uint16_t new_mtu);
    
    /**
    * @brief Used for propogating a updated MTU from gatt_client to att_server
    * @param con_handle
    * @param mtu
    */
    void att_dispatch_client_mtu_exchanged(hci_con_handle_t con_handle, uint16_t new_mtu);
    

## ATT Server API {#sec:att_server_api}


**att_server.h** 


    
    /**
     * @title ATT Server
     *
     * @TLV 'BTC' x
     */
    
    /*
     * @brief setup ATT server
     * @param db attribute database created by compile-gatt.py
     * @param read_callback, see att_db.h, can be NULL
     * @param write_callback, see attl.h, can be NULL
     */
    void att_server_init(uint8_t const * db, att_read_callback_t read_callback, att_write_callback_t write_callback);
    
    /**
     * @brief Enable support for Enhanced ATT bearer
     * @note Requires ENABLE_GATT_OVER_EATT
     * @param num_eatt_bearers
     * @param storage_buffer
     * @param storage_size must be >= num_eatt_bearers * sizeof(att_server_eatt_bearer_t)
     * @return status   ERROR_CODE_SUCCESS
     *                  ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if buffer too small or no entry in l2cap service pool
     *                  L2CAP_SERVICE_ALREADY_REGISTERED if called twice
     */
    uint8_t att_server_eatt_init(uint8_t num_eatt_bearers, uint8_t * storage_buffer, uint16_t storage_size);
    
    /*
     * @brief register packet handler for ATT server events:
     *        - ATT_EVENT_CAN_SEND_NOW
     *        - ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE
     *        - ATT_EVENT_MTU_EXCHANGE_COMPLETE 
     * @param handler
     */
    void att_server_register_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief register read/write callbacks for specific handle range
     * @param att_service_handler_t
     */
    void att_server_register_service_handler(att_service_handler_t * handler);
    
    /**
     * @brief Request callback when sending is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_register_can_send_now_callback(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Return ATT MTU
     * @param con_handle
     * @return mtu if ok, 0 otherwise
     */
    uint16_t att_server_get_mtu(hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when sending notifcation is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t att_server_request_to_send_notification(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when sending indication is possible
     * @note callback might happend during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t att_server_request_to_send_indication(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief notify client about attribute value change
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_notify(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);
    
    /**
     * @brief notify client about multiple attribute value changes
     * @param con_handle
     * @param num_attributes
     * @param attribute_handles[]
     * @param values_data[]
     * @param values_len[]
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_multiple_notify(hci_con_handle_t con_handle, uint8_t num_attributes,
                                       const uint16_t * attribute_handles, const uint8_t ** values_data, const uint16_t * values_len);
    
    /**
     * @brief indicate value change to client. client is supposed to reply with an indication_response
     * @param con_handle
     * @param attribute_handle
     * @param value
     * @param value_len
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_indicate(hci_con_handle_t con_handle, uint16_t attribute_handle, const uint8_t *value, uint16_t value_len);
    
    #ifdef ENABLE_ATT_DELAYED_RESPONSE
    /**
     * @brief response ready - called after returning ATT_READ__RESPONSE_PENDING in an att_read_callback or
     * ATT_ERROR_WRITE_REQUEST_PENDING IN att_write_callback before to trigger callback again and complete the transaction
     * @nore The ATT Server will retry handling the current ATT request
     * @param con_handle
     * @return 0 if ok, error otherwise
     */
    uint8_t att_server_response_ready(hci_con_handle_t con_handle);
    #endif
    
    /**
     * De-Init ATT Server 
     */
    void att_server_deinit(void);
    
    // the following functions will be removed soon
    
    /**
     * @brief tests if a notification or indication can be send right now
     * @param con_handle
     * @return 1, if packet can be sent
     */
    int  att_server_can_send_packet_now(hci_con_handle_t con_handle);
    
    /**
     * @brief Request emission of ATT_EVENT_CAN_SEND_NOW as soon as possible
     * @note ATT_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param con_handle
     */
    void att_server_request_can_send_now_event(hci_con_handle_t con_handle);
    // end of deprecated functions
    

## ANCS Client API {#sec:ancs_client_api}


**ancs_client.h** 


    
    void ancs_client_init(void);
    void ancs_client_register_callback(btstack_packet_handler_t callback);
    const char * ancs_client_attribute_name_for_id(uint16_t id);
    

## Battery Service Client API {#sec:battery_service_client_api}


**battery_service_client.h** 


    
        
    /**
     * @brief Initialize Battery Service. 
     */
    void battery_service_client_init(void);
    
    /**
     * @brief Connect to Battery Services of remote device. The client will try to register for notifications. 
     * If notifications are not supported by remote Battery Service, the client will poll battery level
     * If poll_interval_ms is 0, polling is disabled, and only notifications will be received.
     * In either case, the battery level is received via GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL event.
     * The battery level is reported as percentage, i.e. 100 = full and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * For manual polling, see battery_service_client_read_battery_level below.
     *
     * Event GATTSERVICE_SUBEVENT_BATTERY_SERVICE_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no battery service is found, or ATT errors (see bluetooth.h). 
     * This event event also returns number of battery instances found on remote server, as well as poll bitmap that indicates which indexes 
     * of services require polling, i.e. they do not support notification on battery level change,
     *
     * @param con_handle
     * @param packet_handler
     * @param poll_interval_ms or 0 to disable polling
     * @param battery_service_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED 
     */
    uint8_t battery_service_client_connect(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler, uint32_t poll_interval_ms, uint16_t * battery_service_cid);
    
    /**
     * @brief Read battery level for service with given index. Event GATTSERVICE_SUBEVENT_BATTERY_SERVICE_LEVEL is 
     * received with battery level (unit is in percentage, i.e. 100 = full). The battery level is valid if the ATTT status 
     * is equal to ATT_ERROR_SUCCESS, see ATT errors (see bluetooth.h) for other values.
     * @param battery_service_cid
     * @param service_index
     * @return status
     */
    uint8_t battery_service_client_read_battery_level(uint16_t battery_service_cid, uint8_t service_index);
    
    /**
     * @brief Disconnect from Battery Service.
     * @param battery_service_cid
     * @return status
     */
    uint8_t battery_service_client_disconnect(uint16_t battery_service_cid);
    
    /**
     * @brief De-initialize Battery Service. 
     */
    void battery_service_client_deinit(void);
    

## Battery Service Server API {#sec:battery_service_server_api}


**battery_service_server.h** 


    
    /**
     * @brief Init Battery Service Server with ATT DB
     * @param battery_value in range 0-100
     */
    void battery_service_server_init(uint8_t battery_value);
    
    /**
     * @brief Update battery value
     * @note triggers notifications if subscribed
     * @param battery_value in range 0-100
     */
    void battery_service_server_set_battery_value(uint8_t battery_value);
    

## Battery Service Server v1.1 API {#sec:battery_service_v1_server_api}


**battery_service_v1_server.h** 


    
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
    

## Bond Management Service Server API {#sec:bond_management_service_server_api}


**bond_management_service_server.h** 


    #define BOND_MANAGEMENT_CONTROL_POINT_OPCODE_NOT_SUPPORTED 0x80
    #define BOND_MANAGEMENT_OPERATION_FAILED 0x81
    
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_AND_LE                   0x00001
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_AND_LE_WITH_AUTH         0x00002
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC                          0x00004
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_WITH_AUTH                0x00008
    #define BMF_DELETE_ACTIVE_BOND_LE                               0x00010
    #define BMF_DELETE_ACTIVE_BOND_LE_WITH_AUTH                     0x00020
    #define BMF_DELETE_ALL_BONDS_CLASSIC_AND_LE                     0x00040
    #define BMF_DELETE_ALL_BONDS_CLASSIC_AND_LE_WITH_AUTH           0x00080
    #define BMF_DELETE_ALL_BONDS_CLASSIC                            0x00100
    #define BMF_DELETE_ALL_BONDS_CLASSIC_WITH_AUTH                  0x00200
    #define BMF_DELETE_ALL_BONDS_LE                                 0x00400
    #define BMF_DELETE_ALL_BONDS_LE_WITH_AUTH                       0x00800
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE           0x01000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE_WITH_AUTH 0x02000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC                  0x04000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_WITH_AUTH        0x08000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_LE                       0x10000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_LE_WITH_AUT              0x20000
    
    typedef enum {
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_CLASSIC_AND_LE = 0x01,
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_CLASSIC_AND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_LE
    } bond_management_cmd_t;
    
    /**
     * @brief Init Bond Management Service with ATT DB
     * @param supported_features
     */
    void bond_management_service_server_init(uint32_t supported_features);
    
    /**
     * @brief Set authorisation string
     * @note String is not copied
     * @param authorisation_string
     */
    void bond_management_service_server_set_authorisation_string(const char * authorisation_string);
    

## Cycling Power Service Server API {#sec:cycling_power_service_server_api}


**cycling_power_service_server.h** 


    #define CYCLING_POWER_MANUFACTURER_SPECIFIC_DATA_MAX_SIZE   16
    
    typedef enum {
        CP_PEDAL_POWER_BALANCE_REFERENCE_UNKNOWN = 0,
        CP_PEDAL_POWER_BALANCE_REFERENCE_LEFT,
        CP_PEDAL_POWER_BALANCE_REFERENCE_NOT_SUPPORTED
    } cycling_power_pedal_power_balance_reference_t;
    
    typedef enum {
        CP_TORQUE_SOURCE_WHEEL = 0,
        CP_TORQUE_SOURCE_CRANK,
        CP_TORQUE_SOURCE_NOT_SUPPORTED
    } cycling_power_torque_source_t;
    
    typedef enum {
        CP_SENSOR_MEASUREMENT_CONTEXT_FORCE = 0,
        CP_SENSOR_MEASUREMENT_CONTEXT_TORQUE
    } cycling_power_sensor_measurement_context_t;
    
    typedef enum {
        CP_DISTRIBUTED_SYSTEM_UNSPECIFIED = 0,
        CP_DISTRIBUTED_SYSTEM_NOT_SUPPORTED,
        CP_DISTRIBUTED_SYSTEM_SUPPORTED
    } cycling_power_distributed_system_t;
    
    typedef enum {
        CP_MEASUREMENT_FLAG_PEDAL_POWER_BALANCE_PRESENT = 0,
        CP_MEASUREMENT_FLAG_PEDAL_POWER_BALANCE_REFERENCE, // 0 - unknown, 1 - left
        CP_MEASUREMENT_FLAG_ACCUMULATED_TORQUE_PRESENT,
        CP_MEASUREMENT_FLAG_ACCUMULATED_TORQUE_SOURCE, // 0 - wheel based, 1 - crank based
        CP_MEASUREMENT_FLAG_WHEEL_REVOLUTION_DATA_PRESENT,
        CP_MEASUREMENT_FLAG_CRANK_REVOLUTION_DATA_PRESENT,
        CP_MEASUREMENT_FLAG_EXTREME_FORCE_MAGNITUDES_PRESENT,
        CP_MEASUREMENT_FLAG_EXTREME_TORQUE_MAGNITUDES_PRESENT,
        CP_MEASUREMENT_FLAG_EXTREME_ANGLES_PRESENT,
        CP_MEASUREMENT_FLAG_TOP_DEAD_SPOT_ANGLE_PRESENT,
        CP_MEASUREMENT_FLAG_BOTTOM_DEAD_SPOT_ANGLE_PRESENT,
        CP_MEASUREMENT_FLAG_ACCUMULATED_ENERGY_PRESENT,
        CP_MEASUREMENT_FLAG_OFFSET_COMPENSATION_INDICATOR,
        CP_MEASUREMENT_FLAG_RESERVED
    } cycling_power_measurement_flag_t;
    
    typedef enum {
        CP_INSTANTANEOUS_MEASUREMENT_DIRECTION_UNKNOWN = 0,
        CP_INSTANTANEOUS_MEASUREMENT_DIRECTION_TANGENTIAL_COMPONENT,
        CP_INSTANTANEOUS_MEASUREMENT_DIRECTION_RADIAL_COMPONENT,
        CP_INSTANTANEOUS_MEASUREMENT_DIRECTION_LATERAL_COMPONENT
    } cycling_power_instantaneous_measurement_direction_t;
    
    typedef enum {
        CP_VECTOR_FLAG_CRANK_REVOLUTION_DATA_PRESENT = 0,
        CP_VECTOR_FLAG_FIRST_CRANK_MEASUREMENT_ANGLE_PRESENT, 
        CP_VECTOR_FLAG_INSTANTANEOUS_FORCE_MAGNITUDE_ARRAY_PRESENT,
        CP_VECTOR_FLAG_INSTANTANEOUS_TORQUE_MAGNITUDE_ARRAY_PRESENT,
        CP_VECTOR_FLAG_INSTANTANEOUS_MEASUREMENT_DIRECTION = 4, // 2 bit
        CP_VECTOR_FLAG_RESERVED = 6
    } cycling_power_vector_flag_t;
    
    typedef enum {
        CP_SENSOR_LOCATION_OTHER,
        CP_SENSOR_LOCATION_TOP_OF_SHOE,
        CP_SENSOR_LOCATION_IN_SHOE,
        CP_SENSOR_LOCATION_HIP,
        CP_SENSOR_LOCATION_FRONT_WHEEL,
        CP_SENSOR_LOCATION_LEFT_CRANK,
        CP_SENSOR_LOCATION_RIGHT_CRANK,
        CP_SENSOR_LOCATION_LEFT_PEDAL,
        CP_SENSOR_LOCATION_RIGHT_PEDAL,
        CP_SENSOR_LOCATION_FRONT_HUB,
        CP_SENSOR_LOCATION_REAR_DROPOUT,
        CP_SENSOR_LOCATION_CHAINSTAY,
        CP_SENSOR_LOCATION_REAR_WHEEL,
        CP_SENSOR_LOCATION_REAR_HUB,
        CP_SENSOR_LOCATION_CHEST,
        CP_SENSOR_LOCATION_SPIDER,
        CP_SENSOR_LOCATION_CHAIN_RING,
        CP_SENSOR_LOCATION_RESERVED
    } cycling_power_sensor_location_t;
    
    typedef enum {
        CP_FEATURE_FLAG_PEDAL_POWER_BALANCE_SUPPORTED = 0,
        CP_FEATURE_FLAG_ACCUMULATED_TORQUE_SUPPORTED,
        CP_FEATURE_FLAG_WHEEL_REVOLUTION_DATA_SUPPORTED,
        CP_FEATURE_FLAG_CRANK_REVOLUTION_DATA_SUPPORTED,
        CP_FEATURE_FLAG_EXTREME_MAGNITUDES_SUPPORTED,
        CP_FEATURE_FLAG_EXTREME_ANGLES_SUPPORTED,
        CP_FEATURE_FLAG_TOP_AND_BOTTOM_DEAD_SPOT_ANGLE_SUPPORTED,
        CP_FEATURE_FLAG_ACCUMULATED_ENERGY_SUPPORTED,
        CP_FEATURE_FLAG_OFFSET_COMPENSATION_INDICATOR_SUPPORTED,
        CP_FEATURE_FLAG_OFFSET_COMPENSATION_SUPPORTED,
        CP_FEATURE_FLAG_CYCLING_POWER_MEASUREMENT_CHARACTERISTIC_CONTENT_MASKING_SUPPORTED,
        CP_FEATURE_FLAG_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED,
        CP_FEATURE_FLAG_CRANK_LENGTH_ADJUSTMENT_SUPPORTED,
        CP_FEATURE_FLAG_CHAIN_LENGTH_ADJUSTMENT_SUPPORTED,
        CP_FEATURE_FLAG_CHAIN_WEIGHT_ADJUSTMENT_SUPPORTED,
        CP_FEATURE_FLAG_SPAN_LENGTH_ADJUSTMENT_SUPPORTED,
        CP_FEATURE_FLAG_SENSOR_MEASUREMENT_CONTEXT, // 0-force based, 1-torque based
        CP_FEATURE_FLAG_INSTANTANEOUS_MEASUREMENT_DIRECTION_SUPPORTED,
        CP_FEATURE_FLAG_FACTORY_CALIBRATION_DATE_SUPPORTED,
        CP_FEATURE_FLAG_ENHANCED_OFFSET_COMPENSATION_SUPPORTED,
        CP_FEATURE_FLAG_DISTRIBUTED_SYSTEM_SUPPORT = 20,  // 0-unspecified, 1-not for use in distr. system, 2-used in distr. system, 3-reserved
        CP_FEATURE_FLAG_RESERVED = 22
    } cycling_power_feature_flag_t;
    
    typedef enum {
        CP_CALIBRATION_STATUS_INCORRECT_CALIBRATION_POSITION = 0x01,  
        CP_CALIBRATION_STATUS_MANUFACTURER_SPECIFIC_ERROR_FOLLOWS = 0xFF
    } cycling_power_calibration_status_t;
    
    
    /**
     * @brief Init Server with ATT DB
     *
     * @param feature_flags
     * @param pedal_power_balance_reference
     * @param torque_source
     * @param supported_sensor_locations
     * @param num_supported_sensor_locations
     * @param current_sensor_location
     */
    void cycling_power_service_server_init(uint32_t feature_flags,
                                           cycling_power_pedal_power_balance_reference_t pedal_power_balance_reference, cycling_power_torque_source_t torque_source,
                                           cycling_power_sensor_location_t * supported_sensor_locations, uint16_t num_supported_sensor_locations,
                                           cycling_power_sensor_location_t current_sensor_location);
    
    /**
     * @brief Setup measurement as advertisement data
     * @param adv_interval
     * @param adv_buffer
     * @param adv_size
     * @return
     */
    int cycling_power_get_measurement_adv(uint16_t adv_interval, uint8_t * adv_buffer, uint16_t adv_size);
    
    /**
     * @brief Register callback for the calibration and broadcast updates.
     *
     * @param callback
     */
    void cycling_power_service_server_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Report calibration done.
     * @param measurement_type
     * @param calibrated_value
     */
    void  cycling_power_server_calibration_done(cycling_power_sensor_measurement_context_t measurement_type, uint16_t calibrated_value);
    
    /**
     * @brief Report enhanced calibration done.
     *
     * @param measurement_type
     * @param calibrated_value
     * @param manufacturer_company_id
     * @param num_manufacturer_specific_data
     * @param manufacturer_specific_data
     */
    void cycling_power_server_enhanced_calibration_done(cycling_power_sensor_measurement_context_t measurement_type,
                                                        uint16_t calibrated_value, uint16_t manufacturer_company_id,
                                                        uint8_t num_manufacturer_specific_data, uint8_t * manufacturer_specific_data);
    
    /**
     * @brief Set factory calibration date.
     * @param date
     * @return 1 if successful
     */
    int  cycling_power_service_server_set_factory_calibration_date(gatt_date_time_t date);
    
    /**
     * @brief Set sampling rate.
     * @param sampling_rate_Hz
     */
    void cycling_power_service_server_set_sampling_rate(uint8_t sampling_rate_Hz);
    
    /**
     * @brief Accumulate torque value.
     * @param torque_Nm
     */
    void cycling_power_service_server_add_torque(int16_t torque_Nm);
    
    /**
     * @brief Accumulate wheel revolution value and set the time of the last measurement.
     * @param wheel_revolution
     * @param wheel_event_time_s  unit: seconds
     */
    void cycling_power_service_server_add_wheel_revolution(int32_t wheel_revolution, uint16_t wheel_event_time_s);
    
    /**
     * @brief Accumulate crank revolution value and set the time of the last measurement.
     * @param crank_revolution
     * @param crank_event_time_s  unit: seconds
     */
    void cycling_power_service_server_add_crank_revolution(uint16_t crank_revolution, uint16_t crank_event_time_s);
    
    /**
     * @brief Accumulate energy.
     * @param energy_kJ  unit: kilo Joule
     */
    void cycling_power_service_add_energy(uint16_t energy_kJ);
    
    /**
     * @brief Set the value of the power. The Instantaneous Power field represents either
     * the total power the user is producing or a part of the total power depending on the
     * type of sensor (e.g., single sensor or distributed power sensor system).
     *
     * @param instantaneous_power_W  unit: watt
     */
    void cycling_power_service_server_set_instantaneous_power(int16_t instantaneous_power_W);
    
    /**
     * @brief Set the pedal power balance value. The Pedal Power Balance field represents the ratio between
     * the total amount of power measured by the sensor and a reference (either unknown or left).
     *
     * @param pedal_power_balance_percentage
     */
    void cycling_power_service_server_set_pedal_power_balance(uint8_t pedal_power_balance_percentage);
    
    /**
     * @brief Set the minimum and maximum force value measured in a single crank revolution.
     *
     * This, so called, Extreme Force Magnitude field pair will be included in the Cycling Power Measurement
     * characteristic only if the device supports the Extreme Magnitudes feature and
     * the Sensor Measurement Context of the Cycling Power Feature characteristic is set to 0 (Force-based).
     *
     * @param min_force_magnitude_N  unit: newton
     * @param max_force_magnitude_N  unit: newton
     */
    void cycling_power_service_server_set_force_magnitude(int16_t min_force_magnitude_N, int16_t max_force_magnitude_N);
    
    /**
     * @brief Set the minimum and maximum torque value measured in a single crank revolution.
     *
     * This, so called, Extreme Torque Magnitude field pair will be included in the Cycling Power Measurement
     * characteristic only if the device supports the Extreme Magnitudes feature and
     * the Sensor Measurement Context of the Cycling Power Feature characteristic is set to 1 (Torque-based).
     *
     * @param min_torque_magnitude_Nm  unit: newton meter
     * @param max_torque_magnitude_Nm  unit: newton meter
     */
    void cycling_power_service_server_set_torque_magnitude(int16_t min_torque_magnitude_Nm, int16_t max_torque_magnitude_Nm);
    
    /**
     * @brief Set the minimum and maximum angle of the crank measured in a single crank revolution (unit: degree).
     *
     * This, so called, Extreme Angles Magnitude field pair will be included in the Cycling Power Measurement
     * characteristic only if the device supports the Extreme Angles feature.
     *
     * @param min_angle_degree
     * @param max_angle_degree
     */
    void cycling_power_service_server_set_angle(uint16_t min_angle_degree, uint16_t max_angle_degree);
    
    /**
     * @brief Set the value of the crank angle measured when the value of the Instantaneous Power value becomes positive.
     *
     * This field will be included in the Cycling Power Measurement characteristic
     * only if the device supports the Top and Bottom Dead Spot Angles feature.
     *
     * @param top_dead_spot_angle_degree
     */
    void cycling_power_service_server_set_top_dead_spot_angle(uint16_t top_dead_spot_angle_degree);
    
    /**
     * @brief Set the value of the crank angle measured when the value of the Instantaneous Power value becomes negative.
     *
     * This field will be included in the Cycling Power Measurement characteristic
     * only if the device supports the Top and Bottom Dead Spot Angles feature.
     *
     * @param bottom_dead_spot_angle_degree
     */
    void cycling_power_service_server_set_bottom_dead_spot_angle(uint16_t bottom_dead_spot_angle_degree);
    
    /**
     * @brief Set the raw sensor force magnitude measurements.
     *
     * @param force_magnitude_count
     * @param force_magnitude_N_array  unit: newton
     */
    void cycling_power_service_server_set_force_magnitude_values(int force_magnitude_count, int16_t * force_magnitude_N_array);
    
    /**
     * @brief Set the raw sensor torque magnitude measurements.
     *
     * @param force_magnitude_count
     * @param force_magnitude_N_array  unit: newton meter
     */
    void cycling_power_service_server_set_torque_magnitude_values(int torque_magnitude_count, int16_t * torque_magnitude_Nm_array);
    
    /**
     * @brief Set the instantaneous measurement direction. The field describes
     * the direction of the Instantaneous Magnitude values the Server measures
     * (e.g., Unknown, Tangential Component, Radial Component, or Lateral Component).
     *
     * @param direction
     */
    void cycling_power_service_server_set_instantaneous_measurement_direction(cycling_power_instantaneous_measurement_direction_t direction);
    
    /**
     * Set first crank measurement angle. The value will be present only in the first packet of
     * the Cycling Power Vector notification.
     *
     * @param first_crank_measurement_angle_degree
     */
    void cycling_power_service_server_set_first_crank_measurement_angle(uint16_t first_crank_measurement_angle_degree);
    
    /**
     * Get measurement flags bitmask.
     * @return measurement_flags_bitmask, see cycling_power_measurement_flag_t
     */
    uint16_t cycling_power_service_measurement_flags(void);
    
    /**
     * Get vector flags bitmask.
     * @return vector_flags_bitmask, see cycling_power_vector_flag_t
     */
    uint8_t  cycling_power_service_vector_flags(void);
    
    /**
     * @brief Trigger notification if subscribed
     */
    void cycling_power_service_server_update_values(void);
    

## Cycling Speed and Cadence Service Server API {#sec:cycling_speed_and_cadence_service_server_api}


**cycling_speed_and_cadence_service_server.h** 


    
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
    

## Device Information Service Client API {#sec:device_information_service_client_api}


**device_information_service_client.h** 


    
    /**
     * @brief Initialize Device Information Service. 
     */
    void device_information_service_client_init(void);
    
    /**
     * @brief Query Device Information Service. The client will query the remote service and emit events: 
     *
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MANUFACTURER_NAME
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_MODEL_NUMBER     
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SERIAL_NUMBER    
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_HARDWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_FIRMWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SOFTWARE_REVISION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_SYSTEM_ID        
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_IEEE_REGULATORY_CERTIFICATION
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_PNP_ID 
     * - GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_UDI_FOR_MEDICAL_DEVICES  
     *
     * Event GATTSERVICE_SUBEVENT_DEVICE_INFORMATION_DONE is received when all queries are done, of if service was not found.
     * The status field of this event indicated ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @return status ERROR_CODE_SUCCESS on success, otherwise GATT_CLIENT_IN_WRONG_STATE if query is already in progress
     */
    uint8_t device_information_service_client_query(hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler);
    
    
    /**
     * @brief De-initialize Device Information Service. 
     */
    void device_information_service_client_deinit(void);
    

## Device Information Service Server API {#sec:device_information_service_server_api}


**device_information_service_server.h** 


    
    /**
     * @text Th eDevice Information Service allows to query manufacturer and/or 
     * vendor information about a device.
     * 
     * To use with your application, add `#import <device_information_service.gatt>` to your .gatt file. 
     *
     * *Note*: instead of calling all setters, you can create a local copy of the .gatt file and remove
     * all Characteristics that are not relevant for your application and define all fixed values in the .gatt file.
     */
    
    /**
     * @brief Init Device Information Service Server with ATT DB
     * @param battery_value in range 0-100
     */
    void device_information_service_server_init(void);
    
    /**
     * @brief Set Manufacturer Name
     * @param manufacturer_name
     */
    void device_information_service_server_set_manufacturer_name(const char * manufacturer_name);
    
    /**
     * @brief Set Model Number
     * @param model_number
     */
    void device_information_service_server_set_model_number(const char * model_number);
    
    /**
     * @brief Set Serial Number
     * @param serial_number
     */
    void device_information_service_server_set_serial_number(const char * serial_number);
    
    /**
     * @brief Set Hardware Revision
     * @param hardware_revision
     */
    void device_information_service_server_set_hardware_revision(const char * hardware_revision);
    
    /**
     * @brief Set Firmware Revision
     * @param firmware_revision
     */
    void device_information_service_server_set_firmware_revision(const char * firmware_revision);
    
    /**
     * @brief Set Software Revision
     * @param software_revision
     */
    void device_information_service_server_set_software_revision(const char * software_revision);
    
    /**
     * @brief Set System ID
     * @param manufacturer_identifier uint40
     * @param organizationally_unique_identifier uint24
     */
    void device_information_service_server_set_system_id(uint64_t manufacturer_identifier, uint32_t organizationally_unique_identifier);
    
    /**
     * @brief Set IEEE 11073-20601 regulatory certification data list
     * @note: format duint16. duint16 is two uint16 values concatenated together.
     * @param value_a
     * @param value_b
     */
    void device_information_service_server_set_ieee_regulatory_certification(uint16_t value_a, uint16_t value_b);
    
    /**
     * @brief Set PnP ID
     * @param vendor_source_id
     * @param vendor_id
     * @param product_id
     * @param product_version
     */
    void device_information_service_server_set_pnp_id(uint8_t vendor_source_id, uint16_t vendor_id, uint16_t product_id, uint16_t product_version);
    
    /**
     * @brief Set UDI for medical devices
     * @param label
     * @param device_id
     * @param issuer
     * @param authority
     */
    void device_information_service_server_set_udi_for_medical_devices(const char * label, const char * device_id, const char * issuer, const char * authority);
    

## Heart Rate Service Server API {#sec:heart_rate_service_server_api}


**heart_rate_service_server.h** 


    
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
    

## HID Service Server API {#sec:hids_device_api}


**hids_device.h** 


    
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
    

## HID Service Client - HIDS Host API {#sec:hids_host_api}


**hids_host.h** 


    
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
    

## Immediate Alert Service Client API {#sec:immediate_alert_service_client_api}


**immediate_alert_service_client.h** 


    
        
    /**
     * @brief Initialize Immediate Alert Service. 
     */
    void immediate_alert_service_client_init(void);
    
     /**
     * @brief Connect to a Immediate Alert Service instance of remote device. The client will automatically register for notifications.
     * The mute state is received via GATTSERVICE_SUBEVENT_LLS_CLIENT_MUTE event.
     * The mute state can be 0 - off, 1 - on, 2 - disabled and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * Event GATTSERVICE_SUBEVENT_LLS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param iac_connection
     * @param iac_characteristics_storage           storage for characteristics
     * @param iac_characteristics_num               == IMMEDIATE_ALERT_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t immediate_alert_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             ias_client_connection_t * iac_connection,
             uint16_t * ias_cid
    );
    
    uint8_t immediate_alert_service_client_write_alert_level(uint16_t ias_cid, ias_alert_level_t alert_level);
    
    
    /**
     * @brief Disconnect.
     * @param ias_cid
     * @return status
     */
    uint8_t immediate_alert_service_client_disconnect(uint16_t ias_cid);
    
    /**
     * @brief De-initialize Immediate Alert Service. 
     */
    void immediate_alert_service_client_deinit(void);
    

## Immediate Alert Service Server API {#sec:immediate_alert_service_server_api}


**immediate_alert_service_server.h** 


    
    /**
     * @brief Init Immediate Alert Service Server with ATT DB
     * @param mute_state
     */
    void immediate_alert_service_server_init(void);
    
    /**
     * @brief Register packet handler
     */
    void immediate_alert_service_server_register_packet_handler(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Set alert level.
     * @param alert_level
     */
    uint8_t immediate_alert_service_server_set_alert_level(const ias_alert_level_t alert_level);
    
    /**
     * @brief Set alert timeout. Default is 0 that equals to no timeout.
     * @param alert_timeout_ms
     */
    uint8_t immediate_alert_service_server_set_alert_timeout(uint32_t alert_timeout_ms);
    
    
    void immediate_alert_service_server_stop_alerting(void);
    

## Immediate Alert Service Util API {#sec:immediate_alert_service_util_api}


**immediate_alert_service_util.h** 



## Link Loss Service Client API {#sec:link_loss_service_client_api}


**link_loss_service_client.h** 


    
        
    /**
     * @brief Initialize Link Loss Service. 
     */
    void link_loss_service_client_init(void);
    
     /**
     * @brief Connect to a Link Loss Service instance of remote device. The client will automatically register for notifications.
     * The mute state is received via GATTSERVICE_SUBEVENT_LLS_CLIENT_MUTE event.
     * The mute state can be 0 - off, 1 - on, 2 - disabeled and it is valid if the ATT status is equal to ATT_ERROR_SUCCESS,
     * see ATT errors (see bluetooth.h) for other values.
     *   
     * Event GATTSERVICE_SUBEVENT_LLS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param llc_connection
     * @param llc_characteristics_storage           storage for characteristics
     * @param llc_characteristics_num               == LINK_LOSS_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t link_loss_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             lls_client_connection_t * llc_connection,
             gatt_service_client_characteristic_t * llc_characteristics_storage, uint8_t llc_characteristics_num,
             uint16_t * lls_cid
    );
    
    uint8_t link_loss_service_client_read_alert_level(uint16_t lls_cid);
    uint8_t link_loss_service_client_write_alert_level(uint16_t lls_cid, lls_alert_level_t alert_level);
    
    
    /**
     * @brief Disconnect.
     * @param lls_cid
     * @return status
     */
    uint8_t link_loss_service_client_disconnect(uint16_t lls_cid);
    
    /**
     * @brief De-initialize Link Loss Service. 
     */
    void link_loss_service_client_deinit(void);
    

## Link Loss Service Server API {#sec:link_loss_service_server_api}


**link_loss_service_server.h** 


    
    /**
     * @brief Init Link Loss Service Server with ATT DB
     * @param mute_state
     */
    void link_loss_service_server_init(void);
    
    /**
     * @brief Register packet handler
     */
    void link_loss_service_server_register_packet_handler(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Set alert level. Default is no alert.
     * @param alert_level
     */
    uint8_t link_loss_service_server_set_alert_level(lls_alert_level_t alert_level);
    
    /**
     * @brief Set alert timeout. Default is 0 that equals to no timeout.
     * @param alert_timeout_ms
     */
    uint8_t link_loss_service_server_set_alert_timeout(uint32_t alert_timeout_ms);
    
    
    void link_loss_service_server_stop_alerting(void);
    

## Link Loss Service Util API {#sec:link_loss_service_util_api}


**link_loss_service_util.h** 



## Nordic SPP Service Server API {#sec:nordic_spp_service_server_api}


**nordic_spp_service_server.h** 


    
    /**
     * @text The Nordic SPP Service is implementation of the Nordic SPP-like profile.
     *
     * To use with your application, add `#import <nordic_spp_service.gatt>` to your .gatt file
     * and call all functions below. All strings and blobs need to stay valid after calling the functions.
     */
    
    /**
     * @brief Init Nordic SPP Service Server with ATT DB
     * @param packet_handler for events and tx data from peer as RFCOMM_DATA_PACKET
     */
    void nordic_spp_service_server_init(btstack_packet_handler_t packet_handler);
    
    /** 
     * @brief Queue send request. When called, one packet can be send via nordic_spp_service_send below
     * @param request
     * @param con_handle
     */
    void nordic_spp_service_server_request_can_send_now(btstack_context_callback_registration_t * request, hci_con_handle_t con_handle);
    
    /**
     * @brief Send data
     * @param con_handle
     * @param data
     * @param size
     */
    int nordic_spp_service_server_send(hci_con_handle_t con_handle, const uint8_t * data, uint16_t size);
    

## Scan Parameters Service Client API {#sec:scan_parameters_service_client_api}


**scan_parameters_service_client.h** 


    
    /**
     * @brief Initialize Scan Parameters Service. 
     */
    void scan_parameters_service_client_init(void);
    
    /**
     * @brief Set Scan Parameters Service. It will update all connected devices.
     * @param scan_interval
     * @param scan_window
     */
    void scan_parameters_service_client_set(uint16_t scan_interval, uint16_t scan_window);
    
    /**
     * @brief Connect to Scan Parameters Service of remote device. 
     *
     * The GATTSERVICE_SUBEVENT_SCAN_PARAMETERS_SERVICE_CONNECTED event completes the request. 
     * Its status is set to ERROR_CODE_SUCCESS if remote service and SCAN_INTERVAL_WINDOW characteristic are found. 
     * *
     * Other status codes of this event:
     * - GATT_CLIENT_IN_WRONG_STATE: client in wrong state
     * - ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE: service or characteristic not found
     * - ATT errors, see bluetooth.h
     *
     * @param con_handle
     * @param packet_handler
     * @param sps_connection
     * @param sps_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if client with con_handle not found
     */
    uint8_t scan_parameters_service_client_connect(
            hci_con_handle_t con_handle, btstack_packet_handler_t packet_handler,
            sps_client_connection_t * sps_connection,
            uint16_t * sps_cid);
    
    /**
     * @brief Disconnect from Scan Parameters Service.
     * @param scan_parameters_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if client with con_handle is not found
     */
    uint8_t scan_parameters_service_client_disconnect(uint16_t scan_parameters_cid);
    
    
    /**
     * @brief De-initialize Scan Parameters Service. 
     */
    void scan_parameters_service_client_deinit(void);
    

## Scan Parameters Service Server API {#sec:scan_parameters_service_server_api}


**scan_parameters_service_server.h** 


    
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
    

## Immediate Alert Service Client API {#sec:tx_power_service_client_api}


**tx_power_service_client.h** 


    
        
    /**
     * @brief Initialize Immediate Alert Service. 
     */
    void tx_power_service_client_init(void);
    
     /**
     * @brief Connect to a TX Power Service instance of remote device. The client will automatically register for notifications.
     * The TX Power level state is received via GATTSERVICE_SUBEVENT_TXPS_CLIENT_TX_POWER_LEVEL event.
     *
     * Event GATTSERVICE_SUBEVENT_TXPS_CLIENT_CONNECTED is emitted with status ERROR_CODE_SUCCESS on success, otherwise
     * GATT_CLIENT_IN_WRONG_STATE, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if no audio input control service is found, or ATT errors (see bluetooth.h). 
     *
     * @param con_handle
     * @param packet_handler
     * @param iac_connection
     * @param iac_characteristics_storage           storage for characteristics
     * @param iac_characteristics_num               == TX_POWER_SERVICE_CLIENT_NUM_CHARACTERISTICS
     * @return status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED if there is already a client associated with con_handle, or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t tx_power_service_client_connect(
             hci_con_handle_t con_handle,
             btstack_packet_handler_t packet_handler,
             txps_client_connection_t * iac_connection,
             gatt_service_client_characteristic_t * iac_characteristics_storage, uint8_t iac_characteristics_num,
             uint16_t * txps_cid
    );
    
    /**
     * @note Trigger read of TX Power Level characteristic.
     * @param txps_cid
     * @return status
     */
    uint8_t tx_power_service_client_read_tx_power_level(uint16_t txps_cid);
    
    /**
     * @brief Disconnect.
     * @param txps_cid
     * @return status
     */
    uint8_t tx_power_service_client_disconnect(uint16_t txps_cid);
    
    /**
     * @brief De-i  nitialize Immediate Alert Service.
     */
    void tx_power_service_client_deinit(void);
    

## TX Power Service Server API {#sec:tx_power_service_server_api}


**tx_power_service_server.h** 


    
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
    

## u-blox SPP Service Server API {#sec:ublox_spp_service_server_api}


**ublox_spp_service_server.h** 


    
    /**
     * @text The u-blox SPP Service is implementation of the u-Blox SPP-like profile.
     *
     * To use with your application, add `#import <ublox_spp_service.gatt>` to your .gatt file
     * and call all functions below. All strings and blobs need to stay valid after calling the functions.
     */
    
    /**
     * @brief Init ublox SPP Service Server with ATT DB
     * @param packet_handler for events and tx data from peer as RFCOMM_DATA_PACKET
     */
    void ublox_spp_service_server_init(btstack_packet_handler_t packet_handler);
    
    /** 
     * @brief Queue send request. When called, one packet can be send via ublox_spp_service_send below
     * @param request
     * @param con_handle
     */
    void ublox_spp_service_server_request_can_send_now(btstack_context_callback_registration_t * request, hci_con_handle_t con_handle);
    
    /**
     * @brief Send data
     * @param con_handle
     * @param data
     * @param size
     */
    int ublox_spp_service_server_send(hci_con_handle_t con_handle, const uint8_t * data, uint16_t size);
    

## GATT Client API {#sec:gatt_client_api}


**gatt_client.h** 


    
    typedef struct {
        uint16_t start_group_handle;
        uint16_t end_group_handle;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_service_t;
    
    typedef struct {
        uint16_t start_handle;
        uint16_t value_handle;
        uint16_t end_handle;
        uint16_t properties;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_characteristic_t;
    
    typedef struct {
        uint16_t handle;
        uint16_t uuid16;
        uint8_t  uuid128[16];
    } gatt_client_characteristic_descriptor_t;
    
    /** 
     * @brief Set up GATT client.
     */
    void gatt_client_init(void);
    
    /**
     * @brief Set minimum required security level for GATT Client
     * @note  The Bluetooth specification makes the GATT Server responsible to check for security.
     *        This allows an attacker to spoof an existing device with a GATT Servers, but skip the authentication part.
     *        If your application is exchanging sensitive data with a remote device, you would need to manually check
     *        the security level before sending/receive such data.
     *        With level > 0, the GATT Client triggers authentication for all GATT Requests and defers any exchange
     *        until the required security level is established.
     *        gatt_client_request_can_write_without_response_event does not trigger authentication
     *        gatt_client_request_to_write_without_response does not trigger authentication
     *  @pram level, default LEVEL_0 (no encryption required)
     */
    void gatt_client_set_required_security_level(gap_security_level_t level);
    
    /**
     * @brief Connect to remote GATT Server over Classic (BR/EDR) Connection
     *        GATT_EVENT_CONNECTED with status and con_handle for other API functions
     *        is emitted on connection complete.
     * @note requires ENABLE_GATT_OVER_CLASSIC.
     * @param addr
     * @return status
     */
    uint8_t gatt_client_classic_connect(btstack_packet_handler_t callback, bd_addr_t addr);
    
    /**
     * @brief Disconnect o Classic (BR/EDR) connection to a remote GATT Server
     * @note requires ENABLE_GATT_OVER_CLASSIC
     * @param con_handle
     * @return status
     */
    uint8_t gatt_client_classic_disconnect(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Setup Enhanced LE Bearer with up to 5 channels on existing LE connection
     * @param callback for GATT_EVENT_CONNECTED and GATT_EVENT_DISCONNECTED events
     * @param con_handle
     * @param num_channels
     * @param storage_buffer for L2CAP connection
     * @param storage_size - each channel requires (2 * ATT MTU) + 10 bytes
     * @return
     */
    uint8_t gatt_client_le_enhanced_connect(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint8_t num_channels, uint8_t * storage_buffer, uint16_t storage_size);
    
    /**
     * @brief MTU is available after the first query has completed. If status is equal to ERROR_CODE_SUCCESS, it returns the real value, 
     * otherwise the default value ATT_DEFAULT_MTU (see bluetooth.h). 
     * @param  con_handle   
     * @param  mtu or 0 in case of error
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if MTU is not exchanged and MTU auto-exchange is disabled
     *                ERROR_CODE_SUCCESS                                        on success
     */
    uint8_t gatt_client_get_mtu(hci_con_handle_t con_handle, uint16_t * mtu);
    
    /**
     * @brief Sets whether a MTU Exchange Request shall be automatically send before the 
     * first attribute read request is send. Default is enabled.
     * @param enabled   
     */
    void gatt_client_mtu_enable_auto_negotiation(uint8_t enabled);
    
    /**
     * @brief Sends a MTU Exchange Request, this allows for the client to exchange MTU 
     * when gatt_client_mtu_enable_auto_negotiation is disabled.
     * @param  callback   
     * @param  con_handle
     */
    void gatt_client_send_mtu_negotiation(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Returns 1 if the GATT client is ready to receive a query. It is used with daemon. 
     * @param  con_handle
     * @return is_ready_status     0 - if no GATT client for con_handle is found, or is not ready, otherwise 1
     */
    int gatt_client_is_ready(hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers all primary services. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_primary_services(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers all secondary services. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_secondary_services(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /** 
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param callback   
     * @param con_handle
     * @param uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t uuid16);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times.
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param callback
     * @param con_handle
     * @param uuid16
     * @param service_id    - context provided to callback in events
     * @param connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_primary_services_by_uuid16_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                         uint16_t uuid16, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param  callback   
     * @param  con_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, const uint8_t * uuid128);
    
    /**
     * @brief Discovers a specific primary service given its UUID. This service may exist multiple times. 
     * For each found service a GATT_EVENT_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * @param callback   
     * @param con_handle
     * @param uuid128
     * @param service_id    - context provided to callback in events
     * @param connection_id - context provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_discover_primary_services_by_uuid128_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                          const uint8_t * uuid128, uint16_t service_id, uint16_t connection_id);
    
    /** 
     * @brief Finds included services within the specified service. 
     * For each found included service a GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT event will be emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery. 
     * Information about included service type (primary/secondary) can be retrieved either by sending 
     * an ATT find information request for the returned start group handle 
     * (returning the handle and the UUID for primary or secondary service) or by comparing the service 
     * to the list of all primary services. 
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered  
     */
    uint8_t gatt_client_find_included_services_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service);
    
    /**
     * @brief Finds included services within the specified service.
     * For each found included service a GATT_EVENT_INCLUDED_SERVICE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * Information about included service type (primary/secondary) can be retrieved either by sending
     * an ATT find information request for the returned start group handle
     * (returning the handle and the UUID for primary or secondary service) or by comparing the service
     * to the list of all primary services.
     * @param  callback
     * @param  con_handle
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @param  service_id
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_find_included_services_for_service_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                        gatt_client_service_t * service, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Discovers all characteristics within the specified service. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will be emited. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service);
    
    /**
     * @brief Discovers all characteristics within the specified service.
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will be emited.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback
     * @param  con_handle
     * @param  service
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_characteristics_for_service_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service,
                                                                          uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief The following four functions are used to discover all characteristics within 
     * the specified service or handle range, and return those that match the given UUID. 
     * 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_handle_range_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service, uint16_t uuid16);
    
    /** 
     * @brief The following four functions are used to discover all characteristics within the 
     * specified service or handle range, and return those that match the given UUID. 
     * For each found characteristic a GATT_EVENT_CHARACTERISTIC_QUERY_RESULT event will emitted. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  service
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristics_for_service_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_service_t * service, const uint8_t * uuid128);
    
    /** 
     * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic. 
     * For each found descriptor a GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT event will be emitted.
     * 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_discover_characteristic_descriptors(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    
    /**
     * @brief Discovers attribute handle and UUID of a characteristic descriptor within the specified characteristic.
     * For each found descriptor a GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT event will be emitted.
     *
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of discovery.
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_discover_characteristic_descriptors_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                         gatt_client_characteristic_t * characteristic, uint16_t service_id, uint16_t connection_it);
    
    /** 
     * @brief Reads the characteristic value using the characteristic's value handle. 
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /** 
     * @brief Reads the characteristic value using the characteristic's value handle. 
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle);
    
    /**
     * @brief Reads the characteristic value using the characteristic's value handle.
     * If the characteristic value is found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_value_of_characteristic_using_value_handle_with_context(btstack_packet_handler_t callback,
                                                                                     hci_con_handle_t con_handle,
                                                                                     uint16_t value_handle,
                                                                                     uint16_t service_id,
                                                                                     uint16_t connection_id);
    
    /**
     * @brief Reads the characteric value of all characteristics with the uuid. 
     * For each characteristic value found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid16
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristics_by_uuid16(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, uint16_t uuid16);
    
    /**
     * @brief Reads the characteric value of all characteristics with the uuid. 
     * For each characteristic value found a GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  start_handle
     * @param  end_handle
     * @param  uuid128
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_value_of_characteristics_by_uuid128(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t start_handle, uint16_t end_handle, const uint8_t * uuid128);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  characteristic 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle);
    
    /**
     * @brief Reads the long characteristic value using the characteristic's value handle.
     * The value will be returned in several blobs.
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_context(btstack_packet_handler_t callback,
                                                                                          hci_con_handle_t con_handle, uint16_t value_handle,
                                                                                          uint16_t service_id, uint16_t connection_id);
    
    /** 
     * @brief Reads the long characteristic value using the characteristic's value handle. 
     * The value will be returned in several blobs. 
     * For each blob, a GATT_EVENT_LONG_CHARACTERISTIC_VALUE_QUERY_RESULT event with updated value offset will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  offset 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t offset);
    
    /*
     * @brief Read multiple characteristic values.
     * The all results are emitted via single GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event, 
     * followed by the GATT_EVENT_QUERY_COMPLETE event, which marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  num_value_handles 
     * @param  value_handles list of handles 
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_read_multiple_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
    
    /*
     * @brief Read multiple varaible characteristic values. Only supported over LE Enhanced Bearer
     * The all results are emitted via single GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT event,
     * followed by the GATT_EVENT_QUERY_COMPLETE event, which marks the end of read.
     * @param  callback
     * @param  con_handle
     * @param  num_value_handles
     * @param  value_handles list of handles
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                ERROR_CODE_SUCCESS         , if query is successfully registered
     */
    uint8_t gatt_client_read_multiple_variable_characteristic_values(btstack_packet_handler_t callback, hci_con_handle_t con_handle, int num_value_handles, uint16_t * value_handles);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle without 
     * an acknowledgment that the write was successfully performed.
     * @param  con_handle   
     * @param  value_handle
     * @param  value_length
     * @param  value is copied on success and does not need to be retained
     * @return status BTSTACK_MEMORY_ALLOC_FAILED, if no GATT client for con_handle is found 
     *                GATT_CLIENT_IN_WRONG_STATE , if GATT client is not ready
     *                BTSTACK_ACL_BUFFERS_FULL   , if L2CAP cannot send, there are no free ACL slots
     *                ERROR_CODE_SUCCESS         , if query is successfully registered 
     */
    uint8_t gatt_client_write_value_of_characteristic_without_response(hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the authenticated characteristic value using the characteristic's value handle 
     * without an acknowledgment that the write was successfully performed.
     * @note GATT_EVENT_QUERY_COMPLETE is emitted with ATT_ERROR_SUCCESS for success,
     * or ATT_ERROR_BONDING_INFORMATION_MISSING if there is no bonding information stored.
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  message_len
     * @param  message is not copied, make sure memory is accessible until write is done
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_signed_write_without_response(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                      uint16_t message_len, uint8_t * message);
    
    /** 
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed, if the event's att_status field is set to 
     * ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic value using the characteristic's value handle.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed, if the event's att_status field is set to
     * ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_value_of_characteristic_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                                   uint16_t value_length, uint8_t * value, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle,
                                                                        uint16_t value_length, uint8_t * value, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Writes the characteristic value using the characteristic's value handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_value_of_characteristic_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes of the long characteristic value using the characteristic's value handle. 
     * It uses server response to validate that the write was correctly received. 
     * The GATT_EVENT_QUERY_COMPLETE EVENT marks the end of write.
     * The write is successfully performed, if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes). 
     * @param  callback   
     * @param  con_handle
     * @param  value_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_reliable_write_long_value_of_characteristic(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t value_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Reads the characteristic descriptor using its handle. 
     * If the characteristic descriptor is found, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor);
    
    /** 
     * @brief Reads the characteristic descriptor using its handle. 
     * If the characteristic descriptor is found, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle);
    
    /** 
     * @brief Reads the long characteristic descriptor using its handle. It will be returned in several blobs. 
     * For each blob, a GATT_EVENT_CHARACTERISTIC_DESCRIPTOR_QUERY_RESULT event will be emitted.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of read.
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  offset
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_descriptor_t * descriptor, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the characteristic descriptor using its handle. 
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback   
     * @param  con_handle
     * @param  descriptor_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t descriptor_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /** 
     * @brief Writes the client characteristic configuration of the specified characteristic. 
     * It is used to subscribe for notifications or indications of the characteristic value. 
     * For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
     * resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write. 
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  configuration                                                    GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found 
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated 
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED     if configuring notification, but characteristic has no notification property set
     *                GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED       if configuring indication, but characteristic has no indication property set
     *                ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE         if configuration is invalid
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_client_characteristic_configuration(btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic, uint16_t configuration);
    
    /**
     * @brief Writes the client characteristic configuration of the specified characteristic.
     * It is used to subscribe for notifications or indications of the characteristic value.
     * For notifications or indications specify: GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION
     * resp. GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION as configuration value.
     * The GATT_EVENT_QUERY_COMPLETE event marks the end of write.
     * The write is successfully performed if the event's att_status field is set to ATT_ERROR_SUCCESS (see bluetooth.h for ATT_ERROR codes).
     * @param  callback
     * @param  con_handle
     * @param  characteristic
     * @param  configuration                                                    GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_NOTIFICATION, GATT_CLIENT_CHARACTERISTICS_CONFIGURATION_INDICATION
     * @param  service_id    - context provided to callback in events
     * @param  connection_id - contest provided to callback in events
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER                  if no HCI connection for con_handle is found
     *                BTSTACK_MEMORY_ALLOC_FAILED                               if no GATT client for con_handle could be allocated
     *                GATT_CLIENT_IN_WRONG_STATE                                if GATT client is not ready
     *                GATT_CLIENT_CHARACTERISTIC_NOTIFICATION_NOT_SUPPORTED     if configuring notification, but characteristic has no notification property set
     *                GATT_CLIENT_CHARACTERISTIC_INDICATION_NOT_SUPPORTED       if configuring indication, but characteristic has no indication property set
     *                ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE         if configuration is invalid
     *                ERROR_CODE_SUCCESS                                        if query is successfully registered
     */
    uint8_t gatt_client_write_client_characteristic_configuration_with_context(btstack_packet_handler_t callback, hci_con_handle_t con_handle,
                                                                               gatt_client_characteristic_t * characteristic, uint16_t configuration, uint16_t service_id, uint16_t connection_id);
    
    /**
     * @brief Register for changes to the Service Changed and Database Hash Characteristics of the remote GATT Service
     * *
     * When configured, GATT_EVENT_QUERY_COMPLETE event is emitted
     * If supported, the Database Hash is read as well
     *
     * Requires ENABLE_GATT_CLIENT_CACHING
     *
     * @param callback
     */
    void gatt_client_add_service_changed_handler(btstack_packet_callback_registration_t * callback);
    
    /**
     * @brief Remove callback for service changes
     *
     * Requires ENABLE_GATT_CLIENT_CACHING
     *
     * @param callback
     */
    void gatt_client_remove_service_changed_handler(btstack_packet_callback_registration_t * callback);
    
    /**
     * @brief Register for notifications and indications of a characteristic enabled by 
     * the gatt_client_write_client_characteristic_configuration function.
     * @param notification struct used to store registration
     * @param callback
     * @param con_handle or GATT_CLIENT_ANY_CONNECTION to receive updates from all connected devices
     * @param characteristic or NULL to receive updates for all characteristics
     */
    void gatt_client_listen_for_characteristic_value_updates(gatt_client_notification_t * notification, btstack_packet_handler_t callback, hci_con_handle_t con_handle, gatt_client_characteristic_t * characteristic);
    
    /**
     * @brief Stop listening to characteristic value updates registered with
     * the gatt_client_listen_for_characteristic_value_updates function.
     * @param notification struct used in gatt_client_listen_for_characteristic_value_updates
     */
    void gatt_client_stop_listening_for_characteristic_value_updates(gatt_client_notification_t * notification);
    
    /**
     * @brief Register for notifications and indications of characteristic in a service
     * the gatt_client_write_client_characteristic_configuration function.
     * @param notification struct used to store registration
     * @param callback
     * @param con_handle or GATT_CLIENT_ANY_CONNECTION to receive updates from all connected devices
     * @param service
     * @param end_handle
     * @param service_id    - context provided to callback in events
     * @param connection_id - contest provided to callback in events
     */
    void gatt_client_listen_for_service_characteristic_value_updates(gatt_client_service_notification_t * notification,
                                                                     btstack_packet_handler_t callback,
                                                                     hci_con_handle_t con_handle,
                                                                     gatt_client_service_t * service,
                                                                     uint16_t service_id,
                                                                     uint16_t connection_id);
    
    /**
     * @brief Stop listening to characteristic value updates for registered service with
     * the gatt_client_listen_for_characteristic_value_updates function.
     * @param notification struct used in gatt_client_listen_for_characteristic_value_updates
     */
    void gatt_client_stop_listening_for_service_characteristic_value_updates(gatt_client_service_notification_t * notification);
    
    
    /**
     * @brief Transactional write. It can be called as many times as it is needed to write the characteristics within the same transaction. 
     * Call the gatt_client_execute_write function to commit the transaction.
     * @param  callback   
     * @param  con_handle
     * @param  attribute_handle
     * @param  offset of value
     * @param  value_length
     * @param  value is not copied, make sure memory is accessible until write is done, i.e. GATT_EVENT_QUERY_COMPLETE is received
     */
    uint8_t gatt_client_prepare_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint16_t value_length, uint8_t * value);
    
    /**
     * @brief Commit transactional write. GATT_EVENT_QUERY_COMPLETE is received.
     * @param  callback   
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_execute_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Abort transactional write. GATT_EVENT_QUERY_COMPLETE is received.
     * @param  callback   
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_cancel_write(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when regular gatt query can be sent
     * @note callback might happen during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection handle unknown,
     *       - ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if GATT client memory exceeded, or
     *       - ERROR_CODE_COMMAND_DISALLOWED if GATT client already registered callback
     */
    uint8_t gatt_client_request_to_send_gatt_query(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Remove queued callback for regular gatt queries, to be used on disconnect for example
     * @param callback_registration
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok
     */
    uint8_t gatt_client_remove_gatt_query(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    /**
     * @brief Request callback when writing characteristic value without response is possible
     * @note callback might happen during call to this function
     * @param callback_registration to point to callback function and context information
     * @param con_handle
     * @return ERROR_CODE_SUCCESS if ok, ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection handle unknown, and ERROR_CODE_COMMAND_DISALLOWED if callback already registered
     */
    uint8_t gatt_client_request_to_write_without_response(btstack_context_callback_registration_t * callback_registration, hci_con_handle_t con_handle);
    
    
    // the following functions are marked as deprecated and will be removed eventually
    /**
     * @brief Requests GATT_EVENT_CAN_WRITE_WITHOUT_RESPONSE that guarantees
     * a single successful gatt_client_write_value_of_characteristic_without_response call.
     * @deprecated please use gatt_client_request_to_write_without_response instead
     * @param  callback
     * @param  con_handle
     * @return status
     */
    uint8_t gatt_client_request_can_write_without_response_event(btstack_packet_handler_t callback, hci_con_handle_t con_handle);
    
    #ifdef ENABLE_GATT_CLIENT_CACHING
    /**
     * @brief Get GATT Database Hash for remote device
     * @param con_handle
     * @return database hash or NULL if not available
     */
    const uint8_t * gatt_client_get_database_hash(hci_con_handle_t con_handle);
    
    /**
     * @brief Get next cache id for remote device. This allows to enumerate all GATT Service Client requets
     * @param con_handle
     * @return
     */
    uint16_t gatt_client_get_next_cache_id(hci_con_handle_t con_handle);
    #endif
    
    /**
     * @brief Map ATT Error Code to (extended) Error Codes
     * @param att_error_code
     * @return
     */
    uint8_t gatt_client_att_status_to_error_code(uint8_t att_error_code);
    

## GATT Service Client API {#sec:gatt_service_client_api}


**gatt_service_client.h** 


    
    
    /**
     * @brief Initialize GATT Service Client infrastructure
     */
    void gatt_service_client_init(void);
    
    /**
     * @brief Register new GATT Service Client with list of Characteristic UUID16s
     * @param client
     * @param connection_and_notification_handler packet handler that receives events: GATT_EVENT_NOTIFICATION, GATT_EVENT_IDENTIFICATION and two HCI_EVENT_GATTSERVICE_META subevents: GATTSERVICE_SUBEVENT_CLIENT_CONNECTED/DISCONNECTED
     * @param characteristic_uuid16s
     * @param characteristic_uuid16s_num
     */
    void gatt_service_client_register_client_with_uuid16s(gatt_service_client_t *client, btstack_packet_handler_t connection_and_notification_handler,
                                             const uint16_t *characteristic_uuid16s, uint16_t characteristic_uuid16s_num);
    
    /**
     * @brief Register new GATT Service Client with list of Characteristic UUID128s
     * @param client
     * @param connection_and_notification_handler packet handler that receives events: GATT_EVENT_NOTIFICATION, GATT_EVENT_IDENTIFICATION and two HCI_EVENT_GATTSERVICE_META subevents: GATTSERVICE_SUBEVENT_CLIENT_CONNECTED/DISCONNECTED
     * @param characteristic_uuid128s
     * @param characteristic_uuid128s_num
     */
    
    void gatt_service_client_register_client_with_uuid128s(gatt_service_client_t *client, btstack_packet_handler_t packet_handler,
                                             const uuid128_t *characteristic_uuid128s, uint16_t characteristic_uuid128s_num);
    
    /**
     * @brief Get Characteristic UUID16 for given Characteristic index
     *
     * @param client
     * @param characteristic_index
     * @return uuid16 or 0 if index out of range
     */
    uint16_t gatt_service_client_characteristic_uuid16_for_index(const gatt_service_client_t * client, uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic UUID128 for given Characteristic index
     *
     * @param client
     * @param characteristic_index
     * @return uuid128 or NULL if index out of range
     */
    const uuid128_t * gatt_service_client_characteristic_uuid128_for_index(const gatt_service_client_t * client, uint8_t characteristic_index);
    
    /**
     * @brief Unregister GATT Service Client
     * @param client
     */
    void gatt_service_client_unregister_client(gatt_service_client_t * client);
    
    /**
     * @brief Connect to the first instance of Primary GATT Service with UUID16
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid16
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_primary_service_with_uuid16(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                            gatt_service_client_connection_t *connection,
                                                            uint16_t service_uuid16,
                                                            gatt_service_client_characteristic_t *characteristics,
                                                            uint8_t characteristics_num);
    
    /**
     * @brief Connect to the n-th instance of Primary GATT Service with UUID16
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid128
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_primary_service_with_uuid128(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                                    gatt_service_client_connection_t *connection,
                                                                    const uuid128_t * service_uuid128,
                                                                    gatt_service_client_characteristic_t *characteristics,
                                                                    uint8_t characteristics_num);
    
    /**
     * @brief Connect to the Secondary GATT Service with given handle range
     *
     * UUID16 and Service Index are stored for GATT Service Client user only
     *
     * @param con_handle
     * @param client
     * @param connection
     * @param service_uuid16
     * @param service_start_handle
     * @param service_end_handle
     * @param service_index
     * @param characteristics
     * @param characteristics_num
     * @return
     */
    uint8_t gatt_service_client_connect_secondary_service_with_uuid16(hci_con_handle_t con_handle, gatt_service_client_t *client,
                                                              gatt_service_client_connection_t *connection,
                                                              uint16_t service_uuid16, uint8_t service_index,
                                                              uint16_t service_start_handle, uint16_t service_end_handle,
                                                              gatt_service_client_characteristic_t *characteristics,
                                                              uint8_t characteristics_num);
    
    /**
     * @brief Disconnect service client
     * @param client
     * @param connection
     * @return
     */
    uint8_t gatt_service_client_disconnect(gatt_service_client_connection_t *connection);
    
    /**
     * @brief Check if Characteristic is available and can be queried
     * @param client
     * @param connection
     * @param characteristic_index
     * @return ERROR_CODE_SUCCESS if ready, ERROR_CODE_COMMAND_DISALLOWED or ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE otherwise
     */
    uint8_t gatt_service_client_can_query_characteristic(const gatt_service_client_connection_t *connection,
                                                 uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic Value Handle for given Characteristic index
     * *
     * @param connection
     * @param characteristic_index
     * @return
     */
    uint16_t gatt_service_client_characteristic_value_handle_for_index(const gatt_service_client_connection_t *connection,
                                                                       uint8_t characteristic_index);
    
    /**
     * @brief Get Characteristic index Handle for given Characteristic Value Handle
     * *
     * @param connection
     * @param value_handle
     * @return index of characteristic in initial list or GATT_SERVICE_CLIENT_INVALID_INDEX
     */
    uint8_t gatt_service_client_characteristic_index_for_value_handle(const gatt_service_client_connection_t *connection,
                                                                      uint16_t value_handle);
    
    /**
     * @brief Get connection id
     * @param client
     * @param connection
     * @returns connection_id
     */
    uint16_t gatt_service_client_get_connection_id(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get connection handle
     * @param client
     * @param connection
     * @returns con_handle
     */
    hci_con_handle_t gatt_service_client_get_con_handle(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get service index provided in connect call
     * @param client
     * @param connection
     * @returns connection_id
     */
    uint8_t gatt_service_client_get_service_index(const gatt_service_client_connection_t * connection);
    
    /**
     * @brief Get remote MTU
     * @param client
     * @param connection
     * @returns MTU or 0 in case of error
     */
    uint16_t gatt_service_client_get_mtu(const gatt_service_client_connection_t *connection);
    
    
    /**
     * @brief Dump characteristic value handles
     * @param client
     * @param connection
     * @param characteristic_names
     */
    void gatt_service_client_dump_characteristic_value_handles(const gatt_service_client_connection_t *connection,
                                                               const char **characteristic_names);
    
    /**
     * Return connection for given CID
     * @param client
     * @param connection_cid
     * @return
     */
    gatt_service_client_connection_t * gatt_service_client_get_connection_for_cid(const gatt_service_client_t *client, uint16_t connection_cid);
    
    /**
     * Substitute subevent ID with the given one, and send the event
     * @param callback
     * @param packet
     * @param size
     * @param subevent_id
     */
    void gatt_service_client_replace_subevent_id_and_emit(btstack_packet_handler_t callback, uint8_t * packet, uint16_t size, uint8_t subevent_id);
    
    /**
     * @brief De-Init
     * @param client
     */
    void gatt_service_client_deinit(void);
    
    

## Device Database API {#sec:le_device_db_api}


**le_device_db.h** 


    
    /**
     * @brief init
     */
    void le_device_db_init(void);
    
    
    /**
     * @brief sets local bd addr. allows for db per Bluetooth controller
     * @param bd_addr
     */
    void le_device_db_set_local_bd_addr(bd_addr_t bd_addr);
    
    /**
     * @brief add device to db
     * @param addr_type, address of the device
     * @param irk of the device
     * @return index if successful, -1 otherwise
     */
    int le_device_db_add(int addr_type, bd_addr_t addr, sm_key_t irk);
    
    /**
     * @brief get number of devices in db
     * @return number of device in db
     */
    int le_device_db_count(void);
    
    /**
     * @brief get max number of devices in db for enumeration
     * @return max number of device in db
     */
    int le_device_db_max_count(void);
    
    /**
     * @brief get device information: addr type and address needed to identify device
     * @param index
     * @param addr_type, address of the device as output
     * @param irk of the device
     */
    void le_device_db_info(int index, int * addr_type, bd_addr_t addr, sm_key_t irk);
    
    
    /**
     * @brief set remote encryption info
     * @brief index
     * @brief ediv 
     * @brief rand
     * @brief ltk
     * @brief key size
     * @brief authenticated
     * @brief authorized
     * @breif secure_connection
     */
    void le_device_db_encryption_set(int index, uint16_t ediv, uint8_t rand[8], sm_key_t ltk, int key_size, int authenticated, int authorized, int secure_connection);
    
    /**
     * @brief get remote encryption info
     * @brief index
     * @brief ediv 
     * @brief rand
     * @brief ltk
     * @brief key size
     * @brief authenticated
     * @brief authorized
     * @breif secure_connection
     */
    void le_device_db_encryption_get(int index, uint16_t * ediv, uint8_t rand[8], sm_key_t ltk,  int * key_size, int * authenticated, int * authorized, int * secure_connection);
    
    #ifdef ENABLE_LE_SIGNED_WRITE
    
    /**
     * @brief set local signing key for this device
     * @param index
     * @param signing key as input
     */
    void le_device_db_local_csrk_set(int index, sm_key_t csrk);
    
    /**
     * @brief get local signing key for this device
     * @param index
     * @param signing key as output
     */
    void le_device_db_local_csrk_get(int index, sm_key_t csrk);
    
    /**
     * @brief set remote signing key for this device
     * @param index
     * @param signing key as input
     */
    void le_device_db_remote_csrk_set(int index, sm_key_t csrk);
    
    /**
     * @brief get remote signing key for this device
     * @param index
     * @param signing key as output
     */
    void le_device_db_remote_csrk_get(int index, sm_key_t csrk);
    
    /**
     * @brief query last used/seen signing counter
     * @param index
     * @return next expected counter, 0 after devices was added
     */
    uint32_t le_device_db_remote_counter_get(int index);
    
    /**
     * @brief update signing counter
     * @param index
     * @param counter to store
     */
    void le_device_db_remote_counter_set(int index, uint32_t counter);
    
    /**
     * @brief query last used/seen signing counter
     * @param index
     * @return next expected counter, 0 after devices was added
     */
    uint32_t le_device_db_local_counter_get(int index);
    
    /**
     * @brief update signing counter
     * @param index
     * @param counter to store
     */
    void le_device_db_local_counter_set(int index, uint32_t counter);
    
    #endif
    
    /**
     * @brief free device
     * @param index
     */
    void le_device_db_remove(int index);
    
    void le_device_db_dump(void);
    

## Device Database TLV API {#sec:le_device_db_tlv_api}


**le_device_db_tlv.h** : 
@TLV 'BTD' x


    
    /**
     * @brief configure le device db for use with btstack tlv instance
     * @param btstack_tlv_impl to use
     * @param btstack_tlv_context
     */
    
    void le_device_db_tlv_configure(const btstack_tlv_t * btstack_tlv_impl, void * btstack_tlv_context);
    

## Security Manager API {#sec:sm_api}


**sm.h** : 
@TLV 'SMER'
@TLV 'SMIR'


    
    /**
     * @brief Initializes the Security Manager, connects to L2CAP
     */
    void sm_init(void);
    
    /**
     * @brief Set secret ER key for key generation as described in Core V4.0, Vol 3, Part G, 5.2.2
     * @note If not set and btstack_tlv is configured, ER key is generated and stored in TLV by SM
     * @param er key
     */
    void sm_set_er(sm_key_t er);
    
    /**
     * @brief Set secret IR key for key generation as described in Core V4.0, Vol 3, Part G, 5.2.2
     * @note If not set and btstack_tlv is configured, IR key is generated and stored in TLV by SM
     * @param ir key
     */
    void sm_set_ir(sm_key_t ir);
    
    /**
     * @brief Registers OOB Data Callback. The callback should set the oob_data and return 1 if OOB data is availble
     * @param get_oob_data_callback
     */
    void sm_register_oob_data_callback( int (*get_oob_data_callback)(uint8_t address_type, bd_addr_t addr, uint8_t * oob_data));
    
    /**
     * @brief Add event packet handler. 
     * @param callback_handler
     */
    void sm_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Remove event packet handler.
     * @param callback_handler
     */
    void sm_remove_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Limit the STK generation methods. Bonding is stopped if the resulting one isn't in the list
     * @param OR combination of SM_STK_GENERATION_METHOD_ 
     */
    void sm_set_accepted_stk_generation_methods(uint8_t accepted_stk_generation_methods);
    
    /**
     * @brief Set the accepted encryption key size range. Bonding is stopped if the result isn't within the range
     * @param min_size (default 7)
     * @param max_size (default 16)
     */
    void sm_set_encryption_key_size_range(uint8_t min_size, uint8_t max_size);
    
    /**
     * @brief Sets the requested authentication requirements, bonding yes/no, MITM yes/no, SC yes/no, keypress yes/no
     * @param OR combination of SM_AUTHREQ_ flags
     */
    void sm_set_authentication_requirements(uint8_t auth_req);
    
    /**
     * @brief Sets the available IO Capabilities
     * @param IO_CAPABILITY_
     */
    void sm_set_io_capabilities(io_capability_t io_capability);
    
    /**
     * @brief Enable/disable Secure Connections Mode only
     * @param enable secure connections only mode
     */
    void sm_set_secure_connections_only_mode(bool enable);
    
    /**
     * @brief Let Peripheral request an encrypted connection right after connecting
     * @param enable
     * @note Not used normally. Bonding is triggered by access to protected attributes in ATT Server
     */
    void sm_set_request_security(bool enable);
    
    /** 
     * @brief Trigger Security Request
     * @deprecated please use sm_request_pairing instead
     */
    void sm_send_security_request(hci_con_handle_t con_handle);
    
    /**
     * @brief Decline bonding triggered by event before
     * @param con_handle
     */
    void sm_bonding_decline(hci_con_handle_t con_handle);
    
    /**
     * @brief Confirm Just Works bonding 
     * @param con_handle
     */
    void sm_just_works_confirm(hci_con_handle_t con_handle);
    
    /**
     * @brief Confirm value from SM_EVENT_NUMERIC_COMPARISON_REQUEST for Numeric Comparison bonding 
     * @param con_handle
     */
    void sm_numeric_comparison_confirm(hci_con_handle_t con_handle);
    
    /**
     * @brief Reports passkey input by user
     * @param con_handle
     * @param passkey in [0..999999]
     */
    void sm_passkey_input(hci_con_handle_t con_handle, uint32_t passkey);
    
    /**
     * @brief Send keypress notification for keyboard only devices
     * @param con_handle
     * @param action see SM_KEYPRESS_* in bluetooth.h
     */
    void sm_keypress_notification(hci_con_handle_t con_handle, uint8_t action);
    
    /**
     * @brief Used by att_server.c and gatt_client.c to request user authentication
     * @param con_handle
     */
    void sm_request_pairing(hci_con_handle_t con_handle);
    
    /**
     * @brief Report user authorization decline.
     * @param con_handle
     */
    void sm_authorization_decline(hci_con_handle_t con_handle);
    
    /**
     * @brief Report user authorization grant.
     * @param con_handle
     */
    void sm_authorization_grant(hci_con_handle_t con_handle);
    
    /**
     * @brief Support for signed writes, used by att_server.
     * @return ready
     */
    int sm_cmac_ready(void);
    
    /**
     * @brief Support for signed writes, used by att_server.
     * @note Message is in little endian to allows passing in ATT PDU without flipping. 
     * @note signing data: [opcode, attribute_handle, message, sign_counter]
     * @note calculated hash in done_callback is big endian and has 16 byte. 
     * @param key
     * @param opcde
     * @param attribute_handle
     * @param message_len
     * @param message
     * @param sign_counter
     */
    void sm_cmac_signed_write_start(const sm_key_t key, uint8_t opcode, uint16_t attribute_handle, uint16_t message_len, const uint8_t * message, uint32_t sign_counter, void (*done_callback)(uint8_t * hash));
    
    /**
     * @brief Match address against bonded devices
     * @param address_type
     * @param address
     * @return 0 if successfully added to lookup queue
     * @note Triggers SM_IDENTITY_RESOLVING_* events
     */
    int sm_address_resolution_lookup(uint8_t address_type, bd_addr_t address);
    
    /**
     * @brief Get Identity Resolving state
     * @param con_handle
     * @return irk_lookup_state_t
     * @note return IRK_LOOKUP_IDLE if connection does not exist
     */
    irk_lookup_state_t sm_identity_resolving_state(hci_con_handle_t con_handle);
    
    /**
     * @brief Identify device in LE Device DB.
     * @param con_handle
     * @return index from le_device_db or -1 if not found/identified
     */
    int sm_le_device_index(hci_con_handle_t con_handle);
    
    /**
     * @brief Get LTK for encrypted connection
     * @param con_handle
     * @param ltk buffer to store long term key
     * @return ERROR_CODE_SUCCESS ok
     *         ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if no connection for this con handle exists
     *         ERROR_CODE_PIN_OR_KEY_MISSING if connection is not encrypted
     */
    uint8_t sm_get_ltk(hci_con_handle_t con_handle, sm_key_t ltk);
    
    /**
     * @brief Use fixec passkey for Legacy and SC instead of generating a random number
     * @note Can be used to improve security over Just Works if no keyboard or displary are present and 
     *       individual random passkey can be printed on the device during production
     * @param passkey
     */
    void sm_use_fixed_passkey_in_display_role(uint32_t passkey);
    
    /**
     * @brief Allow connection re-encryption in Peripheral (Responder) role for LE Legacy Pairing 
     *       without entry for Central device stored in LE Device DB
     * @note BTstack in Peripheral Role (Responder) supports LE Legacy Pairing without a persistent LE Device DB as
     *       the LTK is reconstructed from a local secret IRK and EDIV + Random stored on Central (Initiator) device
     *       On the downside, it's not really possible to delete a pairing if this is enabled.
     * @param allow encryption using reconstructed LTK without stored entry (Default: 1)
     */
    void sm_allow_ltk_reconstruction_without_le_device_db_entry(int allow);
    
    /**
     * @brief Generate OOB data for LE Secure Connections
     * @note This generates a 128 bit random number ra and then calculates Ca = f4(PKa, PKa, ra, 0)
     *       New OOB data should be generated for each pairing. Ra is used for subsequent OOB pairings
     * @param callback
     * @return status
     */
    uint8_t sm_generate_sc_oob_data(void (*callback)(const uint8_t * confirm_value, const uint8_t * random_value));
    
    /**
     * @brief Registers OOB Data Callback for LE Secure Conections. The callback should set all arguments and return 1 if OOB data is availble
     * @note the oob_sc_local_random usually is the random_value returend by sm_generate_sc_oob_data
     * @param get_oob_data_callback
     */
    void sm_register_sc_oob_data_callback( int (*get_sc_oob_data_callback)(uint8_t address_type, bd_addr_t addr, uint8_t * oob_sc_peer_confirm, uint8_t * oob_sc_peer_random));
    
    /**
     * @bbrief Register LTK Callback that allows to provide a custom LTK on re-encryption. The callback returns true if LTK was modified
     * @param get_ltk_callback
     */
    void sm_register_ltk_callback( bool (*get_ltk_callback)(hci_con_handle_t con_handle, uint8_t address_type, bd_addr_t addr, uint8_t * ltk));
    

## Audio Interface API {#sec:btstack_audio_api}


**btstack_audio.h** : 
Abstraction layer for 16-bit audio playback and recording within BTstack.

Most embedded implementations, e.g. the one for ESP32, use a single I2S interface which
requires that the sample rate is the same for sink and source roles


    
    typedef struct {
        btstack_time_us_t timestamp;
    } btstack_audio_context_t;
    
    typedef struct {
    
        /**
         * @brief Setup audio codec for specified samplerate and number of channels
         * @param Channels (1=mono, 2=stereo)
         * @param Sample rate
         * @param Playback callback with sample buffer, number of samples, and, if available playback time of first sample
         * @return 0 on success
         */
        int (*init)(uint8_t channels,
                    uint32_t samplerate, 
                    void (*playback) (int16_t *  buffer,
                                      uint16_t num_samples,
                                      const btstack_audio_context_t * timeinfo));
    
        /**
         * @brief Get the current playback sample rate, may differ from the
         *        specified sample rate
         */
        uint32_t (*get_samplerate)(void);
    
        /**
         * @brief Set volume
         * @param Volume 0..127
         */
        void (*set_volume)(uint8_t volume);
    
        /**
         * @brief Start stream
         */
        void (*start_stream)(void);
    
        /** 
         * @brief Stop stream
         */
        void (*stop_stream)(void);
    
        /**
         * @brief Close audio codec
         */
        void (*close)(void);
    
    } btstack_audio_sink_t;
    
    
    typedef struct {
    
        /**
         * @brief Setup audio codec for specified samplerate and number of channels
         * @param Channels (1=mono, 2=stereo)
         * @param Sample rate
         * @param Recording callback with sample buffer, number of samples, and, if available recording time of first sample
         * @return 0 on success
         */
        int (*init)(uint8_t channels,
                    uint32_t samplerate, 
                    void (*recording)(const int16_t * buffer,
                                      uint16_t num_samples,
                                      const btstack_audio_context_t * timeinfo));
    
        /**
         * @brief Get the current recording sample rate, may differ from the
         *        specified sameple rate
         */
        uint32_t (*get_samplerate)(void);
    
        /**
         * @brief Set Gain
         * @param Gain 0..127
         */
        void (*set_gain)(uint8_t gain);
    
        /**
         * @brief Start stream
         */
        void (*start_stream)(void);
    
        /** 
         * @brief Stop stream
         */
        void (*stop_stream)(void);
    
        /**
         * @brief Close audio codec
         */
        void (*close)(void);
    
    } btstack_audio_source_t;
    
    
    /**
     * @brief Get BTstack Audio Sink Instance
     * @return btstack_audio_sink implementation
     */
    const btstack_audio_sink_t * btstack_audio_sink_get_instance(void);
    
    /**
     * @brief Get BTstack Audio Source Instance
     * @return btstack_audio_source implementation
     */
    const btstack_audio_source_t * btstack_audio_source_get_instance(void);
    
    
    /**
     * @brief Get BTstack Audio Sink Instance
     * @param btstack_audio_sink implementation
     */
    void btstack_audio_sink_set_instance(const btstack_audio_sink_t * audio_sink_impl);
    
    /**
     * @brief Get BTstack Audio Source Instance
     * @param btstack_audio_source implementation
     */
    void btstack_audio_source_set_instance(const btstack_audio_source_t * audio_source_impl);
    
    
    // common implementations
    const btstack_audio_sink_t *   btstack_audio_portaudio_sink_get_instance(void);
    const btstack_audio_source_t * btstack_audio_portaudio_source_get_instance(void);
    
    const btstack_audio_sink_t *   btstack_audio_alsa_sink_get_instance(void);
    
    const btstack_audio_sink_t *   btstack_audio_embedded_sink_get_instance(void);
    const btstack_audio_source_t * btstack_audio_embedded_source_get_instance(void);
    
    const btstack_audio_sink_t *    btstack_audio_esp32_sink_get_instance(void);
    const btstack_audio_source_t *  btstack_audio_esp32_source_get_instance(void);
    
    // platform-specific extension
    void btstack_audio_portaudio_sink_set_device(const char * device_name);
    void btstack_audio_portaudio_source_set_device(const char * device_name);
    

## base64 Decoder API {#sec:btstack_base64_decoder_api}


**btstack_base64_decoder.h** 


    
    #define BTSTACK_BASE64_DECODER_MORE     -1
    #define BTSTACK_BASE64_DECODER_COMPLETE -2
    #define BTSTACK_BASE64_DECODER_INVALID  -3
    #define BTSTACK_BASE64_DECODER_FULL     -4
    
    /**
     * @brief Initialize base64 decoder
     * @param context
     */
    void btstack_base64_decoder_init(btstack_base64_decoder_t * context);
    
    /**
     * @brief Decode single byte
     * @brief context
     * @return value, or BTSTACK_BASE64_DECODER_MORE, BTSTACK_BASE64_DECODER_COMPLETE, BTSTACK_BASE64_DECODER_INVALID
     */
    int  btstack_base64_decoder_process_byte(btstack_base64_decoder_t * context, uint8_t c);
    
    /**
     * @brief Decode block
     * @brief input_data base64 encoded message
     * @brief input_size
     * @breif output_buffer
     * @brief output_max_size
     * @return output_size or BTSTACK_BASE64_DECODER_INVALID, BTSTACK_BASE64_DECODER_FULL
     */
    int btstack_base64_decoder_process_block(const uint8_t * input_data, uint32_t input_size, uint8_t * output_buffer, uint32_t output_max_size);
    

## base64 Encoder API {#sec:btstack_base64_encoder_api}


**btstack_base64_encoder.h** 



## Chipset Driver API {#sec:btstack_chipset_api}


**btstack_chipset.h** : 
The API implements custom chipset initialization and support of proprietary extensions
to set UART baud rate, Bluetooth Address, and similar.



    
    typedef struct {
        /**
         * chipset driver name
         */
        const char * name;
    
        /**
         * init driver
         * allows to reset init script index
         * @param transport_config
         */
        void (*init)(const void * transport_config);
    
        /**
         * support custom init sequences after RESET command
         * @param  hci_cmd_buffer to store generated command
         * @return result see btstack_chipset_result_t
         */
        btstack_chipset_result_t (*next_command)(uint8_t * hci_cmd_buffer); 
    
        /**
         * provide UART Baud Rate change command.
         * @param baudrate
         * @param hci_cmd_buffer to store generated command
         */
        void (*set_baudrate_command)(uint32_t baudrate, uint8_t *hci_cmd_buffer); 
        
        /** provide Set BD Addr command
         * @param baudrate
         * @param hci_cmd_buffer to store generated command
         */
        void (*set_bd_addr_command)(bd_addr_t addr, uint8_t *hci_cmd_buffer); 
    
    } btstack_chipset_t;
    

## Bluetooth Power Control API {#sec:btstack_control_api}


**btstack_control.h** : 
The Bluetooth Hardware Control API allows HCI to manage Bluetooth chipsets via direct hardware controls.



    
    typedef struct {
    	void (*init) (const void *config);
        int  (*on)   (void);  // <-- turn BT module on and configure
        int  (*off)  (void);  // <-- turn BT module off
        int  (*sleep)(void);  // <-- put BT module to sleep    - only to be called after ON
        int  (*wake) (void);  // <-- wake BT module from sleep - only to be called after SLEEP
        void (*register_for_power_notifications)(void (*cb)(POWER_NOTIFICATION_t event));
    } btstack_control_t;
    

## Debug Messages API {#sec:btstack_debug_api}


**btstack_debug.h** : 
Allow to funnel debug and error messages. 



    
    /** 
     * @brief Log Security Manager key via log_info
     * @param name
     * @param key to log
     */
    void log_info_key(const char * name, sm_key_t key);
    
    /**
     * @brief Hexdump via log_info
     * @param data
     * @param size
     */
    void log_info_hexdump(const void *data, int size);
    
    /**
     * @brief Hexdump via log_debug 
     * @param data
     * @param size
     */
    void log_debug_hexdump(const void *data, int size);
    

## EM9304 SPI API {#sec:btstack_em9304_spi_api}


**btstack_em9304_spi.h** : 
BTstack's Hardware Abstraction Layer for EM9304 connected via SPI with additional RDY Interrupt line.



    
    #include <stdint.h>
    typedef struct {
    
        /**
         * @brief Open SPI
         */
        int (*open)(void);
    
        /**
         * @brief Close SPI
         */
        int (*close)(void);
    
        /**
         * @brief Check if full duplex operation via transceive is supported
         * @return 1 if supported
         */
        int  (*get_fullduplex_support)(void);
    
        /**
         * @brief Set callback for RDY
         * @param callback or NULL to disable callback
         */
        void (*set_ready_callback)(void (*callback)(void));
    
        /**
         * @brief Set callback for transfer complete
         * @param callback
         */
        void (*set_transfer_done_callback)(void (*callback)(void));
    
        /**
         * @brief Set Chip Selet
         * @param enable
         */
        void (*set_chip_select)(int enable);
    
        /**
         * @brief Poll READY state
         */
        int (*get_ready)(void);
    
        /**
         * @brief Transmit and Receive bytes via SPI
         * @param tx_data buffer to transmit
         * @param rx_data buffer to receive into
         * @param len 
         */
        void (*transceive)(const uint8_t * tx_data, uint8_t * rx_data, uint16_t len);
    
        /**
         * @brief Transmit bytes via SPI
         * @param tx_data buffer to transmit
         * @param len 
         */
        void (*transmit)(const uint8_t * tx_data, uint16_t len);
    
        /**
         * @brief Receive bytes via SPI
         * @param rx_data buffer to receive into
         * @param len 
         */
        void (*receive)(uint8_t * rx_data, uint16_t len);
    
    } btstack_em9304_spi_t;
    
    /**
     * @brief Get EM9304 SPI instance
     */
    const btstack_em9304_spi_t * btstack_em9304_spi_embedded_instance(void);
    

## Finite State Machine (FSM) API {#sec:btstack_fsm_api}


**btstack_fsm.h** 


    
    /*
     * @brief Request the transition from the current state to the given new state
     * @param me the current state machine
     * @param target the new state to transit to
     * @result transition status
     */
    btstack_fsm_state_t btstack_fsm_transit(btstack_fsm_t * const me, btstack_fsm_state_handler_t target);
    
    /*
     * @brief Constructs a new state hierarchical machine machine, with storage for maximum hierarchy depth.
     * @param me the current state machine
     * @param initial the initial state
     */
    void btstack_fsm_constructor(btstack_fsm_t * const me, btstack_fsm_state_handler_t initial);
    
    /*
     * @brief Takes the initial transition of the state machine and sending it a BTSTACK_HSM_INIT_SIG
     * @param me the current state machine
     * @param e event
     */
    void btstack_fsm_init(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine, if a transition is requested, leave the old states and enter the new on.
     *        Handling entering/exiting all states on the way.
     * @param me the current state machine
     * @param e event
     */
    btstack_fsm_state_t btstack_fsm_dispatch(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine until it was handled.
     * @param me the current state machine
     * @param e event
     */
    void btstack_fsm_dispatch_until(btstack_fsm_t * const me, btstack_fsm_event_t const * const e);
    

## Human Interface Device (HID) API {#sec:btstack_hid_api}


**btstack_hid.h** 


    
    /*
     * @brief Get boot descriptor data
     * @result data
     */
    const uint8_t * btstack_hid_get_boot_descriptor_data(void);
    
    /*
     * @brief Get boot descriptor length
     * @result length
     */
    uint16_t btstack_hid_get_boot_descriptor_len(void);
    

## HID Parser API {#sec:btstack_hid_parser_api}


**btstack_hid_parser.h** : 
Single-pass HID Report Parser: HID Report is directly parsed without preprocessing HID Descriptor to minimize memory.



    
    // HID Descriptor Iterator
    
    /**
     * @brief Init HID Descriptor iterator
     * @param iterator
     * @param hid_descriptor
     * @param hid_descriptor_len
     */
    void btstack_hid_descriptor_iterator_init(btstack_hid_descriptor_iterator_t * iterator, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len);
    
    /**
     * @brief Check if HID Descriptor Items are available
     * @param iterator
     * @return
     */
    bool btstack_hid_descriptor_iterator_has_more(btstack_hid_descriptor_iterator_t * iterator);
    
    /**
     * @brief Get next HID Descriptor Item
     * @param iterator
     * @return
     */
    const hid_descriptor_item_t * btstack_hid_descriptor_iterator_get_item(btstack_hid_descriptor_iterator_t * iterator);
    
    /**
     * @brief Returns if HID Descriptor was well formed
     * @param iterator
     * @return
     */
    bool btstack_hid_descriptor_iterator_valid(btstack_hid_descriptor_iterator_t * iterator);
    
    
    // HID Descriptor Usage Iterator
    
    /**
     * @brief Initialize usages iterator for HID Descriptor and report type
     * @param parser
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @param hid_report_type
     */
    void btstack_hid_usage_iterator_init(btstack_hid_usage_iterator_t * iterator, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len, hid_report_type_t hid_report_type);
    
    /**
     * @brief Checks if more usages are available
     * @param parser
     */
    bool btstack_hid_usage_iterator_has_more(btstack_hid_usage_iterator_t * iterator);
    
    /**
     * @brief Get current usage item
     * @param parser
     * @param item
     */
    void btstack_hid_usage_iterator_get_item(btstack_hid_usage_iterator_t * iterator, btstack_hid_usage_item_t * item);
    
    
    // HID Report Iterator
    
    /**
     * @brief Initialize HID Parser.
     * @param parser state 
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @param hid_report_type
     * @param hid_report
     * @param hid_report_len
     */
    void btstack_hid_parser_init(btstack_hid_parser_t * parser, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len, hid_report_type_t hid_report_type, const uint8_t * hid_report, uint16_t hid_report_len);
    
    /**
     * @brief Checks if more fields are available
     * @param parser
     */
    bool btstack_hid_parser_has_more(btstack_hid_parser_t * parser);
    
    /**
     * @brief Get next field
     * @param parser
     * @param usage_page
     * @param usage
     * @param value provided in HID report
     */
    void btstack_hid_parser_get_field(btstack_hid_parser_t * parser, uint16_t * usage_page, uint16_t * usage, int32_t * value);
    
    
    /**
     * @brief Parses descriptor and returns report size for given report ID and report type
     * @param report_id or HID_REPORT_ID_UNDEFINED
     * @param report_type
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return report size in bytes or 0 on parsing error
     */
    int btstack_hid_get_report_size_for_id(uint16_t report_id, hid_report_type_t report_type, const uint8_t *hid_descriptor,
                                           uint16_t hid_descriptor_len);
    
    /**
     * @brief Parses descriptor and returns status for given report ID
     * @param report_id
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return status for report id
     */
    hid_report_id_status_t btstack_hid_report_id_valid(uint16_t report_id, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len);
    
    /**
     * @brief Parses descriptor and returns true if report ID found
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return true if report ID declared in descriptor
     */
    bool btstack_hid_report_id_declared(const uint8_t *hid_descriptor, uint16_t hid_descriptor_len);
    
    

## Hierarchical State Machine (HSM) API {#sec:btstack_hsm_api}


**btstack_hsm.h** 


    
    /*
     * @brief Request the transition from the current state to the given new state
     * @param me the current state machine
     * @param target the new state to transit to
     * @result transition status
     */
    btstack_hsm_state_t btstack_hsm_transit(btstack_hsm_t * const me, btstack_hsm_state_handler_t const target);
    
    /*
     * @brief Specifies the upper state in a state hierarchy
     * @param me the current state machine
     * @param target the next parent state in the hierarchy
     * @result transition status
     */
    btstack_hsm_state_t btstack_hsm_super(btstack_hsm_t * const me, btstack_hsm_state_handler_t const target);
    
    /*
     * @brief Placeholder state to mark the top most state in a state hierarchy, the root state. Ignores all events.
     * @param me the current state machine
     * @param e event
     * @result ignored status
     */
    btstack_hsm_state_t btstack_hsm_top(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    
    /*
     * @brief Constructs a new state hierarchical machine machine, with storage for maximum hierarchy depth.
     * @param me the current state machine
     * @param initial the initial state
     * @param array of btstack_hsm_state_handler_t elements with the same number of elements as the maximum number of nested state machines.
     * @param The number of nested state machines.
     */
    void btstack_hsm_constructor(btstack_hsm_t * const me, btstack_hsm_state_handler_t initial, btstack_hsm_state_handler_t path[], int8_t depth);
    
    /*
     * @brief Takes the initial transition of the state machine and sending it a BTSTACK_HSM_INIT_SIG
     * @param me the current state machine
     * @param e event
     */
    void btstack_hsm_init(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    
    /*
     * @brief Dispatches the given event to the state machine, if a transition is requested, leave the old states and enter the new on.
     *        Honoring the hierarchy and handling entering/exiting all states on the way.
     * @param me the current state machine
     * @param e event
     */
    btstack_hsm_state_t btstack_hsm_dispatch(btstack_hsm_t * const me, btstack_hsm_event_t const * const e);
    

## LC3 Interface API {#sec:btstack_lc3_api}


**btstack_lc3.h** : 
Interface for LC3 implementations



    
    typedef enum {
        BTSTACK_LC3_FRAME_DURATION_10000US,
        BTSTACK_LC3_FRAME_DURATION_7500US
    } btstack_lc3_frame_duration_t;
    
    typedef struct {
    
        /**
         * Configure Decoder
         * @param context
         * @param sample_rate
         * @param frame_duration
         * @param octets_per_frame
         * @return status
         */
        uint8_t (*configure)(void * context, uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration, uint16_t octets_per_frame);
    
        /**
         * Decode LC3 Frame into signed 16-bit samples
         * @param context
         * @param bytes
         * @param BFI Bad Frame Indication flags
         * @param pcm_out buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param BEC_detect Bit Error Detected flag
         * @return status
         */
         uint8_t (*decode_signed_16)(void * context, const uint8_t *bytes, uint8_t BFI,
                           int16_t* pcm_out, uint16_t stride, uint8_t * BEC_detect);
    
        /**
         * Decode LC3 Frame into signed 24-bit samples, sign-extended to 32-bit
         * @param context
         * @param bytes
         * @param BFI Bad Frame Indication flags
         * @param pcm_out buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param BEC_detect Bit Error Detected flag
         * @return status
         */
        uint8_t (*decode_signed_24)(void * context, const uint8_t *bytes, uint8_t BFI,
                                    int32_t* pcm_out, uint16_t stride, uint8_t * BEC_detect);
    
    } btstack_lc3_decoder_t;
    
    typedef struct {
        /**
         * Configure Decoder
         * @param context
         * @param sample_rate
         * @param frame_duration
         * @param octets_per_frame
         * @return status
         */
        uint8_t (*configure)(void * context, uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration, uint16_t octets_per_frame);
    
        /**
         * Encode LC3 Frame with 16-bit signed PCM samples
         * @param context
         * @param pcm_in buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param bytes
         * @return status
         */
        uint8_t (*encode_signed_16)(void * context, const int16_t* pcm_in, uint16_t stride, uint8_t *bytes);
    
        /**
         * Encode LC3 Frame with 24-bit signed PCM samples, sign-extended to 32 bit
         * @param context
         * @param pcm_in buffer for decoded PCM samples
         * @param stride count between two consecutive samples
         * @param bytes
         * @return status
         */
        uint8_t (*encode_signed_24)(void * context, const int32_t* pcm_in, uint16_t stride, uint8_t *bytes);
    
    } btstack_lc3_encoder_t;
    
    /**
     * @brief Map enum to ISO Interval in us
     * @param frame_duration enum
     * @return frame_duratoin in us or 0 for invalid frame_duration enum
     */
    uint16_t btstack_lc3_frame_duration_in_us(btstack_lc3_frame_duration_t frame_duration);
    
    /**
     * @bbrief Calculate number of samples per ISO Interval
     * @param sample_rate
     * @param frame_duration
     * @return
     */
    uint16_t btstack_lc3_samples_per_frame(uint32_t sample_rate, btstack_lc3_frame_duration_t frame_duration);
    

## LC3 Google Adapter API {#sec:btstack_lc3_google_api}


**btstack_lc3_google.h** 

    
    typedef struct {
        lc3_decoder_mem_48k_t           decoder_mem;
        lc3_decoder_t                   decoder;    // pointer
        uint32_t                        sample_rate;
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
    } btstack_lc3_decoder_google_t;
    
    typedef struct {
        lc3_encoder_mem_48k_t           encoder_mem;
        lc3_encoder_t                   encoder;    // pointer
        uint32_t                        sample_rate;
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
    } btstack_lc3_encoder_google_t;
    
    /**
     * Init LC3 Decoder Instance
     * @param context for EHIMA LC3 decoder
     */
    const btstack_lc3_decoder_t * btstack_lc3_decoder_google_init_instance(btstack_lc3_decoder_google_t * context);
    
    /**
     * Init LC3 Decoder Instance
     * @param context for EHIMA LC3 decoder
     */
    const btstack_lc3_encoder_t * btstack_lc3_encoder_google_init_instance(btstack_lc3_encoder_google_t * context);
    

## Adapter for Fraunhofer LC3plus Coddec API {#sec:btstack_lc3plus_fraunhofer_api}


**btstack_lc3plus_fraunhofer.h** : only uses suitable subset for lc3 testing


    
    typedef struct {
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
        uint16_t                        samples_per_frame;
        uint32_t                        sample_rate;
        // decoder must be 4-byte aligned
        uint8_t                         decoder[LC3PLUS_DEC_MAX_SIZE];
    } btstack_lc3plus_fraunhofer_decoder_t;
    
    typedef struct {
        btstack_lc3_frame_duration_t    frame_duration;
        uint16_t                        octets_per_frame;
        uint32_t                        sample_rate;
        // encoder must be 4-byte aligned
        uint8_t                         encoder[LC3PLUS_ENC_MAX_SIZE];
    } btstack_lc3plus_fraunhofer_encoder_t;
    
    /**
     * Init LC3 Decoder Instance
     * @param context for Fraunhofer LC3plus decoder
     */
    const btstack_lc3_decoder_t * btstack_lc3plus_fraunhofer_decoder_init_instance(btstack_lc3plus_fraunhofer_decoder_t * context);
    
    /**
     * Init LC3 Encoder Instance
     * @param context for Fraunhofer LC3plus encoder
     */
    const btstack_lc3_encoder_t * btstack_lc3plus_fraunhofer_encoder_init_instance(btstack_lc3plus_fraunhofer_encoder_t * context);
    

## Linked List API {#sec:btstack_linked_list_api}


**btstack_linked_list.h** 


    	
    typedef struct btstack_linked_item {
        struct btstack_linked_item *next; // <-- next element in list, or NULL
    } btstack_linked_item_t;
    
    typedef btstack_linked_item_t * btstack_linked_list_t;
    
    typedef struct {
    	int advance_on_next;
        btstack_linked_item_t * prev;	// points to the item before the current one
        btstack_linked_item_t * curr;	// points to the current item (to detect item removal)
    } btstack_linked_list_iterator_t;
    
    
    /**
     * @brief Test if list is empty.
     * @param list
     * @return true if list is empty
     */
    bool btstack_linked_list_empty(btstack_linked_list_t * list);
    
    /**
     * @brief Add item to list as first element.
     * @param list
     * @param item
     * @return true if item was added, false if item already in list
     */
    bool btstack_linked_list_add(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Add item to list as last element.
     * @param list
     * @param item
     * @return true if item was added, false if item already in list
     */
    bool btstack_linked_list_add_tail(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Pop (get + remove) first element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_pop(btstack_linked_list_t * list);
    
    /**
     * @brief Remove item from list
     * @param list
     * @param item
     * @return true if item was removed, false if it is no't in list
     */
    bool btstack_linked_list_remove(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Get first element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_get_first_item(btstack_linked_list_t * list);
    
    /**
     * @breif Get previous element
     * @param list
     * @param item
     * @return previous element or NULL if not found or element was first in liest
     */
    btstack_linked_item_t * btstack_linked_list_get_previous_item(btstack_linked_list_t * list, btstack_linked_item_t * item);
    
    /**
     * @brief Get last element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_get_last_item(btstack_linked_list_t * list);
    
    /**
     * @brief Counts number of items in list
     * @return number of items in list
     */
    int btstack_linked_list_count(btstack_linked_list_t * list);
    
    /**
     * @brief Split list into matching and not-matching items based on a criteria
     * @param input list of items
     * @param matches list of matching items
     * @param other all other items
     * @param predicate is called with linked list item and context, returns true if item matches
     * @param context
     */
    void btstack_linked_list_split(btstack_linked_list_t * input,
                                    btstack_linked_list_t * matches,
                                    btstack_linked_list_t * other,
                                    bool (*predicate)(const btstack_linked_item_t *item, void * context), void * context);
    
    /**
     * @brief Filter matching items into second list
     * @param input list of items
     * @param matches list of matching items
     * @param predicate is called with linked list item and context, returns true if item matches
     * @param context
     */
    void btstack_linked_list_filter(btstack_linked_list_t * input,
                                    btstack_linked_list_t * matches,
                                    bool (*predicate)(const btstack_linked_item_t *item, void * context), void * context);
    
    /**
     * @brief Initialize Linked List Iterator
     * @note robust against removal of current element by btstack_linked_list_remove
     * @param it iterator context
     * @param list
     */
    void btstack_linked_list_iterator_init(btstack_linked_list_iterator_t * it, btstack_linked_list_t * list);
    
    /**
     * @brief Has next element
     * @param it iterator context
     * @return true if next element is available
     */
    bool btstack_linked_list_iterator_has_next(btstack_linked_list_iterator_t * it);
    
    /**
     * @brief Get next list eleemnt
     * @param it iterator context
     * @return list element
     */
    btstack_linked_item_t * btstack_linked_list_iterator_next(btstack_linked_list_iterator_t * it);
    
    /**
     * @brief Remove current list element from list
     * @param it iterator context
     */
    void btstack_linked_list_iterator_remove(btstack_linked_list_iterator_t * it);
    

## Linked Queue API {#sec:btstack_linked_queue_api}


**btstack_linked_queue.h** 


    
    typedef struct {
        btstack_linked_item_t * head;
        btstack_linked_item_t * tail;
    } btstack_linked_queue_t;
    
    /**
     * @brief Tests if queue is empty
     * @return true if emtpy
     */
    bool btstack_linked_queue_empty(btstack_linked_queue_t * queue);
    
    /**
     * @brief Append item to queue
     * @param queue
     * @param item
     */
    void btstack_linked_queue_enqueue(btstack_linked_queue_t * queue, btstack_linked_item_t * item);
    
    /**
     * @brief Pop next item from queue
     * @param queue
     * @return item or NULL if queue empty
     */
    btstack_linked_item_t * btstack_linked_queue_dequeue(btstack_linked_queue_t * queue);
    
    /**
     * @brief Get first item from queue
     * @param queue
     * @return item or NULL if queue empty
     */
    btstack_linked_item_t * btstack_linked_queue_first(btstack_linked_queue_t * queue);
    

## Network Interface API {#sec:btstack_network_api}


**btstack_network.h** 


    
    /**
     * @brief Initialize network interface
     * @param send_packet_callback
     */
    void btstack_network_init(void (*send_packet_callback)(const uint8_t * packet, uint16_t size));
    
    /**
     * @brief Bring up network interfacd
     * @param network_address
     * @return 0 if ok
     */
    int  btstack_network_up(bd_addr_t network_address);
    
    /**
     * @brief Shut down network interfacd
     * @param network_address
     * @return 0 if ok
     */
    int  btstack_network_down(void);
    
    /** 
     * @brief Receive packet on network interface, e.g., forward packet to TCP/IP stack 
     * @param packet
     * @param size
     */
    void btstack_network_process_packet(const uint8_t * packet, uint16_t size);
    
    /** 
     * @brief Notify network interface that packet from send_packet_callback was sent and the next packet can be delivered.
     */
    void btstack_network_packet_sent(void);
    
    /**
     * @brief Get network name after network was activated
     * @note e.g. tapX on Linux, might not be useful on all platforms
     * @return network name
     */
    const char * btstack_network_get_name(void);
    

## Lienar Resampling API {#sec:btstack_resample_api}


**btstack_resample.h** : 
Linear resampling for 16-bit audio code samples using 16 bit/16 bit fixed point math.



    
    /**
     * @brief Init resample context
     * @param num_channels
     * @return btstack_audio implementation
     */
    void btstack_resample_init(btstack_resample_t * context, int num_channels);
    
    /**
     * @brief Set resampling factor
     * @param factor as fixed point value, identity is 0x10000
     */
    void btstack_resample_set_factor(btstack_resample_t * context, uint32_t factor);
    
    /**
     * @brief Process block of input samples
     * @note size of output buffer is not checked
     * @param input_buffer
     * @param num_frames
     * @param output_buffer
     * @return number destination frames
     */
    uint16_t btstack_resample_block(btstack_resample_t * context, const int16_t * input_buffer, uint32_t num_frames, int16_t * output_buffer);
    

## Ring Buffer API {#sec:btstack_ring_buffer_api}


**btstack_ring_buffer.h** 


    
    /**
     * Init ring buffer
     * @param ring_buffer object
     * @param storage
     * @param storage_size in bytes
     */
    void btstack_ring_buffer_init(btstack_ring_buffer_t * ring_buffer, uint8_t * storage, uint32_t storage_size);
    
    /**
     * Reset ring buffer to initial state (empty)
     * @param ring_buffer object
     */
    void btstack_ring_buffer_reset(btstack_ring_buffer_t * ring_buffer);
    
    /**
     * Check if ring buffer is empty
     * @param ring_buffer object
     * @return TRUE if empty
     */
    int btstack_ring_buffer_empty(btstack_ring_buffer_t * ring_buffer);
    
    /**
     * Get number of bytes available for read
     * @param ring_buffer object
     * @return number of bytes available for read
     */
    uint32_t btstack_ring_buffer_bytes_available(btstack_ring_buffer_t * ring_buffer);
    
    /**
     * Get free space available for write
     * @param ring_buffer object
     * @return number of bytes available for write
     */
    uint32_t btstack_ring_buffer_bytes_free(btstack_ring_buffer_t * ring_buffer);
    
    /**
     * Write bytes into ring buffer
     * @param ring_buffer object
     * @param data to store
     * @param data_length
     * @return 0 if ok, ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if not enough space in buffer
     */
    int btstack_ring_buffer_write(btstack_ring_buffer_t * ring_buffer, uint8_t * data, uint32_t data_length);
    
    /**
     * Read from ring buffer
     * @param ring_buffer object
     * @param buffer to store read data
     * @param length to read
     * @param number_of_bytes_read
     */
    void btstack_ring_buffer_read(btstack_ring_buffer_t * ring_buffer, uint8_t * buffer, uint32_t length, uint32_t * number_of_bytes_read); 
    

## Sample rate compensation API {#sec:btstack_sample_rate_compensation_api}


**btstack_sample_rate_compensation.h** : 
Prevents buffer over/under-run at the audio receiver by compensating for varying/different playback/receiving sample rates.

Intended to measure the L2CAP packet sample rate and with the provided playback sample rate calculates a compensation ratio
which compensates for drift between playback and reception.

Requires the audio interface to provide the current playback sample rate.



    
    #define FLOAT_TO_Q15(a)   ((signed)((a)*(UINT16_C(1)<<15)+0.5f))
    #define FLOAT_TO_Q8(a)    ((signed)((a)*(UINT16_C(1)<<8)+0.5f))
    #define FLOAT_TO_Q7(a)    ((signed)((a)*(UINT16_C(1)<<7)+0.5f))
    
    #define Q16_TO_FLOAT(a)   ((float)(a)/(UINT32_C(1)<<16))
    #define Q15_TO_FLOAT(a)   ((float)(a)/(UINT32_C(1)<<15))
    #define Q8_TO_FLOAT(a)    ((float)(a)/(UINT32_C(1)<<8))
    #define Q7_TO_FLOAT(a)    ((float)(a)/(UINT32_C(1)<<7))
    
    //#define DEBUG_RATIO_CALCULATION
    
    typedef struct {
        uint32_t count;         // 17bit are usable to count samples, recommended for max 96kHz
        uint32_t last;          // time stamp of last measurement
        uint32_t rate_state;    // unsigned Q17.8
        uint32_t ratio_state;   // unsigned Q16.16
        uint32_t constant_playback_sample_rate; // playback sample rate if no real one is available
    #ifdef DEBUG_RATIO_CALCULATION
        double sample_rate;
        double ratio;
    #endif
    } btstack_sample_rate_compensation_t;
    
    /**
     * @brief Initialize sample rate compensation
     * @param self pointer to current instance
     * @param time stamp at which to start sample rate measurement
     */
    void btstack_sample_rate_compensation_init( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms, uint32_t sample_rate, uint32_t ratioQ15 );
    
    /**
     * @brief reset sample rate compensation
     * @param self pointer to current instance
     * @param time stamp at which to start sample rate measurement
     */
    void btstack_sample_rate_compensation_reset( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms );
    
    /**
     * @brief update sample rate compensation with the current playback sample rate decoded samples
     * @param self pointer to current instance
     * @param time stamp for current samples
     * @param samples for current time stamp
     * @param playback sample rate
     */
    uint32_t btstack_sample_rate_compensation_update( btstack_sample_rate_compensation_t *self, uint32_t timestamp_ms, uint32_t samples, uint32_t playback_sample_rate );
    

## SCO Transport API {#sec:btstack_sco_transport_api}


**btstack_sco_transport.h** : 
Hardware abstraction for PCM/I2S Inteface used for HSP/HFP profiles.



    
    typedef struct {
    
        /**
         * register packet handler for SCO HCI packets
         */
        void (*register_packet_handler)(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
    
        /**
         * open transport
         * @param con_handle of SCO connection
         * @param sco_format
         */
        void (*open)(hci_con_handle_t con_handle, sco_format_t sco_format);
    
        /**
         * send SCO packet
         */
        void (*send_packet)(const uint8_t *buffer, uint16_t length);
    
        /**
         * close transport
         * @param con_handle of SCO connection
         */
        void (*close)(hci_con_handle_t con_handle);
    
    } btstack_sco_transport_t;
    

## SLIP encoder/decoder API {#sec:btstack_slip_api}


**btstack_slip.h** 


    
    // ENCODER
    
    /**
     * @brief Initialise SLIP encoder with data
     * @param data
     * @param len
     */
    void btstack_slip_encoder_start(const uint8_t * data, uint16_t len);
    
    /**
     * @brief Check if encoder has data ready
     * @return True if data ready
     */
    int  btstack_slip_encoder_has_data(void);
    
    /** 
     * @brief Get next byte from encoder 
     * @return Next bytes from encoder
     */
    uint8_t btstack_slip_encoder_get_byte(void);
    
    // DECODER
    
    /**
     * @brief Initialise SLIP decoder with buffer
     * @param buffer to store received data
     * @param max_size of buffer
     */
    void btstack_slip_decoder_init(uint8_t * buffer, uint16_t max_size);
    
    /**
     * @brief Process received byte
     * @param input
     */
    
    void btstack_slip_decoder_process(uint8_t input);
    
    /**
     * @brief Get size of decoded frame
     * @return size of frame. Size = 0 => frame not complete
     */
    
    uint16_t btstack_slip_decoder_frame_size(void);
    

## Tag-Value-Length Persistent Storage (TLV) API {#sec:btstack_tlv_api}


**btstack_tlv.h** : 
Inteface for BTstack's Tag Value Length Persistent Storage implementations
used to store pairing/bonding data.



    
    typedef struct {
    
    	/**
    	 * Get Value for Tag
    	 * @param context
    	 * @param tag
    	 * @param buffer
    	 * @param buffer_size
    	 * @return size of value
    	 */
    	int (*get_tag)(void * context, uint32_t tag, uint8_t * buffer, uint32_t buffer_size);
    
    	/**
    	 * Store Tag 
    	 * @param context
    	 * @param tag
    	 * @param data
    	 * @param data_size
    	 * @return 0 on success
    	 */
    	int (*store_tag)(void * context, uint32_t tag, const uint8_t * data, uint32_t data_size);
    
    	/**
    	 * Delete Tag
         *  @note it is not expected that delete operation fails, please use at least log_error in case of errors
    	 * @param context
    	 * @param tag
    	 */
    	void (*delete_tag)(void * context,  uint32_t tag);
    
    } btstack_tlv_t;
    
    /** 
     * @brief Make TLV implementation available to BTstack components via Singleton
     * @note Usually called by port after BD_ADDR was retrieved from Bluetooth Controller
     * @param tlv_impl
     * @param tlv_context
     */
    void btstack_tlv_set_instance(const btstack_tlv_t * tlv_impl, void * tlv_context);
    
    /**
     * @brief Get current TLV implementation. Used for bonding information, but can be used by application, too.
     * @param tlv_impl
     * @param tlv_context
     */
    void btstack_tlv_get_instance(const btstack_tlv_t ** tlv_impl, void ** tlv_context);
    

## Empty TLV Instance API {#sec:btstack_tlv_none_api}


**btstack_tlv_none.h** : 
Empty implementation for BTstack's Tag Value Length Persistent Storage implementations
No keys are stored. Can be used as placeholder during porting to new platform. 


    
    /**
     * Init Tag Length Value Store
     * @param context btstack_tlv_none_t 
     * @param hal_flash_bank_impl    of hal_flash_bank interface
     * @Param hal_flash_bank_context of hal_flash_bank_interface
     */
    const btstack_tlv_t * btstack_tlv_none_init_instance(void);
    

## UART API {#sec:btstack_uart_api}


**btstack_uart.h** : 
Common types for UART transports



    
    typedef struct {
        /**
         * init transport
         * @param uart_config
         */
        int (*init)(const btstack_uart_config_t * uart_config);
    
        /**
         * open transport connection
         */
        int (*open)(void);
    
        /**
         * close transport connection
         */
        int (*close)(void);
    
        /**
         * set callback for block received. NULL disables callback
         */
        void (*set_block_received)(void (*block_handler)(void));
    
        /**
         * set callback for sent. NULL disables callback
         */
        void (*set_block_sent)(void (*block_handler)(void));
    
        /**
         * set baudrate
         */
        int (*set_baudrate)(uint32_t baudrate);
    
        /**
         * set parity
         */
        int  (*set_parity)(int parity);
    
        /**
         * set flowcontrol
         */
        int  (*set_flowcontrol)(int flowcontrol);
    
        /**
         * receive block
         */
        void (*receive_block)(uint8_t *buffer, uint16_t len);
    
        /**
         * send block
         */
        void (*send_block)(const uint8_t *buffer, uint16_t length);
    
    
        /** Support for different Sleep Modes in TI's H4 eHCILL and in H5 - can be set to NULL if not used */
    
        /**
         * query supported wakeup mechanisms
         * @return supported_sleep_modes mask
         */
         int (*get_supported_sleep_modes)(void);
    
        /**
         * set UART sleep mode - allows to turn off UART and it's clocks to save energy
         * Supported sleep modes:
         * - off: UART active, RTS low if receive_block was called and block not read yet
         * - RTS high, wake on CTS: RTS should be high. On CTS pulse, UART gets enabled again and RTS goes to low
         * - RTS low, wake on RX: data on RX will trigger UART enable, bytes might get lost
         */
        void (*set_sleep)(btstack_uart_sleep_mode_t sleep_mode);
    
        /** 
         * set wakeup handler - needed to notify hci transport of wakeup requests by Bluetooth controller
         * Called upon CTS pulse or RX data. See sleep modes.
         */
        void (*set_wakeup_handler)(void (*wakeup_handler)(void));
    
    
        /** Support for HCI H5 Transport Mode - can be set to NULL for H4 */
    
        /**
         * H5/SLIP only: set callback for frame received. NULL disables callback
         */
        void (*set_frame_received)(void (*frame_handler)(uint16_t frame_size));
    
        /**
         * H5/SLIP only: set callback for frame sent. NULL disables callback
         */
        void (*set_frame_sent)(void (*block_handler)(void));
    
        /**
         * H5/SLIP only: receive SLIP frame
         */
        void (*receive_frame)(uint8_t *buffer, uint16_t len);
    
        /**
         * H5/SLIP only: send SLIP frame
         */
        void (*send_frame)(const uint8_t *buffer, uint16_t length);
    
    } btstack_uart_t;
    

## UART Block API {#sec:btstack_uart_block_api}


**btstack_uart_block.h** : 
Compatibility layer for ports that use btstack_uart_block_t 



    
    typedef btstack_uart_t btstack_uart_block_t;
    
    // existing block-only implementations
    const btstack_uart_block_t * btstack_uart_block_windows_instance(void);
    const btstack_uart_block_t * btstack_uart_block_embedded_instance(void);
    const btstack_uart_block_t * btstack_uart_block_freertos_instance(void);
    
    // mapper for extended implementation
    static inline const btstack_uart_block_t * btstack_uart_block_posix_instance(void){
        return (const btstack_uart_block_t *) btstack_uart_posix_instance();
    }
    /* API_STOP */
    
    #if defined __cplusplus
    }
    #endif
    
    #endif

## UART SLIP Wrapper API {#sec:btstack_uart_slip_wrapper_api}


**btstack_uart_slip_wrapper.h** : 
Compatibility layer to use new H5 implementation with btstack_uart.h implementations without SLIP support.
Using this compatibility layer caused increased processing as it uses single byte UART reads.

If you're using H5, please consider implement the H5/SLIP functions in your btstack_uart.h or hal_uart_dma.h implementation.



    
    /**
     * @brief Initialize SLIP wrapper for existing btstack_uart_block_t instance without SLIP support
     * @param uart_block_without_slip
     * @return btstack_uart_t instance with SLIP support for use with hci_trasnport_h5
     */
    const btstack_uart_t * btstack_uart_slip_wrapper_instance(const btstack_uart_t * uart_without_slip);
    

## General Utility Functions API {#sec:btstack_util_api}


**btstack_util.h** 


    
    /**
     * @brief Minimum function for uint32_t
     * @param a
     * @param b
     * @return value
     */
    uint32_t btstack_min(uint32_t a, uint32_t b);
    
    /**
     * @brief Maximum function for uint32_t
     * @param a
     * @param b
     * @return value
     */
    uint32_t btstack_max(uint32_t a, uint32_t b);
    
    /**
     * @brief Calculate delta between two uint32_t points in time
     * @return time_a - time_b - result > 0 if time_a is newer than time_b
     */
    int32_t btstack_time_delta(uint32_t time_a, uint32_t time_b);
    
    /**
     * @brief Calculate delta between two uint16_t points in time
     * @return time_a - time_b - result > 0 if time_a is newer than time_b
     */
    int16_t btstack_time16_delta(uint16_t time_a, uint16_t time_b);
    
    /** 
     * @brief Read 08/16/24/32 bit little endian value from buffer
     * @param buffer
     * @param position in buffer
     * @return value
     */
    uint8_t  little_endian_read_08(const uint8_t * buffer, int position);
    uint16_t little_endian_read_16(const uint8_t * buffer, int position);
    uint32_t little_endian_read_24(const uint8_t * buffer, int position);
    uint32_t little_endian_read_32(const uint8_t * buffer, int position);
    
    
    /** 
     * @brief Write 08/16/32 bit little endian value into buffer
     * @param buffer
     * @param position in buffer
     * @param value
     */
    void little_endian_store_08(uint8_t * buffer, uint16_t position, uint8_t value);
    void little_endian_store_16(uint8_t * buffer, uint16_t position, uint16_t value);
    void little_endian_store_24(uint8_t * buffer, uint16_t position, uint32_t value);
    void little_endian_store_32(uint8_t * buffer, uint16_t position, uint32_t value);
    
    /** 
     * @brief Read 08/16/24/32 bit big endian value from buffer
     * @param buffer
     * @param position in buffer
     * @return value
     */
    uint32_t big_endian_read_08(const uint8_t* buffer, int position);
    uint32_t big_endian_read_16(const uint8_t * buffer, int position);
    uint32_t big_endian_read_24(const uint8_t * buffer, int position);
    uint32_t big_endian_read_32(const uint8_t * buffer, int position);
    
    /** 
     * @brief Write 08/16/32 bit big endian value into buffer
     * @param buffer
     * @param position in buffer
     * @param value
     */
    void big_endian_store_08(uint8_t * buffer, uint16_t position, uint8_t value);
    void big_endian_store_16(uint8_t * buffer, uint16_t position, uint16_t value);
    void big_endian_store_24(uint8_t * buffer, uint16_t position, uint32_t value);
    void big_endian_store_32(uint8_t * buffer, uint16_t position, uint32_t value);
    
    
    /**
     * @brief Swap bytes in 16 bit integer
     */
    static inline uint16_t btstack_flip_16(uint16_t value){
        return (uint16_t)((value & 0xffu) << 8) | (value >> 8);
    }
    
    /** 
     * @brief Check for big endian system
     * @return 1 if on big endian
     */
    static inline int btstack_is_big_endian(void){
    	uint16_t test_value = 0x0100;
    	return (int) *(uint8_t*) &test_value;
    }
    
    /** 
     * @brief Check for little endian system
     * @return 1 if on little endian
     */
    static inline int btstack_is_little_endian(void){
    	uint16_t test_value = 0x0001;
    	return (int) *(uint8_t*) &test_value;
    }
    
    /**
     * @brief Copy from source to destination and reverse byte order
     * @param src
     * @param dest
     * @param len
     */
    void reverse_bytes(const uint8_t * src, uint8_t * dest, int len);
    
    /**
     * @brief Wrapper around reverse_bytes for common buffer sizes
     * @param src
     * @param dest
     */
    void reverse_24 (const uint8_t * src, uint8_t * dest);
    void reverse_48 (const uint8_t * src, uint8_t * dest);
    void reverse_56 (const uint8_t * src, uint8_t * dest);
    void reverse_64 (const uint8_t * src, uint8_t * dest);
    void reverse_128(const uint8_t * src, uint8_t * dest);
    void reverse_256(const uint8_t * src, uint8_t * dest);
    
    void reverse_bd_addr(const bd_addr_t src, bd_addr_t dest);
    
    /**
     * @brief Check if all bytes in buffer are zero
     * @param buffer
     * @param size
     * @return true if all bytes is buffer are zero
     */
    bool btstack_is_null(const uint8_t * buffer, uint16_t size);
    
    /**
     * @brief Check if all bytes in a bd_addr_t are zero
     * @param addr
     * @return true if all bytes in addr are zero
     */
    bool btstack_is_null_bd_addr( const bd_addr_t addr );
    
    /** 
     * @brief ASCII character for 4-bit nibble
     * @return character
     */
    char char_for_nibble(uint8_t nibble);
    
    /**
     * @brif 4-bit nibble from ASCII character
     * @return value
     */
    int nibble_for_char(char c);
    
    /**
     * @brief Compare two Bluetooth addresses
     * @param a
     * @param b
     * @return 0 if equal
     */
    int bd_addr_cmp(const bd_addr_t a, const bd_addr_t b);
    
    /**
     * @brief Copy Bluetooth address
     * @param dest
     * @param src
     */
    void bd_addr_copy(bd_addr_t dest, const bd_addr_t src);
    
    /**
     * @brief Use printf to write hexdump as single line of data
     */
    void printf_hexdump(const void * data, int size);
    
    /**
     * @brief Create human readable representation for UUID128
     * @note uses fixed global buffer
     * @return pointer to UUID128 string
     */
    char * uuid128_to_str(const uint8_t * uuid);
    
    /**
     * @brief Create human readable represenationt of Bluetooth address
     * @note uses fixed global buffer
     * @param delimiter
     * @return pointer to Bluetooth address string
     */
    char * bd_addr_to_str_with_delimiter(const bd_addr_t addr, char delimiter);
    
    /**
     * @brief Create human readable represenationt of Bluetooth address
     * @note uses fixed global buffer
     * @return pointer to Bluetooth address string
     */
    char * bd_addr_to_str(const bd_addr_t addr);
    
    /**
     * @brief Replace address placeholder '00:00:00:00:00:00' with Bluetooth address
     * @param buffer
     * @param size
     * @param address
     */
    void btstack_replace_bd_addr_placeholder(uint8_t * buffer, uint16_t size, const bd_addr_t address);
    
    /** 
     * @brief Parse Bluetooth address
     * @param address_string
     * @param buffer for parsed address
     * @return 1 if string was parsed successfully
     */
    int sscanf_bd_addr(const char * addr_string, bd_addr_t addr);
    
    /**
     * @brief Constructs UUID128 from 16 or 32 bit UUID using Bluetooth base UUID
     * @param uuid128 output buffer
     * @param short_uuid
     */
    void uuid_add_bluetooth_prefix(uint8_t * uuid128, uint32_t short_uuid);
    
    /**
     * @brief Checks if UUID128 has Bluetooth base UUID prefix
     * @param uui128 to test
     * @return true if it can be expressed as UUID32
     */
    bool uuid_has_bluetooth_prefix(const uint8_t * uuid128);
    
    /**
     * @brief Parse unsigned number 
     * @param str to parse
     * @return value
     */
    uint32_t btstack_atoi(const char * str);
    
    /**
     * @brief Return number of digits of a uint32 number
     * @param uint32_number
     * @return num_digits
     */
    int string_len_for_uint32(uint32_t i);
    
    /**
     * @brief Return number of set bits in a uint32 number
     * @param uint32_number
     * @return num_set_bits
     */
    int count_set_bits_uint32(uint32_t x);
    
    /**
     * @brief Check CRC8 using ETSI TS 101 369 V6.3.0.
     * @note Only used by RFCOMM
     * @param data
     * @param len
     * @param check_sum
     */
    uint8_t btstack_crc8_check(uint8_t * data, uint16_t len, uint8_t check_sum);
    
    /**
     * @brief Calculate CRC8 using ETSI TS 101 369 V6.3.0. 
     * @note Only used by RFCOMM
     * @param data
     * @param len
     */
    uint8_t btstack_crc8_calc(uint8_t * data, uint16_t len);
    
    
    /**
     * @brief Calculate the initial CRC32 value using ISO 3309 (HDLC), polynomial (normal) 0x04c11db7
     * @note Used by OTS Service. 
     * 
     * @return  The initial crc value.
     */
    uint32_t btstack_crc32_init(void);
    
    /**
     * @brief Update the CRC32 value with new data.
     *
     * @param crc      The current crc value.
     * @param data     Pointer to a buffer of \a data_len bytes.
     * @param data_len Number of bytes in the \a data buffer.
     * @return             The updated crc value.
     */
    uint32_t btstack_crc32_update(uint32_t crc, const uint8_t * data, uint32_t data_len);
    
    /**
     * @brief Calculate the final CRC32 value.
     *
     * @param crc  The current crc value.
     * @return     The final crc value.
     */
    uint32_t btstack_crc32_finalize(uint32_t crc);
    
    /**
     * @brief Get next cid
     * @param current_cid
     * @return next cid skiping 0
     */
    uint16_t btstack_next_cid_ignoring_zero(uint16_t current_cid);
    
    /**
     * @brief Copy string (up to dst_size-1 characters) from src into dst buffer with terminating '\0'
     * @note replaces strncpy + dst[dst_size-1] = '\0'
     * @param dst
     * @param dst_size
     * @param src
     * @retun bytes_copied including trailing 0
     */
    uint16_t btstack_strcpy(char * dst, uint16_t dst_size, const char * src);
    
    /**
     * @brief Append src string to string in dst buffer with terminating '\0'
     * @note max total string length will be dst_size-1 characters
     * @param dst
     * @param dst_size
     * @param src
     */
    void btstack_strcat(char * dst, uint16_t dst_size, const char * src);
    
    /**
     * @brief Calculated the number of characters that would get printed
     * @note same as calling snprintf without a buffer
     * @param format
     * @param argsq
     * @return number of characters, or negative value on error
     */
    int btstack_printf_strlen(const char * format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 1, 2)))
    #endif
    ;
    
    
    /**
     * @brief Format string into buffer with '\0' and assert it is large enough
     * @note same as calling snprintf and assert that the string was not truncated
     * @param buffer
     * @param size of buffer
     * @param format
     * @param argsq
     * @return number of characters
     */
    uint16_t btstack_snprintf_assert_complete(char * buffer, size_t size, const char * format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 3, 4)))
    #endif
    ;
    
    /**
     * @brief Format string into buffer, truncated if necessary. Output string is '\0' terminated
     * @note similar to calling snprintf but returns the length of the output string
     * @param buffer
     * @param size of buffer
     * @param format
     * @param argsq
     * @return number of characters
     */
    uint16_t btstack_snprintf_best_effort(char * buffer, size_t size, const char * format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 3, 4)))
    #endif
    ;
    
    /**
     * Returns the number of leading 0-bits in x, starting at the most significant bit position.
     * If x is 0, the result is undefined.
     * @note maps to __builtin_clz for gcc and clang
     * @param value
     * @return number of leading 0-bits
     */
    uint8_t btstack_clz(uint32_t value);
    
    /**
     * @brief Copy chunk of data into a virtual buffer backed by a physical buffer.
     * Used to provide chunk of data of larger buffer that is constructed on the fly, e.g. serializing data struct
     * 
     * For example, copy field2 to buffer, with buffer_offset = 11
     * 
     *           field1  field2  field3     field4        field5  filed6
     * struct:  -------|-------|----------|-------------|-------|--------------
     * buffer:             ------------------       
     * result:             ----|         
     * 
     * When also copying field3 and field4 to buffer, with buffer_offset = 11
     * 
     *           field1  field2  field3     field4        field5  filed6
     * struct:  -------|-------|----------|-------------|-------|--------------
     * buffer:             ------------------       
     * result:             ----|----------|--         
     *
     * @param field_data
     * @param field_len
     * @param field_offset  position of field in complete data block
     * @param buffer_data
     * @param buffer_len
     * @param buffer_offset position of buffer in complete data block
     * @return bytes_copied number of bytes actually stored in buffer
     */
    uint16_t btstack_virtual_memcpy(
        const uint8_t * field_data, uint16_t field_len, uint16_t field_offset, 
        uint8_t * buffer, uint16_t buffer_size, uint16_t buffer_offset);
    
    /**
     * Convert bytes to hex string
     * @param dst buffer for hex string, needs to be twice as large as src_size + 1
     * @param src_data
     * @param src_size
     */
    void btstack_bytes_to_hex(char * dst, const uint8_t * src_data, uint16_t src_size);
    
    /**
     * Convert hex string to bytes
     * @param dst
     * @param dst_size
     * @param src
     * @return true if conversion was successful
     */
    bool btstack_hex_to_bytes(uint8_t * dst, uint16_t dst_size, const char * src);
    

## General Utility Functions API {#sec:btstack_version_api}


**btstack_version.h** 



## A2DP Sink API {#sec:a2dp_sink_api}


**a2dp_sink.h** : 
Audio/Video Distribution Transport Protocol A2DP Sink is a device that accepts streamed media data.  



    	
    /**
     * @brief Create A2DP Sink service record. 
     * @param service
     * @param service_record_handle
     * @param supported_features 16-bit bitmap, see AVDTP_SINK_SF_* values in avdtp.h
     * @param service_name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param service_provider_name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void a2dp_sink_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);
    
    /**
     * @brief Initialize up A2DP Sink device.
     */
    void a2dp_sink_init(void);
    
    /**
     * @brief Create a stream endpoint of type SINK, and register media codec by specifying its capabilities and the default configuration.
     * @param media_type    			see avdtp_media_type_t values in avdtp.h (audio, video or multimedia)
     * @param media_codec_type 			see avdtp_media_codec_type_t values in avdtp.h 
     * @param codec_capabilities        media codec capabilities as defined in A2DP spec, section 4 - Audio Codec Interoperability Requirements.
     * @param codec_capabilities_len	media codec capabilities length
     * @param codec_configuration 		default media codec configuration
     * @param codec_configuration_len	media codec configuration length 
     *
     * @return local_stream_endpoint 	
     */
    avdtp_stream_endpoint_t *  a2dp_sink_create_stream_endpoint(avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type,
                                                                const uint8_t *codec_capabilities, uint16_t codec_capabilities_len,
                                                                uint8_t * codec_configuration, uint16_t codec_configuration_len);
    
    /**
     *  @brief Unregister stream endpoint and free it's memory
     *  @param stream_endpoint created by a2dp_sink_create_stream_endpoint
     */
    void a2dp_sink_finalize_stream_endpoint(avdtp_stream_endpoint_t * stream_endpoint);
    
    /**
     * @brief Register callback for the A2DP Sink client. It will receive following subevents of HCI_EVENT_A2DP_META HCI event type: 
     * - A2DP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION:		indicates from remote chosen SBC media codec configuration 
     * - A2DP_SUBEVENT_SIGNALING_MEDIA_CODEC_OTHER_CONFIGURATION:	indicates from remote chosen other then SBC media codec configuration 
     * - A2DP_SUBEVENT_STREAM_ESTABLISHED:							received when stream to a remote device is established
     * - A2DP_SUBEVENT_STREAM_STARTED:								received when stream is started
     * - A2DP_SUBEVENT_STREAM_SUSPENDED:							received when stream is paused
     * - A2DP_SUBEVENT_STREAM_STOPED:							    received when stream is aborted or stopped
     * - A2DP_SUBEVENT_STREAM_RELEASED:								received when stream is released
     * - A2DP_SUBEVENT_SIGNALING_CONNECTION_RELEASED: 				received when signaling channel is disconnected
     *
     * @param callback
     */
    void a2dp_sink_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Register media handler for the A2DP Sink client. 
     * @param callback
     * @param packet
     * @param size
     */
    void a2dp_sink_register_media_handler(void (*callback)(uint8_t local_seid, uint8_t *packet, uint16_t size));
    
    /**
     * @brief Establish stream.
     * @param remote
     * @param out_a2dp_cid 		Assigned A2DP channel identifier used for furhter A2DP commands.
     */
    uint8_t a2dp_sink_establish_stream(bd_addr_t remote, uint16_t * out_a2dp_cid);
    
    #ifdef ENABLE_AVDTP_ACCEPTOR_EXPLICIT_START_STREAM_CONFIRMATION
    /**
     * @brief Accept starting the stream on A2DP_SUBEVENT_START_STREAM_REQUESTED event.
     * @param a2dp_cid      A2DP channel identifier.   
     * @param local_seid        ID of a local stream endpoint.
     */
    uint8_t a2dp_sink_start_stream_accept(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Reject starting the stream on A2DP_SUBEVENT_START_STREAM_REQUESTED event.
     * @param a2dp_cid      A2DP channel identifier.
     * @param local_seid        ID of a local stream endpoint.
     */
    uint8_t a2dp_sink_start_stream_reject(uint16_t a2dp_cid, uint8_t local_seid);
    #endif
    
    /**
     * @brief Release stream and disconnect from remote.
     * @param a2dp_cid 			A2DP channel identifier.
     */
    void a2dp_sink_disconnect(uint16_t a2dp_cid);
    
    /**
     * @brief Select and configure SBC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     SBC Configuration
     * @return status
     */
    uint8_t a2dp_sink_set_config_sbc(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_sbc_t * configuration);
    
    /**
     * @brief Select and configure MPEG AUDIO endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AUDIO Configuration
     * @return status
     */
    uint8_t a2dp_sink_set_config_mpeg_audio(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_mpeg_audio_t * configuration);
    
    /**
     * @brief Select and configure MPEG AAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AAC Configuration
     * @return status
     */
    uint8_t a2dp_sink_set_config_mpeg_aac(uint16_t a2dp_cid,  uint8_t local_seid,  uint8_t remote_seid, const avdtp_configuration_mpeg_aac_t * configuration);
    
    /**
     * @brief Select and configure ATRAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     ATRAC Configuration
     * @return status
     */
    uint8_t a2dp_sink_set_config_atrac(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_atrac_t * configuration);
    
    /**
     * @brief Select and configure Non-A2DP endpoint. Bytes 0-3 of codec information contain Vendor ID, bytes 4-5 contain Vendor Specific Codec ID (little endian)
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param media_codec_information
     * @param media_codec_information_len
     * @return status
     */
    uint8_t a2dp_sink_set_config_other(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const uint8_t * media_codec_information, uint8_t media_codec_information_len);
    
    /**
     * @brief Register media configuration validator. Can reject insuitable configuration or report stream endpoint as currently busy
     * @note validator has to return AVDTP error codes like: AVDTP_ERROR_CODE_SEP_IN_USE or AVDTP_ERROR_CODE_UNSUPPORTED_CONFIGURATION
     *       the callback receives the media configuration in the same format as the existing A2dP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION
     *       and similar
     * @param callback
     */
    void a2dp_sink_register_media_config_validator(uint8_t (*callback)(const avdtp_stream_endpoint_t * stream_endpoint, const uint8_t * event, uint16_t size));
    
    /**
     * @brief De-Init A2DP Sink device.
     */
    void a2dp_sink_deinit(void);
    

## A2DP Source API {#sec:a2dp_source_api}


**a2dp_source.h** : 
Audio/Video Distribution Transport Protocol A2DP Source is a device that streames media data.



    
    /**
     * @brief Create A2DP Source service record. 
     * @param service
     * @param service_record_handle
     * @param supported_features 16-bit bitmap, see AVDTP_SOURCE_SF_* values in avdtp.h
     * @param service_name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param service_provider_name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void a2dp_source_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);
    
    /**
     * @brief Initialize up A2DP Source device.
     */
    void a2dp_source_init(void);
    
    /**
     * @brief Create a stream endpoint of type SOURCE, and register media codec by specifying its capabilities and the default configuration.
     * @param media_type    			See avdtp_media_type_t values in avdtp.h (audio, video or multimedia).
     * @param media_codec_type 			See avdtp_media_codec_type_t values in avdtp.h 
     * @param codec_capabilities        Media codec capabilities as defined in A2DP spec, section 4 - Audio Codec Interoperability Requirements.
     * @param codec_capabilities_len	Media codec capabilities length.
     * @param codec_configuration 		Default media codec configuration.
     * @param codec_configuration_len	Media codec configuration length. 
     *
     * @return local_stream_endpoint 				
     */
    avdtp_stream_endpoint_t * a2dp_source_create_stream_endpoint(avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type,
                                                                 const uint8_t *codec_capabilities, uint16_t codec_capabilities_len,
                                                                 uint8_t * codec_configuration, uint16_t codec_configuration_len);
    
    /**
     *  @brief Unregister stream endpoint and free it's memory
     *  @param stream_endpoint created by a2dp_source_create_stream_endpoint
     */
    void a2dp_source_finalize_stream_endpoint(avdtp_stream_endpoint_t * stream_endpoint);
    
    /**
     * @brief Register callback for the A2DP Source client. It will receive following subevents of HCI_EVENT_A2DP_META HCI event type: 
     * - A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW:			Indicates that the next media packet can be sent.
     *
     * - A2DP_SUBEVENT_SIGNALING_CONNECTION_ESTABLISHED             Received when signaling connection with a remote is established.
     * - A2DP_SUBEVENT_SIGNALING_CONNECTION_RELEASED                Received when signaling connection with a remote is released
     * - A2DP_SUBEVENT_STREAM_ESTABLISHED                           Received when stream to a remote device is established.
     * - A2DP_SUBEVENT_STREAM_STARTED                               Received when stream is started.
     * - A2DP_SUBEVENT_STREAM_SUSPENDED                             Received when stream is paused.
     * - A2DP_SUBEVENT_STREAM_STOPED                                received when stream is aborted or stopped.
     * - A2DP_SUBEVENT_STREAM_RELEASED                              Received when stream is released.
     * - A2DP_SUBEVENT_SIGNALING_DELAY_REPORTING_CAPABILITY         Currently the only capability that is passed to client
     * - A2DP_SUBEVENT_SIGNALING_CAPABILITIES_DONE                  Signals that all capabilities are reported
     * - A2DP_SUBEVENT_SIGNALING_DELAY_REPORT                       Delay report
     * - A2DP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION      SBC configuration
     * - A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW          Signals that a media packet can be sent
     * - A2DP_SUBEVENT_COMMAND_REJECTED                             Command reject
     * @param callback
     */
    void a2dp_source_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Open stream.
     * @param remote_addr
     * @param avdtp_cid 		Assigned A2DP channel identifier used for further A2DP commands.
     */
    uint8_t a2dp_source_establish_stream(bd_addr_t remote_addr, uint16_t *avdtp_cid);
    
        /**
         * @brief Reconfigure stream.
         * @param local_seid	 	  ID assigned to a local stream endpoint
         * @param sampling_frequency  New sampling frequency to use. Cannot be called while stream is active
         */
    uint8_t a2dp_source_reconfigure_stream_sampling_frequency(uint16_t a2dp_cid, uint32_t sampling_frequency);
    
    /**
     * @brief Start stream.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid	 	ID of a local stream endpoint.
     */
    uint8_t a2dp_source_start_stream(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Pause stream.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     */
    uint8_t a2dp_source_pause_stream(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Release stream and disconnect from remote. 
     * @param a2dp_cid 			A2DP channel identifier.
     */
    uint8_t a2dp_source_disconnect(uint16_t a2dp_cid);
    
    /**
     * @brief Request to send a media packet. Packet can be then sent on reception of A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW event.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     */
    void a2dp_source_stream_endpoint_request_can_send_now(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Return maximal media payload size, does not include media header.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @return max_media_payload_size_without_media_header
     */
    int a2dp_max_media_payload_size(uint16_t a2dp_cid, uint8_t local_seid);
    
    /**
     * @brief Send media payload.
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param marker
     * @param timestamp         in sample rate units
     * @param payload
     * @param payload_size
     * @param marker
     * @return status
     */
    uint8_t a2dp_source_stream_send_media_payload_rtp(uint16_t a2dp_cid, uint8_t local_seid, uint8_t marker, uint32_t timestamp,
                                              uint8_t *payload, uint16_t payload_size);
    
    /**
     * @brief Send media packet
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param packet
     * @param size
     * @return status
     */
    uint8_t a2dp_source_stream_send_media_packet(uint16_t a2dp_cid, uint8_t local_seid, const uint8_t * packet, uint16_t size);
    
    /**
     * @brief Select and configure SBC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     SBC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_sbc(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_sbc_t * configuration);
    
    /**
     * @brief Select and configure MPEG AUDIO endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AUDIO Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_mpeg_audio(uint16_t a2dp_cid, uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_mpeg_audio_t * configuration);
    
    /**
     * @brief Select and configure MPEG AAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     MPEG AAC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_mpeg_aac(uint16_t a2dp_cid,  uint8_t local_seid,  uint8_t remote_seid, const avdtp_configuration_mpeg_aac_t * configuration);
    
    /**
     * @brief Select and configure ATRAC endpoint
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param configuration     ATRAC Configuration
     * @return status
     */
    uint8_t a2dp_source_set_config_atrac(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const avdtp_configuration_atrac_t * configuration);
    
    /**
     * @brief Select and configure Non-A2DP endpoint. Bytes 0-3 of codec information contain Vendor ID, bytes 4-5 contain Vendor Specific Codec ID (little endian)
     * @param a2dp_cid 			A2DP channel identifier.
     * @param local_seid  		ID of a local stream endpoint.
     * @param remote_seid  		ID of a remote stream endpoint.
     * @param media_codec_information
     * @param media_codec_information_len
     * @return status
     */
    uint8_t a2dp_source_set_config_other(uint16_t a2dp_cid,  uint8_t local_seid, uint8_t remote_seid, const uint8_t * media_codec_information, uint8_t media_codec_information_len);
    
    /**
     * @brief Register media configuration validator. Can reject insuitable configuration or report stream endpoint as currently busy
     * @note validator has to return AVDTP error codes like: AVDTP_ERROR_CODE_SEP_IN_USE or AVDTP_ERROR_CODE_UNSUPPORTED_CONFIGURATION
     *       the callback receives the media configuration in the same format as the existing A2dP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION
     *       and similar
     * @param callback
     */
    void a2dp_source_register_media_config_validator(uint8_t (*callback)(const avdtp_stream_endpoint_t * stream_endpoint, const uint8_t * event, uint16_t size));
    
    /**
     * @brief De-Init A2DP Source device.
     */
    void a2dp_source_deinit(void);
    

## AVDTP Sink API {#sec:avdtp_sink_api}


**avdtp_sink.h** : 
Audio/Video Distribution Transport Protocol (AVDTP) Sink is a device that accepts streamed media data.  



    
    /**
     * @brief Set up AVDTP Sink device.
     */
    void avdtp_sink_init(void);
    
    // returns avdtp_stream_endpoint_t *
    avdtp_stream_endpoint_t * avdtp_sink_create_stream_endpoint(avdtp_sep_type_t sep_type, avdtp_media_type_t media_type);
    
    /**
     *  @brief Unregister stream endpoint and free it's memory
     *  @param stream_endpoint created by avdtp_sink_create_stream_endpoint
     */
    void avdtp_sink_finalize_stream_endpoint(avdtp_stream_endpoint_t * stream_endpoint);
    
    void avdtp_sink_register_media_transport_category(uint8_t seid);
    void avdtp_sink_register_reporting_category(uint8_t seid);
    void avdtp_sink_register_delay_reporting_category(uint8_t seid);
    void avdtp_sink_register_recovery_category(uint8_t seid, uint8_t maximum_recovery_window_size, uint8_t maximum_number_media_packets);
    void avdtp_sink_register_header_compression_category(uint8_t seid, uint8_t back_ch, uint8_t media, uint8_t recovery);
    void avdtp_sink_register_multiplexing_category(uint8_t seid, uint8_t fragmentation);
    
    void avdtp_sink_register_media_codec_category(uint8_t seid, avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type, const uint8_t *media_codec_info, uint16_t media_codec_info_len);
    void avdtp_sink_register_content_protection_category(uint8_t seid, uint16_t cp_type, const uint8_t * cp_type_value, uint8_t cp_type_value_len);
    
    /**
     * @brief Register callback for the AVDTP Sink client. 
     * @param callback
     */
    void avdtp_sink_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to device with a bluetooth address. (and perform configuration?)
     * @param bd_addr
     * @param avdtp_cid Assigned avdtp cid
     */
    uint8_t avdtp_sink_connect(bd_addr_t bd_addr, uint16_t * avdtp_cid);
    
    void avdtp_sink_register_media_handler(void (*callback)(uint8_t local_seid, uint8_t *packet, uint16_t size));
    /**
     * @brief Disconnect from device with connection handle. 
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_disconnect(uint16_t avdtp_cid);
    
    /**
     * @brief Discover stream endpoints
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_discover_stream_endpoints(uint16_t avdtp_cid);
    
    /**
     * @brief Get capabilities
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_get_capabilities(uint16_t avdtp_cid, uint8_t acp_seid);
    
    /**
     * @brief Get all capabilities
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_get_all_capabilities(uint16_t avdtp_cid, uint8_t acp_seid);
    
    /**
     * @brief Set configuration
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_set_configuration(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid, uint16_t configured_services_bitmap, avdtp_capabilities_t configuration);
    
    /**
     * @brief Reconfigure stream
     * @param avdtp_cid
     * @param seid
     */
    uint8_t avdtp_sink_reconfigure(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid, uint16_t configured_services_bitmap, avdtp_capabilities_t configuration);
    
    /**
     * @brief Get configuration
     * @param avdtp_cid
     */
    uint8_t avdtp_sink_get_configuration(uint16_t avdtp_cid, uint8_t acp_seid);
    
    
    /**
     * @brief Open stream
     * @param avdtp_cid
     * @param seid
     */
    uint8_t avdtp_sink_open_stream(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid);
    
    
    /**
     * @brief Start stream
     * @param local_seid
     */
    uint8_t avdtp_sink_start_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Abort stream
     * @param local_seid
     */
    uint8_t avdtp_sink_abort_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Start stream
     * @param local_seid
     */
    uint8_t avdtp_sink_stop_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Suspend stream
     * @param local_seid
     */
    uint8_t avdtp_sink_suspend(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Report delay
     * @param local_seid
     * @param delay_100us
     */
    uint8_t avdtp_sink_delay_report(uint16_t avdtp_cid, uint8_t local_seid, uint16_t delay_100us);
    
    /**
     * @brief Register media configuration validator. Can reject insuitable configuration or report stream endpoint as currently busy
     * @note validator has to return AVDTP error codes like: AVDTP_ERROR_CODE_SEP_IN_USE or AVDTP_ERROR_CODE_UNSUPPORTED_CONFIGURATION
     *       the callback receives the media configuration in the same format as the existing AVDTP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION
     *       and similar
     * @param callback
     */
    void avdtp_sink_register_media_config_validator(uint8_t (*callback)(const avdtp_stream_endpoint_t * stream_endpoint, const uint8_t * event, uint16_t size));
    
    /**
     * @brief De-Init AVDTP Sink.
     */
    void avdtp_sink_deinit(void);
    
    // AVDTP_SI_DELAYREPORT
    

## AVDTP Source API {#sec:avdtp_source_api}


**avdtp_source.h** : 
Audio/Video Distribution Transport Protocol (AVDTP) Source is a device that streames media data.



    
    /**
     * @brief Register media transport category with local stream endpoint identified by seid
     * @param seid
     */
    void avdtp_source_register_media_transport_category(uint8_t seid);
    
    /**
     * @brief Register reporting category with local stream endpoint identified by seid
     * @param seid
     */
    void avdtp_source_register_reporting_category(uint8_t seid);
    
    /**
     * @brief Register delay reporting category with local stream endpoint identified by seid
     * @param seid
     */
    void avdtp_source_register_delay_reporting_category(uint8_t seid);
    
    /**
     * @brief Register recovery category with local stream endpoint identified by seid
     * @param seid
     * @param maximum_recovery_window_size
     * @param maximum_number_media_packets
     */
    void avdtp_source_register_recovery_category(uint8_t seid, uint8_t maximum_recovery_window_size, uint8_t maximum_number_media_packets);
    
    /**
     * @brief Register content protection category with local stream endpoint identified by seid
     * @param seid
     * @param cp_type
     * @param cp_type_value
     * @param cp_type_value_len
     */
    void avdtp_source_register_content_protection_category(uint8_t seid, uint16_t cp_type, const uint8_t * cp_type_value, uint8_t cp_type_value_len);
    
    /**
     * @brief Register header compression category with local stream endpoint identified by seid
     * @param seid
     * @param back_ch
     * @param media
     * @param recovery
     */
    void avdtp_source_register_header_compression_category(uint8_t seid, uint8_t back_ch, uint8_t media, uint8_t recovery);
    
    /**
     * @brief Register media codec category with local stream endpoint identified by seid
     * @param seid
     * @param media_type
     * @param media_codec_type
     * @param media_codec_info
     * @param media_codec_info_len
     */
    void avdtp_source_register_media_codec_category(uint8_t seid, avdtp_media_type_t media_type, avdtp_media_codec_type_t media_codec_type, const uint8_t *media_codec_info, uint16_t media_codec_info_len);
    
    /**
     * @brief Register multiplexing category with local stream endpoint identified by seid
     * @param seid
     * @param fragmentation
     */
    void avdtp_source_register_multiplexing_category(uint8_t seid, uint8_t fragmentation);
    
    /**
     * @brief Initialize up AVDTP Source device.
     */
    void avdtp_source_init(void);
    
    /**
     * @brief Register callback for the AVDTP Source client. See btstack_defines.h for AVDTP_SUBEVENT_* events
     *
     * @param callback
     */
    void avdtp_source_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to device with a bluetooth address. (and perform configuration?)
     * @param bd_addr
     * @param avdtp_cid Assigned avdtp cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise BTSTACK_MEMORY_ALLOC_FAILED, SDP_QUERY_BUSY
     */
    uint8_t avdtp_source_connect(bd_addr_t bd_addr, uint16_t * avdtp_cid);
    
    /**
     * @brief Disconnect from device with connection handle. 
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER
     */
    uint8_t avdtp_source_disconnect(uint16_t avdtp_cid);
    
    /**
     * @brief Discover stream endpoints
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_discover_stream_endpoints(uint16_t avdtp_cid);
    
    /**
     * @brief Get capabilities
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_get_capabilities(uint16_t avdtp_cid, uint8_t acp_seid);
    
    /**
     * @brief Get all capabilities
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_get_all_capabilities(uint16_t avdtp_cid, uint8_t acp_seid);
    
    /**
     * @brief Set configuration
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_set_configuration(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid, uint16_t configured_services_bitmap, avdtp_capabilities_t configuration);
    
    /**
     * @brief Reconfigure stream
     * @param avdtp_cid
     * @param seid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_reconfigure(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid, uint16_t configured_services_bitmap, avdtp_capabilities_t configuration);
    
    /**
     * @brief Get configuration
     * @param avdtp_cid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_get_configuration(uint16_t avdtp_cid, uint8_t acp_seid);
    
    
    /**
     * @brief Open stream
     * @param avdtp_cid
     * @param seid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_open_stream(uint16_t avdtp_cid, uint8_t int_seid, uint8_t acp_seid);
    
    /**
     * @brief Start stream
     * @param local_seid
    * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_start_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Abort stream
     * @param local_seid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t avdtp_source_abort_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Start stream
     * @param local_seid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED 
     */
    uint8_t avdtp_source_stop_stream(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Suspend stream
     * @param local_seid
     * @return status ERROR_CODE_SUCCESS if succesful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED 
     */
    uint8_t avdtp_source_suspend(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Create stream endpoint
     * @param sep_type          AVDTP_SOURCE or AVDTP_SINK, see avdtp.h
     * @param media_type        AVDTP_AUDIO, AVDTP_VIDEO or AVDTP_MULTIMEDIA, see avdtp.h
     * @return pointer to newly created stream endpoint, or NULL if allocation failed
     */
    avdtp_stream_endpoint_t * avdtp_source_create_stream_endpoint(avdtp_sep_type_t sep_type, avdtp_media_type_t media_type);
    
    /**
     *  @brief Unregister stream endpoint and free it's memory
     *  @param stream_endpoint created by avdtp_sink_create_stream_endpoint
     */
    void avdtp_source_finalize_stream_endpoint(avdtp_stream_endpoint_t * stream_endpoint);
    
    /**
     * @brief Send media packet
     * @param avdtp_cid         AVDTP channel identifier.
     * @param local_seid        ID of a local stream endpoint.
     * @param packet
     * @param size
     * @return status
     */
    uint8_t avdtp_source_stream_send_media_packet(uint16_t avdtp_cid, uint8_t local_seid, const uint8_t * packet, uint16_t size);
    
    /**
     * @brief Send media payload including RTP header
     * @param avdtp_cid         AVDTP channel identifier.
     * @param local_seid        ID of a local stream endpoint.
     * @param marker
     * @param timestamp         in sample rate units
     * @param payload
     * @param size
     * @return status
     */
    uint8_t avdtp_source_stream_send_media_payload_rtp(uint16_t avdtp_cid, uint8_t local_seid, uint8_t marker, uint32_t timestamp,
                                               const uint8_t *payload, uint16_t size);
    
    /**
     * @brief Request to send a media packet. Packet can be then sent on reception of AVDTP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW event.
     * @param avdtp_cid         AVDTP channel identifier.
     * @param local_seid        ID of a local stream endpoint.
     */
    void avdtp_source_stream_endpoint_request_can_send_now(uint16_t avddp_cid, uint8_t local_seid);
    
    /**
     * @brief Return maximal media payload size, does not include media header.
     * @param avdtp_cid         AVDTP channel identifier.
     * @param local_seid        ID of a local stream endpoint.
     */
    int avdtp_max_media_payload_size(uint16_t avdtp_cid, uint8_t local_seid);
    
    /**
     * @brief Register media configuration validator. Can reject insuitable configuration or report stream endpoint as currently busy
     * @note validator has to return AVDTP error codes like: AVDTP_ERROR_CODE_SEP_IN_USE or AVDTP_ERROR_CODE_UNSUPPORTED_CONFIGURATION
     *       the callback receives the media configuration in the same format as the existing AVDTP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION
     *       and similar
     * @param callback
     */
    void avdtp_source_register_media_config_validator(uint8_t (*callback)(const avdtp_stream_endpoint_t * stream_endpoint, const uint8_t * event, uint16_t size));
    
    /**
     * @brief De-Init AVDTP Source.
     */
    void avdtp_source_deinit(void);
    

## AVRCP Browsing API {#sec:avrcp_browsing_api}


**avrcp_browsing.h** 


    /**
     * @brief Set up AVRCP Browsing service
     */
    void avrcp_browsing_init(void);
    
    /**
     * @brief Register callback for the AVRCP Browsing Controller client. 
     * @param callback
     */
    void avrcp_browsing_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief   Connect to AVRCP Browsing service on a remote device, emits AVRCP_SUBEVENT_BROWSING_CONNECTION_ESTABLISHED with status
     * @param   remote_addr
     * @param   ertm_buffer
     * @param   ertm_buffer_size
     * @param   ertm_config
     * @param   avrcp_browsing_cid  outgoing parameter, valid if status == ERROR_CODE_SUCCESS
     * @return status     
     */
    uint8_t avrcp_browsing_connect(bd_addr_t remote_addr, uint8_t * ertm_buffer, uint32_t ertm_buffer_size, l2cap_ertm_config_t * ertm_config, uint16_t * avrcp_browsing_cid);
    
    /**
     * @brief Configure incoming connection for Browsing Service.
     * @param avrcp_browsing_cid
     * @param ertm_buffer
     * @param ertm_buffer_size
     * @param ertm_config
     * @return status
     */
    uint8_t avrcp_browsing_configure_incoming_connection(uint16_t avrcp_browsing_cid, uint8_t * ertm_buffer, uint32_t ertm_buffer_size, l2cap_ertm_config_t * ertm_config);
    
    /**
     * @brief Decline incoming connection Browsing Service.
     * @param avrcp_browsing_cid
     * @return status
     */
    uint8_t avrcp_browsing_decline_incoming_connection(uint16_t avrcp_browsing_cid);
    
    /**
     * @brief   Disconnect from AVRCP Browsing service
     * @param   avrcp_browsing_cid
     * @return status
     */
    uint8_t avrcp_browsing_disconnect(uint16_t avrcp_browsing_cid);
    
    /**
     * @brief De-Init AVRCP Browsing
     */
    void avrcp_browsing_deinit(void);
    

## AVRCP Browsing Controller API {#sec:avrcp_browsing_controller_api}


**avrcp_browsing_controller.h** 


    
    /**
     * @brief Set up AVRCP Browsing Controller device.
     */
    void avrcp_browsing_controller_init(void);
    
    /**
     * @brief Register callback for the AVRCP Browsing Controller client to receive:
     * - event AVRCP_SUBEVENT_BROWSING_DONE marking the end of operation. If the browsing status field of this field is equal to AVRCP_BROWSING_ERROR_CODE_SUCCESS, operation was successful.
     * - data packet marked by AVRCP_BROWSING_DATA_PACKET subpacket type containg the response load.
     * 
     * @param callback
     */
    void avrcp_browsing_controller_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Retrieve a list of media players.
     * @param avrcp_browsing_cid
     * @param start_item
     * @param end_item
     * @param attr_bitmap Use AVRCP_MEDIA_ATTR_ALL for all, and AVRCP_MEDIA_ATTR_NONE for none. Otherwise, see avrcp_media_attribute_id_t for the bitmap position of attrs. 
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query.
     */
    uint8_t avrcp_browsing_controller_get_media_players(uint16_t avrcp_browsing_cid, uint32_t start_item, uint32_t end_item, uint32_t attr_bitmap);
    
    /**
     * @brief Retrieve a list of folders and media items of the browsed player.
     * @param avrcp_browsing_cid
     * @param start_item
     * @param end_item
     * @param attr_bitmap Use AVRCP_MEDIA_ATTR_ALL for all, and AVRCP_MEDIA_ATTR_NONE for none. Otherwise, see avrcp_media_attribute_id_t for the bitmap position of attrs. 
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query.
     */
    uint8_t avrcp_browsing_controller_browse_file_system(uint16_t avrcp_browsing_cid, uint32_t start_item, uint32_t end_item, uint32_t attr_bitmap);
    
    /**
     * @brief Retrieve a list of media items of the browsed player.
     * @param avrcp_browsing_cid
     * @param start_item
     * @param end_item
     * @param attr_bitmap Use AVRCP_MEDIA_ATTR_ALL for all, and AVRCP_MEDIA_ATTR_NONE for none. Otherwise, see avrcp_media_attribute_id_t for the bitmap position of attrs. 
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query.
     */
    uint8_t avrcp_browsing_controller_browse_media(uint16_t avrcp_browsing_cid, uint32_t start_item, uint32_t end_item, uint32_t attr_bitmap);
    
    /**
     * @brief Retrieve a list of folders and media items of the addressed player.
     * @param avrcp_browsing_cid
     * @param start_item
     * @param end_item
     * @param attr_bitmap Use AVRCP_MEDIA_ATTR_ALL for all, and AVRCP_MEDIA_ATTR_NONE for none. Otherwise, see avrcp_media_attribute_id_t for the bitmap position of attrs. 
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query.
     */
    uint8_t avrcp_browsing_controller_browse_now_playing_list(uint16_t avrcp_browsing_cid, uint32_t start_item, uint32_t end_item, uint32_t attr_bitmap);
    
    /** 
     * @brief Set browsed player. Calling this command is required prior to browsing the player's file system. Some players may support browsing only when set as the Addressed Player.
     * @param avrcp_browsing_cid
     * @param browsed_player_id
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query.
     */
    uint8_t avrcp_browsing_controller_set_browsed_player(uint16_t avrcp_browsing_cid, uint16_t browsed_player_id);
    
    /** 
     * @brief Get total num attributes
     * @param avrcp_browsing_cid
     * @param scope 
     */
    uint8_t avrcp_browsing_controller_get_total_nr_items_for_scope(uint16_t avrcp_browsing_cid, avrcp_browsing_scope_t scope);
    
    /**
     * @brief Navigate one level up or down in thhe virtual filesystem. Requires that s browsed player is set.
     * @param avrcp_browsing_cid
     * @param direction     0-folder up, 1-folder down    
     * @param folder_uid    8 bytes long
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_controller_change_path(uint16_t avrcp_browsing_cid, uint8_t direction, uint8_t * folder_uid);
    uint8_t avrcp_browsing_controller_go_up_one_level(uint16_t avrcp_browsing_cid);
    uint8_t avrcp_browsing_controller_go_down_one_level(uint16_t avrcp_browsing_cid, uint8_t * folder_uid);
    
    
    /**
     * @brief Retrives metadata information (title, artist, album, ...) about a media element with given uid. 
     * @param avrcp_browsing_cid
     * @param uid 			 media element uid 
     * @param uid_counter    Used to detect change to the media database on target device. A TG device that supports the UID Counter shall update the value of the counter on each change to the media database.
     * @param attr_bitmap    0x00000000 - retrieve all, chek avrcp_media_attribute_id_t in avrcp.h for detailed bit position description.
     * @param scope          check avrcp_browsing_scope_t in avrcp.h
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_controller_get_item_attributes_for_scope(uint16_t avrcp_browsing_cid, uint8_t * uid, uint16_t uid_counter, uint32_t attr_bitmap, avrcp_browsing_scope_t scope);
    
    /**
     * @brief Searches are performed from the current folder in the Browsed Players virtual filesystem. The search applies to the current folder and all folders below that.
     * @param avrcp_browsing_cid
     * @param search_str_len
     * @param search_str
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query,
     *       - ERROR_CODE_PARAMETER_OUT_OF_MANDATORY_RANGE if search string is NULL. 
     */
    uint8_t avrcp_browsing_controller_search(uint16_t avrcp_browsing_cid, uint16_t search_str_len, char * search_str);
    
    /**
     * @brief De-Init AVRCP Browsing Controller
     */
    void avrcp_browsing_controller_deinit(void);
    

## AVRCP Browsing Target API {#sec:avrcp_browsing_target_api}


**avrcp_browsing_target.h** 


    
    /**
     * @brief Set up AVRCP Browsing Target client.
     */
    void avrcp_browsing_target_init(void);
    
    /**
     * @brief Register callback for the AVRCP Browsing Target client to receive events:
     * - AVRCP_SUBEVENT_BROWSING_SEARCH
     * - AVRCP_SUBEVENT_BROWSING_GET_FOLDER_ITEMS
     * - AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS
     * - AVRCP_SUBEVENT_BROWSING_SET_BROWSED_PLAYER
     * - AVRCP_SUBEVENT_BROWSING_CHANGE_PATH
     * - AVRCP_SUBEVENT_BROWSING_GET_ITEM_ATTRIBUTES
     *  
     * @param callback
     */
    void avrcp_browsing_target_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Accept set browsed player
     * @param browsing_cid
     * @param uid_counter
     * @param browsed_player_id
     * @param response
     * @param response_size
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_accept_set_browsed_player(uint16_t browsing_cid, uint16_t uid_counter, uint16_t browsed_player_id, uint8_t * response, uint16_t response_len);
    
    /**
     * @brief Reject set browsed player
     * @param browsing_cid
     * @param status
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_reject_set_browsed_player(uint16_t browsing_cid, avrcp_status_code_t status);
    
    /**
     * @brief Send answer to get folder items query on event AVRCP_SUBEVENT_BROWSING_GET_FOLDER_ITEMS. The first byte of this event defines the scope of the query, see avrcp_browsing_scope_t.
     * @param browsing_cid
     * @param uid_counter
     * @param attr_list
     * @param attr_list_size
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_folder_items_response(uint16_t browsing_cid, uint16_t uid_counter, uint8_t * attr_list, uint16_t attr_list_size, uint16_t num_items);
    
    /**
     * @brief Send answer to get total number of items query on event AVRCP_SUBEVENT_BROWSING_GET_TOTAL_NUM_ITEMS. The first byte of this event defines the scope of the query, see avrcp_browsing_scope_t.
     * @param browsing_cid
     * @param uid_counter
     * @param total_num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_total_num_items_response(uint16_t browsing_cid, uint16_t uid_counter, uint32_t total_num_items);
    
    /**
     * @brief Send answer to change path command on event AVRCP_SUBEVENT_BROWSING_CHANGE_PATH.
     * @param browsing_cid
     * @param status
     * @param num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_change_path_response(uint16_t browsing_cid, avrcp_status_code_t status, uint32_t num_items);
    
    /**
     * @brief Send answer to get item attribute response on event AVRCP_SUBEVENT_BROWSING_GET_ITEM_ATTRIBUTES.
     * @param browsing_cid
     * @param status
     * @param attr_list
     * @param attr_list_size
     * @param num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_get_item_attributes_response(uint16_t browsing_cid, avrcp_status_code_t status, uint8_t * attr_list, uint16_t attr_list_size, uint8_t num_items);
    
    /**
     * @brief Send answer to search query on event AVRCP_SUBEVENT_BROWSING_SEARCH.
     * @param browsing_cid
     * @param uid_counter
     * @param total_num_items
     * @return ERROR_CODE_SUCCESS if successful, otherwise:
     *       - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if unknown connection cid,
     *       - ERROR_CODE_COMMAND_DISALLOWED if client is not done with previous query. 
     */
    uint8_t avrcp_browsing_target_send_search_response(uint16_t browsing_cid, avrcp_status_code_t status, uint16_t uid_counter, uint32_t num_items);
    
    /**
     * @brief De-Init AVRCP Browsing Target
     */
    void avrcp_browsing_target_deinit(void);
    

## AVRCP Controller API {#sec:avrcp_controller_api}


**avrcp_controller.h** 


    
    typedef enum {
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER = 0,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_MONITOR_OR_AMPLIFIER,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_TUNER,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_CATEGORY_MENU,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_RESERVED_4,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_RESERVED_5,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_BROWSING,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_COVER_ART_GET_IMAGE_PROPERTIES,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_COVER_ART_GET_IMAGE,
        AVRCP_CONTROLLER_SUPPORTED_FEATURE_COVER_ART_GET_LINKED_THUMBNAIL
    } avrcp_controller_supported_feature_t;
    
    /**
     * @brief AVRCP Controller service record. 
     * @param service
     * @param service_record_handle
     * @param supported_features 16-bit bitmap, see AVRCP_FEATURE_MASK_* in avrcp.h
     * @param service_name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param service_provider_name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void avrcp_controller_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);
    
    /**
     * @brief Set up AVRCP Controller service.
     */
    void avrcp_controller_init(void);
    
    /**
     * @brief Register callback for the AVRCP Controller client. 
     * @param callback
     */
    void avrcp_controller_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Set max num fragments in whuch message can be transmited.
     * @param avrcp_cid
     * @param max_num_fragments
     * @return status
     */
    uint8_t avrcp_controller_set_max_num_fragments(uint16_t avrcp_cid, uint8_t max_num_fragments);
    
    
    /**
     * @brief Unit info.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_unit_info(uint16_t avrcp_cid);
    
    /**
     * @brief Subunit info.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_subunit_info(uint16_t avrcp_cid);
    
    /**
     * @brief Get capabilities.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_supported_company_ids(uint16_t avrcp_cid);
    
    /**
     * @brief Get supported Events.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_supported_events(uint16_t avrcp_cid);
    
    
    /**
     * @brief Start continuous cmd (play, pause, volume up, ...). Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_start_press_and_hold_cmd(uint16_t avrcp_cid, avrcp_operation_id_t operation_id);
    
    /**
     * @brief Stops continuous cmd (play, pause, volume up, ...). Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_release_press_and_hold_cmd(uint16_t avrcp_cid);
    
    /**
     * Send PASS THROUGH command. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @param operation_id
     * @return
     */
    uint8_t avrcp_controller_send_pass_through_cmd(uint16_t avrcp_cid, avrcp_operation_id_t operation_id);
    
    /**
     * @brief Play. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_play(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_play(uint16_t avrcp_cid);
    
    /**
     * @brief Stop. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_stop(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_stop(uint16_t avrcp_cid);
    
    /**
     * @brief Pause. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_pause(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_pause(uint16_t avrcp_cid);
    
    /**
     * @brief Single step - fast forward. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_fast_forward(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_fast_forward(uint16_t avrcp_cid);
    
    
    /**
     * @brief Single step rewind. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_rewind(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_rewind(uint16_t avrcp_cid);
    
    /**
     * @brief Forward. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_forward(uint16_t avrcp_cid); 
    uint8_t avrcp_controller_press_and_hold_forward(uint16_t avrcp_cid); 
    
    /**
     * @brief Backward. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_backward(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_backward(uint16_t avrcp_cid);
    
    /**
     * @brief Turn up volume. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_volume_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_volume_up(uint16_t avrcp_cid);
    /**
     * @brief Turn down volume. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_volume_down(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_volume_down(uint16_t avrcp_cid);
    
    /**
     * @brief Mute sound. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_mute(uint16_t avrcp_cid);
    uint8_t avrcp_controller_press_and_hold_mute(uint16_t avrcp_cid);
    
    /**
     * @brief Record. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_record(uint16_t avrcp_cid);
    
    /**
     * @brief Eject. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_eject(uint16_t avrcp_cid);
    
    // Basic Group Navigation
    /**
     * @brief Move to the first song in the next group.
     * @param avrcp_cid
     */
    uint8_t avrcp_controller_next_group(uint16_t avrcp_cid);
    
    /**
     * @brief Move to the first song in the previous group.
     * @param avrcp_cid
     */
    uint8_t avrcp_controller_previous_group(uint16_t avrcp_cid);
    
    // Category 3
    uint8_t avrcp_controller_0(uint16_t avrcp_cid);
    uint8_t avrcp_controller_1(uint16_t avrcp_cid);
    uint8_t avrcp_controller_2(uint16_t avrcp_cid);
    uint8_t avrcp_controller_3(uint16_t avrcp_cid);
    uint8_t avrcp_controller_4(uint16_t avrcp_cid);
    uint8_t avrcp_controller_5(uint16_t avrcp_cid);
    uint8_t avrcp_controller_6(uint16_t avrcp_cid);
    uint8_t avrcp_controller_7(uint16_t avrcp_cid);
    uint8_t avrcp_controller_8(uint16_t avrcp_cid);
    uint8_t avrcp_controller_9(uint16_t avrcp_cid);
    uint8_t avrcp_controller_dot(uint16_t avrcp_cid);
    uint8_t avrcp_controller_enter(uint16_t avrcp_cid);
    uint8_t avrcp_controller_clear(uint16_t avrcp_cid);
    uint8_t avrcp_controller_channel_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_channel_down(uint16_t avrcp_cid);
    uint8_t avrcp_controller_previous_channel(uint16_t avrcp_cid);
    uint8_t avrcp_controller_sound_select(uint16_t avrcp_cid);
    uint8_t avrcp_controller_input_select(uint16_t avrcp_cid);
    uint8_t avrcp_controller_display_information(uint16_t avrcp_cid);
    uint8_t avrcp_controller_help(uint16_t avrcp_cid);
    uint8_t avrcp_controller_power(uint16_t avrcp_cid);
    uint8_t avrcp_controller_angle(uint16_t avrcp_cid);
    uint8_t avrcp_controller_subpicture(uint16_t avrcp_cid);
    uint8_t avrcp_controller_F1(uint16_t avrcp_cid);
    uint8_t avrcp_controller_F2(uint16_t avrcp_cid);
    uint8_t avrcp_controller_F3(uint16_t avrcp_cid);
    uint8_t avrcp_controller_F4(uint16_t avrcp_cid);
    uint8_t avrcp_controller_F5(uint16_t avrcp_cid);
    
    // Category 4
    uint8_t avrcp_controller_select(uint16_t avrcp_cid);
    uint8_t avrcp_controller_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_down(uint16_t avrcp_cid);
    uint8_t avrcp_controller_left(uint16_t avrcp_cid);
    uint8_t avrcp_controller_right(uint16_t avrcp_cid);
    uint8_t avrcp_controller_right_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_right_down(uint16_t avrcp_cid);
    uint8_t avrcp_controller_left_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_left_down(uint16_t avrcp_cid);
    uint8_t avrcp_controller_root_menu(uint16_t avrcp_cid);
    uint8_t avrcp_controller_setup_menu(uint16_t avrcp_cid);
    uint8_t avrcp_controller_contents_menu(uint16_t avrcp_cid);
    uint8_t avrcp_controller_favorite_menu(uint16_t avrcp_cid);
    uint8_t avrcp_controller_exit(uint16_t avrcp_cid);
    uint8_t avrcp_controller_page_up(uint16_t avrcp_cid);
    uint8_t avrcp_controller_page_down(uint16_t avrcp_cid);
    /**
     * @brief Get play status. Returns event of type AVRCP_SUBEVENT_PLAY_STATUS (length, position, play_status).
     * If TG does not support SongLength And SongPosition on TG, then TG shall return 0xFFFFFFFF.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_play_status(uint16_t avrcp_cid);
    
    /**
     * @brief Enable notification. Response via AVRCP_SUBEVENT_NOTIFICATION_STATE.
     * @param avrcp_cid
     * @param event_id
     * @return status
     */
    uint8_t avrcp_controller_enable_notification(uint16_t avrcp_cid, avrcp_notification_event_id_t event_id);
    
    /**
     * @brief Disable notification. Response via AVRCP_SUBEVENT_NOTIFICATION_STATE.
     * @param avrcp_cid
     * @param event_id
     * @return status
     */
    uint8_t avrcp_controller_disable_notification(uint16_t avrcp_cid, avrcp_notification_event_id_t event_id);
    
    /**
     * @brief Get info on now playing media using subset of attribute IDs
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_element_attributes(uint16_t avrcp_cid, uint8_t num_attributes, avrcp_media_attribute_id_t * attributes);
    
    /**
     * @brief Get info on now playing media using all IDs.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_now_playing_info(uint16_t avrcp_cid);
    
    /**
     * @brief Get info on now playing media using specific media attribute ID.
     * @param media_attribute_id
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_get_now_playing_info_for_media_attribute_id(uint16_t avrcp_cid, avrcp_media_attribute_id_t media_attribute_id);
    
    /**
     * @brief Set absolute volume 0-127 (corresponds to 0-100%). Response via AVRCP_SUBEVENT_SET_ABSOLUTE_VOLUME_RESPONSE
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_set_absolute_volume(uint16_t avrcp_cid, uint8_t volume);
    
    
    /**
     * @brief Skip to next playing media. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_skip(uint16_t avrcp_cid);
    
    uint8_t avrcp_controller_query_player_application_setting_attributes(uint16_t avrcp_cid);
    uint8_t avrcp_controller_query_player_application_setting_values(uint16_t avrcp_cid, avrcp_player_application_setting_attribute_id_t attribute_id);
    
    uint8_t avrcp_controller_query_player_application_setting_attribute_text(uint16_t avrcp_cid, uint8_t attr_ids_num, avrcp_player_application_setting_attribute_id_t * attr_ids);
    uint8_t avrcp_controller_query_player_application_setting_value_text(uint16_t avrcp_cid, avrcp_player_application_setting_attribute_id_t attr_id, uint8_t attr_id_values_num, uint8_t * attr_id_values);
    
    uint8_t avrcp_controller_inform_displayable_characterset(uint16_t avrcp_cid, uint8_t character_set_num, uint16_t * character_set);
    uint8_t avrcp_controller_inform_battery_status(uint16_t avrcp_cid, avrcp_battery_status_t battery_status);
    
    /**
     * @brief Query repeat and shuffle mode. Response via AVRCP_SUBEVENT_SHUFFLE_AND_REPEAT_MODE.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_query_shuffle_and_repeat_modes(uint16_t avrcp_cid);
    
    /**
     * @brief Set shuffle mode. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_set_shuffle_mode(uint16_t avrcp_cid, avrcp_shuffle_mode_t mode);
    
    /**
     * @brief Set repeat mode. Event AVRCP_SUBEVENT_OPERATION_COMPLETE returns operation id and status.
     * @param avrcp_cid
     * @return status
     */
    uint8_t avrcp_controller_set_repeat_mode(uint16_t avrcp_cid, avrcp_repeat_mode_t mode);
    
    /**
     * @brief The PlayItem command starts playing an item indicated by the UID. It is routed to the Addressed Player.
     * @param avrcp_cid
     * @param uid
     * @param uid_counter
     * @param scope
     **/
    uint8_t avrcp_controller_play_item_for_scope(uint16_t avrcp_cid, uint8_t * uid, uint16_t uid_counter, avrcp_browsing_scope_t scope);
    
    /**
     * @brief Adds an item indicated by the UID to the Now Playing queue.
     * @param avrcp_cid
     * @param uid
     * @param uid_counter
     * @param scope
     **/
    uint8_t avrcp_controller_add_item_from_scope_to_now_playing_list(uint16_t avrcp_cid, uint8_t * uid, uint16_t uid_counter, avrcp_browsing_scope_t scope);
    
    /** 
     * @brief Set addressed player.  
     * @param avrcp_cid
     * @param addressed_player_id
     */
    uint8_t avrcp_controller_set_addressed_player(uint16_t avrcp_cid, uint16_t addressed_player_id);
    
    /** 
     * @brief Send custom command
     * @param avrcp_cid
     * @param command_type
     * @param subunit_type
     * @param subunit_id
     * @param pdu_id
     * @param company_id
     * @param data
     * @param data_len
     */
    uint8_t avrcp_controller_send_custom_command(uint16_t avrcp_cid, 
        avrcp_command_type_t command_type, 
        avrcp_subunit_type_t subunit_type, avrcp_subunit_id_t subunit_id, 
        avrcp_pdu_id_t pdu_id, uint32_t company_id, 
        const uint8_t * data, uint16_t data_len);
    
    /**
     * @brief De-Init AVRCP Controller
     */
    void avrcp_controller_deinit(void);
    

## AVRCP Cover Art Client API {#sec:avrcp_cover_art_client_api}


**avrcp_cover_art_client.h** 


    
    /**
     * @brief Set up AVRCP Cover Art client
     */
    void avrcp_cover_art_client_init(void);
    
    /**
     * @brief   Connect to AVRCP Cover Art service on a remote device, emits AVRCP_SUBEVENT_COVER_ART_CONNECTION_ESTABLISHED with status
     * @param   packet_handler
     * @param   remote_addr
     * @param   ertm_buffer
     * @param   ertm_buffer_size
     * @param   ertm_config
     * @param   avrcp_cover_art_cid  outgoing parameter, valid if status == ERROR_CODE_SUCCESS
     * @return status     
     */
    uint8_t avrcp_cover_art_client_connect(avrcp_cover_art_client_t *cover_art_client, btstack_packet_handler_t packet_handler,
                                   bd_addr_t remote_addr, uint8_t *ertm_buffer, uint32_t ertm_buffer_size,
                                   const l2cap_ertm_config_t *ertm_config, uint16_t *avrcp_cover_art_cid);
    
    /**
     * @brief Request cover art thumbnail for cover with a given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_linked_thumbnail(uint16_t avrcp_cover_art_cid, const char * image_handle);
    
    /**
     * @brief Request cover art image for given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     *        and given image descriptor
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @param image_descriptor
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_image(uint16_t avrcp_cover_art_cid, const char * image_handle, const char * image_descriptor);
    
    /**
     * @brief Request image properties for given image handle retrieved via
     *        - avrcp_controller_get_now_playing_info or
     *        - avrcp_controller_get_element_attributes(... AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART ...)
     * @param avrcp_cover_art_cid
     * @param image_handle
     * @return status
     */
    uint8_t avrcp_cover_art_client_get_image_properties(uint16_t avrcp_cover_art_cid, const char * image_handle);
    
    /**
     * @brief   Disconnect from AVRCP Cover Art service
     * @param   avrcp_cover_art_cid
     * @return status
     */
    uint8_t avrcp_cover_art_client_disconnect(uint16_t avrcp_cover_art_cid);
    
    /**
     * @brief De-Init AVRCP Cover Art Client
     */
    void avrcp_cover_art_client_deinit(void);
    

## AVRCP Media Item Iterator API {#sec:avrcp_media_item_iterator_api}


**avrcp_media_item_iterator.h** 


    
    typedef struct avrcp_media_item_context {
         const uint8_t * data;
         uint16_t   offset;
         uint16_t   length;
    } avrcp_media_item_context_t;
    
    // Media item data iterator
    void avrcp_media_item_iterator_init(avrcp_media_item_context_t *context, uint16_t avrcp_media_item_len, const uint8_t * avrcp_media_item_data);
    int  avrcp_media_item_iterator_has_more(const avrcp_media_item_context_t * context);
    void avrcp_media_item_iterator_next(avrcp_media_item_context_t * context);
    
    // Access functions
    uint32_t          avrcp_media_item_iterator_get_attr_id(const avrcp_media_item_context_t * context);
    uint16_t         avrcp_media_item_iterator_get_attr_charset(const avrcp_media_item_context_t * context);
    uint16_t         avrcp_media_item_iterator_get_attr_value_len(const avrcp_media_item_context_t * context);
    const uint8_t *  avrcp_media_item_iterator_get_attr_value(const avrcp_media_item_context_t * context);
    

## AVRCP Target API {#sec:avrcp_target_api}


**avrcp_target.h** 


    
    typedef enum {
        AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER = 0,
        AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_MONITOR_OR_AMPLIFIER,
        AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_TUNER,
        AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_MENU,
        AVRCP_TARGET_SUPPORTED_FEATURE_PLAYER_APPLICATION_SETTINGS, // AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER must be 1 for this feature to be set
        AVRCP_TARGET_SUPPORTED_FEATURE_GROUP_NAVIGATION,            // AVRCP_TARGET_SUPPORTED_FEATURE_CATEGORY_PLAYER_OR_RECORDER must be 1 for this feature to be set
        AVRCP_TARGET_SUPPORTED_FEATURE_BROWSING,
        AVRCP_TARGET_SUPPORTED_FEATURE_MULTIPLE_MEDIA_PLAYER_APPLICATIONS,
        AVRCP_TARGET_SUPPORTED_FEATURE_COVER_ART,
    } avrcp_target_supported_feature_t;
    
    /**
     * @brief AVRCP Target service record. 
     * @param service
     * @param service_record_handle
     * @param supported_features 16-bit bitmap, see AVRCP_FEATURE_MASK_* in avrcp.h
     * @param service_name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param service_provider_name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void    avrcp_target_create_sdp_record(uint8_t * service, uint32_t service_record_handle, uint16_t supported_features, const char * service_name, const char * service_provider_name);
    
    /**
     * @brief Set up AVRCP Target service.
     */
    void    avrcp_target_init(void);
    
    /**
     * @brief Register callback for the AVRCP Target client. 
     * @param callback
     */
    void    avrcp_target_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Select Player that is controlled by Controller
     * @param callback
     * @note Callback should return if selected player is valid
     */
    void avrcp_target_register_set_addressed_player_handler(bool (*callback)(uint16_t player_id));
    
    /**
     * @brief Register a list of Company IDs supported by target. 
     * @param avrcp_cid
     * @param num_companies
     * @param companies
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_support_companies(uint16_t avrcp_cid, uint8_t num_companies, const uint32_t *companies);
    
    /**
     * @brief Register event ID supported by target.
     * @param avrcp_cid
     * @param event_id 
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE for unsupported event id, otherwise ERROR_CODE_SUCCESS, 
     */
    uint8_t avrcp_target_support_event(uint16_t avrcp_cid, avrcp_notification_event_id_t event_id);
    
    /**
     * @brief Send a play status.
     * @note  The avrcp_target_packet_handler will receive AVRCP_SUBEVENT_PLAY_STATUS_QUERY event. Use this function to respond.
     * @param avrcp_cid
     * @param song_length_ms
     * @param song_position_ms
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_play_status(uint16_t avrcp_cid, uint32_t song_length_ms, uint32_t song_position_ms, avrcp_playback_status_t status);
    
    /**
     * @brief Set Now Playing Info that is send to Controller if notifications are enabled
     * @param avrcp_cid
     * @param current_track
     * @param total_tracks
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, ERROR_CODE_COMMAND_DISALLOWED if no track is provided, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_set_now_playing_info(uint16_t avrcp_cid, const avrcp_track_t * current_track, uint16_t total_tracks);
    
    /**
     * @brief Set Playing status and send to Controller
     * @param avrcp_cid
     * @param playback_status
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_set_playback_status(uint16_t avrcp_cid, avrcp_playback_status_t playback_status);
    
    /**
     * @brief Set Unit Info
     * @param avrcp_cid
     * @param unit_type
     * @param company_id
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_set_unit_info(uint16_t avrcp_cid, avrcp_subunit_type_t unit_type, uint32_t company_id);
    
    /**
     * @brief Set Subunit Info
     * @param avrcp_cid
     * @param subunit_type
     * @param subunit_info_data
     * @param subunit_info_data_size
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_set_subunit_info(uint16_t avrcp_cid, avrcp_subunit_type_t subunit_type, const uint8_t * subunit_info_data, uint16_t subunit_info_data_size);
    
    /**
     * @brief Send Playing Content Changed Notification if enabled
     * @param avrcp_cid
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_playing_content_changed(uint16_t avrcp_cid);
    
    /**
     * @brief Send Addressed Player Changed Notification if enabled
     * @param avrcp_cid
     * @param player_id
     * @param uid_counter
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_addressed_player_changed(uint16_t avrcp_cid, uint16_t player_id, uint16_t uid_counter);
    
    /**
     * @brief Set Battery Status Changed and send notification if enabled
     * @param avrcp_cid
     * @param battery_status
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_battery_status_changed(uint16_t avrcp_cid, avrcp_battery_status_t battery_status);
    
    /**
     * @brief Overwrite the absolute volume requested by controller with the actual absolute volume. 
     * This function can only be called on AVRCP_SUBEVENT_NOTIFICATION_VOLUME_CHANGED event, which indicates a set absolute volume request by controller. 
     * If the absolute volume requested by controller does not match the granularity of volume control the TG provides, you can use this function to adjust the actual value.
     *
     * @param avrcp_cid
     * @param absolute_volume
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_adjust_absolute_volume(uint16_t avrcp_cid, uint8_t absolute_volume);
    
    
    /**
     * @brief Set Absolute Volume and send notification if enabled 
     * @param avrcp_cid
     * @param absolute_volume
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_volume_changed(uint16_t avrcp_cid, uint8_t absolute_volume);
    
    /**
     * @brief Set Track and send notification if enabled
     * @param avrcp_cid
     * @param trackID
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_track_changed(uint16_t avrcp_cid, uint8_t * trackID);
    
    /**
     * @brief Send Operation Rejected message
     * @param avrcp_cid
     * @param opid
     * @param operands_length
     * @param operand
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_operation_rejected(uint16_t avrcp_cid, avrcp_operation_id_t opid, uint8_t operands_length, uint8_t operand);
    
    /**
     * @brief Send Operation Accepted message
     * @param avrcp_cid
     * @param opid
     * @param operands_length
     * @param operand
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_operation_accepted(uint16_t avrcp_cid, avrcp_operation_id_t opid, uint8_t operands_length, uint8_t operand);
    
    /**
     * @brief Send Operation Not Implemented message
     * @param avrcp_cid
     * @param opid
     * @param operands_length
     * @param operand
     * @return status ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection is not found, otherwise ERROR_CODE_SUCCESS
     */
    uint8_t avrcp_target_operation_not_implemented(uint16_t avrcp_cid, avrcp_operation_id_t opid, uint8_t operands_length, uint8_t operand);
    
    uint8_t avrcp_target_uids_changed(uint16_t avrcp_cid, uint16_t uid_counter);
    uint8_t avrcp_target_send_response_for_play_item_cmd(uint16_t avrcp_cid, avrcp_status_code_t status);
    uint8_t avrcp_target_send_response_for_add_to_now_playing_cmd(uint16_t avrcp_cid, avrcp_status_code_t status);
    
    /**
     * @brief De-Init AVRCP Browsing Target
     */
    void avrcp_target_deinit(void);
    

## BNEP API {#sec:bnep_api}


**bnep.h** 


    
    /**
     * @brief Set up BNEP.
     */
    void bnep_init(void);
    
    /**
     * @brief Check if a data packet can be send out.
     */
    int bnep_can_send_packet_now(uint16_t bnep_cid);
    
    /** 
     * @brief Request emission of BNEP_CAN_SEND_NOW as soon as possible
     * @note BNEP_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param bnep_cid
     */
    void bnep_request_can_send_now_event(uint16_t bnep_cid);
    
    /**
     * @brief Send a data packet.
     */
    int bnep_send(uint16_t bnep_cid, uint8_t *packet, uint16_t len);
    
    /**
     * @brief Set the network protocol filter.
     */
    int bnep_set_net_type_filter(uint16_t bnep_cid, bnep_net_filter_t *filter, uint16_t len);
    
    /**
     * @brief Set the multicast address filter.
     */
    int bnep_set_multicast_filter(uint16_t bnep_cid, bnep_multi_filter_t *filter, uint16_t len);
    
    /**
     * @brief Set security level required for incoming connections, need to be called before registering services.
     * @deprecated use gap_set_security_level instead
     */
    void bnep_set_required_security_level(gap_security_level_t security_level);
    
    /**
     * @brief Creates BNEP connection (channel) to a given server on a remote device with baseband address. A new baseband connection will be initiated if necessary. 
     */
    int bnep_connect(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint16_t l2cap_psm, uint16_t uuid_src, uint16_t uuid_dest);
    
    /**
     * @brief Disconnects BNEP channel with given identifier. 
     */
    void bnep_disconnect(bd_addr_t addr);
    
    /**
     * @brief Registers BNEP service, set a maximum frame size and assigns a packet handler. On embedded systems, use NULL for connection parameter. 
     */
    uint8_t bnep_register_service(btstack_packet_handler_t packet_handler, uint16_t service_uuid, uint16_t max_frame_size);
    
    /**
     * @brief Unregister BNEP service.
     */
    void bnep_unregister_service(uint16_t service_uuid);
    
    /**
     * @brief De-Init BNEP
     */
    void bnep_deinit(void);
    

## Link Key DB API {#sec:btstack_link_key_db_api}


**btstack_link_key_db.h** : 
Interface to provide link key storage.



    typedef struct {
    
        // management
    
        /**
         * @brief Open the Link Key DB
         */
        void (*open)(void);
    
        /**
         * @brief Sets BD Addr of local Bluetooth Controller.
         * @note Only needed if Bluetooth Controller can be swapped, e.g. USB Dongles on desktop systems
         */
        void (*set_local_bd_addr)(bd_addr_t bd_addr);
    
        /**
         * @brief Close the Link Key DB
         */
        void (*close)(void);
        
        // get/set/delete link key
    
        /**
         * @brief Get Link Key for given address
         * @param addr to lookup
         * @param link_key (out)
         * @param type (out)
         * @return 1 on success
         */
        int  (*get_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t * type);
    
        /**
         * @brief Update/Store Link key
         * @brief addr
         * @brief link_key
         * @brief type of link key
         */
        void (*put_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t type);
    
        /**
         * @brief Delete Link Keys
         * @brief addr
         */
        void (*delete_link_key)(bd_addr_t bd_addr);
    
        // iterator: it's allowed to delete 
    
        /**
         * @brief Setup iterator
         * @param it
         * @return 1 on success
         */
        int (*iterator_init)(btstack_link_key_iterator_t * it);
    
        /**
         * @brief Get next Link Key
         * @param it
         * @brief addr
         * @brief link_key
         * @brief type of link key
         * @return 1, if valid link key found
         */
        int  (*iterator_get_next)(btstack_link_key_iterator_t * it, bd_addr_t bd_addr, link_key_t link_key, link_key_type_t * type);
    
        /**
         * @brief Frees resources allocated by iterator_init
         * @note Must be called after iteration to free resources
         * @param it
         */
        void (*iterator_done)(btstack_link_key_iterator_t * it);
    
    } btstack_link_key_db_t;
    

## In-Memory Link Key Storage API {#sec:btstack_link_key_db_memory_api}


**btstack_link_key_db_memory.h** 


    
    /*
     * @brief
     */
    const btstack_link_key_db_t * btstack_link_key_db_memory_instance(void);
    
    typedef struct {
        btstack_linked_item_t item;
        bd_addr_t bd_addr;
        link_key_t link_key;
        link_key_type_t link_key_type;
    } btstack_link_key_db_memory_entry_t;
    

## Static Link Key Storage API {#sec:btstack_link_key_db_static_api}


**btstack_link_key_db_static.h** : 
Static Link Key Storage implementation to use during development/porting:
- Link keys have to be manually added to this file to make them usable
+ Link keys are preserved on reflash in constrast to the program flash based link key store



    
    /*
     * @brief
     */
    const btstack_link_key_db_t * btstack_link_key_db_static_instance(void);
    

## Link Key TLV Storage API {#sec:btstack_link_key_db_tlv_api}


**btstack_link_key_db_tlv.h** : 
Interface to provide link key storage via BTstack's TLV storage.

@TLV 'BLT' x


    
    /**
     * Init Link Key DB using TLV
     * @param btstack_tlv_impl of btstack_tlv interface
     * @Param btstack_tlv_context of btstack_tlv_interface
     */
    const btstack_link_key_db_t * btstack_link_key_db_tlv_get_instance(const btstack_tlv_t * btstack_tlv_impl, void * btstack_tlv_context);
    

## Device ID Server API {#sec:device_id_server_api}


**device_id_server.h** : 
Create Device ID SDP Records.



    
    /**
     * @brief Create SDP record for Device ID service
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param vendor_id_source usually DEVICE_ID_VENDOR_ID_SOURCE_BLUETOOTH or DEVICE_ID_VENDOR_ID_SOURCE_USB
     * @param vendor_id
     * @param product_it
     * @param version
     */
    void device_id_create_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t vendor_id_source, uint16_t vendor_id, uint16_t product_id, uint16_t version);
    

## GATT SDP API {#sec:gatt_sdp_api}


**gatt_sdp.h** 


    
    /** 
     * @brief Creates SDP record forG ATT service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param gatt_start_handle
     * @param gatt_end_handle
     */
    void gatt_create_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t gatt_start_handle, uint16_t gatt_end_handle);
    

## GOEP Client API {#sec:goep_client_api}


**goep_client.h** : 
Communicate with remote OBEX server - General Object Exchange



    
    typedef struct {
        btstack_linked_item_t item;
    
        uint16_t         cid;
    
        goep_client_state_t     state;
        bd_addr_t        bd_addr;
        uint16_t         uuid;
        hci_con_handle_t con_handle;
        uint8_t          incoming;
    
        btstack_packet_handler_t client_handler;
        btstack_context_callback_registration_t sdp_query_request;
    
        uint8_t          rfcomm_port;
        uint16_t         l2cap_psm;
        uint16_t         bearer_cid;
        uint16_t         bearer_mtu;
    
        uint16_t         record_index;
    
        // cached higher layer information PBAP + MAP
        uint32_t         profile_supported_features;
        uint8_t          map_mas_instance_id;
        uint8_t          map_supported_message_types;
        uint16_t         map_version;
    
        // needed to select one of multiple MAS Instances
        struct {
            uint32_t supported_features;
            uint8_t  instance_id;
            uint8_t  supported_message_types;
            uint8_t  rfcomm_port;
            uint16_t version;
    #ifdef ENABLE_GOEP_L2CAP
            uint16_t l2cap_psm;
    #endif
        } mas_info;
    
        uint8_t          obex_opcode;
        uint32_t         obex_connection_id;
        int              obex_connection_id_set;
    
    #ifdef ENABLE_GOEP_L2CAP
        l2cap_ertm_config_t ertm_config;
        uint16_t            ertm_buffer_size;
        uint8_t           * ertm_buffer;
    #endif
    } goep_client_t;
    
    
    // remote does not expose PBAP features in SDP record
    #define PBAP_FEATURES_NOT_PRESENT ((uint32_t) -1)
    #define MAP_FEATURES_NOT_PRESENT ((uint32_t) -1)
    #define PROFILE_FEATURES_NOT_PRESENT ((uint32_t) -1)
    
    /**
     * Setup GOEP Client
     */
    void goep_client_init(void);
    
    /**
     * @brief Connect to a GEOP server with specified UUID on a remote device.
     * @param goep_client
     * @param l2cap_ertm_config
     * @param l2cap_ertm_buffer_size
     * @param l2cap_ertm_buffer
     * @param handler
     * @param addr
     * @param uuid
     * @param instance_id e.g. used to connect to different MAP Access Servers, default: 0
     * @param out_cid
     * @return
     */
    uint8_t goep_client_connect(goep_client_t *goep_client, l2cap_ertm_config_t *l2cap_ertm_config, uint8_t *l2cap_ertm_buffer,
                        uint16_t l2cap_ertm_buffer_size, btstack_packet_handler_t handler, bd_addr_t addr, uint16_t uuid,
                        uint8_t instance_id, uint16_t *out_cid);
    
    /**
     * @brief Connect to a GEOP server over L2CAP with specified PSM on a remote device.
     * @note In contrast to goep_client_connect which searches for a OBEX service with a given UUID, this
     *       function only supports GOEP v2.0 (L2CAP) to a specified L2CAP PSM
     * @param goep_client
     * @param l2cap_ertm_config
     * @param l2cap_ertm_buffer_size
     * @param l2cap_ertm_buffer
     * @param handler
     * @param addr
     * @param l2cap_psm
     * @param out_cid
     * @return
     */
    uint8_t goep_client_connect_l2cap(goep_client_t *goep_client, l2cap_ertm_config_t *l2cap_ertm_config, uint8_t *l2cap_ertm_buffer,
                        uint16_t l2cap_ertm_buffer_size, btstack_packet_handler_t handler, bd_addr_t addr, uint16_t l2cap_psm,
                        uint16_t *out_cid);
    
    /** 
     * @brief Disconnects GOEP connection with given identifier.
     * @param goep_cid
     * @return status
     */
    uint8_t goep_client_disconnect(uint16_t goep_cid);
    
    /** 
     * @brief Request emission of GOEP_SUBEVENT_CAN_SEND_NOW as soon as possible
     * @note GOEP_SUBEVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param goep_cid
     */
    void goep_client_request_can_send_now(uint16_t goep_cid);
    
    /**
     * @brief Get Opcode from last created request, needed for parsing of OBEX response packet
     * @param goep_cid
     * @return opcode
     */
    uint8_t goep_client_get_request_opcode(uint16_t goep_cid);
    
    /**
     * @brief Get PBAP Supported Features found in SDP record during connect
     */
    uint32_t goep_client_get_pbap_supported_features(uint16_t goep_cid);
    
    /**
     * @brief Get MAP Supported Features found in SDP record during connect
     */
    uint32_t goep_client_get_map_supported_features(uint16_t goep_cid);
    
    /**
     * @brief Get MAP MAS Instance ID found in SDP record during connect
     */
    uint8_t goep_client_get_map_mas_instance_id(uint16_t goep_cid);
    
    /**
     * @brief Get MAP MAS Supported Message Types found in SDP record during connect
     */
    uint8_t goep_client_get_map_supported_message_types(uint16_t goep_cid);
    
    /**
     * @brief Check if GOEP 2.0 or higher features can be used
     * @return true if GOEP Version 2.0 or higher
     */
    bool goep_client_version_20_or_higher(uint16_t goep_cid);
    
    /**
     * @brief Set Connection ID used for newly created requests
     * @param goep_cid
     */
    void goep_client_set_connection_id(uint16_t goep_cid, uint32_t connection_id);
    
    /**
     * @brief Start Connect request
     * @param goep_cid
     * @param obex_version_number
     * @param flags
     * @param maximum_obex_packet_length
     */
    void goep_client_request_create_connect(uint16_t goep_cid, uint8_t obex_version_number, uint8_t flags, uint16_t maximum_obex_packet_length);
    
    /**
     * @brief Start Disconnect request
     * @param goep_cid
     */
    void goep_client_request_create_disconnect(uint16_t goep_cid);
    
    /**
     * @brief Create Get request
     * @param goep_cid
     */
    void goep_client_request_create_get(uint16_t goep_cid);
    
    /**
     * @brief Create Abort request
     * @param goep_cid
     */
    void goep_client_request_create_abort(uint16_t goep_cid);
    
    /**
     * @brief Start Set Path request
     * @param goep_cid
     */
    void goep_client_request_create_set_path(uint16_t goep_cid, uint8_t flags);
    
    /**
     * @brief Create Put request
     * @param goep_cid
     */
    void goep_client_request_create_put(uint16_t goep_cid);
    
    /**
     * @brief Get max size of body data that can be added to current response with goep_client_body_add_static
     * @param goep_cid
     * @param data
     * @param length
     * @return size in bytes or 0
     */
    uint16_t goep_client_request_get_max_body_size(uint16_t goep_cid);
    
    /**
     * @brief Add SRM Enable
     * @param goep_cid
     */
    void goep_client_header_add_srm_enable(uint16_t goep_cid);
    
    /**
     * @brief Add SRMP Waiting
     * @param goep_cid
     */
    void goep_client_header_add_srmp_waiting(uint16_t goep_cid);
    
    /**
     * @brief Add header with single byte value (8 bit)
     * @param goep_cid
     * @param header_type
     * @param value
     */
    void goep_client_header_add_byte(uint16_t goep_cid, uint8_t header_type, uint8_t value);
    
    /**
     * @brief Add header with word value (32 bit)
     * @param goep_cid
     * @param header_type
     * @param value
     */
    void goep_client_header_add_word(uint16_t goep_cid, uint8_t header_type, uint32_t value);
    
    /**
     * @brief Add header with variable size
     * @param goep_cid
     * @param header_type
     * @param header_data
     * @param header_data_length
     */
    void goep_client_header_add_variable(uint16_t goep_cid, uint8_t header_type, const uint8_t * header_data, uint16_t header_data_length);
    
    /**
     * @brief Add name header to current request
     * @param goep_cid
     * @param name
     */
    void goep_client_header_add_name(uint16_t goep_cid, const char * name);
    
    /**
     * @brief Add name header to current request
     * @param goep_cid
     * @param name
     * @param name_len
     */
    void goep_client_header_add_name_prefix(uint16_t goep_cid, const char * name, uint16_t name_len);
    
    /**
     * @brief Add string encoded as unicode to current request
     * @param goep_cid
     * @param name
     * @param name_len
     */
    void goep_client_header_add_unicode_prefix(uint16_t goep_cid, uint8_t header_id, const char * name, uint16_t name_len);
    
    /**
     * @brief Add target header to current request
     * @param goep_cid
     * @param target
     * @param length of target
     */
    void goep_client_header_add_target(uint16_t goep_cid, const uint8_t * target, uint16_t length);
    
    /**
     * @brief Add type header to current request
     * @param goep_cid
     * @param type
     */
    void goep_client_header_add_type(uint16_t goep_cid, const char * type);
    
    /**
     * @brief Add count header to current request
     * @param goep_cid
     * @param count
     */
    void goep_client_header_add_count(uint16_t goep_cid, uint32_t count);
    
    /**
     * @brief Add length header to current request
     * @param goep_cid
     * @param length
     */
    void goep_client_header_add_length(uint16_t goep_cid, uint32_t length);
    
    /**
     * @brief Add application parameters header to current request
     * @param goep_cid
     * @param data 
     * @param lenght of application parameters
     */
    void goep_client_header_add_application_parameters(uint16_t goep_cid, const uint8_t * data, uint16_t length);
    
    /**
     * @brief Add application parameters header to current request
     * @param goep_cid
     * @param data
     * @param lenght of challenge response
     */
    void goep_client_header_add_challenge_response(uint16_t goep_cid, const uint8_t * data, uint16_t length);
    
    /**
     * @brief Add body
     * @param goep_cid
     * @param data
     * @param lenght 
     */
    void goep_client_body_add_static(uint16_t goep_cid, const uint8_t * data, uint32_t length);
    
    /**
     * @brief Query remaining buffer size
     * @param goep_cid
     * @return size
     */
    uint16_t goep_client_body_get_outgoing_buffer_len(uint16_t goep_cid);
    
    /**
     * @brief Add body
     * @param goep_cid
     * @param data
     * @param length
     * @param ret_length
     */
    void goep_client_body_fillup_static(uint16_t goep_cid, const uint8_t * data, uint32_t length, uint32_t * ret_length);
    
    /**
     * @brief Execute prepared request
     * @param goep_cid
     * @param daa 
     */
    int goep_client_execute(uint16_t goep_cid);
    
    /**
     * @brief Execute prepared request with final bit
     * @param goep_cid
     * @param final
     */
    int goep_client_execute_with_final_bit(uint16_t goep_cid, bool final);
    
    /**
     * @brief De-Init GOEP Client
     */
    void goep_client_deinit(void);
    

## HFP Audio Gateway (AG) API {#sec:hfp_ag_api}


**hfp_ag.h** 


    typedef struct {
        uint8_t type;
        const char * number;
        hfp_phone_service_t service;
    } hfp_phone_number_t;
    
    /**
     * @brief Create HFP Audio Gateway (AG) SDP service record. 
     * @param service
     * @param rfcomm_channel_nr
     * @param name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param ability_to_reject_call
     * @param supported_features 32-bit bitmap, see HFP_AGSF_* values in hfp.h
     * @param codecs_nr
     * @param codecs
     */
    void hfp_ag_create_sdp_record_with_codecs(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr,
                                              const char * name, uint8_t ability_to_reject_call, uint16_t supported_features,
                                              uint8_t codecs_nr, const uint8_t * codecs);
    
    /**
     * @brief Set up HFP Audio Gateway (AG) device without additional supported features.
     * @param rfcomm_channel_nr
     */
    void hfp_ag_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Set codecs. 
     * @param codecs_nr
     * @param codecs
     */
    void hfp_ag_init_codecs(uint8_t codecs_nr, const uint8_t * codecs);
    
    /**
     * @brief Set supported features.
     * @param supported_features 32-bit bitmap, see HFP_AGSF_* values in hfp.h
     */
    void hfp_ag_init_supported_features(uint32_t supported_features);
    
    /**
     * @brief Set AG indicators. 
     * @param indicators_nr
     * @param indicators
     */
    void hfp_ag_init_ag_indicators(int ag_indicators_nr, const hfp_ag_indicator_t * ag_indicators);
    
    /**
     * @brief Set HF indicators. 
     * @param indicators_nr
     * @param indicators
     */
    void hfp_ag_init_hf_indicators(int hf_indicators_nr, const hfp_generic_status_indicator_t * hf_indicators);
    
    /**
     * @brief Set Call Hold services. 
     * @param indicators_nr
     * @param indicators
     */
    void hfp_ag_init_call_hold_services(int call_hold_services_nr, const char * call_hold_services[]);
    
    /**
     * @brief Provide Apple Device information upon AT+XAPL
     * @param device
     * @param features
     */
    void hfp_ag_init_apple_identification(const char * device, uint8_t features);
    
    
    /**
     * @brief Register callback for the HFP Audio Gateway (AG) client. 
     * @param callback
     */
    void hfp_ag_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Register custom AT command.
     * @param hfp_custom_at_command (with 'AT+' prefix)
     */
    void hfp_ag_register_custom_at_command(hfp_custom_at_command_t * custom_at_command);
    
    /**
     * @brief Enable/Disable in-band ring tone.
     *
     * @param use_in_band_ring_tone
     */
    void hfp_ag_set_use_in_band_ring_tone(int use_in_band_ring_tone);
    
    
    // actions used by local device / user
    
    /**
     * @brief Establish RFCOMM connection, and perform service level connection agreement:
     * - exchange of supported features
     * - report Audio Gateway (AG) indicators and their status 
     * - enable indicator status update in the AG
     * - accept the information about available codecs in the Hands-Free (HF), if sent
     * - report own information describing the call hold and multiparty services, if possible
     * - report which HF indicators are enabled on the AG, if possible
     * The status of SLC connection establishment is reported via
     * HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED.
     *
     * @param  bd_addr of HF
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *                  - ERROR_CODE_COMMAND_DISALLOWED if connection already exists or connection in wrong state, or
     *                  - BTSTACK_MEMORY_ALLOC_FAILED 
     *
     */
    uint8_t hfp_ag_establish_service_level_connection(bd_addr_t bd_addr);
    
    /**
     * @brief Release the RFCOMM channel and the audio connection between the HF and the AG. 
     * If the audio connection exists, it will be released.
     * The status of releasing the SLC connection is reported via
     * HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_release_service_level_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Establish audio connection.
     * The status of Audio connection establishment is reported via HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_establish_audio_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Release audio connection.
     * The status of releasing the Audio connection is reported via HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_release_audio_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Put the current call on hold, if it exists, and accept incoming call. 
     */
    void hfp_ag_answer_incoming_call(void);
    
    /**
     * @brief Join held call with active call.
     */
    void hfp_ag_join_held_call(void);
    
    /**
     * @brief Reject incoming call, if exists, or terminate active call.
     */
    void hfp_ag_terminate_call(void);
    
    /**
     * @brief Put incoming call on hold.
     */
    void hfp_ag_hold_incoming_call(void);
    
    /**
     * @brief Accept the held incoming call.
     */
    void hfp_ag_accept_held_incoming_call(void);
    
    /**
     * @brief Reject the held incoming call.
     */
    void hfp_ag_reject_held_incoming_call(void);
    
    /**
     * @brief Set microphone gain. 
     *
     * @param acl_handle
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid gain range
     */
    uint8_t hfp_ag_set_microphone_gain(hci_con_handle_t acl_handle, int gain);
    
    /**
     * @brief Set speaker gain.
     *
     * @param acl_handle
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid gain range
     */
    uint8_t hfp_ag_set_speaker_gain(hci_con_handle_t acl_handle, int gain);
    
    /**
     * @brief Set battery level.
     *
     * @param battery_level Valid range: [0,5]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid battery level range
     */
    uint8_t hfp_ag_set_battery_level(int battery_level);
    
    /**
     * @brief Clear last dialed number.
     */
    void hfp_ag_clear_last_dialed_number(void);
    
    /**
     * @brief Set last dialed number.
     */
    void hfp_ag_set_last_dialed_number(const char * number);
    
    /**
     * @brief Notify the HF that an incoming call is waiting 
     * during an ongoing call. The notification will be sent only if the HF has
     * has previously enabled the "Call Waiting notification" in the AG. 
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if call waiting notification is not enabled
     */
    uint8_t hfp_ag_notify_incoming_call_waiting(hci_con_handle_t acl_handle);
    
    // Voice Recognition
    
    /**
     * @brief Activate voice recognition and emit HFP_SUBEVENT_VOICE_RECOGNITION_ACTIVATED event with status ERROR_CODE_SUCCESS 
     * if successful. Prerequisite is established SLC.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_VOICE_RECOGNITION_FUNCTION is not supported by HF and AG, or already activated
     */
    uint8_t hfp_ag_activate_voice_recognition(hci_con_handle_t acl_handle);
    
    /**
     * @brief Deactivate voice recognition and emit HFP_SUBEVENT_VOICE_RECOGNITION_DEACTIVATED event with status ERROR_CODE_SUCCESS 
     * if successful. Prerequisite is established SLC.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_VOICE_RECOGNITION_FUNCTION is not supported by HF and AG, or already deactivated
     */
    uint8_t hfp_ag_deactivate_voice_recognition(hci_con_handle_t acl_handle);
    
    /**
     * @brief Notify HF that sound will be played and HFP_SUBEVENT_ENHANCED_VOICE_RECOGNITION_AG_IS_STARTING_SOUND event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED.
     *
     * @param acl_handle
     * @param activate
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_ENHANCED_VOICE_RECOGNITION_STATUS is not supported by HF and AG
     */
    uint8_t hfp_ag_enhanced_voice_recognition_report_sending_audio(hci_con_handle_t acl_handle);
    
    /**
     * @brief Notify HF that AG is ready for input and emit HFP_SUBEVENT_ENHANCED_VOICE_RECOGNITION_AG_READY_TO_ACCEPT_AUDIO_INPUT event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED.
     *
     * @param acl_handle
     * @param activate
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_ENHANCED_VOICE_RECOGNITION_STATUS is not supported by HF and AG
     */
    uint8_t hfp_ag_enhanced_voice_recognition_report_ready_for_audio(hci_con_handle_t acl_handle);
    
    /**
     * @brief Notify that AG is processing input and emit HFP_SUBEVENT_ENHANCED_VOICE_RECOGNITION_AG_IS_PROCESSING_AUDIO_INPUT event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED.
     *
     * @param acl_handle
     * @param activate
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_ENHANCED_VOICE_RECOGNITION_STATUS is not supported by HF and AG
     */
    uint8_t hfp_ag_enhanced_voice_recognition_report_processing_input(hci_con_handle_t acl_handle);
    
    /**
     * @brief Send enhanced audio recognition message and HFP_SUBEVENT_ENHANCED_VOICE_RECOGNITION_AG_MESSAGE_SENT event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED.
     *
     * @param acl_handle
     * @param activate
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, 
                    - ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if the message size exceeds the HFP_MAX_VR_TEXT_SIZE, or the command does not fit into a single packet frame,
     *              - ERROR_CODE_COMMAND_DISALLOWED if HF and AG do not support features: HFP_(HF/AG)SF_ENHANCED_VOICE_RECOGNITION_STATUS and HFP_(HF/AG)SF_VOICE_RECOGNITION_TEXT
     */
    uint8_t hfp_ag_enhanced_voice_recognition_send_message(hci_con_handle_t acl_handle, hfp_voice_recognition_state_t state, hfp_voice_recognition_message_t msg);
    
    /**
     * @brief Send a phone number back to the HF.
     *
     * @param acl_handle
     * @param phone_number
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_send_phone_number_for_voice_tag(hci_con_handle_t acl_handle, const char * phone_number);
    
    /**
     * @brief Reject sending a phone number to the HF.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_reject_phone_number_for_voice_tag(hci_con_handle_t acl_handle);
    
    /**
     * @brief Store phone number with initiated call.
     * @param type
     * @param number
     */
    void hfp_ag_set_clip(uint8_t type, const char * number);
    
    
    // Cellular Actions
    
    /**
     * @brief Pass the accept incoming call event to the AG.
     */
    void hfp_ag_incoming_call(void);
    
    /**
     * @brief Outgoing call initiated
     */
    void hfp_ag_outgoing_call_initiated(void);
    
    /**
     * @brief Pass the reject outgoing call event to the AG.
     */
    void hfp_ag_outgoing_call_rejected(void);
    
    /**
     * @brief Pass the accept outgoing call event to the AG.
     */
    void hfp_ag_outgoing_call_accepted(void);
    
    /**
     * @brief Pass the outgoing call ringing event to the AG.
     */
    void hfp_ag_outgoing_call_ringing(void);
    
    /**
     * @brief Pass the outgoing call established event to the AG.
     */
    void hfp_ag_outgoing_call_established(void);
    
    /**
     * @brief Pass the call dropped event to the AG.
     */
    void hfp_ag_call_dropped(void);
    
    /**
     * @brief Set network registration status.  
     * @param status 0 - not registered, 1 - registered 
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid registration status
     */
    uint8_t hfp_ag_set_registration_status(int registration_status);
    
    /**
     * @brief Set network signal strength.
     *
     * @param signal_strength [0-5]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid signal strength range
     */
    uint8_t hfp_ag_set_signal_strength(int signal_strength);
    
    /**
     * @brief Set roaming status.
     *
     * @param roaming_status 0 - no roaming, 1 - roaming active
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid roaming status
     */
    uint8_t hfp_ag_set_roaming_status(int roaming_status);
    
    /**
     * @brief Set subscriber number information, e.g. the phone number
     * @param numbers
     * @param numbers_count
     */
    void hfp_ag_set_subscriber_number_information(hfp_phone_number_t * numbers, int numbers_count);
    
    /**
     * @brief Called by cellular unit after a DTMF code was transmitted, so that the next one can be emitted.
     *
     * @param acl_handle 
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_ag_send_dtmf_code_done(hci_con_handle_t acl_handle);
    
    /**
     * @brief Report Extended Audio Gateway Error result codes in the AG.
     * Whenever there is an error relating to the functionality of the AG as a 
     * result of AT command, the AG shall send +CME ERROR:
     * - +CME ERROR: 0  - AG failure
     * - +CME ERROR: 1  - no connection to phone 
     * - +CME ERROR: 3  - operation not allowed 
     * - +CME ERROR: 4  - operation not supported 
     * - +CME ERROR: 5  - PH-SIM PIN required 
     * - +CME ERROR: 10 - SIM not inserted 
     * - +CME ERROR: 11 - SIM PIN required 
     * - +CME ERROR: 12 - SIM PUK required 
     * - +CME ERROR: 13 - SIM failure
     * - +CME ERROR: 14 - SIM busy
     * - +CME ERROR: 16 - incorrect password 
     * - +CME ERROR: 17 - SIM PIN2 required 
     * - +CME ERROR: 18 - SIM PUK2 required 
     * - +CME ERROR: 20 - memory full
     * - +CME ERROR: 21 - invalid index
     * - +CME ERROR: 23 - memory failure
     * - +CME ERROR: 24 - text string too long
     * - +CME ERROR: 25 - invalid characters in text string
     * - +CME ERROR: 26 - dial string too long
     * - +CME ERROR: 27 - invalid characters in dial string
     * - +CME ERROR: 30 - no network service
     * - +CME ERROR: 31 - network Timeout.
     * - +CME ERROR: 32 - network not allowed - Emergency calls only
     *
     * @param acl_handle
     * @param error
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if extended audio gateway error report is disabled
     */
    uint8_t hfp_ag_report_extended_audio_gateway_error_result_code(hci_con_handle_t acl_handle, hfp_cme_error_t error);
    
    /**
     * @brief Send unsolicited result code (most likely a response to a vendor-specific command not part of standard HFP).
     * @note  Emits HFP_SUBEVENT_CUSTOM_AT_MESSAGE_SENT when result code was sent
     *
     * @param unsolicited_result_code to send
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if extended audio gateway error report is disabled
     */
    uint8_t hfp_ag_send_unsolicited_result_code(hci_con_handle_t acl_handle, const char * unsolicited_result_code);
    
    /**
     * @brief Send result code for AT command received via HFP_SUBEVENT_CUSTOM_AT_COMMAND
     * @note  Emits HFP_SUBEVENT_COMPLETE when result code was sent
     *
     * @param ok for OK, or ERROR
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if extended audio gateway error report is disabled
     */
    uint8_t hfp_ag_send_command_result_code(hci_con_handle_t acl_handle, bool ok);
    
    /**
     * @brief De-Init HFP AG
     */
    void hfp_ag_deinit(void);
    
    /**
     * @brief Create HFP Audio Gateway (AG) SDP service record.
     * @deprecated Use hfp_ag_create_sdp_record_with_codecs instead
     * @param service
     * @param rfcomm_channel_nr
     * @param name
     * @param ability_to_reject_call
     * @param supported_features 32-bit bitmap, see HFP_AGSF_* values in hfp.h
     * @param wide_band_speech supported
     */
    void hfp_ag_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint8_t ability_to_reject_call, uint16_t supported_features, int wide_band_speech);
    

## HFP Audio Encoder API {#sec:hfp_codec_api}


**hfp_codec.h** : @brief Create SCO packet with H2 Synchronization Header and encoded audio samples


    
    typedef struct hfp_codec hfp_codec_t;
    
    #ifdef ENABLE_HFP_WIDE_BAND_SPEECH
    /**
     * @brief Initialize HFP Audio Codec for mSBC
     * @param hfp_codec
     * @param msbc_encoder_context for msbc encoder
     * @return status
     */
    void hfp_codec_init_msbc_with_codec(hfp_codec_t * hfp_codec, const btstack_sbc_encoder_t * sbc_encoder, void * sbc_encoder_context);
    
    /**
     * @brief Initialize HFP Audio Codec for mSBC
     * @deprecated Please use hfp_codec_init_msbc_with_codec
     * @param hfp_codec
     * @param msbc_encoder_context for msbc encoder
     * @return status
     */
    void hfp_codec_init_msbc(hfp_codec_t * hfp_codec, btstack_sbc_encoder_state_t * msbc_encoder_context);
    #endif
    
    #ifdef ENABLE_HFP_SUPER_WIDE_BAND_SPEECH
    /**
     * @brief Initialize HFP Audio Codec for LC3-SWB
     * @param hfp_codec
     * @param lc3_encoder
     * @param lc3_encoder_context for lc3_encoder
     * @return status
     */
    void hfp_codec_init_lc3_swb(hfp_codec_t * hfp_codec, const btstack_lc3_encoder_t * lc3_encoder, void * lc3_encoder_context);
    #endif
    
    /**
     * @brief Get number of audio samples per HFP SCO frame
     * @param hfp_codec
     * @return num audio samples per 7.5ms SCO packet
     */
    uint16_t hfp_codec_num_audio_samples_per_frame(const hfp_codec_t * hfp_codec);
    
    /**
     * @brief Checks if next frame can be encoded
     * @param hfp_codec
     */
    bool hfp_codec_can_encode_audio_frame_now(const hfp_codec_t * hfp_codec);
    
    /**
     * Get number of bytes ready for sending
     * @param hfp_codec
     * @return num bytes ready for current packet
     */
    uint16_t hfp_codec_num_bytes_available(const hfp_codec_t * hfp_codec);
    
    /**
     * Encode audio samples for HFP SCO packet
     * @param hfp_codec
     * @param pcm_samples - complete audio frame of hfp_msbc_num_audio_samples_per_frame int16 samples
     */
    void hfp_codec_encode_audio_frame(hfp_codec_t * hfp_codec, int16_t * pcm_samples);
    
    /**
     * Read from stream into SCO packet buffer
     * @param hfp_codec
     * @param buffer to store stream
     * @param size num bytes to read from stream
     */
    void hfp_codec_read_from_stream(hfp_codec_t * hfp_codec, uint8_t * buffer, uint16_t size);
    
    /**
     * @param hfp_codec
     */
    void hfp_codec_deinit(hfp_codec_t * hfp_codec);
    

## HFP GSM Model API {#sec:hfp_gsm_model_api}


**hfp_gsm_model.h** 


    typedef struct {
        bool used_slot;
        hfp_enhanced_call_status_t enhanced_status;
        hfp_enhanced_call_dir_t direction;
        hfp_enhanced_call_mode_t mode;
        hfp_enhanced_call_mpty_t mpty;
        // TODO: sort on drop call, so that index corresponds to table index
        int index;
        uint8_t clip_type;
        char    clip_number[25];
    } hfp_gsm_call_t;
    
    hfp_callheld_status_t hfp_gsm_callheld_status(void);
    hfp_call_status_t hfp_gsm_call_status(void);
    hfp_callsetup_status_t hfp_gsm_callsetup_status(void);
    
    int hfp_gsm_get_number_of_calls(void);
    char * hfp_gsm_last_dialed_number(void);
    void hfp_gsm_clear_last_dialed_number(void);
    void hfp_gsm_set_last_dialed_number(const char* number);
    
    hfp_gsm_call_t * hfp_gsm_call(int index);
    
    int hfp_gsm_call_possible(void);
    
    uint8_t hfp_gsm_clip_type(void);
    char *  hfp_gsm_clip_number(void);
    
    void hfp_gsm_init(void);
    void hfp_gsm_deinit(void);
    
    void hfp_gsm_handler(hfp_ag_call_event_t event, uint8_t index, uint8_t type, const char * number);
    
    

## HFP Hands-Free (HF) API {#sec:hfp_hf_api}


**hfp_hf.h** 


    
    /**
     * @brief Create HFP Hands-Free (HF) SDP service record.
     * @param service
     * @param rfcomm_channel_nr
     * @param name or NULL for default value. Provide "" (empty string) to skip attribute
     * @param supported_features 32-bit bitmap, see HFP_HFSF_* values in hfp.h
     * @param codecs_nr  number of codecs in codecs argument
     * @param codecs
     */
    void hfp_hf_create_sdp_record_with_codecs(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr,
                                               const char * name, uint16_t supported_features, uint8_t codecs_nr, const uint8_t * codecs);
    
    /**
     * @brief Set up HFP Hands-Free (HF) device without additional supported features. 
     * @param rfcomm_channel_nr
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *                  - L2CAP_SERVICE_ALREADY_REGISTERED, 
     *                  - RFCOMM_SERVICE_ALREADY_REGISTERED or 
     *                  - BTSTACK_MEMORY_ALLOC_FAILED if allocation of any of RFCOMM or L2CAP services failed 
     */
    uint8_t hfp_hf_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Set codecs. 
     * @param codecs_nr  number of codecs in codecs argument
     * @param codecs
     */
    void hfp_hf_init_codecs(uint8_t codecs_nr, const uint8_t * codecs);
    
    /**
     * @brief Set supported features.
     * @param supported_features 32-bit bitmap, see HFP_HFSF_* values in hfp.h
     */
    void hfp_hf_init_supported_features(uint32_t supported_features);
    
    /**
     * @brief Set HF indicators. 
     * @param indicators_nr
     * @param indicators
     */
    void hfp_hf_init_hf_indicators(int indicators_nr, const uint16_t * indicators);
    
    
    /**
     * @brief Register callback for the HFP Hands-Free (HF) client. 
     * @param callback
     */
    void hfp_hf_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Set microphone gain used during SLC for Volume Synchronization.
     *
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS if invalid gain range
     */
    uint8_t hfp_hf_set_default_microphone_gain(uint8_t gain);
    
    /**
     * @brief Set speaker gain used during SLC for Volume Synchronization.
     *
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS if invalid gain range
     */
    uint8_t hfp_hf_set_default_speaker_gain(uint8_t gain);
    
    /**
     * @brief Establish RFCOMM connection with the AG with given Bluetooth address, 
     * and perform service level connection (SLC) agreement:
     * - exchange supported features
     * - retrieve Audio Gateway (AG) indicators and their status 
     * - enable indicator status update in the AG
     * - notify the AG about its own available codecs, if possible
     * - retrieve the AG information describing the call hold and multiparty services, if possible
     * - retrieve which HF indicators are enabled on the AG, if possible
     * The status of SLC connection establishment is reported via
     * HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED.
     *
     * @param bd_addr Bluetooth address of the AG
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_COMMAND_DISALLOWED if connection already exists, or
     *              - BTSTACK_MEMORY_ALLOC_FAILED 
     */
    uint8_t hfp_hf_establish_service_level_connection(bd_addr_t bd_addr);
    
    /**
     * @brief Release the RFCOMM channel and the audio connection between the HF and the AG. 
     * The status of releasing the SLC connection is reported via
     * HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_release_service_level_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Enable status update for all indicators in the AG.
     * The status field of the HFP_SUBEVENT_COMPLETE reports if the command was accepted.
     * The status of an AG indicator is reported via HFP_SUBEVENT_AG_INDICATOR_STATUS_CHANGED.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_enable_status_update_for_all_ag_indicators(hci_con_handle_t acl_handle);
    
    /**
     * @brief Disable status update for all indicators in the AG.
     * The status field of the HFP_SUBEVENT_COMPLETE reports if the command was accepted.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_disable_status_update_for_all_ag_indicators(hci_con_handle_t acl_handle);
    
    /**
     * @brief Enable or disable status update for the individual indicators in the AG using bitmap.
     * The status field of the HFP_SUBEVENT_COMPLETE reports if the command was accepted.
     * The status of an AG indicator is reported via HFP_SUBEVENT_AG_INDICATOR_STATUS_CHANGED.
     * 
     * @param acl_handle
     * @param indicators_status_bitmap 32-bit bitmap, 0 - indicator is disabled, 1 - indicator is enabled
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_set_status_update_for_individual_ag_indicators(hci_con_handle_t acl_handle, uint32_t indicators_status_bitmap);
    
    /**
     * @brief Query the name of the currently selected Network operator by AG. 
     * 
     * The name is restricted to max 16 characters. The result is reported via 
     * HFP_SUBEVENT_NETWORK_OPERATOR_CHANGED subtype 
     * containing network operator mode, format and name.
     * If no operator is selected, format and operator are omitted.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if connection in wrong state
     */
    uint8_t hfp_hf_query_operator_selection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Enable Extended Audio Gateway Error result codes in the AG.
     * Whenever there is an error relating to the functionality of the AG as a 
     * result of AT command, the AG shall send +CME ERROR. This error is reported via 
     * HFP_SUBEVENT_EXTENDED_AUDIO_GATEWAY_ERROR, see hfp_cme_error_t in hfp.h
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_enable_report_extended_audio_gateway_error_result_code(hci_con_handle_t acl_handle);
    
    /**
     * @brief Disable Extended Audio Gateway Error result codes in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
     uint8_t hfp_hf_disable_report_extended_audio_gateway_error_result_code(hci_con_handle_t acl_handle);
    
    /**
     * @brief Establish audio connection. 
     * The status of audio connection establishment is reported via HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if connection in wrong state
     */
    uint8_t hfp_hf_establish_audio_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Release audio connection.
     * The status of releasing of the audio connection is reported via
     * HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_release_audio_connection(hci_con_handle_t acl_handle);
    
    /**
     * @brief Answer incoming call.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if answering incoming call with wrong callsetup status
     */
    uint8_t hfp_hf_answer_incoming_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Reject incoming call.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_reject_incoming_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Release all held calls or sets User Determined User Busy (UDUB) for a waiting call.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_user_busy(hci_con_handle_t acl_handle);
    
    /**
     * @brief Release all active calls (if any exist) and accepts the other (held or waiting) call.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_end_active_and_accept_other(hci_con_handle_t acl_handle);
    
    /**
     * @brief Place all active calls (if any exist) on hold and accepts the other (held or waiting) call.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_swap_calls(hci_con_handle_t acl_handle);
    
    /**
     * @brief Add a held call to the conversation.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_join_held_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Connect the two calls and disconnects the subscriber from both calls (Explicit Call Transfer).
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_connect_calls(hci_con_handle_t acl_handle);
    
    /**
     * @brief Terminate an incoming or an outgoing call. HFP_SUBEVENT_CALL_TERMINATED is sent upon call termination.
     * 
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_terminate_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Terminate all held calls.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_terminate_held_calls(hci_con_handle_t acl_handle);
    
    /**
     * @brief Initiate outgoing voice call by providing the destination phone number to the AG. 
     *
     * @param acl_handle
     * @param number
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_dial_number(hci_con_handle_t acl_handle, char * number);
    
    /**
     * @brief Initiate outgoing voice call using the memory dialing feature of the AG.
     *
     * @param acl_handle
     * @param memory_id
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_dial_memory(hci_con_handle_t acl_handle, int memory_id);
    
    /**
     * @brief Initiate outgoing voice call by recalling the last number dialed by the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_redial_last_number(hci_con_handle_t acl_handle);
    
    /**
     * @brief Enable the "Call Waiting notification" function in the AG. 
     * The AG shall send the corresponding result code to the HF whenever 
     * an incoming call is waiting during an ongoing call. In that event,
     * the HFP_SUBEVENT_CALL_WAITING_NOTIFICATION is emitted.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_activate_call_waiting_notification(hci_con_handle_t acl_handle);
    
    /**
     * @brief Disable the "Call Waiting notification" function in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_deactivate_call_waiting_notification(hci_con_handle_t acl_handle);
    
    /**
     * @brief Enable the "Calling Line Identification notification" function in the AG. 
     * The AG shall issue the corresponding result code just after every RING indication,
     * when the HF is alerted in an incoming call. In that event,
     * the HFP_SUBEVENT_CALLING_LINE_INDETIFICATION_NOTIFICATION is emitted.
     * @param acl_handle
     */
    uint8_t hfp_hf_activate_calling_line_notification(hci_con_handle_t acl_handle);
    
    /**
     * @brief Disable the "Calling Line Identification notification" function in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_deactivate_calling_line_notification(hci_con_handle_t acl_handle);
    
    
    /**
     * @brief Deactivate echo canceling (EC) and noise reduction (NR) in the AG and emit 
     * HFP_SUBEVENT_ECHO_CANCELING_NOISE_REDUCTION_DEACTIVATE with status ERROR_CODE_SUCCESS if AG supports EC and AG.
     * If the AG supports its own embedded echo canceling and/or noise reduction function, 
     * it shall have EC and NR activated until this function is called.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or 
     *              - ERROR_CODE_COMMAND_DISALLOWED if HFP_(HF/AG)SF_EC_NR_FUNCTION feature is not supported by AG and HF
     */
    uint8_t hfp_hf_deactivate_echo_canceling_and_noise_reduction(hci_con_handle_t acl_handle);
    
    /**
     * @brief Activate voice recognition and emit HFP_SUBEVENT_VOICE_RECOGNITION_ACTIVATED event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED. Prerequisite is established SLC.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_VOICE_RECOGNITION_FUNCTION is not supported by HF and AG, or already activated
     */
    uint8_t hfp_hf_activate_voice_recognition(hci_con_handle_t acl_handle);
    
    /**
     * @brief Dectivate voice recognition and emit HFP_SUBEVENT_VOICE_RECOGNITION_DEACTIVATED event with status ERROR_CODE_SUCCESS 
     * if successful, otherwise ERROR_CODE_COMMAND_DISALLOWED. Prerequisite is established SLC.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_VOICE_RECOGNITION_FUNCTION is not supported by HF and AG, or already activated
     */
    uint8_t hfp_hf_deactivate_voice_recognition(hci_con_handle_t acl_handle);
    
    
    /**
     * @brief Indicate that the HF is ready to accept audio. Prerequisite is established voice recognition session.
     * The HF may call this function during an ongoing AVR (Audio Voice Recognition) session to terminate audio output from 
     * the AG (if there is any) and prepare the AG for new audio input.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_COMMAND_DISALLOWED if feature HFP_(HF/AG)SF_ENHANCED_VOICE_RECOGNITION_STATUS is not supported by HF and AG, or wrong VRA status
     */
    uint8_t hfp_hf_enhanced_voice_recognition_report_ready_for_audio(hci_con_handle_t acl_handle);
    
    
    /**
     * @brief Set microphone gain. 
     *
     * @param acl_handle
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid gain range
     */
    uint8_t hfp_hf_set_microphone_gain(hci_con_handle_t acl_handle, int gain);
    
    /**
     * @brief Set speaker gain.
     *
     * @param acl_handle
     * @param gain Valid range: [0,15]
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if invalid gain range
     */
    uint8_t hfp_hf_set_speaker_gain(hci_con_handle_t acl_handle, int gain);
    
    /**
     * @brief Instruct the AG to transmit a DTMF code.
     *
     * @param acl_handle
     * @param dtmf_code
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_send_dtmf_code(hci_con_handle_t acl_handle, char code);
    
    /**
     * @brief Read numbers from the AG for the purpose of creating 
     * a unique voice tag and storing the number and its linked voice
     * tag in the HF's memory. 
     * The number is reported via HFP_SUBEVENT_NUMBER_FOR_VOICE_TAG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_request_phone_number_for_voice_tag(hci_con_handle_t acl_handle);
    
    /**
     * @brief Query the list of current calls in AG. 
     * The result is received via HFP_SUBEVENT_ENHANCED_CALL_STATUS.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_query_current_call_status(hci_con_handle_t acl_handle);
    
    /**
     * @brief Release a call with index in the AG.
     *
     * @param acl_handle
     * @param index
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_release_call_with_index(hci_con_handle_t acl_handle, int index);
    
    /**
     * @brief Place all parties of a multiparty call on hold with the 
     * exception of the specified call.
     *
     * @param acl_handle
     * @param index
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_private_consultation_with_call(hci_con_handle_t acl_handle, int index);
    
    /**
     * @brief Query the status of the "Response and Hold" state of the AG.
     * The result is reported via HFP_SUBEVENT_RESPONSE_AND_HOLD_STATUS.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_rrh_query_status(hci_con_handle_t acl_handle);
    
    /**
     * @brief Put an incoming call on hold in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_rrh_hold_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Accept held incoming call in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_rrh_accept_held_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Reject held incoming call in the AG.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_rrh_reject_held_call(hci_con_handle_t acl_handle);
    
    /**
     * @brief Query the AG subscriber number. The result is reported via HFP_SUBEVENT_SUBSCRIBER_NUMBER_INFORMATION.
     *
     * @param acl_handle
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist
     */
    uint8_t hfp_hf_query_subscriber_number(hci_con_handle_t acl_handle);
    
    /**
     * @brief Set HF indicator.
     *
     * @param acl_handle
     * @param assigned_number
     * @param value
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if indicator is either not found or not enabled
     */
    uint8_t hfp_hf_set_hf_indicator(int assigned_number, int value);
    
    /**
     * @brief Tests if in-band ringtone is active on AG (requires SLC)
     *
     * @param acl_handler
     * @return 0 if unknown acl_handle or in-band ring-tone disabled, otherwise 1
     */
    int hfp_hf_in_band_ringtone_active(hci_con_handle_t acl_handle);
    
    /**
     * @brief Provide Apple Accessory information after SLC
     * @param vendor_id
     * @param product_id
     * @param version
     * @param features bitmask: bit 0 = reserved, bit 1 = battery reporting, bit 2 = docked or powered, bit 3 = Siri
     */
    void hfp_hf_apple_set_identification(uint16_t vendor_id, uint16_t product_id, const char * version, uint8_t features);
    
    /**
     * @brief Set Apple Accessory Battery Level
     * @param battery_level range: 0..9
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS battery level out of range
     */
    uint8_t hfp_hf_apple_set_battery_level(uint8_t battery_level);
    
    /**
     * @brief Set Apple Accessory Docked State
     * @param docked 0 = undocked, 1 = docked
     * @return status ERROR_CODE_SUCCESS if successful, otherwise ERROR_CODE_INVALID_HCI_COMMAND_PARAMETERS docked state invalid
     */
    uint8_t hfp_hf_apple_set_docked_state(uint8_t docked);
    
    /**
     * @brief Send AT command (most likely a vendor-specific command not part of standard HFP).
     * @note  Result (OK/ERROR) is reported via HFP_SUBEVENT_CUSTOM_AT_MESSAGE_SENT
     *        To receive potential unsolicited result code, add ENABLE_HFP_AT_MESSAGES to get all message via HFP_SUBEVENT_AT_MESSAGE_RECEIVED
     *
     * @param acl_handle
     * @param at_command to send
     * @return status ERROR_CODE_SUCCESS if successful, otherwise:
     *              - ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER if connection does not exist, or
     *              - ERROR_CODE_COMMAND_DISALLOWED if extended audio gateway error report is disabled
     */
    uint8_t hfp_hf_send_at_command(hci_con_handle_t acl_handle, const char * at_command);
    
    /**
     * @brief Register custom AT command.
     * @param hfp_custom_at_command (with '+' prefix)
     */
    void hfp_hf_register_custom_at_command(hfp_custom_at_command_t * custom_at_command);
    
    /**
     * @brief De-Init HFP HF
     */
    void hfp_hf_deinit(void);
    
    /**
     * @brief Create HFP Hands-Free (HF) SDP service record.
     * @deprecated Use hfp_hf_create_sdp_record_with_codecs instead
     * @param service
     * @param rfcomm_channel_nr
     * @param name
     * @param suported_features 32-bit bitmap, see HFP_HFSF_* values in hfp.h
     * @param wide_band_speech supported
     */
    void hfp_hf_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint16_t supported_features, int wide_band_speech);
    
    #ifdef ENABLE_TESTING_SUPPORT
    bool test_hfp_hf_vra_state_machine(hfp_connection_t * hfp_connection, hfp_hf_vra_event_type_t event);
    #endif
    

## HFP mSBC Encoder API {#sec:hfp_msbc_api}


**hfp_msbc.h** 


    
    /**
     *
     */
    void hfp_msbc_init(void);
    
    /**
     *
     */
    int  hfp_msbc_num_audio_samples_per_frame(void);
    
    /**
     *
     */
    int  hfp_msbc_can_encode_audio_frame_now(void);
    
    /**
     * @param pcm_samples - complete audio frame of hfp_msbc_num_audio_samples_per_frame int16 samples
     */
    void hfp_msbc_encode_audio_frame(int16_t * pcm_samples);
    
    /**
     *
     */
    int  hfp_msbc_num_bytes_in_stream(void);
    
    /**
     *
     * @param buffer to store stream
     * @param size num bytes to read from stream
     */
    void hfp_msbc_read_from_stream(uint8_t * buffer, int size);
    
    /**
     * @brief De-Init HFP mSBC Codec
     */
    void hfp_msbc_deinit(void);
    

## HID Device API {#sec:hid_device_api}


**hid_device.h** 


    
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
    

## HID Host API {#sec:hid_host_api}


**hid_host.h** 


    
    /**
     * @brief Set up HID Host and provide storage for HID Descriptors
     * If storage for HID Descriptors is provided, the HID Host will fetch and store the HID Descriptors
     * for each connection and inform the application via HID_SUBEVENT_DESCRIPTOR_AVAILABLE event.
     * The app can then access the HID descriptor for a particular connection with
     * hid_descriptor_storage_get_descriptor_data and hid_descriptor_storage_get_descriptor_len.
     * @note if hid_descriptor_storage is NULL, the HID Host will not store the HID Descriptor
     * @param hid_descriptor_storage
     * @param hid_descriptor_storage_len
     */
    void hid_host_init(uint8_t * hid_descriptor_storage, uint16_t hid_descriptor_storage_len);
    
    /**
     * @brief Register callback for the HID Host. 
     * @param callback
     */
    void hid_host_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Create HID connection to HID Device and emit HID_SUBEVENT_CONNECTION_OPENED event with status code, 
     * followed by HID_SUBEVENT_DESCRIPTOR_AVAILABLE that informs if the HID Descriptor was found. In the case of incoming
     * connection, i.e. HID Device initiating the connection, the HID_SUBEVENT_DESCRIPTOR_AVAILABLE is delayed, and the reports 
     * may already come via HID_SUBEVENT_REPORT event. It is up to the application code if 
     * these reports should be buffered or ignored until the descriptor is available.
     * @note  HID_PROTOCOL_MODE_REPORT_WITH_FALLBACK_TO_BOOT will try ti set up REPORT mode, but fallback to BOOT mode if necessary. 
     * @note  HID_SUBEVENT_DESCRIPTOR_AVAILABLE possible status values are:
     * - ERROR_CODE_SUCCESS if descriptor available, 
     * - ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE if not, and 
     * - ERROR_CODE_MEMORY_CAPACITY_EXCEEDED if descriptor is larger then the available space
     * @param remote_addr
     * @param protocol_mode see hid_protocol_mode_t in btstack_hid.h
     * @param hid_cid to use for other commands
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_COMMAND_DISALLOWED, BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t hid_host_connect(bd_addr_t remote_addr, hid_protocol_mode_t protocol_mode, uint16_t * hid_cid);
    
    
    /**
     * @brief Accept incoming HID connection, this should be called upon receiving HID_SUBEVENT_INCOMING_CONNECTION event.
     * @param hid_cid
     * @param protocol_mode see hid_protocol_mode_t in btstack_hid.h
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_accept_connection(uint16_t hid_cid, hid_protocol_mode_t protocol_mode);
    
    /**
     * @brief Decline incoming HID connection, this should be called upon receiving HID_SUBEVENT_INCOMING_CONNECTION event.
     * @param hid_cid
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_decline_connection(uint16_t hid_cid);
    
    /**
     * @brief Disconnect from HID Device and emit HID_SUBEVENT_CONNECTION_CLOSED event.
     * @param hid_cid
     */
    void hid_host_disconnect(uint16_t hid_cid);
    
    // Control messages:
    
    /**
     * @brief Send SUSPEND control signal to connected HID Device. A Bluetooth HID Device which receives a SUSPEND control signal
     * may optionally disconnect from the Bluetooth HID Host.
     * @param hid_cid
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_suspend(uint16_t hid_cid);
    
    /**
     * @brief Order connected HID Device to exit suspend mode. 
     * The Bluetooth HID Device shall send a report to the Bluetooth HID Host.
     * @param hid_cid
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_exit_suspend(uint16_t hid_cid);
    
    /**
     * @brief Unplug connected HID Device. 
     * @note This is the only command that can be also received from HID Device. It will be indicated by receiving 
     * HID_SUBEVENT_VIRTUAL_CABLE_UNPLUG event, as well as disconnecting HID Host from device.
     * @param hid_cid
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_virtual_cable_unplug(uint16_t hid_cid);
    
    /**
     * @brief Set Protocol Mode on the Bluetooth HID Device and emit HID_SUBEVENT_SET_PROTOCOL_RESPONSE event with handshake_status, see hid_handshake_param_type_t
     * @param hid_cid
     * @param protocol_mode see hid_protocol_mode_t in btstack_hid.h
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_set_protocol_mode(uint16_t hid_cid, hid_protocol_mode_t protocol_mode);
    
    /**
     * @brief Retrieve the Protocol Mode of the Bluetooth HID Device and emit HID_SUBEVENT_GET_PROTOCOL_RESPONSE with handshake_status, see hid_handshake_param_type_t
     * @param hid_cid
     * @param protocol_mode see hid_protocol_mode_t in btstack_hid.h
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_get_protocol(uint16_t hid_cid);
    
    /**
     * @brief Send report to a Bluetooth HID Device and emit HID_SUBEVENT_SET_REPORT_RESPONSE with handshake_status, see hid_handshake_param_type_t. The Bluetooth HID Host shall send complete reports.
     * @param hid_cid
     * @param report_type see hid_report_type_t in btstack_hid.h
     * @param report_id or HID_REPORT_ID_UNDEFINED
     * @param report
     * @param report_len
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_set_report(uint16_t hid_cid, hid_report_type_t report_type, uint16_t report_id, const uint8_t * report, uint8_t report_len);
    
    /**
     * @brief Request a HID report from the Bluetooth HID Device and emit HID_SUBEVENT_GET_REPORT_RESPONSE event with with handshake_status, see hid_handshake_param_type_t. 
     * Polling Bluetooth HID Devices using the GET_REPORT transfer is costly in terms of time and overhead, 
     * and should be avoided whenever possible. The GET_REPORT transfer is typically only used by applications 
     * to determine the initial state of a Bluetooth HID Device. If the state of a report changes frequently, 
     * then the report should be reported over the more efficient Interrupt channel, see hid_host_send_report.
     * @param hid_cid
     * @param report_type see hid_report_type_t in btstack_hid.h
     * @param report_id or HID_REPORT_ID_UNDEFINED
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_get_report(uint16_t hid_cid, hid_report_type_t report_type, uint16_t report_id);
    
    /**
     * @brief Send HID output report on interrupt channel.
     * @param hid_cid
     * @param report_id or HID_REPORT_ID_UNDEFINED
     * @param report
     * @param report_len
     * @result status ERROR_CODE_SUCCESS on success, otherwise ERROR_CODE_UNKNOWN_CONNECTION_IDENTIFIER, ERROR_CODE_COMMAND_DISALLOWED
     */
    uint8_t hid_host_send_report(uint16_t hid_cid, uint16_t report_id, const uint8_t * report, uint8_t report_len);
    
    /**
     * @brief Get descriptor data
     * @param hid_cid
     * @result data
     */
    const uint8_t * hid_descriptor_storage_get_descriptor_data(uint16_t hid_cid);
    
    /**
     * @brief Get descriptor length
     * @param hid_cid
     * @result length
     */
    uint16_t hid_descriptor_storage_get_descriptor_len(uint16_t hid_cid);
    
    /**
     * @brief De-Init HID Device
     */
    void hid_host_deinit(void);
    

## HSP Audio Gateway API {#sec:hsp_ag_api}


**hsp_ag.h** 


    
    /**
     * @brief Set up HSP AG.
     * @param rfcomm_channel_nr
     */
    void hsp_ag_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Create HSP Audio Gateway (AG) SDP service record. 
     * @param service Empty buffer in which a new service record will be stored.
     * @param service_record_handle
     * @param rfcomm_channel_nr
     * @param name
     */
    void hsp_ag_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name);
    
    /**
     * @brief Register packet handler to receive HSP AG events.
    
     * The HSP AG event has type HCI_EVENT_HSP_META with following subtypes:  
     * - HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE
     * - HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE
     * - HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE    
     * - HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE                       
     * - HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED      
     * - HSP_SUBEVENT_SPEAKER_GAIN_CHANGED         
     * - HSP_SUBEVENT_HS_COMMAND      
     *
     * @param callback 
     */
    void hsp_ag_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to HSP Headset.
     *
     * Perform SDP query for an RFCOMM service on a remote device, 
     * and establish an RFCOMM connection if such service is found. Reception of the  
     * HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully established. 
     *
     * @param bd_addr
     */
    void hsp_ag_connect(bd_addr_t bd_addr);
    
    /**
     * @brief Disconnect from HSP Headset
     *
     * Reception of the HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_ag_disconnect(void);
    
    
    /**
     * @brief Establish audio connection.
     * 
     * Reception of the HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE with status 0
     * indicates if the audio connection is successfully established. 
     * @param bd_addr
     */
    void hsp_ag_establish_audio_connection(void);
    
    /**
     * @brief Release audio connection.
     *
     * Reception of the HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_ag_release_audio_connection(void);
    
    /**
     * @brief Set microphone gain. 
     * @param gain Valid range: [0,15]
     */
    void hsp_ag_set_microphone_gain(uint8_t gain);
    
    /**
     * @brief Set speaker gain. 
     * @param gain Valid range: [0,15]
     */
    void hsp_ag_set_speaker_gain(uint8_t gain);
    
    /**
     * @brief Start ringing because of incoming call.
     */
    void hsp_ag_start_ringing(void);
    
    /**
     * @brief Stop ringing (e.g. call was terminated).
     */
    void hsp_ag_stop_ringing(void);
    
    /**
     * @brief Enable custom AT commands.
     * 
     * Custom commands are disabled by default. 
     * When enabled, custom AT commands are received via the HSP_SUBEVENT_HS_COMMAND.
     * @param enable
     */
    void hsp_ag_enable_custom_commands(int enable);
    
    /**
     * @brief Send a custom AT command to HSP Headset.
     *
     * On HSP_SUBEVENT_AG_INDICATION, the client needs to respond
     * with this function with the result to the custom command.
     * @param result 
     */
    int hsp_ag_send_result(char * result);
    
    /**
     * @brief Set packet types used for outgoing SCO connection requests
     * @param common single packet_types: SCO_PACKET_TYPES_*
     */
    void hsp_ag_set_sco_packet_types(uint16_t packet_types);
    
    /**
     * @brief De-Init HSP AG
     */
    void hsp_ag_deinit(void);
    

## HSP Headset API {#sec:hsp_hs_api}


**hsp_hs.h** 


    
    /**
     * @brief Set up HSP HS.
     * @param rfcomm_channel_nr
     */
    void hsp_hs_init(uint8_t rfcomm_channel_nr);
    
    /**
     * @brief Create HSP Headset (HS) SDP service record. 
     * @param service Empty buffer in which a new service record will be stored.
     * @param rfcomm_channel_nr
     * @param name
     * @param have_remote_audio_control 
     */
    void hsp_hs_create_sdp_record(uint8_t * service, uint32_t service_record_handle, int rfcomm_channel_nr, const char * name, uint8_t have_remote_audio_control);
    
    /**
     * @brief Register packet handler to receive HSP HS events.
     *
     * The HSP HS event has type HCI_EVENT_HSP_META with following subtypes:                      
     * - HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE
     * - HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE
     * - HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE    
     * - HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE 
     * - HSP_SUBEVENT_RING                         
     * - HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED      
     * - HSP_SUBEVENT_SPEAKER_GAIN_CHANGED         
     * - HSP_SUBEVENT_AG_INDICATION     
     *
     * @param callback 
     */
    void hsp_hs_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Connect to HSP Audio Gateway.
     *
     * Perform SDP query for an RFCOMM service on a remote device, 
     * and establish an RFCOMM connection if such service is found. Reception of the  
     * HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully established. 
     *
     * @param bd_addr
     */
    void hsp_hs_connect(bd_addr_t bd_addr);
    
    /**
     * @brief Disconnect from HSP Audio Gateway
     *
     * Releases the RFCOMM channel. Reception of the  
     * HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_hs_disconnect(void);
    
    
    /**
     * @brief Send button press action. Toggle establish/release of audio connection. 
     */
    void hsp_hs_send_button_press(void);
    
    /**
     * @brief Triger establishing audio connection.
     * 
     * Reception of the HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE with status 0
     * indicates if the audio connection is successfully established. 
     * @param bd_addr
     */
    void hsp_hs_establish_audio_connection(void);
    
    /**
     * @brief Trigger releasing audio connection.
     *
     * Reception of the HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE with status 0
     * indicates if the connection is successfully released. 
     * @param bd_addr
     */
    void hsp_hs_release_audio_connection(void);
    
    /**
     * @brief Set microphone gain. 
     * 
     * The new gain value will be confirmed by the HSP Audio Gateway. 
     * A HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED event will be received.
     * @param gain Valid range: [0,15]
     */
    void hsp_hs_set_microphone_gain(uint8_t gain);
    
    /**
     * @brief Set speaker gain. 
     * 
     * The new gain value will be confirmed by the HSP Audio Gateway. 
     * A HSP_SUBEVENT_SPEAKER_GAIN_CHANGED event will be received.
     * @param gain - valid range: [0,15]
     */
    void hsp_hs_set_speaker_gain(uint8_t gain);
    
    
    
    /**
     * @brief Enable custom indications.
     * 
     * Custom indications are disabled by default. 
     * When enabled, custom indications are received via the HSP_SUBEVENT_AG_INDICATION.
     * @param enable
     */
    void hsp_hs_enable_custom_indications(int enable);
    
    /**
     * @brief Send answer to custom indication.
     *
     * On HSP_SUBEVENT_AG_INDICATION, the client needs to respond
     * with this function with the result to the custom indication
     * @param result 
     */
    int hsp_hs_send_result(const char * result);
    
    /**
     * @brief Set packet types used for incoming SCO connection requests
     * @param common single packet_types: SCO_PACKET_TYPES_*
     */
    void hsp_hs_set_sco_packet_types(uint16_t packet_types);
    
    /**
     * @brief De-Init HSP AG
     */
    void hsp_hs_deinit(void);
    

## Personal Area Network (PAN) API {#sec:pan_api}


**pan.h** 


    
    /** 
     * @brief Creates SDP record for PANU BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param name if NULL, the default service name will be assigned
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     */
    void pan_create_panu_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc);
    
    /** 
     * @brief Creates SDP record for GN BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param name if NULL, the default service name will be assigned
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
     * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
     */
    void pan_create_gn_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc, const char *IPv4Subnet,
    	const char *IPv6Subnet);
    
    /** 
     * @brief Creates SDP record for NAP BNEP service in provided empty buffer.
     * @note Make sure the buffer is big enough.
     *
     * @param service is an empty buffer to store service record
     * @param service_record_handle for new service
     * @param name if NULL, the default service name will be assigned
     * @param network_packet_types array of types terminated by a 0x0000 entry
     * @param description if NULL, the default service description will be assigned
     * @param security_desc 
     * @param net_access_type type of available network access
     * @param max_net_access_rate based on net_access_type measured in byte/s
     * @param IPv4Subnet is optional subnet definition, e.g. "10.0.0.0/8"
     * @param IPv6Subnet is optional subnet definition given in the standard IETF format with the absolute attribute IDs
     */
    void pan_create_nap_sdp_record(uint8_t *service, uint32_t service_record_handle, uint16_t * network_packet_types, const char *name,
    	const char *description, security_description_t security_desc, net_access_type_t net_access_type,
    	uint32_t max_net_access_rate, const char *IPv4Subnet, const char *IPv6Subnet);
    

## PBAP Client API {#sec:pbap_api}


**pbap.h** 


    
    // PBAP Supported Features
    
    #define PBAP_SUPPORTED_FEATURES_DOWNLOAD                        (1<<0)
    #define PBAP_SUPPORTED_FEATURES_BROWSING                        (1<<1)
    #define PBAP_SUPPORTED_FEATURES_DATABASE_IDENTIFIER             (1<<2)
    #define PBAP_SUPPORTED_FEATURES_FOLDER_VERSION_COUNTERS         (1<<3)
    #define PBAP_SUPPORTED_FEATURES_VCARD_SELECTING                 (1<<4)
    #define PBAP_SUPPORTED_FEATURES_ENHANCED_MISSED_CALLS           (1<<5)
    #define PBAP_SUPPORTED_FEATURES_X_BT_UCI_VCARD_PROPERTY         (1<<6)
    #define PBAP_SUPPORTED_FEATURES_X_BT_UID_VCARD_PROPERTY         (1<<7)
    #define PBAP_SUPPORTED_FEATURES_CONTACT_REFERENCING             (1<<8)
    #define PBAP_SUPPORTED_FEATURES_DEFAULT_CONTACT_IMAGE_FORMAT    (1<<9)
    
    // PBAP Property Mask - also used for vCardSelector
    #define PBAP_PROPERTY_MASK_VERSION              (1<< 0) // vCard Version
    #define PBAP_PROPERTY_MASK_FN                   (1<< 1) // Formatted Name
    #define PBAP_PROPERTY_MASK_N                    (1<< 2) // Structured Presentation of Name
    #define PBAP_PROPERTY_MASK_PHOTO                (1<< 3) // Associated Image or Photo
    #define PBAP_PROPERTY_MASK_BDAY                 (1<< 4) // Birthday
    #define PBAP_PROPERTY_MASK_ADR                  (1<< 5) // Delivery Address
    #define PBAP_PROPERTY_MASK_LABEL                (1<< 6) // Delivery
    #define PBAP_PROPERTY_MASK_TEL                  (1<< 7) // Telephone Number
    #define PBAP_PROPERTY_MASK_EMAIL                (1<< 8) // Electronic Mail Address
    #define PBAP_PROPERTY_MASK_MAILER               (1<< 9) // Electronic Mail
    #define PBAP_PROPERTY_MASK_TZ                   (1<<10) // Time Zone
    #define PBAP_PROPERTY_MASK_GEO                  (1<<11) // Geographic Position
    #define PBAP_PROPERTY_MASK_TITLE                (1<<12) // Job
    #define PBAP_PROPERTY_MASK_ROLE                 (1<<13) // Role within the Organization
    #define PBAP_PROPERTY_MASK_LOGO                 (1<<14) // Organization Logo
    #define PBAP_PROPERTY_MASK_AGENT                (1<<15) // vCard of Person Representing
    #define PBAP_PROPERTY_MASK_ORG                  (1<<16) // Name of Organization
    #define PBAP_PROPERTY_MASK_NOTE                 (1<<17) // Comments
    #define PBAP_PROPERTY_MASK_REV                  (1<<18) // Revision
    #define PBAP_PROPERTY_MASK_SOUND                (1<<19) // Pronunciation of Name
    #define PBAP_PROPERTY_MASK_URL                  (1<<20) // Uniform Resource Locator
    #define PBAP_PROPERTY_MASK_UID                  (1<<21) // Unique ID
    #define PBAP_PROPERTY_MASK_KEY                  (1<<22) // Public Encryption Key
    #define PBAP_PROPERTY_MASK_NICKNAME             (1<<23) // Nickname
    #define PBAP_PROPERTY_MASK_CATEGORIES           (1<<24) // Categories
    #define PBAP_PROPERTY_MASK_PROID                (1<<25) // Product ID
    #define PBAP_PROPERTY_MASK_CLASS                (1<<26) // Class information
    #define PBAP_PROPERTY_MASK_SORT_STRING          (1<<27) // String used for sorting operations
    #define PBAP_PROPERTY_MASK_X_IRMC_CALL_DATETIME (1<<28) // Time stamp
    #define PBAP_PROPERTY_MASK_X_BT_SPEEDDIALKEY    (1<<29) // Speed-dial shortcut
    #define PBAP_PROPERTY_MASK_X_BT_UCI             (1<<30) // Uniform Caller Identifier
    #define PBAP_PROPERTY_MASK_X_BT_UID             (1<<31) // Bluetooth Contact Unique Identifier
    
    // PBAP vCardSelectorOperator
    #define PBAP_VCARD_SELECTOR_OPERATOR_OR          0
    #define PBAP_VCARD_SELECTOR_OPERATOR_AND         1
    
    // PBAP Format
    typedef enum {
        PBAP_FORMAT_VCARD_21 = 0,
        PBAP_FORMAT_VCRAD_30
    } pbap_format_vcard_t;
    
    // PBAP Object Types
    typedef enum {
        PBAP_OBJECT_TYPE_INVALID = 0,
        PBAP_OBJECT_TYPE_PHONEBOOOK,
        PBAP_OBJECT_TYPE_VCARD_LISTING,
        PBAP_OBJECT_TYPE_VCARD,
    } pbap_object_type_t;
    
    // PBAP Phonebooks
    typedef enum {
        PBAP_PHONEBOOK_INVALID = 0,
        PBAP_PHONEBOOK_TELECOM_CCH,
        PBAP_PHONEBOOK_TELECOM_FAV,
        PBAP_PHONEBOOK_TELECOM_ICH,
        PBAP_PHONEBOOK_TELECOM_MCH,
        PBAP_PHONEBOOK_TELECOM_OCH,
        PBAP_PHONEBOOK_TELECOM_PB,
        PBAP_PHONEBOOK_TELECOM_SPD,
        PBAP_PHONEBOOK_SIM_TELECOM_CCH,
        PBAP_PHONEBOOK_SIM_TELECOM_ICH,
        PBAP_PHONEBOOK_SIM_TELECOM_MCH,
        PBAP_PHONEBOOK_SIM_TELECOM_OCH,
        PBAP_PHONEBOOK_SIM_TELECOM_PB
    } pbap_phonebook_t;
    
    // lengths
    #define PBAP_DATABASE_IDENTIFIER_LEN 16
    #define PBAP_FOLDER_VERSION_LEN 16
    
    

## PBAP Client API {#sec:pbap_client_api}


**pbap_client.h** 


    
    /**
     * Setup PhoneBook Access Client
     */
    void pbap_client_init(void);
    
    /**
     * @brief Create PBAP connection to a Phone Book Server (PSE) server on a remote device.
     * If the server requires authentication, a PBAP_SUBEVENT_AUTHENTICATION_REQUEST is emitted, which
     * can be answered with pbap_authentication_password(..).
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event,
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     *
     * This function allows for multiple parallel connections.
     *
     * @param client storage for connection state. Must stay valid until connection closes
     * @param l2cap_ertm_config
     * @param l2cap_ertm_buffer_size
     * @param l2cap_ertm_buffer
     * @param handler
     * @param addr
     * @param out_cid to use for further commands
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_MEMORY_ALLOC_FAILED if PBAP or GOEP connection already exists.
     */
    
    uint8_t pbap_client_connect(pbap_client_t * client, l2cap_ertm_config_t *l2cap_ertm_config, uint8_t *l2cap_ertm_buffer,
                                uint16_t l2cap_ertm_buffer_size, btstack_packet_handler_t handler, bd_addr_t addr, uint16_t * out_cid);
    
    /**
     * @brief Create PBAP connection to a Phone Book Server (PSE) server on a remote device. 
     * If the server requires authentication, a PBAP_SUBEVENT_AUTHENTICATION_REQUEST is emitted, which
     * can be answered with pbap_authentication_password(..).
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event, 
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     *
     * This function uses a single pbap_client_t and l2cap ertm buffer instance and can only be used for a single connection.
     * Fur multiple parallel connections, use pbap_client_connect.
     *
     * @param handler 
     * @param addr
     * @param out_cid to use for further commands
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_MEMORY_ALLOC_FAILED if PBAP or GOEP connection already exists.  
     */
    uint8_t pbap_connect(btstack_packet_handler_t handler, bd_addr_t addr, uint16_t * out_cid);
    
    /**
     * Create SDP Record for Phonebook Access Client
     * @param service
     * @param service_record_handle
     * @param service_name
     */
    void pbap_client_create_sdp_record(uint8_t *service, uint32_t service_record_handle, const char *service_name);
    
    /**
     * @brief Provide password for OBEX Authentication after receiving PBAP_SUBEVENT_AUTHENTICATION_REQUEST.
     * The status of PBAP connection establishment is reported via PBAP_SUBEVENT_CONNECTION_OPENED event, 
     * i.e. on success status field is set to ERROR_CODE_SUCCESS.
     * 
     * @param pbap_cid
     * @param password (null terminated string) - not copied, needs to stay valid until connection completed
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_authentication_password(uint16_t pbap_cid, const char * password);
    
    /** 
     * @brief Disconnects PBAP connection with given identifier.
     * Event PBAP_SUBEVENT_CONNECTION_CLOSED indicates that PBAP connection is closed.
     *
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_disconnect(uint16_t pbap_cid);
    
    /** 
     * @brief Set current folder on PSE. The status is reported via PBAP_SUBEVENT_OPERATION_COMPLETED event. 
     * 
     * @param pbap_cid
     * @param path - note: path is not copied
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_phonebook(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Set vCard Selector for get/pull phonebook. No event is emitted.
     * 
     * @param pbap_cid
     * @param vcard_selector - combination of PBAP_PROPERTY_MASK_* properties
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_vcard_selector(uint16_t pbap_cid, uint32_t vcard_selector);
    
    /**
     * @brief Set vCard Selector for get/pull phonebook. No event is emitted.
     * 
     * @param pbap_cid
     * @param vcard_selector_operator - PBAP_VCARD_SELECTOR_OPERATOR_OR (default) or PBAP_VCARD_SELECTOR_OPERATOR_AND
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_vcard_selector_operator(uint16_t pbap_cid, int vcard_selector_operator);
    
    /**
     * @brief Set Property Selector. No event is emitted.
     * @param pbap_cid
     * @param property_selector - combination of PBAP_PROPERTY_MASK_* properties
     * @return
     */
    uint8_t pbap_set_property_selector(uint16_t pbap_cid, uint32_t property_selector);
    
    /**
     * @brief Set number of items returned by pull phonebook.
     * @param pbap_cid
     * @param max_list_count
     * @return
     */
    uint8_t pbap_set_max_list_count(uint16_t pbap_cid, uint16_t max_list_count);
    
    /**
     * @bbrief Set start offset for pull phonebook
     * @param pbap_cid
     * @param list_start_offset
     * @return
     */
    uint8_t pbap_set_list_start_offset(uint16_t pbap_cid, uint16_t list_start_offset);
    
    /**
     * @bbrief Set order for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param order
     * @return
     */
    uint8_t pbap_set_order(uint16_t pbap_cid, uint8_t order);
    
    /**
     * @bbrief Set search property for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param search_property
     * @return
     */
    uint8_t pbap_set_search_property(uint16_t pbap_cid, uint8_t search_property);
    
    /**
     * @bbrief Set search property for pbap_pull_vcard_listing
     * @param pbap_cid
     * @param search_value
     * @return
     */
    uint8_t pbap_set_search_value(uint16_t pbap_cid, const char * search_value);
    
    /**
     * @brief Get size of phone book from PSE. The result is reported via PBAP_SUBEVENT_PHONEBOOK_SIZE event. 
     * @note This function uses the PullPhoneBook command
     *
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_get_phonebook_size(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull phone book from PSE. The result is reported via registered packet handler (see pbap_connect function),
     * with packet type set to PBAP_DATA_PACKET. Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the phone book. 
     * 
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_phonebook(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Get size of phone book from PSE. The result is reported via PBAP_SUBEVENT_PHONEBOOK_SIZE event.
     *
     * @note This function uses the PullvCardListing command
     * @param pbap_cid
     * @param path - note: path is not copied, common path 'telecom/pb.vcf'
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_get_vcard_listing_size(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull vCard listing. vCard data is emitted via PBAP_SUBEVENT_CARD_RESULT event. 
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of vCard listing.
     *
     * @param pbap_cid
     * @param path
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_vcard_listing(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Pull vCard entry. The result is reported via callback (see pbap_connect function),
     * with packet type set to PBAP_DATA_PACKET.
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the vCard entry.
     * 
     * @param pbap_cid
     * @param path
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_pull_vcard_entry(uint16_t pbap_cid, const char * path);
    
    /**
     * @brief Lookup contact(s) by phone number. vCard data is emitted via PBAP_SUBEVENT_CARD_RESULT event. 
     * Event PBAP_SUBEVENT_OPERATION_COMPLETED marks the end of the lookup.
     *
     * @param pbap_cid
     * @param phone_number
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_lookup_by_number(uint16_t pbap_cid, const char * phone_number);
    
    /**
     * @brief Abort current operation. No event is emitted.
     * 
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_abort(uint16_t pbap_cid);
    
    
    /**
     * @brief Set flow control mode - default is off. No event is emitted.
     * @note When enabled, pbap_next_packet needs to be called after a packet was processed to receive the next one
     *
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_set_flow_control_mode(uint16_t pbap_cid, int enable);
        
    /**
     * @brief Trigger next packet from PSE when Flow Control Mode is enabled.
     * @param pbap_cid
     * @return status ERROR_CODE_SUCCESS on success, otherwise BTSTACK_BUSY if in a wrong state.
     */
    uint8_t pbap_next_packet(uint16_t pbap_cid);
    
    /**
     * @brief De-Init PBAP Client
     */
    void pbap_client_deinit(void);
    

## RFCOMM API {#sec:rfcomm_api}


**rfcomm.h** 


    
    /** 
     * @brief Set up RFCOMM.
     */
    void rfcomm_init(void);
    
    /** 
     * @brief Set security level required for incoming connections, need to be called before registering services.
     * @deprecated use gap_set_security_level instead
     */
    void rfcomm_set_required_security_level(gap_security_level_t security_level);
    
    /* 
     * @brief Create RFCOMM connection to a given server channel on a remote deivce.
     * This channel will automatically provide enough credits to the remote side.
     * @param addr
     * @param server_channel
     * @param out_cid
     * @result status
     */
    uint8_t rfcomm_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint16_t * out_cid);
    
    /* 
     * @brief Create RFCOMM connection to a given server channel on a remote deivce.
     * This channel will use explicit credit management. During channel establishment, an initial  amount of credits is provided.
     * @param addr
     * @param server_channel
     * @param initial_credits
     * @param out_cid
     * @result status
     */
    uint8_t rfcomm_create_channel_with_initial_credits(btstack_packet_handler_t packet_handler, bd_addr_t addr, uint8_t server_channel, uint8_t initial_credits, uint16_t * out_cid);
    
    /** 
     * @brief Disconnects RFCOMM channel with given identifier.
     * @return status
     */
    uint8_t rfcomm_disconnect(uint16_t rfcomm_cid);
    
    /** 
     * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler.
     * This channel provides credits automatically to the remote side -> no flow control
     * @param packet handler for all channels of this service
     * @param channel 
     * @param max_frame_size
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t rfcomm_register_service(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size);
    
    /** 
     * @brief Registers RFCOMM service for a server channel and a maximum frame size, and assigns a packet handler. 
     * This channel will use explicit credit management. During channel establishment, an initial amount of credits is provided.
     * @param packet handler for all channels of this service
     * @param channel 
     * @param max_frame_size
     * @param initial_credits
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t rfcomm_register_service_with_initial_credits(btstack_packet_handler_t packet_handler, uint8_t channel, uint16_t max_frame_size, uint8_t initial_credits);
    
    /** 
     * @brief Unregister RFCOMM service.
     */
    void rfcomm_unregister_service(uint8_t service_channel);
    
    /** 
     * @brief Accepts incoming RFCOMM connection.
     * @return status
     */
    uint8_t rfcomm_accept_connection(uint16_t rfcomm_cid);
    
    /** 
     * @brief Deny incoming RFCOMM connection.
     * @return status
     */
    uint8_t rfcomm_decline_connection(uint16_t rfcomm_cid);
    
    /** 
     * @brief Grant more incoming credits to the remote side for the given RFCOMM channel identifier.
     * @return status
     */
    uint8_t rfcomm_grant_credits(uint16_t rfcomm_cid, uint8_t credits);
    
    /** 
     * @brief Checks if RFCOMM can send packet. 
     * @param rfcomm_cid
     * @result true if can send now
     */
    bool rfcomm_can_send_packet_now(uint16_t rfcomm_cid);
    
    /** 
     * @brief Request emission of RFCOMM_EVENT_CAN_SEND_NOW as soon as possible
     * @note RFCOMM_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param rfcomm_cid
     * @prarm status
     */
    uint8_t rfcomm_request_can_send_now_event(uint16_t rfcomm_cid);
    
    /** 
     * @brief Sends RFCOMM data packet to the RFCOMM channel with given identifier.
     * @param rfcomm_cid
     * @return status
     */
    uint8_t rfcomm_send(uint16_t rfcomm_cid, uint8_t *data, uint16_t len);
    
    /** 
     * @brief Sends Local Line Status, see LINE_STATUS_..
     * @param rfcomm_cid
     * @param line_status
     * @return status
     */
    uint8_t rfcomm_send_local_line_status(uint16_t rfcomm_cid, uint8_t line_status);
    
    /** 
     * @brief Send local modem status. see MODEM_STAUS_..
     * @param rfcomm_cid
     * @param modem_status
     * @return status
     */
    uint8_t rfcomm_send_modem_status(uint16_t rfcomm_cid, uint8_t modem_status);
    
    /** 
     * @brief Configure remote port 
     * @param rfcomm_cid
     * @param baud_rate
     * @param data_bits
     * @param stop_bits
     * @param parity
     * @param flow_control
     * @return status
     */
    uint8_t rfcomm_send_port_configuration(uint16_t rfcomm_cid, rpn_baud_t baud_rate, rpn_data_bits_t data_bits, rpn_stop_bits_t stop_bits, rpn_parity_t parity, uint8_t flow_control);
    
    /** 
     * @brief Query remote port 
     * @param rfcomm_cid
     * @return status
     */
    uint8_t rfcomm_query_port_configuration(uint16_t rfcomm_cid);
    
    /** 
     * @brief Query max frame size
     * @param rfcomm_cid
     * @return max frame size
     */
    uint16_t rfcomm_get_max_frame_size(uint16_t rfcomm_cid);
    
    /** 
     * @brief Reserve packet buffer to allow to create RFCOMM packet in place
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     *
     * if (rfcomm_can_send_packet_now(cid)){
     *     rfcomm_reserve_packet_buffer();
     *     uint8_t * buffer = rfcomm_get_outgoing_buffer();
     *     uint16_t buffer_size = rfcomm_get_max_frame_size(cid);
     *     .. setup data in buffer with len ..
     *     rfcomm_send_prepared(cid, len)
     * }
     */
    void rfcomm_reserve_packet_buffer(void);
    
    /**
     * @brief Get outgoing buffer
     * @return pointer to outgoing rfcomm buffer
     */
    uint8_t * rfcomm_get_outgoing_buffer(void);
    
    /**
     * @brief Send packet prepared in outgoing buffer
     * @note This releases the outgoing rfcomm buffer
     * @param rfcomm_cid
     * @param len
     * @return status
     */
    uint8_t rfcomm_send_prepared(uint16_t rfcomm_cid, uint16_t len);
    
    /**
     * @brief Release outgoing buffer in case rfcomm_send_prepared was not called
    */
    void rfcomm_release_packet_buffer(void);
    
    /**
     * @brief Enable L2CAP ERTM mode for RFCOMM. request callback is used to provide ERTM buffer. released callback returns buffer
     *
     * @note on request_callback, the app must set the ertm_config, buffer, size fields to enable ERTM for the current connection
     * @note if buffer is not set, BASIC mode will be used instead
     *
     * @note released_callback provides ertm_id from earlier request to match request and release
     *
     * @param request_callback
     * @param released_callback
     */
    void rfcomm_enable_l2cap_ertm(void request_callback(rfcomm_ertm_request_t * request), void released_callback(uint16_t ertm_id));
    
    
    // Event getters for RFCOMM_EVENT_PORT_CONFIGURATION
    
    /**
     * @brief Get field rfcomm_cid from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return rfcomm_cid
     */
    static inline uint16_t rfcomm_event_port_configuration_get_rfcomm_cid(const uint8_t * event){
        return little_endian_read_16(event, 2);
    }
    
    /**
     * @brief Get field local from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return remote - false for local port, true for remote port
     */
    static inline bool rfcomm_event_port_configuration_get_remote(const uint8_t * event){
        return event[4] != 0;
    }
    
    /**
     * @brief Get field baud_rate from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return baud_rate
     */
    
    static inline rpn_baud_t rfcomm_event_port_configuration_get_baud_rate(const uint8_t * event){
        return (rpn_baud_t) event[5];
    }
    
    /**
     * @brief Get field data_bits from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return data_bits
     */
    
    static inline rpn_data_bits_t rfcomm_event_port_configuration_get_data_bits(const uint8_t * event){
        return (rpn_data_bits_t) (event[6] & 3);
    }
    /**
     * @brief Get field stop_bits from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return stop_bits
     */
    static inline rpn_stop_bits_t rfcomm_event_port_configuration_get_stop_bits(const uint8_t * event){
        return (rpn_stop_bits_t) ((event[6] >> 2) & 1);
    }
    
    /**
     * @brief Get field parity from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return parity
     */
    static inline rpn_parity_t rfcomm_event_port_configuration_get_parity(const uint8_t * event){
        return (rpn_parity_t) ((event[6] >> 3) & 7);
    }
    
    /**
     * @brief Get field flow_control from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return flow_control
     */
    
    static inline uint8_t rfcomm_event_port_configuration_get_flow_control(const uint8_t * event){
        return event[7] & 0x3f;
    }
    
    /**
     * @brief Get field xon from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return xon
     */
    static inline uint8_t rfcomm_event_port_configuration_get_xon(const uint8_t * event){
        return event[8];
    }
    
    /**
     * @brief Get field xoff from event RFCOMM_EVENT_PORT_CONFIGURATION
     * @param event packet
     * @return xoff
     */
    static inline uint8_t rfcomm_event_port_configuration_get_xoff(const uint8_t * event){
        return event[9];
    }
    
    /**
     * @brief De-Init RFCOMM
     */
    void rfcomm_deinit(void);
    

## SDP Client API {#sec:sdp_client_api}


**sdp_client.h** 


    
    typedef struct de_state {
        uint8_t  in_state_GET_DE_HEADER_LENGTH ;
        uint32_t addon_header_bytes;
        uint32_t de_size;
        uint32_t de_offset;
    } de_state_t; 
    
    void de_state_init(de_state_t * state);
    int  de_state_size(uint8_t eventByte, de_state_t *de_state);
    
    /**
     * @brief SDP Client Init
     */
    void sdp_client_init(void);
    
    /** 
     * @brief Checks if the SDP Client is ready
     * @deprecated Please use sdp_client_register_query_callback instead
     * @return true when no query is active
     */
    bool sdp_client_ready(void);
    
    /**
     * @brief Requests a callback, when the SDP Client is ready and can be used
     * @note The callback might happens before sdp_client_register_query_callback has returned
     * @param callback_registration
     */
    uint8_t sdp_client_register_query_callback(btstack_context_callback_registration_t * callback_registration);
    
    /** 
     * @brief Queries the SDP service of the remote device given a service search pattern and a list of attribute IDs. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @param callback for attributes values and done event
     * @param remote address
     * @param des_service_search_pattern 
     * @param des_attribute_id_list
     */
    uint8_t sdp_client_query(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern, const uint8_t * des_attribute_id_list);
    
    /*
     * @brief Searches SDP records on a remote device for all services with a given UUID.
     * @note calls sdp_client_query with service search pattern based on uuid16
     */
    uint8_t sdp_client_query_uuid16(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /*
     * @brief Searches SDP records on a remote device for all services with a given UUID.
     * @note calls sdp_client_query with service search pattern based on uuid128
     */
    uint8_t sdp_client_query_uuid128(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t* uuid128);
    
    
    /** 
     * @brief Retrieves all attribute IDs of a SDP record specified by its service record handle and a list of attribute IDs. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
     * @param callback for attributes values and done event 
     * @param remote address
     * @param search_service_record_handle 
     * @param des_attributeIDList
     */
    uint8_t sdp_client_service_attribute_search(btstack_packet_handler_t callback, bd_addr_t remote, uint32_t search_service_record_handle, const uint8_t * des_attributeIDList);
    
    /** 
     * @brief Query the list of SDP records that match a given service search pattern. 
     * The remote data is handled by the SDP parser. The SDP parser delivers attribute values and done event via the callback.
     * @note only provided if ENABLE_SDP_EXTRA_QUERIES is defined
     * @param callback for attributes values and done event
     * @param remote address
     * @param des_service_search_pattern 
     */
    uint8_t sdp_client_service_search(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_service_search_pattern);
    
    #ifdef ENABLE_SDP_EXTRA_QUERIES
    void sdp_client_parse_service_record_handle_list(uint8_t* packet, uint16_t total_count, uint16_t current_count);
    #endif
    
    /**
     * @brief De-Init SDP Client
     */
    void sdp_client_deinit(void);
    

## SDP Client RFCOMM API {#sec:sdp_client_rfcomm_api}


**sdp_client_rfcomm.h** 


    
    /** 
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 16-bit UUID anywhere.
     * @note calls sdp_service_search_pattern_for_uuid16 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid16
     */
     uint8_t sdp_client_query_rfcomm_channel_and_name_for_uuid(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /**
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 16-bit UUID in its ServiceClassIDList
     * @note calls sdp_service_search_pattern_for_uuid16 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid16
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_service_class_uuid(btstack_packet_handler_t callback, bd_addr_t remote, uint16_t uuid16);
    
    /**
     * @brief Searches SDP records on a remote device for RFCOMM services with a given 128-bit UUID anywhere
     * @note calls sdp_service_search_pattern_for_uuid128 that uses global buffer
     * @param callback handler
     * @param remote BD_ADDR
     * @param uuid128
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_uuid128(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * uuid128);
    
    /** 
     * @brief Searches SDP records on a remote device for RFCOMM services with a given service search pattern.
     */
    uint8_t sdp_client_query_rfcomm_channel_and_name_for_search_pattern(btstack_packet_handler_t callback, bd_addr_t remote, const uint8_t * des_serviceSearchPattern);
    

## SDP Server API {#sec:sdp_server_api}


**sdp_server.h** 


    
    /** 
     * @brief Set up SDP Server.
     */
    void sdp_init(void);
    
    /**
     * @brief Register Service Record with database using ServiceRecordHandle stored in record
     * @pre AttributeIDs are in ascending order
     * @pre ServiceRecordHandle is first attribute and valid
     * @param record is not copied!
     * @result status
     */
    uint8_t sdp_register_service(const uint8_t * record);
    
    /** 
     * @brief Unregister service record internally.
     */
    void sdp_unregister_service(uint32_t service_record_handle);
    
    /**
     * @brief gets service record handle from record
     * @resutl service record handle or 0
     */
    uint32_t sdp_get_service_record_handle(const uint8_t * record);
    
    /**
     * @brief Finds an unused valid service record handle
     * @result handle
     */
    uint32_t sdp_create_service_record_handle(void);
    
    /**
     * @brief gets record for handle
     * @result record
     */
    
    uint8_t * sdp_get_record_for_handle(uint32_t handle);
    
    /**
     * @brief De-Init SDP Server
     */
    void sdp_deinit(void);
    

## SDP Utils API {#sec:sdp_util_api}


**sdp_util.h** 


        
    // OFFSETS FOR LOCALIZED ATTRIBUTES - BLUETOOTH_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST
    #define SDP_Offset_ServiceName        0x0000
    #define SDP_Offset_ServiceDescription 0x0001
    #define SDP_Offset_ProviderName       0x0002
    
    typedef enum {
        DE_NIL = 0,
        DE_UINT,
        DE_INT,
        DE_UUID,
        DE_STRING,
        DE_BOOL,
        DE_DES,
        DE_DEA,
        DE_URL
    } de_type_t;
    
    typedef enum {
        DE_SIZE_8 = 0,
        DE_SIZE_16,
        DE_SIZE_32,
        DE_SIZE_64,
        DE_SIZE_128,
        DE_SIZE_VAR_8,
        DE_SIZE_VAR_16,
        DE_SIZE_VAR_32
    } de_size_t;
    
    // MARK: DateElement
    void      de_dump_data_element(const uint8_t * record);
    uint32_t  de_get_len(const uint8_t * header);
    
    
    // @note returned "string" is not NULL terminated
    const uint8_t * de_get_string(const uint8_t * element);
    
    de_size_t de_get_size_type(const uint8_t * header);
    de_type_t de_get_element_type(const uint8_t * header);
    uint32_t  de_get_header_size(const uint8_t * header);
    // returns true if uint16 was read
    bool      de_element_get_uint16(const uint8_t * element, uint16_t * value);
    uint32_t  de_get_data_size(const uint8_t * header);
    uint32_t  de_get_uuid32(const uint8_t * element);
    // returns true if uint16 was read
    bool      de_get_normalized_uuid(uint8_t *uuid128, const uint8_t *element);
    
    void      de_create_sequence(uint8_t * header);
    void      de_store_descriptor_with_len(uint8_t * header, de_type_t type, de_size_t size, uint32_t len);
    uint8_t * de_push_sequence(uint8_t *sequence);
    void      de_pop_sequence(uint8_t * parent, uint8_t * child);
    void      de_add_number(uint8_t *sequence, de_type_t type, de_size_t size, uint32_t value);
    void      de_add_data(uint8_t *sequence, de_type_t type, uint16_t size, uint8_t *data);
    void      de_add_uuid128(uint8_t * sequence, uint8_t * uuid);
    
    // returns data element  len if date element is smaller than size
    uint32_t de_get_len_safe(const uint8_t * header, uint32_t size);
    
    // MARK: DES iterator
    typedef struct {
        uint8_t * element;
        uint16_t pos;
        uint16_t length;
    } des_iterator_t;
    
    bool      des_iterator_init(des_iterator_t * it, uint8_t * element);
    bool      des_iterator_has_more(des_iterator_t * it);
    de_type_t des_iterator_get_type (des_iterator_t * it);
    uint16_t  des_iterator_get_size (des_iterator_t * it);
    uint8_t * des_iterator_get_element(des_iterator_t * it);
    void      des_iterator_next(des_iterator_t * it);
    
    // MARK: SDP
    uint16_t  sdp_append_attributes_in_attributeIDList(uint8_t *record, uint8_t *attributeIDList, uint16_t startOffset, uint16_t maxBytes, uint8_t *buffer);
    uint8_t * sdp_get_attribute_value_for_attribute_id(uint8_t * record, uint16_t attributeID);
    bool      sdp_set_attribute_value_for_attribute_id(uint8_t * record, uint16_t attributeID, uint32_t value);
    bool      sdp_record_matches_service_search_pattern(uint8_t *record, uint8_t *serviceSearchPattern);
    uint16_t  sdp_get_filtered_size(uint8_t *record, uint8_t *attributeIDList);
    bool      sdp_filter_attributes_in_attributeIDList(uint8_t *record, uint8_t *attributeIDList, uint16_t startOffset, uint16_t maxBytes, uint16_t *usedBytes, uint8_t *buffer);
    bool      sdp_attribute_list_contains_id(uint8_t *attributeIDList, uint16_t attributeID);
    bool      sdp_attribute_list_valid(uint8_t *attributeIDList);
    bool      sdp_valid_service_search_pattern(uint8_t *service_search_pattern);
    /*
     * @brief Returns service search pattern for given UUID-16
     * @note Uses fixed buffer
     */
    uint8_t* sdp_service_search_pattern_for_uuid16(uint16_t uuid16);
    
    /*
     * @brief Returns service search pattern for given UUID-128
     * @note Uses fixed buffer
     */
    uint8_t* sdp_service_search_pattern_for_uuid128(const uint8_t * uuid128);
    

## SPP Server API {#sec:spp_server_api}


**spp_server.h** : 
Create SPP SDP Records.



    
    /**
     * @brief Create SDP record for SPP service with official SPP Service Class
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param rfcomm_channel
     * @param name or NULL for default value. Provide "" (empty string) to skip attribute
     */
    void spp_create_sdp_record(uint8_t *service, uint32_t service_record_handle, int rfcomm_channel, const char *name);
    
    /**
     * @brief Create SDP record for SPP service with custom service UUID (e.g. for use with Android)
     * @param service buffer - needs to large enough
     * @param service_record_handle
     * @param service_uuid128 buffer
     * @param rfcomm_channel
     * @param name
     */
    void spp_create_custom_sdp_record(uint8_t *service, uint32_t service_record_handle, const uint8_t * service_uuid128, int rfcomm_channel, const char *name);
    

## Genral Access Profile (GAP) API {#sec:gap_api}


**gap.h** 


    
    // Classic + LE
    
    /**
     * @brief Read RSSI
     * @param con_handle
     * @events: GAP_EVENT_RSSI_MEASUREMENT
     */
    int gap_read_rssi(hci_con_handle_t con_handle);
    
    
    /**
     * @brief Gets local address.
     */
    void gap_local_bd_addr(bd_addr_t address_buffer);
    
    /**
     * @brief Disconnect connection with handle
     * @param handle
     * @return status
     */
    uint8_t gap_disconnect(hci_con_handle_t handle);
    
    /**
     * @brief Get connection type
     * @param con_handle
     * @result connection_type
     */
    gap_connection_type_t gap_get_connection_type(hci_con_handle_t connection_handle);
    
    /**
     * @brief Get HCI connection role
     * @param con_handle
     * @result hci_role_t HCI_ROLE_MASTER / HCI_ROLE_SLAVE / HCI_ROLE_INVALID (if connection does not exist)
     */
    hci_role_t gap_get_role(hci_con_handle_t connection_handle);
    
    // Classic
    
    /**
     * @brief Request role switch
     * @note this only requests the role switch. A HCI_EVENT_ROLE_CHANGE is emitted and its status field will indicate if the switch was succesful
     * @param addr
     * @param hci_role_t HCI_ROLE_MASTER / HCI_ROLE_SLAVE
     * @result status
     */
    uint8_t gap_request_role(const bd_addr_t addr, hci_role_t role);
    
    /**
     * @brief Sets local name.
     * @note Default name is 'BTstack 00:00:00:00:00:00'
     * @note '00:00:00:00:00:00' in local_name will be replaced with actual bd addr
     * @param name is not copied, make sure memory stays valid
     */
    void gap_set_local_name(const char * local_name);
    
    /**
     * @brief Set Extended Inquiry Response data
     * @note If not set, local name will be used for EIR data (see gap_set_local_name)
     * @note '00:00:00:00:00:00' in local_name will be replaced with actual bd addr
     * @param eir_data size HCI_EXTENDED_INQUIRY_RESPONSE_DATA_LEN (240) bytes, is not copied make sure memory stays valid
     */
    void gap_set_extended_inquiry_response(const uint8_t * data);
    
    /**
     * @brief Set class of device
     */
    void gap_set_class_of_device(uint32_t class_of_device);
    
    /**
     * @brief Set default link policy settings for all classic ACL links
     * @param default_link_policy_settings - see LM_LINK_POLICY_* in bluetooth.h
     * @note common value: LM_LINK_POLICY_ENABLE_ROLE_SWITCH | LM_LINK_POLICY_ENABLE_SNIFF_MODE to enable role switch and sniff mode
     */
    void gap_set_default_link_policy_settings(uint16_t default_link_policy_settings);
    
    /**
     * @brief Set Allow Role Switch param for outgoing classic ACL links
     * @param allow_role_switch - true: allow remote device to request role switch, false: stay master
     */
    void gap_set_allow_role_switch(bool allow_role_switch);
    
    /**
     * @brief Set  link supervision timeout for outgoing classic ACL links
     * @param default_link_supervision_timeout * 0.625 ms, default 0x7d00 = 20 seconds, 0 = no link supervision timeout
     */
    void gap_set_link_supervision_timeout(uint16_t link_supervision_timeout);
    
    /**
     * @brief Enable link watchdog. If no ACL packet is sent within timeout_ms, the link will get disconnected
     * note: current implementation uses the automatic flush timeout controller feature with a max timeout of 1.28s
     * @param timeout_ms
     */
    void gap_enable_link_watchdog(uint16_t timeout_ms);
    
    /**
     * @brief Enable/disable bonding. Default is enabled.
     * @param enabled
     */
    void gap_set_bondable_mode(int enabled);
    
    /**  
     * @brief Get bondable mode.
     * @return 1 if bondable
     */
    int gap_get_bondable_mode(void);
    
    /**
     * @brief Set security mode for all outgoing and incoming connections. Default: GAP_SECURITY_MODE_4
     * @param security_mode is GAP_SECURITY_MODE_2 or GAP_SECURITY_MODE_4
     * @return status ERROR_CODE_SUCCESS or ERROR_CODE_UNSUPPORTED_FEATURE_OR_PARAMETER_VALUE
     */
    uint8_t gap_set_security_mode(gap_security_mode_t security_mode);
    
    /**
     * @brief Get security mode
     * @return security_mode
     */
    gap_security_mode_t gap_get_security_mode(void);
    
    /**
     * @brief Set security level for all outgoing and incoming connections. Default: LEVEL_2
     * @param security_level
     * @note has to be called before services or profiles are initialized
     */
    void gap_set_security_level(gap_security_level_t security_level);
    
    /**
     * @brief Get security level
     * @return security_level
     */
    gap_security_level_t gap_get_security_level(void);
    
    /**
     * @brief Set Secure Connections Only Mode for BR/EDR (Classic) Default: false
     * @param enable
     */
    void gap_set_secure_connections_only_mode(bool enable);
    
    /**
     * @breif Get Secure Connections Only Mode
     * @param enabled
     */
    bool gap_get_secure_connections_only_mode(void);
    
    /**
     * @brief Set minimal security level for registered services
     * @param security_level
     * @note Called by L2CAP based on registered services
     */
    void gap_set_minimal_service_security_level(gap_security_level_t security_level);
    
    /**
     * @brief Register filter for rejecting classic connections. Callback will return 1 accept connection, 0 on reject.
     */
    void gap_register_classic_connection_filter(int (*accept_callback)(bd_addr_t addr, hci_link_type_t link_type));
    
    /* Configure Secure Simple Pairing */
    
    /**
     * @brief Enable will enable SSP during init. Default: true
     */
    void gap_ssp_set_enable(int enable);
    
    /**
     * @brief Set IO Capability. BTstack will return capability to SSP requests
     */
    void gap_ssp_set_io_capability(int ssp_io_capability);
    
    /**
     * @brief Set Authentication Requirements using during SSP
     */
    void gap_ssp_set_authentication_requirement(int authentication_requirement);
    
    /**
     * @brief Enable/disable Secure Connections. Default: true if supported by Controller
     */
    void gap_secure_connections_enable(bool enable);
    
    /**
     * @brief Query if Secure Connections can be used for Classic connections.
     * @note Requires gap_secure_connections_enable == true and Controller to support it
     */
    bool gap_secure_connections_active(void);
    
    /**
     * @brief If set, BTstack will confirm a numeric comparison and enter '000000' if requested.
     */
    void gap_ssp_set_auto_accept(int auto_accept);
    
    /**
     * @brief Set required encryption key size for GAP Levels 1-3 on classic connections.
     * @note If you need to reduce the required encryption key size, please consider enabling
     *       ENABLE_MUTUAL_AUTHENTICATION_FOR_LEGACY_SECURE_CONNECTIONS to prevent BIAS attacks.
     *       However, the re-authentication for Legacy Secure Connections can cause a link loss
     *       in some Controller combinations.
     * @param encryption_key_size in bytes. Valid 7..16, default: 16
     */
    void gap_set_required_encryption_key_size(uint8_t encryption_key_size);
    
    /**
     * @brief Start dedicated bonding with device. Disconnect after bonding.
     * @param device
     * @param request MITM protection
     * @return error, if max num acl connections active
     * @result GAP_DEDICATED_BONDING_COMPLETE
     */
    int gap_dedicated_bonding(bd_addr_t device, int mitm_protection_required);
    
    gap_security_level_t gap_security_level_for_link_key_type(link_key_type_t link_key_type);
    
    /**
     * @brief map link keys to secure connection yes/no
     */
    bool gap_secure_connection_for_link_key_type(link_key_type_t link_key_type);
    
    /**
     * @brief map link keys to authenticated
     */
    bool gap_authenticated_for_link_key_type(link_key_type_t link_key_type);
    
    gap_security_level_t gap_security_level(hci_con_handle_t con_handle);
    
    void gap_request_security_level(hci_con_handle_t con_handle, gap_security_level_t level);
    
    bool gap_mitm_protection_required_for_security_level(gap_security_level_t level);
    
    /**
     * @brief Set Page Scan Type
     * @param page_scan_interval * 0.625 ms, range: 0x0012..0x1000, default: 0x0800
     * @param page_scan_windows  * 0.625 ms, range: 0x0011..page_scan_interval, default: 0x0012
     */
    void gap_set_page_scan_activity(uint16_t page_scan_interval, uint16_t page_scan_window);
    
    /**
     * @brief Set Page Scan Type
     * @param page_scan_mode
     */
    void gap_set_page_scan_type(page_scan_type_t page_scan_type);
    
    /**
     * @brief Set Page Timeout
     * @param page_timeout * 0.625 ms, range: 0x0001..0xffff, default: 0x6000 (ca 15 seconds)
     */
    void gap_set_page_timeout(uint16_t page_timeout);
    
    // LE
    
    /**
     * @brief Set parameters for LE Scan
     * @param scan_type 0 = passive, 1 = active
     * @param scan_interval range 0x0004..0x4000, unit 0.625 ms
     * @param scan_window range 0x0004..0x4000, unit 0.625 ms
     * @param scanning_filter_policy 0 = all devices, 1 = all from whitelist
     * @return status
     */
    uint8_t gap_set_scan_params(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window,
                                uint8_t scanning_filter_policy);
    
    /**
     * @brief Set parameters for LE Scan
     * @deprecated Use gap_set_scan_params instead
     */
    void gap_set_scan_parameters(uint8_t scan_type, uint16_t scan_interval, uint16_t scan_window);
    
    /**
     * @brief Set duplicate filter for LE Scan
     * @param enabled if enabled, only one advertisements per BD_ADDR is reported, default: false
     */
    void gap_set_scan_duplicate_filter(bool enabled);
    
    /**
     * @brief Set PHYs for LE Scan
     * @param phy bitmask: 1 = LE 1M PHY, 4 = LE Coded PHY
     */
    void gap_set_scan_phys(uint8_t phys);
    
    /**
     * @brief Start LE Scan 
     */
    void gap_start_scan(void);
    
    /**
     * @brief Stop LE Scan
     */
    void gap_stop_scan(void);
    
    /**
     * @brief Enable privacy by using random addresses
     * @param random_address_type to use (incl. OFF)
     */
    void gap_random_address_set_mode(gap_random_address_type_t random_address_type);
    
    /**
     * @brief Get privacy mode
     */
    gap_random_address_type_t gap_random_address_get_mode(void);
    
    /**
     * @brief Sets update period for random address
     * @param period_ms in ms
     */
     void gap_random_address_set_update_period(int period_ms);
    
    /** 
     * @brief Sets a fixed random address for advertising
     * @param addr
     * @note Sets random address mode to type static
     */
    void gap_random_address_set(const bd_addr_t addr);
    
    /**
     * @brief Set Advertisement Data
     * @param advertising_data_length
     * @param advertising_data (max 31 octets)
     * @note data is not copied, pointer has to stay valid
     * @note '00:00:00:00:00:00' in advertising_data will be replaced with actual bd addr
     */
    void gap_advertisements_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
    
    /**
     * @brief Set Advertisement Parameters
     * @param adv_int_min
     * @param adv_int_max
     * @param adv_type
     * @param direct_address_type
     * @param direct_address
     * @param channel_map
     * @param filter_policy
     * @note own_address_type is used from gap_random_address_set_mode
     */
    void gap_advertisements_set_params(uint16_t adv_int_min, uint16_t adv_int_max, uint8_t adv_type,
    	uint8_t direct_address_typ, bd_addr_t direct_address, uint8_t channel_map, uint8_t filter_policy);
    
    /** 
     * @brief Enable/Disable Advertisements. OFF by default.
     * @param enabled
     */
    void gap_advertisements_enable(int enabled);
    
    /** 
     * @brief Set Scan Response Data
     *
     * @note For scan response data, scannable undirected advertising (ADV_SCAN_IND) need to be used
     *
     * @param advertising_data_length
     * @param advertising_data (max 31 octets)
     * @note data is not copied, pointer has to stay valid
     * @note '00:00:00:00:00:00' in scan_response_data will be replaced with actual bd addr
     */
    void gap_scan_response_set_data(uint8_t scan_response_data_length, uint8_t * scan_response_data);
    
    
    /**
     * @brief Set update interval for resolvable private addresses generated by the Controller
     * @param update_s timeout for updates in seconds
     * @return status
     */
    uint8_t gap_extended_advertising_set_resolvable_private_address_update(uint16_t update_s);
    
    /**
     * @brief Provide storage for new advertising set and setup on Controller
     * @note Using RPA as own_address_type requires ENABLE_LE_ENHANCED_CONNECTION_COMPLETE_EVENT is required for pairing
     * @param storage to use by stack, needs to stay valid until adv set is removed with gap_extended_advertising_remove
     * @param advertising_parameters
     * @param out_advertising_handle to use with other adv config commands
     * @return status
     * @events: GAP_SUBEVENT_ADVERTISING_SET_INSTALLED
     */
    uint8_t gap_extended_advertising_setup(le_advertising_set_t * storage, const le_extended_advertising_parameters_t * advertising_parameters, uint8_t * out_advertising_handle);
    
    /**
     * @param Set advertising params for advertising set
     * @note Using RPA as own_address_type requires ENABLE_LE_ENHANCED_CONNECTION_COMPLETE_EVENT is required for pairing
     * @param advertising_handle
     * @param advertising_parameters
     * @return status
     */
    uint8_t gap_extended_advertising_set_params(uint8_t advertising_handle, const le_extended_advertising_parameters_t * advertising_parameters);
    
    /**
     * @param Get advertising params for advertising set, e.g. to update params
     * @param advertising_handle
     * @param advertising_parameters
     * @return status
     */
    uint8_t gap_extended_advertising_get_params(uint8_t advertising_handle, le_extended_advertising_parameters_t * advertising_parameters);
    
    /**
     * @param Set periodic advertising params for advertising set
     * @param advertising_handle
     * @param advertising_parameters
     * @return status
     */
    uint8_t gap_periodic_advertising_set_params(uint8_t advertising_handle, const le_periodic_advertising_parameters_t * advertising_parameters);
    
    /**
     * @param Get params for periodic advertising set, e.g. to update params
     * @param advertising_handle
     * @param advertising_parameters
     * @return status
     */
    uint8_t gap_periodic_advertising_get_params(uint8_t advertising_handle, le_periodic_advertising_parameters_t * advertising_parameters);
    
    /**
     * @param Set random addrress for advertising set
     * @param advertising_handle
     * @param random_address
     * @return status
     */
    uint8_t gap_extended_advertising_set_random_address(uint8_t advertising_handle, bd_addr_t random_address);
    
    /**
     * @brief Set Advertising Data for a advertisement set
     * @param advertising_handle
     * @param advertising_data_length
     * @param advertising_data
     * @return status
     */
    uint8_t gap_extended_advertising_set_adv_data(uint8_t advertising_handle, uint16_t advertising_data_length, const uint8_t * advertising_data);
    
    /**
     * @brief Set Scan Response Data for a advertisement set
     * @param advertising_handle
     * @param scan_response_data_length
     * @param scan_response_data
     * @return status
     */
    uint8_t gap_extended_advertising_set_scan_response_data(uint8_t advertising_handle, uint16_t scan_response_data_length, const uint8_t * scan_response_data);
    
    /**
     * @brief Set data for periodic advertisement set
     * @param advertising_handle
     * @param periodic_data_length
     * @param periodic_data
     * @return status
     */
    uint8_t gap_periodic_advertising_set_data(uint8_t advertising_handle, uint16_t periodic_data_length, const uint8_t * periodic_data);
    
    /**
     * @brief Start advertising advertising set
     * @param advertising_handle
     * @param timeout in 10ms, or 0 == no timeout
     * @param num_extended_advertising_events Controller shall send, or 0 == no max number
     * @return status
     */
    uint8_t gap_extended_advertising_start(uint8_t advertising_handle, uint16_t timeout, uint8_t num_extended_advertising_events);
    
    /**
     * @brief Stop advertising
     * @param advertising_handle
     * @return status
     */
    uint8_t gap_extended_advertising_stop(uint8_t advertising_handle);
    
    /**
     * @brief Start periodic advertising for given advertising set
     * @param advertising_handle
     * @param include_adi
     * @return status
     */
    uint8_t gap_periodic_advertising_start(uint8_t advertising_handle, bool include_adi);
    
    /**
     * @brief Stop periodic advertising for given advertising set
     * @param advertising_handle
     * @return status
     */
    uint8_t gap_periodic_advertising_stop(uint8_t advertising_handle);
    
    /**
     * @brief Set Default Periodic Advertising Sync Transfer Parameters
     * @note The parameters are used for all subsequent connections over the LE transport.
     *       If mode != 0, an HCI_LE_Periodic_Advertising_Sync_Transfer_Received event will be emitted by the Controller
     * @param mode 0 = ignore (default), 1 = periodic advertising events disabled
     *             2 = periodic advertising events enabled with duplicate filtering
     *             3 = periodic advertising events enabled with duplicate filtering
     * @param skip The number of periodic advertising packets that can be skipped after a successful receive
     * @param sync_timeout Range: 0x000A to 0x4000, Time = N*10 ms, Time Range: 100 ms to 163.84 s
     * @param cte_type  bit 0 = Do not sync to packets with an AoA Constant Tone Extension
     *                  bit 1 = Do not sync to packets with an AoD Constant Tone Extension with 1 us slots
     *                  bit 2 = Do not sync to packets with an AoD Constant Tone Extension with 2 us slots
     *                  bit 3 = Do not sync to packets without a Constant Tone Extension
     * @return status
     */
    uint8_t gap_periodic_advertising_sync_transfer_set_default_parameters(uint8_t mode, uint16_t skip, uint16_t sync_timeout, uint8_t cte_type);
    
    /**
     * @brief Send Periodic Advertising Sync Transfer to connected device
     * @param con_handle of connected device
     * @param service_data 16-bit data to transfer to remote host
     * @param sync_handle of synchronized periodic advertising train to transfer
     * @return status
     */
    uint8_t gap_periodic_advertising_sync_transfer_send(hci_con_handle_t con_handle, uint16_t service_data, hci_con_handle_t sync_handle);
    
    /**
     * @brief Send Periodic Advertising Set Info Transfer to connected device
     * @param con_handle of connected device
     * @param service_data 16-bit data to transfer to remote host
     * @param advertising_handle of local periodic advertising train to transfer
     * @return status
     */
    uint8_t gap_periodic_advertising_set_info_transfer_send(hci_con_handle_t con_handle, uint16_t service_data, uint8_t advertising_handle);
    
    /**
     * @brief Remove advertising set from Controller
     * @param advertising_handle
     * @return status
     * @events GAP_SUBEVENT_ADVERTISING_SET_REMOVED
     */
    uint8_t gap_extended_advertising_remove(uint8_t advertising_handle);
    
    /**
     * @brief Create Broadcast Isochronous Group (BIG)
     * @param storage to use by stack, needs to stay valid until adv set is removed with gap_big_terminate
     * @param big_params
     * @return status
     * @events GAP_SUBEVENT_BIG_CREATED unless interrupted by call to gap_big_terminate
     */
    uint8_t gap_big_create(le_audio_big_t * storage, le_audio_big_params_t * big_params);
    
    /**
     * @brief Terminate Broadcast Isochronous Group (BIG)
     * @param big_handle
     * @return status
     * @events GAP_SUBEVENT_BIG_TERMINATED
     */
    uint8_t gap_big_terminate(uint8_t big_handle);
    
    /**
     * @brief Synchronize to Broadcast Isochronous Group (BIG)
     * @param storage to use by stack, needs to stay valid until adv set is removed with gap_big_terminate
     * @param big_sync_params
     * @return status
     * @events GAP_SUBEVENT_BIG_SYNC_CREATED unless interrupted by call to gap_big_sync_terminate
     */
    uint8_t gap_big_sync_create(le_audio_big_sync_t * storage, le_audio_big_sync_params_t * big_sync_params);
    
    /**
     * @brief Stop synchronizing to Broadcast Isochronous Group (BIG). Triggers GAP_SUBEVENT_BIG_SYNC_STOPPED
     * @note Also used to stop synchronizing before BIG Sync was established
     * @param big_handle
     * @return status
     * @events GAP_SUBEVENT_BIG_SYNC_STOPPED
     */
    uint8_t gap_big_sync_terminate(uint8_t big_handle);
    
    /**
     * @brief Create Connected Isochronous Group (CIG)
     * @param storage to use by stack, needs to stay valid until CIG removed with gap_cig_remove
     * @param cig_params
     * @return status
     * @events GAP_SUBEVENT_CIG_CREATED unless interrupted by call to gap_cig_remove
     */
    uint8_t gap_cig_create(le_audio_cig_t * storage, le_audio_cig_params_t * cig_params);
    
    /**
     * @brief Remove Connected Isochronous Group (CIG)
     * @param cig_id
     * @return status
     * @events GAP_SUBEVENT_CIG_TERMINATED
     */
    uint8_t gap_cig_remove(uint8_t cig_id);
    
    /**
     * @brief Create Connected Isochronous Streams (CIS)
     * @note number of CIS from cig_params in gap_cig_create is used
     * @param cig_id
     * @param cis_con_handles array of CIS Connection Handles
     * @param acl_con_handles array of ACL Connection Handles
     * @return status
     * @events GAP_SUBEVENT_CIS_CREATED unless interrupted by call to gap_cig_remove
     */
    uint8_t gap_cis_create(uint8_t cig_id, hci_con_handle_t cis_con_handles [], hci_con_handle_t acl_con_handles []);
    
    /**
     * @brief Accept Connected Isochronous Stream (CIS)
     * @param cis_con_handle
     * @return status
     * @events GAP_SUBEVENT_CIS_CREATED
     */
    uint8_t gap_cis_accept(hci_con_handle_t cis_con_handle);
    
    #ifdef ENABLE_LE_AUDIO_CODEC_OFFLOAD
    /**
     * @brief Set codec configuration send to Controller for LC3 Offloading
     * @param cis_handle
     * @param data_direction
     * @param offload_config
     */
    uint8_t gap_cis_set_codec_configuration(hci_con_handle_t cis_handle, uint8_t data_direction,
                                            const le_audio_offload_config_t* offload_config);
    #endif
    
    /**
     * @brief Reject Connected Isochronous Stream (CIS)
     * @param cis_con_handle
     * @return status
     * @events GAP_SUBEVENT_CIS_CREATED
     */
    uint8_t gap_cis_reject(hci_con_handle_t cis_con_handle);
    
    /**
     * @brief Set connection parameters for outgoing connections and connection parameter updates
     * @param conn_scan_interval (unit: 0.625 msec), default: 60 ms
     * @param conn_scan_window (unit: 0.625 msec), default: 30 ms
     * @param conn_interval_min (unit: 1.25ms), default: 10 ms
     * @param conn_interval_max (unit: 1.25ms), default: 30 ms
     * @param conn_latency, default: 4
     * @param supervision_timeout (unit: 10ms), default: 720 ms
     * @param min_ce_length (unit: 0.625ms), default: 10 ms
     * @param max_ce_length (unit: 0.625ms), default: 30 ms
     */
    void gap_set_connection_parameters(uint16_t conn_scan_interval, uint16_t conn_scan_window, 
        uint16_t conn_interval_min, uint16_t conn_interval_max, uint16_t conn_latency,
        uint16_t supervision_timeout, uint16_t min_ce_length, uint16_t max_ce_length);
    
    /**
     * @brief Set initiating PHYs for outgoing connections
     * @param phy bitmask: 1 = LE 1M PHY, 2 = LE 2M PHY, 4 = LE Coded PHY
     */
    void gap_set_connection_phys(uint8_t phys);
    
    /**
     * @brief Request an update of the connection parameter for a given LE connection
     * @param handle
     * @param conn_interval_min (unit: 1.25ms)
     * @param conn_interval_max (unit: 1.25ms)
     * @param conn_latency
     * @param supervision_timeout (unit: 10ms)
     * @return 0 if ok
     */
    int gap_request_connection_parameter_update(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
    
    /**
     * @brief Updates the connection parameters for a given LE connection
     * @param handle
     * @param conn_interval_min (unit: 1.25ms)
     * @param conn_interval_max (unit: 1.25ms)
     * @param conn_latency
     * @param supervision_timeout (unit: 10ms)
     * @return 0 if ok
     */
    int gap_update_connection_parameters(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    	uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);
    
    /**
     * @brief Set accepted connection parameter range
     * @param range
     */
    void gap_get_connection_parameter_range(le_connection_parameter_range_t * range);
    
    /**
     * @brief Get accepted connection parameter range
     * @param range
     */
    void gap_set_connection_parameter_range(le_connection_parameter_range_t * range);
    
    /**
     * @brief Test if connection parameters are inside in existing rage
     * @param conn_interval_min (unit: 1.25ms)
     * @param conn_interval_max (unit: 1.25ms)
     * @param conn_latency
     * @param supervision_timeout (unit: 10ms)
     * @return 1 if included
     */
    int gap_connection_parameter_range_included(le_connection_parameter_range_t * existing_range, uint16_t le_conn_interval_min, uint16_t le_conn_interval_max, uint16_t le_conn_latency, uint16_t le_supervision_timeout);
    
    /**
     * @brief Request an update of the connection subrating for a given LE connection
     * @param handle
     * @param subrate_min
     * @param subrate_max
     * @param max_latency (in units of subrated connection intervals)
     * @param continuation_number (Minimum number of underlying connection events to remain active after a packet containing a Link Layer PDU with a non-zero Length field is sent or received)
     * @param supervision_timeout (unit: 10ms) range: 10..3200 (100 ms to 32 s)
     * @return status
     */
    uint8_t gap_request_connection_subrating(hci_con_handle_t con_handle, uint16_t subrate_min, uint16_t subrate_max,
                                         uint16_t max_latency, uint16_t continuation_number, uint16_t supervision_timeout);
    
    /**
     * @brief Set max number of connections in LE Peripheral role (if Bluetooth Controller supports it)
     * @note: default: 1
     * @param max_peripheral_connections
     */
    void gap_set_max_number_peripheral_connections(int max_peripheral_connections);
    
    /**
     * @brief Add Device to Whitelist
     * @param address_typ
     * @param address
     * @return status
     */
    uint8_t gap_whitelist_add(bd_addr_type_t address_type, const bd_addr_t address);
    
    /**
     * @brief Remove Device from Whitelist
     * @param address_typ
     * @param address
     * @return status
     */
    uint8_t gap_whitelist_remove(bd_addr_type_t address_type, const bd_addr_t address);
    
    /**
     * @brief Clear Whitelist
     * @return status
     */
    uint8_t gap_whitelist_clear(void);
    
    /**
     * @brief Connect to remote LE device
     * @return status
     */
    uint8_t gap_connect(const bd_addr_t addr, bd_addr_type_t addr_type);
    
    /**
     * @brief Connect with Whitelist
     * @note Explicit whitelist management and this connect with whitelist replace deprecated gap_auto_connection_* functions
     * @return status
     */
    uint8_t gap_connect_with_whitelist(void);
    
    /**
     * @brief Cancel connection process initiated by gap_connect
     * @return status
     */
    uint8_t gap_connect_cancel(void);
    
    /**
     * @brief Auto Connection Establishment - Start Connecting to device
     * @deprecated Please setup Whitelist with gap_whitelist_* and start connecting with gap_connect_with_whitelist
     * @param address_type
     * @param address
     * @return status
     */
    uint8_t gap_auto_connection_start(bd_addr_type_t address_type, const bd_addr_t address);
    
    /**
     * @brief Auto Connection Establishment - Stop Connecting to device
     * @deprecated Please setup Whitelist with gap_whitelist_* and start connecting with gap_connect_with_whitelist
     * @param address_type
     * @param address
     * @return status
     */
    uint8_t gap_auto_connection_stop(bd_addr_type_t address_type, const bd_addr_t address);
    
    /**
     * @brief Auto Connection Establishment - Stop everything
     * @deprecated Please setup Whitelist with gap_whitelist_* and start connecting with gap_connect_with_whitelist
     * @note  Convenience function to stop all active auto connection attempts
     * @return status
     */
    uint8_t gap_auto_connection_stop_all(void);
    
    /**
     * @brief Set LE PHY
     * @param con_handle
     * @param all_phys 0 = set rx/tx, 1 = set only rx, 2 = set only tx
     * @param tx_phys 1 = 1M, 2 = 2M, 4 = Coded
     * @param rx_phys 1 = 1M, 2 = 2M, 4 = Coded
     * @param phy_options 0 = no preferred coding for Coded, 1 = S=2 coding (500 kbit), 2 = S=8 coding (125 kbit)
     * @return status
     */
    uint8_t gap_le_set_phy(hci_con_handle_t con_handle, uint8_t all_phys, uint8_t tx_phys, uint8_t rx_phys, uint16_t phy_options);
    
    /**
     * @brief Get connection interval
     * @param con_handle
     * @return connection interval, otherwise 0 if error 
     */
    uint16_t gap_le_connection_interval(hci_con_handle_t con_handle);
    
    /**
     *
     * @brief Get encryption key size.
     * @param con_handle
     * @return 0 if not encrypted, 7-16 otherwise
     */
    uint8_t gap_encryption_key_size(hci_con_handle_t con_handle);
    
    /**
     * @brief Get authentication property.
     * @param con_handle
     * @return true if bonded with OOB/Passkey (AND MITM protection)
     */
    bool gap_authenticated(hci_con_handle_t con_handle);
    
    /**
     * @brief Get secure connection property
     * @param con_handle
     * @return true if bonded usiung LE Secure Connections
     */
    bool gap_secure_connection(hci_con_handle_t con_handle);
    
    /**
     * @brief Queries authorization state.
     * @param con_handle
     * @return authorization_state for the current session
     */
    authorization_state_t gap_authorization_state(hci_con_handle_t con_handle);
    
    /**
     * @brief Get bonded property (BR/EDR/LE)
     * @note LE: has to be called after identity resolving is complete
     * @param con_handle
     * @return true if bonded
     */
    bool gap_bonded(hci_con_handle_t con_handle);
    
    // Classic
    #ifdef ENABLE_CLASSIC
    
    /**
     * @brief Override page scan mode. Page scan mode enabled by l2cap when services are registered
     * @note Might be used to reduce power consumption while Bluetooth module stays powered but no (new)
     *       connections are expected
     */
    void gap_connectable_control(uint8_t enable);
    
    /**
     * @brief Allows to control if device is discoverable. OFF by default.
     */
    void gap_discoverable_control(uint8_t enable);
    
    /**
     * @brief Deletes link key for remote device with baseband address.
     * @param addr
     * @note On most desktop ports, the Link Key DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     */
    void gap_drop_link_key_for_bd_addr(bd_addr_t addr);
    
    /**
     * @brief Delete all stored link keys
     * @note On most desktop ports, the Link Key DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     */
    void gap_delete_all_link_keys(void);
    
    /** 
     * @brief Store link key for remote device with baseband address
     * @param addr
     * @param link_key
     * @param link_key_type
     * @note On most desktop ports, the Link Key DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     */
    void gap_store_link_key_for_bd_addr(bd_addr_t addr, link_key_t link_key, link_key_type_t type);
    
    /**
     * @brief Get link for remote device with basband address
     * @param addr
     * @param link_key (out) is stored here
     * @param link_key_type (out) is stored here
     * @note On most desktop ports, the Link Key DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     */
    bool gap_get_link_key_for_bd_addr(bd_addr_t addr, link_key_t link_key, link_key_type_t * type);
    
    /**
     * @brief Setup Link Key iterator
     * @param it
     * @return 1 on success
     * @note On most desktop ports, the Link Key DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     */
    int gap_link_key_iterator_init(btstack_link_key_iterator_t * it);
    
    /**
     * @brief Get next Link Key
     * @param it
     * @brief addr
     * @brief link_key
     * @brief type of link key
     * @return 1, if valid link key found
     * @see note on gap_link_key_iterator_init
     */
    int gap_link_key_iterator_get_next(btstack_link_key_iterator_t * it, bd_addr_t bd_addr, link_key_t link_key, link_key_type_t * type);
    
    /**
     * @brief Frees resources allocated by iterator_init
     * @note Must be called after iteration to free resources
     * @param it
     * @see note on gap_link_key_iterator_init
     */
    void gap_link_key_iterator_done(btstack_link_key_iterator_t * it);
    
    /**
     * @brief Start GAP Classic Inquiry
     * @param duration in 1.28s units
     * @return status
     * @events: GAP_EVENT_INQUIRY_RESULT, GAP_EVENT_INQUIRY_COMPLETE
     */
    int gap_inquiry_start(uint8_t duration_in_1280ms_units);
    
    /**
     * @brief Start GAP Classic Periodic Inquiry
     * @param duration in 1.28s units
     * @param max_period_length between consecutive inquiries in 1.28s units
     * @param min_period_length between consecutive inquiries in 1.28s units
     * @return status
     * @events: GAP_EVENT_INQUIRY_RESULT, GAP_EVENT_INQUIRY_COMPLETE
     */
    uint8_t gap_inquiry_periodic_start(uint8_t duration, uint16_t max_period_length, uint16_t min_period_length);
    
    /**
     * @brief Stop GAP Classic Inquiry (regular or periodic)
     * @return 0 if ok
     * @events GAP_EVENT_INQUIRY_COMPLETE
     */
    int gap_inquiry_stop(void);
    
    /**
     * @brief Set LAP for GAP Classic Inquiry
     * @param lap GAP_IAC_GENERAL_INQUIRY (default), GAP_IAC_LIMITED_INQUIRY
     */
    void gap_inquiry_set_lap(uint32_t lap);
    
    /**
     * @brief Set Inquiry Scan Activity
     * @param inquiry_scan_interval range: 0x0012 to 0x1000; only even values are valid, Time = N * 0.625 ms
     * @param inquiry_scan_window range: 0x0011 to 0x1000; Time = N * 0.625 ms
     */
    void gap_inquiry_set_scan_activity(uint16_t inquiry_scan_interval, uint16_t inquiry_scan_window);
    
    /**
     * @brief Set Inquiry Transmit Power Level
     * @param tx_power range: -70 to 20 dBm
     */
    void gap_inquiry_set_transmit_power_level(int8_t tx_power);
    
    /**
     * @brief Remote Name Request
     * @param addr
     * @param page_scan_repetition_mode
     * @param clock_offset only used when bit 15 is set - pass 0 if not known
     * @events HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE
     */
    int gap_remote_name_request(const bd_addr_t addr, uint8_t page_scan_repetition_mode, uint16_t clock_offset);
    
    /**
     * @brief Legacy Pairing Pin Code Response
     * @note data is not copied, pointer has to stay valid
     * @param addr
     * @param pin
     * @return 0 if ok
     */
    int gap_pin_code_response(const bd_addr_t addr, const char * pin);
    
    /**
     * @brief Legacy Pairing Pin Code Response for binary data / non-strings
     * @note data is not copied, pointer has to stay valid
     * @param addr
     * @param pin_data
     * @param pin_len
     * @return 0 if ok
     */
    int gap_pin_code_response_binary(const bd_addr_t addr, const uint8_t * pin_data, uint8_t pin_len);
    
    /**
     * @brief Abort Legacy Pairing
     * @param addr
     * @param pin
     * @return 0 if ok
     */
    int gap_pin_code_negative(bd_addr_t addr);
    
    /**
     * @brief SSP Passkey Response
     * @param addr
     * @param passkey [0..999999]
     * @return 0 if ok
     */
    int gap_ssp_passkey_response(const bd_addr_t addr, uint32_t passkey);
    
    /**
     * @brief Abort SSP Passkey Entry/Pairing
     * @param addr
     * @param pin
     * @return 0 if ok
     */
    int gap_ssp_passkey_negative(const bd_addr_t addr);
    
    /**
     * @brief Accept SSP Numeric Comparison
     * @param addr
     * @param passkey
     * @return 0 if ok
     */
    int gap_ssp_confirmation_response(const bd_addr_t addr);
    
    /**
     * @brief Abort SSP Numeric Comparison/Pairing
     * @param addr
     * @param pin
     * @return 0 if ok
     */
    int gap_ssp_confirmation_negative(const bd_addr_t addr);
    
    /**
     * @brief Generate new OOB data
     * @note OOB data will be provided in GAP_EVENT_LOCAL_OOB_DATA and be used in future pairing procedures
     */
    void gap_ssp_generate_oob_data(void);
    
    /**
     * @brief Report Remote OOB Data
     * @note Pairing Hash and Randomizer are expected in big-endian byte format
     * @param bd_addr
     * @param c_192 Simple Pairing Hash C derived from P-192 public key
     * @param r_192 Simple Pairing Randomizer derived from P-192 public key
     * @param c_256 Simple Pairing Hash C derived from P-256 public key
     * @param r_256 Simple Pairing Randomizer derived from P-256 public key
     * @return status
     */
    uint8_t gap_ssp_remote_oob_data(const bd_addr_t addr, const uint8_t * c_192, const uint8_t * r_192, const uint8_t * c_256, const uint8_t * r_256);
    
    /**
     * Send SSP IO Capabilities Reply
     * @note IO Capabilities (Negative) Reply is sent automatically unless ENABLE_EXPLICIT_IO_CAPABILITIES_REPLY
     * @param addr
     * @return status
     */
    uint8_t gap_ssp_io_capabilities_response(const bd_addr_t addr);
    
    /**
     * Send SSP IO Capabilities Negative Reply
     * @note IO Capabilities (Negative) Reply is sent automatically unless ENABLE_EXPLICIT_IO_CAPABILITIES_REPLY
     * @param addr
     * @return status
     */
    uint8_t gap_ssp_io_capabilities_negative(const bd_addr_t addr);
    
    /**
     * Send Link Key Reponse
     * @note Link Key (Negative) Reply is sent automatically unless ENABLE_EXPLICIT_LINK_KEY_RESPONSE
     * @param addr
     * @param link_key
     * @param type or INVALID_LINK_KEY if link key not available
     * @return status
     */
     uint8_t gap_send_link_key_response(const bd_addr_t addr, link_key_t link_key, link_key_type_t type);
    
    /**
     * @brief Enter Sniff mode
     * @param con_handle
     * @param sniff_min_interval range: 0x0002 to 0xFFFE; only even values are valid, Time = N * 0.625 ms
     * @param sniff_max_interval range: 0x0002 to 0xFFFE; only even values are valid, Time = N * 0.625 ms
     * @param sniff_attempt Number of Baseband receive slots for sniff attempt.
     * @param sniff_timeout Number of Baseband receive slots for sniff timeout.
     * @return status
     */
    uint8_t gap_sniff_mode_enter(hci_con_handle_t con_handle, uint16_t sniff_min_interval, uint16_t sniff_max_interval, uint16_t sniff_attempt, uint16_t sniff_timeout);
    
    /**
     * @brief Exit Sniff mode
     * @param con_handle
     * @return status
     */
    uint8_t gap_sniff_mode_exit(hci_con_handle_t con_handle);
    
    /**
     * @brief Configure Sniff Subrating
     * @param con_handle
     * @param max_latency range: 0x0002 to 0xFFFE; Time = N * 0.625 ms
     * @param min_remote_timeout range:  0x0002 to 0xFFFE; Time = N * 0.625 ms
     * @param min_local_timeout range:  0x0002 to 0xFFFE; Time = N * 0.625 ms
     * @return status
     */
    uint8_t gap_sniff_subrating_configure(hci_con_handle_t con_handle, uint16_t max_latency, uint16_t min_remote_timeout, uint16_t min_local_timeout);
    
    /**
     * @Brief Set QoS
     * @param con_handle
     * @param service_type
     * @param token_rate
     * @param peak_bandwidth
     * @param latency
     * @param delay_variation
     * @return status
     */
    uint8_t gap_qos_set(hci_con_handle_t con_handle, hci_service_type_t service_type, uint32_t token_rate, uint32_t peak_bandwidth, uint32_t latency, uint32_t delay_variation);
    
    #endif
    
    // LE
    
    /**
     * @brief Get own addr type and address used for LE for next scan/advertisement/connect operation
     */
    void gap_le_get_own_address(uint8_t * addr_type, bd_addr_t addr);
    
    /**
     * @brief Get own addr type and address used for LE advertisements (Peripheral)
     */
    void gap_le_get_own_advertisements_address(uint8_t * addr_type, bd_addr_t addr);
    
    /**
     * @brief Get own addr type and address used for LE Extended Advertisiing (Peripheral)
     */
    void gap_le_get_own_advertising_set_address(uint8_t * addr_type, bd_addr_t addr, uint8_t advertising_handle);
    
    /**
     * @brief Get own addr type and address used for LE connections (Central)
     */
    void gap_le_get_own_connection_address(uint8_t * addr_type, bd_addr_t addr);
    
    /**
     * @brief Get state of connection re-encryption for bonded devices when in central role
     * @note used by gatt_client and att_server to wait for re-encryption
     * @param con_handle
     * @return 1 if security setup is active
     */
    bool gap_reconnect_security_setup_active(hci_con_handle_t con_handle);
    
    /**
     * @brief Delete bonding information for remote device
     * @note On most desktop ports, the LE Device DB uses a TLV and there is one TLV storage per
     *       Controller resp. its Bluetooth Address. As the Bluetooth Address is retrieved during
     *       power up, this function only works, when the stack is in working state for these ports.
     * @param address_type
     * @param address
     */
    void gap_delete_bonding(bd_addr_type_t address_type, bd_addr_t address);
    
    /**
     * LE Privacy 1.2 - requires support by Controller and ENABLE_LE_RESOLVING_LIST to be defined
     */
    
    /**
     * Set Privacy Mode for use in Resolving List. Default: LE_PRIVACY_MODE_DEVICE
     * @note Only applies for new devices added to resolving list, please call before startup
     * @param privacy_mode
     */
    void gap_set_peer_privacy_mode(le_privacy_mode_t privacy_mode );
    
    /**
     * @brief Load LE Device DB entries into Controller Resolving List to allow filtering on
     *        bonded devies with resolvable private addresses
     * @return EROOR_CODE_SUCCESS if supported by Controller
     */
    uint8_t gap_load_resolving_list_from_le_device_db(void);
    
    /**
     * @brief Register callback that gets executed during random address update
     * @note gap_privacy_client_ready needs to be called after callback is received
     * @param client
     * @return status
     */
    void gap_privacy_client_register(gap_privacy_client_t * client);
    
    /**
     * @brief Acknowledge upcoming random address change
     * @param client
     * @return status
     */
    void gap_privacy_client_ready(gap_privacy_client_t * client);
    
    /**
     * @brief Unregister callback from random address updates
     * @param client
     * @return status
     */
    void gap_privacy_client_unregister(gap_privacy_client_t * client);
    
    /**
     * @brief Get local persistent IRK
     */
    const uint8_t * gap_get_persistent_irk(void);
    

## Host Controler Interface (HCI) API {#sec:hci_api}


**hci.h** 


    
        
    // HCI init and configuration
    
    
    /**
     * @brief Set up HCI. Needs to be called before any other function.
     */
    void hci_init(const hci_transport_t *transport, const void *config);
    
    /**
     * @brief Configure Bluetooth chipset driver. Has to be called before power on, or right after receiving the local version information.
     */
    void hci_set_chipset(const btstack_chipset_t *chipset_driver);
    
    /**
     * @brief Enable custom init for chipset driver to send HCI commands before HCI Reset
     */
    void hci_enable_custom_pre_init(void);
    
    /**
     * @brief Configure Bluetooth hardware control. Has to be called before power on.
     * @[aram hardware_control implementation
     */
    void hci_set_control(const btstack_control_t *hardware_control);
    
    #ifdef HAVE_SCO_TRANSPORT
    /**
     * @brief Set SCO Transport implementation for SCO over PCM mode
     * @param sco_transport that sends SCO over I2S or PCM interface
     */
    void hci_set_sco_transport(const btstack_sco_transport_t *sco_transport);
    #endif
    
    #ifdef ENABLE_CLASSIC
    /**
     * @brief Configure Bluetooth hardware control. Has to be called before power on.
     */
    void hci_set_link_key_db(btstack_link_key_db_t const * link_key_db);
    #endif
    
    /**
     * @brief Set callback for Bluetooth Hardware Error
     */
    void hci_set_hardware_error_callback(void (*fn)(uint8_t error));
    
    /**
     * @brief Set Public BD ADDR - passed on to Bluetooth chipset during init if supported in bt_control_h
     */
    void hci_set_bd_addr(bd_addr_t addr);
    
    /**
     * @brief Configure Voice Setting for use with SCO data in HSP/HFP
     */
    void hci_set_sco_voice_setting(uint16_t voice_setting);
    
    /**
     * @brief Get SCO Voice Setting
     * @return current voice setting
     */
    uint16_t hci_get_sco_voice_setting(void);
    
    /**
     * @brief Set number of ISO packets to buffer for BIS/CIS
     * @param num_packets (default = 1)
     */
    void hci_set_num_iso_packets_to_queue(uint8_t num_packets);
    
    /**
     * @brief Set inquiry mode: standard, with RSSI, with RSSI + Extended Inquiry Results. Has to be called before power on.
     * @param inquriy_mode see bluetooth_defines.h
     */
    void hci_set_inquiry_mode(inquiry_mode_t inquriy_mode);
    
    /**
     * @brief Requests the change of BTstack power mode.
     * @param power_mode
     * @return 0 if success, otherwise error
     */
    int  hci_power_control(HCI_POWER_MODE power_mode);
    
    /**
     * @brief Shutdown HCI
     */
    void hci_close(void);
    
    
    // Callback registration
    
    
    /**
     * @brief Add event packet handler. 
     */
    void hci_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Remove event packet handler.
     */
    void hci_remove_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Registers a packet handler for ACL data. Used by L2CAP
     */
    void hci_register_acl_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief Registers a packet handler for SCO data. Used for HSP and HFP profiles.
     */
    void hci_register_sco_packet_handler(btstack_packet_handler_t handler);
    
    /**
     * @brief Registers a packet handler for ISO data. Used for LE Audio profiles
     */
    void hci_register_iso_packet_handler(btstack_packet_handler_t handler);
    
    // Sending HCI Commands
    
    /** 
     * @brief Check if CMD packet can be sent to controller
     * @return true if command can be sent
     */
    bool hci_can_send_command_packet_now(void);
    
    /**
     * @brief Creates and sends HCI command packets based on a template and a list of parameters. Will return error if outgoing data buffer is occupied.
     * @return status
     */
    uint8_t hci_send_cmd(const hci_cmd_t * cmd, ...);
    
    
    // Sending SCO Packets
    
    /** @brief Get SCO payload length for existing SCO connection and current SCO Voice setting
     *  @note  Using SCO packets of the exact length is required for USB transfer in general and some H4 controllers as well
     *  @param sco_con_handle
     *  @return Length of SCO payload in bytes (not audio frames) incl. 3 byte header
     */
    uint16_t hci_get_sco_packet_length_for_connection(hci_con_handle_t sco_con_handle);
    
    /** @brief Get SCO packet length for one of the existing SCO connections and current SCO Voice setting
     *  @deprecated Please use hci_get_sco_packet_length_for_connection instead
     *  @note  Using SCO packets of the exact length is required for USB transfer
     *  @return Length of SCO packets in bytes (not audio frames) incl. 3 byte header
     */
    uint16_t hci_get_sco_packet_length(void);
    
    /**
     * @brief Request emission of HCI_EVENT_SCO_CAN_SEND_NOW as soon as possible
     * @note HCI_EVENT_SCO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it.
     * @param con_handle
     */
    void hci_request_sco_can_send_now_event_for_con_handle(hci_con_handle_t con_handle);
    
    /**
     * @brief Request emission of HCI_EVENT_SCO_CAN_SEND_NOW as soon as possible
     * @deprecated Please use hci_request_sco_can_send_now_event_for_con_handle instead
     * @note HCI_EVENT_SCO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it. con_handle will
     *       be set to HCI_CON_HANDLE_INVALID in the event
     */
    void hci_request_sco_can_send_now_event(void);
    
    /**
     * @brief Check HCI packet buffer and if SCO packet can be sent to controller
     * @param con_handle
     * @return true if sco packet can be sent
     */
    bool hci_can_send_sco_packet_now_for_con_handle(hci_con_handle_t con_handle);
    
    /**
     * @brief Check HCI packet buffer and if SCO packet can be sent to controller
     * @deprecated Please use hci_can_send_sco_packet_now_for_con_handle instead
     * @return true if sco packet can be sent
     */
    bool hci_can_send_sco_packet_now(void);
    
    /**
     * @brief Check if SCO packet can be sent to controller
     * @return true if sco packet can be sent
     */
    bool hci_can_send_prepared_sco_packet_now(void);
    
    /**
     * @brief Send SCO packet prepared in HCI packet buffer
     */
    uint8_t hci_send_sco_packet_buffer(int size);
    
    /**
     * @brief Request emission of HCI_EVENT_BIS_CAN_SEND_NOW for all BIS as soon as possible
     * @param big_handle
     * @note HCI_EVENT_ISO_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     */
    uint8_t hci_request_bis_can_send_now_events(uint8_t big_handle);
    
    /**
     * @brief Request emission of HCI_EVENT_CIS_CAN_SEND_NOW for all outgoing CIS of referenced CIG
     *
     * @note As CIG_IDs are only unique for Central, a CIS Connection Handle is used to identify the CIG.
     *       The group_complete field in HCI_EVENT_CIS_CAN_SEND_NOW can be used to request again.
     *
     * @note HCI_EVENT_CIS_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     *
     * @param cis_con_handle
     */
    uint8_t hci_request_cis_can_send_now_events(hci_con_handle_t cis_con_handle);
    
    /**
     * @brief Send ISO packet prepared in HCI packet buffer
     */
    uint8_t hci_send_iso_packet_buffer(uint16_t size);
    
    /**
     * Reserves outgoing packet buffer.
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     */
    void hci_reserve_packet_buffer(void);
    
    /**
     * Get pointer for outgoing packet buffer
     */
    uint8_t* hci_get_outgoing_packet_buffer(void);
    
    /**
     * Release outgoing packet buffer\
     * @note only called instead of hci_send_prepared
     */
    void hci_release_packet_buffer(void);
    
    /**
    * @brief Sets the master/slave policy
    * @param policy (0: attempt to become master, 1: let connecting device decide)
    */
    void hci_set_master_slave_policy(uint8_t policy);
    
    /**
     * @brief Check if Controller supports BR/EDR (Bluetooth Classic)
     * @return true if supported
     * @note only valid in working state
     */
    bool hci_classic_supported(void);
    
    /**
     * @brief Check if Controller supports LE (Bluetooth Low Energy)
     * @return true if supported
     * @note only valid in working state
     */
    bool hci_le_supported(void);
    
    /**
     * @brief Check if LE Extended Advertising is supported
     * @return true if supported
     */
    bool hci_le_extended_advertising_supported(void);
    
    /** @brief Check if address type corresponds to LE connection
     *  @bparam address_type
     *  @erturn true if LE connection
     */
    bool hci_is_le_connection_type(bd_addr_type_t address_type);
    
    /** @brief Check if address type corresponds to Identity Address
     *  @bparam address_type
     *  @erturn true if LE connection
     */
    bool hci_is_le_identity_address_type(bd_addr_type_t address_type);
    

## HCI Logging API {#sec:hci_dump_api}


**hci_dump.h** : 
Dump HCI trace as BlueZ's hcidump format, Apple's PacketLogger, or stdout.



    
    typedef enum {
        HCI_DUMP_INVALID = 0,
        HCI_DUMP_BLUEZ,
        HCI_DUMP_PACKETLOGGER,
        HCI_DUMP_BTSNOOP,
    } hci_dump_format_t;
    
    typedef struct {
        // reset output, called if max packets is reached, to limit file size
        void (*reset)(void);
        // log packet
        void (*log_packet)(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len);
        // log message
        void (*log_message)(int log_level, const char * format, va_list argptr);
    #ifdef __AVR__ \
        // log message - AVR
        void (*log_message_P)(int log_level, PGM_P * format, va_list argptr);
    #endif
    } hci_dump_t;
    
    /**
     * @brief Init HCI Dump
     * @param hci_dump_impl - platform-specific implementation
     */
    void hci_dump_init(const hci_dump_t * hci_dump_impl);
    
    /**
     * @brief Enable packet logging
     * @param enabled default: true
     */
    void hci_dump_enable_packet_log(bool enabled);
    
    /**
     * @brief
     */
    void hci_dump_enable_log_level(int log_level, int enable);
    
    /**
     * @brief Set max number of packets - output file might be truncated
     * @param packets - number of packets to log, -1 for unlimited
     */
    void hci_dump_set_max_packets(int packets);
    
    /**
     * @brief Dump Packet
     * @param packet_type
     * @param in is 1 for incoming, 0 for outoing
     * @param packet
     * @param len
     */
    void hci_dump_packet(uint8_t packet_type, uint8_t in, uint8_t *packet, uint16_t len);
    
    /**
     * @brief Dump Message
     * @param log_level
     * @param format
     */
    void hci_dump_log(int log_level, const char * format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 2, 3)))
    #endif
    ;
    
    #ifdef __AVR__
    /*
     * @brief Dump Message using format string stored in PGM memory (allows to save RAM)
     * @param log_level
     * @param format
     */
    void hci_dump_log_P(int log_level, PGM_P format, ...)
    #ifdef __GNUC__
    __attribute__ ((format (__printf__, 2, 3)))
    #endif
    ;
    #endif
    
    /**
     * @brief Dump internal BTstack event
     * @note only logged if ENABLE_LOG_BTSTACK_EVENTS is defined
     * @param packet
     * @param len
     */
    void hci_dump_btstack_event(const uint8_t *packet, uint16_t len);
    
    /**
     * @brief Setup header for PacketLogger format
     * @param buffer
     * @param tv_sec
     * @param tv_us
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_packetlogger(uint8_t * buffer, uint32_t tv_sec, uint32_t tv_us, uint8_t packet_type, uint8_t in, uint16_t len);
    
    /**
     * @brief Setup header for BLUEZ (hcidump) format
     * @param buffer
     * @param tv_sec
     * @param tv_us
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_bluez(uint8_t * buffer, uint32_t tv_sec, uint32_t tv_us, uint8_t packet_type, uint8_t in, uint16_t len);
    
    /**
     * @brief Setup header for BT Snoop format
     * @param buffer
     * @param ts_usec_high upper 32-bit of 64-bit microsecond timestamp
     * @param ts_usec_low  lower 2-bit of 64-bit microsecond timestamp
     * @param cumulative_drops since last packet was recorded
     * @param packet_type
     * @param in
     * @param len
     */
    void hci_dump_setup_header_btsnoop(uint8_t * buffer, uint32_t ts_usec_high, uint32_t ts_usec_low, uint32_t cumulative_drops, uint8_t packet_type, uint8_t in, uint16_t len);
    

## HCI Transport API {#sec:hci_transport_api}


**hci_transport.h** : 
The API allows BTstack to use different transport interfaces.



    
    /* HCI packet types */
    typedef struct {
        /**
         * transport name 
         */
        const char * name;
    
        /**
         * init transport
         * @param transport_config
         */
        void   (*init) (const void *transport_config);
    
        /**
         * open transport connection
         */
        int    (*open)(void);
    
        /**
         * close transport connection
         */
        int    (*close)(void);
    
        /**
         * register packet handler for HCI packets: ACL, SCO, and Events
         */
        void   (*register_packet_handler)(void (*handler)(uint8_t packet_type, uint8_t *packet, uint16_t size));
    
        /**
         * support async transport layers, e.g. IRQ driven without buffers
         */
        int    (*can_send_packet_now)(uint8_t packet_type);
    
        /**
         * send packet
         */
        int    (*send_packet)(uint8_t packet_type, uint8_t *packet, int size);
    
        /**
         * extension for UART transport implementations
         */
        int    (*set_baudrate)(uint32_t baudrate);
    
        /**
         * extension for UART H5 on CSR: reset BCSP/H5 Link
         */
        void   (*reset_link)(void);
    
        /**
         * extension for USB transport implementations: config SCO connections
         */
        void   (*set_sco_config)(uint16_t voice_setting, int num_connections);
    
    } hci_transport_t;
    
    typedef enum {
        HCI_TRANSPORT_CONFIG_UART,
        HCI_TRANSPORT_CONFIG_USB,
        HCI_TRANSPORT_CONFIG_LINUX,
    } hci_transport_config_type_t;
    
    typedef struct {
        hci_transport_config_type_t type;
    } hci_transport_config_t;
    
    typedef struct {
        hci_transport_config_type_t type; // == HCI_TRANSPORT_CONFIG_UART
        uint32_t   baudrate_init;         // initial baud rate
        uint32_t   baudrate_main;         // = 0: same as initial baudrate
        int        flowcontrol;           //
        const char *device_name;
        int        parity;                // see btstack_uart.h BTSTACK_UART_PARITY
    } hci_transport_config_uart_t;
    
    typedef struct {
        hci_transport_config_type_t type; // == HCI_TRANSPORT_CONFIG_LINUX
        int device_id;                    // e.g. 0 for HCI0
    } hci_transport_config_linux_t;
    

## HCI Transport EM9304 API API {#sec:hci_transport_em9304_spi_api}


**hci_transport_em9304_spi.h** : 
The EM9304 uses an extended SPI interface and this HCI Transport is based on the the btstack_em9304.h interface.



    
    /*
     * @brief Setup H4 over SPI instance for EM9304 with em9304_spi_driver
     * @param em9304_spi_driver to use
     */
    const hci_transport_t * hci_transport_em9304_spi_instance(const btstack_em9304_spi_t * em9304_spi_driver);
    

## HCI Transport H4 API {#sec:hci_transport_h4_api}


**hci_transport_h4.h** 


    
    /*
     * @brief Setup H4 instance with btstack_uart implementation
     * @param btstack_uart_block_driver to use
     */
    const hci_transport_t * hci_transport_h4_instance_for_uart(const btstack_uart_t * uart_driver);
    
    /*
     * @brief Setup H4 instance with btstack_uart_block implementation
     * @param btstack_uart_block_driver to use
     * @deprecated use hci_transport_h4_instance_for_uart instead
     */
    const hci_transport_t * hci_transport_h4_instance(const btstack_uart_block_t * uart_driver);
    

## HCI Transport H5 API {#sec:hci_transport_h5_api}


**hci_transport_h5.h** 


    
    /*
     * @brief Setup H5 instance with btstack_uart implementation that supports SLIP frames
     * @param uart_driver to use
     */
    const hci_transport_t * hci_transport_h5_instance(const btstack_uart_t * uart_driver);
    
    /*
     * @brief Enable H5 Low Power Mode: enter sleep mode after x ms of inactivity
     * @param inactivity_timeout_ms or 0 for off
     */
    void hci_transport_h5_set_auto_sleep(uint16_t inactivity_timeout_ms);
    
    /*
     * @brief Enable BSCP mode H5, by enabling event parity
     * @deprecated Parity can be enabled in UART driver configuration
     */
    void hci_transport_h5_enable_bcsp_mode(void);
    

## HCI Transport USB API {#sec:hci_transport_usb_api}


**hci_transport_usb.h** 


    
    /*
     * @brief
     */
    const hci_transport_t * hci_transport_usb_instance(void);
    
    /**
     * @brief Specify USB Bluetooth device via port numbers from root to device
     */
    void hci_transport_usb_set_path(int len, uint8_t * port_numbers);
    
    /**
     * @brief Specify USB Bluetooth device via bus and port numbers from hub to device
     */
    void hci_transport_usb_set_bus_and_path(uint8_t bus, int len, uint8_t* port_numbers);
    
    /**
     * @brief Add device to list of known Bluetooth USB Controller
     * @param vendor_id
     * @param product_id
     */
    void hci_transport_usb_add_device(uint16_t vendor_id, uint16_t product_id);
    

## L2CAP API {#sec:l2cap_api}


**l2cap.h** : 
Logical Link Control and Adaption Protocol 



    
    //
    // PSM numbers from https://www.bluetooth.com/specifications/assigned-numbers/logical-link-control 
    //
    #define PSM_SDP           BLUETOOTH_PROTOCOL_SDP
    #define PSM_RFCOMM        BLUETOOTH_PROTOCOL_RFCOMM
    #define PSM_BNEP          BLUETOOTH_PROTOCOL_BNEP
    // @TODO: scrape PSMs Bluetooth SIG site and put in bluetooth_psm.h or bluetooth_l2cap.h
    #define PSM_HID_CONTROL   0x11
    #define PSM_HID_INTERRUPT 0x13
    #define PSM_ATT           0x1f
    #define PSM_IPSP          0x23
    
    /** 
     * @brief Set up L2CAP and register L2CAP with HCI layer.
     */
    void l2cap_init(void);
    
    /**
     * @brief Add event packet handler for LE Connection Parameter Update events
     */
    void l2cap_add_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /**
     * @brief Remove event packet handler.
     */
    void l2cap_remove_event_handler(btstack_packet_callback_registration_t * callback_handler);
    
    /** 
     * @brief Get max MTU for Classic connections based on btstack configuration
     */
    uint16_t l2cap_max_mtu(void);
    
    /** 
     * @brief Get max MTU for LE connections based on btstack configuration
     */
    uint16_t l2cap_max_le_mtu(void);
    
    /**
    * @brief Set the max MTU for LE connections, if not set l2cap_max_mtu() will be used.
    */
    void l2cap_set_max_le_mtu(uint16_t max_mtu);
    
    /** 
     * @brief Creates L2CAP channel to the PSM of a remote device with baseband address. A new baseband connection will be initiated if necessary.
     * @param packet_handler
     * @param address
     * @param psm
     * @param mtu
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm, uint16_t mtu, uint16_t * out_local_cid);
    
    /** 
     * @brief Disconnects L2CAP channel with given identifier. 
     * @param local_cid
     * @return status ERROR_CODE_SUCCESS if successful or L2CAP_LOCAL_CID_DOES_NOT_EXIST
     */
    uint8_t l2cap_disconnect(uint16_t local_cid);
    
    /** 
     * @brief Queries the maximal transfer unit (MTU) for L2CAP channel with given identifier. 
     */
    uint16_t l2cap_get_remote_mtu_for_local_cid(uint16_t local_cid);
    
    /** 
     * @brief Sends L2CAP data packet to the channel with given identifier.
     * @note For channel in credit-based flow control mode, data needs to stay valid until .. event
     * @param local_cid
     * @param data to send
     * @param len of data
     * @return status
     */
    uint8_t l2cap_send(uint16_t local_cid, const uint8_t *data, uint16_t len);
    
    /** 
     * @brief Registers L2CAP service with given PSM and MTU, and assigns a packet handler. 
     * @param packet_handler
     * @param psm
     * @param mtu
     * @param security_level
     * @return status ERROR_CODE_SUCCESS if successful, otherwise L2CAP_SERVICE_ALREADY_REGISTERED or BTSTACK_MEMORY_ALLOC_FAILED
     */
    uint8_t l2cap_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t mtu, gap_security_level_t security_level);
    
    /** 
     * @brief Unregisters L2CAP service with given PSM.
     */
    uint8_t l2cap_unregister_service(uint16_t psm);
    
    /** 
     * @brief Accepts incoming L2CAP connection.
     */
    void l2cap_accept_connection(uint16_t local_cid);
    
    /** 
     * @brief Deny incoming L2CAP connection.
     */
    void l2cap_decline_connection(uint16_t local_cid);
    
    /** 
     * @brief Check if outgoing buffer is available and that there's space on the Bluetooth module
     * @return true if packet can be sent
     */
    bool l2cap_can_send_packet_now(uint16_t local_cid);
    
    /** 
     * @brief Request emission of L2CAP_EVENT_CAN_SEND_NOW as soon as possible
     * @note L2CAP_EVENT_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_request_can_send_now_event(uint16_t local_cid);
    
    /** 
     * @brief Reserve outgoing buffer
     * @note Only for L2CAP Basic Mode Channels
     * @note Must only be called after a 'can send now' check or event
     * @note Asserts if packet buffer is already reserved
     */
    void l2cap_reserve_packet_buffer(void);
    
    /** 
     * @brief Get outgoing buffer and prepare data.
     * @note Only for L2CAP Basic Mode Channels
     */
    uint8_t *l2cap_get_outgoing_buffer(void);
    
    /** 
     * @brief Send L2CAP packet prepared in outgoing buffer to channel
     * @note Only for L2CAP Basic Mode Channels
     */
    uint8_t l2cap_send_prepared(uint16_t local_cid, uint16_t len);
    
    /** 
     * @brief Release outgoing buffer (only needed if l2cap_send_prepared is not called)
     * @note Only for L2CAP Basic Mode Channels
     */
    void l2cap_release_packet_buffer(void);
    
    //
    // Connection-Oriented Channels in Enhanced Retransmission Mode - ERTM
    //
    
    /**
     * @brief Creates L2CAP channel to the PSM of a remote device with baseband address using Enhanced Retransmission Mode.
     *        A new baseband connection will be initiated if necessary.
     * @param packet_handler
     * @param address
     * @param psm
     * @param ertm_config
     * @param buffer to store reassembled rx packet, out-of-order packets and unacknowledged outgoing packets with their tretransmission timers
     * @param size of buffer
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_create_channel(btstack_packet_handler_t packet_handler, bd_addr_t address, uint16_t psm,
                                      l2cap_ertm_config_t * ertm_contig, uint8_t * buffer, uint32_t size, uint16_t * out_local_cid);
    
    /**
     * @brief Accepts incoming L2CAP connection for Enhanced Retransmission Mode
     * @param local_cid
     * @param ertm_config
     * @param buffer to store reassembled rx packet, out-of-order packets and unacknowledged outgoing packets with their tretransmission timers
     * @param size of buffer
     * @return status
     */
    uint8_t l2cap_ertm_accept_connection(uint16_t local_cid, l2cap_ertm_config_t * ertm_contig, uint8_t * buffer, uint32_t size);
    
    /**
     * @brief Deny incoming incoming L2CAP connection for Enhanced Retransmission Mode
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_decline_connection(uint16_t local_cid);
    
    /**
     * @brief ERTM Set channel as busy.
     * @note Can be cleared by l2cap_ertm_set_ready
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_set_busy(uint16_t local_cid);
    
    /**
     * @brief ERTM Set channel as ready
     * @note Used after l2cap_ertm_set_busy
     * @param local_cid
     * @return status
     */
    uint8_t l2cap_ertm_set_ready(uint16_t local_cid);
    
    
    //
    // L2CAP Connection-Oriented Channels in LE Credit-Based Flow-Control Mode - CBM
    //
    
    /**
     * @brief Register L2CAP service in LE Credit-Based Flow-Control Mode
     * @note MTU and initial credits are specified in l2cap_cbm_accept_connection(..) call
     * @param packet_handler
     * @param psm
     * @param security_level
     */
    uint8_t l2cap_cbm_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, gap_security_level_t security_level);
    
    /**
     * @brief Unregister L2CAP service in LE Credit-Based Flow-Control Mode
     * @param psm
     */
    
    uint8_t l2cap_cbm_unregister_service(uint16_t psm);
    
    /*
     * @brief Accept incoming connection LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param receive_buffer        buffer used for reassembly of L2CAP LE Information Frames into service data unit (SDU) with given MTU
     * @param receive_buffer_size   buffer size equals MTU
     * @param initial_credits       Number of initial credits provided to peer or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     */
    
    uint8_t l2cap_cbm_accept_connection(uint16_t local_cid, uint8_t * receive_sdu_buffer, uint16_t mtu, uint16_t initial_credits);
    
    /** 
     * @brief Deecline connection in LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param result                result, see L2CAP_CBM_CONNECTION_RESULT_SUCCESS in bluetooth.h
     */
    
    uint8_t l2cap_cbm_decline_connection(uint16_t local_cid, uint16_t result);
    
    /**
     * @brief Create outgoing channel in LE Credit-Based Flow-Control Mode
     * @param packet_handler        Packet handler for this connection
     * @param con_handle            HCI Connection Handle, LE transport
     * @param psm                   Service PSM to connect to
     * @param receive_buffer        buffer used for reassembly of L2CAP LE Information Frames into service data unit (SDU) with given MTU
     * @param receive_buffer_size   buffer size equals MTU
     * @param initial_credits       Number of initial credits provided to peer or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param security_level        Minimum required security level
     * @param out_local_cid         L2CAP LE Channel Identifier is stored here
     */
    uint8_t l2cap_cbm_create_channel(btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle,
        uint16_t psm, uint8_t * receive_sdu_buffer, uint16_t mtu, uint16_t initial_credits, gap_security_level_t security_level,
        uint16_t * out_local_cid);
    
    /**
     * @brief Provide credits for channel in LE Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param credits               Number additional credits for peer
     */
    uint8_t l2cap_cbm_provide_credits(uint16_t local_cid, uint16_t credits);
    
    /**
     * @brief Returns the number of credits provided by peer
     * @param local_cid
     * @return number of credits
     */
    uint16_t l2cap_cbm_available_credits(uint16_t local_cid);
    
    //
    // L2CAP Connection-Oriented Channels in Enhanced Credit-Based Flow-Control Mode - ECBM
    //
    
    /**
     * @brief Register L2CAP service in Enhanced Credit-Based Flow-Control Mode
     * @note MTU and initial credits are specified in l2cap_enhanced_accept_connection(..) call
     * @param packet_handler
     * @param psm
     * @param min_remote_mtu
     * @param security_level
     * @oaram authorization_required
     * @return status
     */
    uint8_t l2cap_ecbm_register_service(btstack_packet_handler_t packet_handler, uint16_t psm, uint16_t min_remote_mtu,
                                        gap_security_level_t security_level, bool authorization_required);
    
    /**
     * @brief Unregister L2CAP service in Enhanced Credit-Based Flow-Control Mode
     * @param psm
     * @return status
     */
    
    uint8_t l2cap_ecbm_unregister_service(uint16_t psm);
    
    /**
     * @brief Set Minimal MPS for channel in Enhanced Credit-Based Flow-Control Mode
     * @param mps_min
     */
    void l2cap_ecbm_mps_set_min(uint16_t mps_min);
    
    /**
     * @brief Set Minimal MPS for channel in Enhanced Credit-Based Flow-Control Mode
     * @param mps_max
     */
    void l2cap_ecbm_mps_set_max(uint16_t mps_max);
    
    /**
     * @brief Create outgoing channel in Enhanced Credit-Based Flow-Control Mode
     * @note receive_buffer points to an array of receive buffers with num_channels elements
     * @note out_local_cid points to an array where CID is stored with num_channel elements
     * @param packet_handler        Packet handler for this connection
     * @param con_handle            HCI Connection Handle
     * @param security_level        Minimum required security level
     * @param psm                   Service PSM to connect to
     * @param num_channels          number of channels to create
     * @param initial_credits       Number of initial credits provided to peer per channel or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param receive_buffer_size   buffer size equals MTU
     * @param receive_buffers       Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @param out_local_cids        Array of L2CAP Channel Identifiers is stored here on success
     * @return status
     */
    uint8_t l2cap_ecbm_create_channels(btstack_packet_handler_t packet_handler, hci_con_handle_t con_handle,
                                           gap_security_level_t security_level,
                                           uint16_t psm, uint8_t num_channels, uint16_t initial_credits, uint16_t receive_buffer_size,
                                           uint8_t ** receive_buffers, uint16_t * out_local_cids);
    
    /**
     * @brief  Accept incoming connection Enhanced Credit-Based Flow-Control Mode
     * @param local_cid            from L2CAP_EVENT_INCOMING_DATA_CONNECTION
     * @param num_channels
     * @param initial_credits      Number of initial credits provided to peer per channel or L2CAP_LE_AUTOMATIC_CREDITS to enable automatic credits
     * @param receive_buffer_size
     * @param receive_buffers      Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @param out_local_cids       Array of L2CAP Channel Identifiers is stored here on success
     * @return status
     */
    uint8_t l2cap_ecbm_accept_channels(uint16_t local_cid, uint8_t num_channels, uint16_t initial_credits,
                                                uint16_t receive_buffer_size, uint8_t ** receive_buffers, uint16_t * out_local_cids);
    /**
     * @brief Decline connection in Enhanced Credit-Based Flow-Control Mode
     * @param local_cid           from L2CAP_EVENT_INCOMING_DATA_CONNECTION
     * @param result              See L2CAP_ECBM_CONNECTION_RESULT_ALL_SUCCESS in bluetooth.h
     * @return status
     */
    uint8_t l2cap_ecbm_decline_channels(uint16_t local_cid, uint16_t result);
    
    /**
     * @brief Provide credits for channel in Enhanced Credit-Based Flow-Control Mode
     * @param local_cid             L2CAP Channel Identifier
     * @param credits               Number additional credits for peer
     * @return status
     */
    uint8_t l2cap_ecbm_provide_credits(uint16_t local_cid, uint16_t credits);
    
    /**
     * @brief Request emission of L2CAP_EVENT_ECBM_CAN_SEND_NOW as soon as possible
     * @note L2CAP_EVENT_ECBM_CAN_SEND_NOW might be emitted during call to this function
     *       so packet handler should be ready to handle it
     * @param local_cid             L2CAP Channel Identifier
     * @return status
     */
    uint8_t l2cap_ecbm_request_can_send_now_event(uint16_t local_cid);
    
    /**
     * @brief Reconfigure MPS/MTU of local channels
     * @param num_cids
     * @param local_cids            array of local_cids to reconfigure
     * @param receive_buffer_size   buffer size equals MTU
     * @param receive_buffers       Array of buffers used for reassembly of L2CAP Information Frames into service data unit (SDU) with given MTU
     * @return status
     */
    uint8_t l2cap_ecbm_reconfigure_channels(uint8_t num_cids, uint16_t * local_cids, int16_t receive_buffer_size, uint8_t ** receive_buffers);
    
    /**
     * @brief Trigger pending connection responses after pairing completed
     * @note Must be called after receiving an SM_PAIRING_COMPLETE event, will be removed eventually
     * @param con_handle
     */
    void l2cap_ecbm_trigger_pending_connection_responses(hci_con_handle_t con_handle);
    
    /**
     * @brief Returns the number of outgoing credits provided by peer
     * @param local_cid
     * @return number of credits
     */
    uint16_t l2cap_ecbm_available_credits(uint16_t local_cid);
    
    /**
     * @brief De-Init L2CAP
     */
    void l2cap_deinit(void);
    

## Audio Stream Control Service Client API {#sec:broadcast_audio_scan_service_client_api}


**broadcast_audio_scan_service_client.h** 


    
    typedef enum {
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_IDLE = 0,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W2_QUERY_SERVICE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W4_SERVICE_RESULT,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W2_QUERY_CHARACTERISTICS,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W4_CHARACTERISTIC_RESULT,
    
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W2_QUERY_CHARACTERISTIC_DESCRIPTORS,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W4_CHARACTERISTIC_DESCRIPTORS_RESULT,
    
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W2_REGISTER_NOTIFICATION,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_W4_NOTIFICATION_REGISTERED,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_STATE_READY,
    
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_READ_CHARACTERISTIC_CONFIGURATION,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_CHARACTERISTIC_CONFIGURATION_RESULT,
    
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_START_SCAN,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_START_SCAN,
        
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_STOP_SCAN,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_STOP_SCAN,
        
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_ADD_SOURCE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_ADD_SOURCE,
        
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_MODIFY_SOURCE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_MODIFY_SOURCE,
        
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_SET_BROADCAST_CODE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_SET_BROADCAST_CODE,
        
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_WRITE_CONTROL_POINT_REMOVE_SOURCE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_WRITE_CONTROL_POINT_REMOVE_SOURCE,
    
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W2_READE_RECEIVE_STATE,
        BROADCAST_AUDIO_SCAN_SERVICE_CLIENT_W4_READE_RECEIVE_STATE,
    
    } broadcast_audio_scan_service_client_state_t;
    
    typedef struct {
        // used for add source command
        bass_source_data_t data;
    
        // received via notification
        bool    in_use;
        uint8_t source_id;
        le_audio_big_encryption_t big_encryption;
        uint8_t  bad_code[16];
    
        // characteristic
        uint16_t receive_state_value_handle;
        uint16_t receive_state_ccc_handle;
        uint16_t receive_state_properties;
        uint16_t receive_state_end_handle;
    } bass_client_source_t;
    
    typedef struct {
        btstack_linked_item_t item;
        
        hci_con_handle_t  con_handle;
        uint16_t          cid;
        uint16_t          mtu;
        broadcast_audio_scan_service_client_state_t  state;
        
        // service
        uint16_t start_handle;
        uint16_t end_handle;
        uint16_t control_point_value_handle; 
        
        // used for memory capacity checking
        uint8_t  service_instances_num;
        uint8_t  receive_states_instances_num;
        // used for notification registration
        uint8_t  receive_states_index;
        
        uint8_t max_receive_states_num;
        bass_client_source_t * receive_states;
    
        // used for write segmentation
        uint8_t  buffer[BASS_CLIENT_MAX_ATT_BUFFER_SIZE];
        uint16_t buffer_offset;
        uint16_t data_size;
    
        gatt_client_notification_t notification_listener;
        
        // used for adding and modifying source
        const bass_source_data_t * control_point_operation_data;
        uint8_t control_point_operation_source_id;
        // used for setting the broadcast code
        const uint8_t * broadcast_code;
    } bass_client_connection_t;
    
    /**
     * @brief Init Broadcast Audio Scan Service Client. Register packet handler to receive events:
     * - LEAUDIO_SUBEVENT_BASS_CLIENT_CONNECTED
     * - LEAUDIO_SUBEVENT_BASS_CLIENT_DISCONNECTED
     * - LEAUDIO_SUBEVENT_BASS_CLIENT_SCAN_OPERATION_COMPLETE
     * - LEAUDIO_SUBEVENT_BASS_CLIENT_SOURCE_OPERATION_COMPLETE
     * - LEAUDIO_SUBEVENT_BASS_CLIENT_NOTIFICATION_COMPLETE
     * @param packet_handler for events
     */
    void broadcast_audio_scan_service_client_init(btstack_packet_handler_t packet_handler);
    
    /**
     * @brief Connect to BASS Service on remote device
     * @note LEAUDIO_SUBEVENT_BASS_CLIENT_CONNECTED will be emitted
     * @param connection struct provided by user, needs to stay valid until disconnect event is received
     * @param sources buffer to store information on Broadcast Sources on the service
     * @param num_sources
     * @param con_handle to connect to
     * @param bass_cid connection id for this connection for other functions
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_connect(bass_client_connection_t * connection, bass_client_source_t * sources, uint8_t num_sources, hci_con_handle_t con_handle, uint16_t * bass_cid);
    
    /**
     * @brief Notify BASS Service that scanning has started
     * @param bass_cid
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_scanning_started(uint16_t bass_cid);
    
    /**
     * @brief Notify BASS Service that scanning has stopped
     * @note emits LEAUDIO_SUBEVENT_BASS_CLIENT_SOURCE_OPERATION_COMPLETE
     * @param bass_cid
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_scanning_stopped(uint16_t bass_cid);
    
    /**
     * @brief Add Broadcast Source on service
     * @note LEAUDIO_SUBEVENT_BASS_NOTIFICATION_COMPLETE will contain source_id for other functions
     * @param bass_cid
     * @param add_source_data data to add, needs to stay valid until LEAUDIO_SUBEVENT_BASS_CLIENT_SOURCE_OPERATION_COMPLETE
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_add_source(uint16_t bass_cid, const bass_source_data_t * add_source_data);
    
    /**
     * @brief Modify information about Broadcast Source on service
     * @param bass_cid
     * @param source_id
     * @param modify_source_data data to modify, needs to stay valid until LEAUDIO_SUBEVENT_BASS_CLIENT_SOURCE_OPERATION_COMPLETE
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_modify_source(uint16_t bass_cid, uint8_t source_id, const bass_source_data_t * modify_source_data);
    
    /**
     * @brief Set Broadcast Code for a Broadcast Source to allow remote do decrypt audio stream
     * @param bass_cid
     * @param source_id
     * @param broadcast_code
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_set_broadcast_code(uint16_t bass_cid, uint8_t source_id, const uint8_t * broadcast_code);
    
    /**
     * @brief Remove information about Broadcast Source
     * @param bass_cid
     * @param source_id
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_remove_source(uint16_t bass_cid, uint8_t source_id);
    
    /**
     * @param Provide read-only access to Broadcast Receive State of given Broadcast Source on service
     * @param bass_cid
     * @param source_id
     * @return pointer to source data or NULL, if source_id not found
     */
    const bass_source_data_t * broadcast_audio_scan_service_client_get_source_data(uint16_t bass_cid, uint8_t source_id);
    
    /**
     * @param Get BIG Encryption and Bad Code from Broadcast Receive State of given Broadcast Source on service
     * @param bass_cid
     * @param source_id
     * @param out_big_encryption
     * @param out_bad_code 16-byte buffer
     * @return status
     */
    uint8_t broadcast_audio_scan_service_client_get_encryption_state(uint16_t bass_cid, uint8_t source_id,
                                                                     le_audio_big_encryption_t * out_big_encryption, uint8_t * out_bad_code);
    
    /**
     * @brief Deinit Broadcast Audio Scan Service Client
     */
    void broadcast_audio_scan_service_client_deinit(uint16_t bass_cid);
    

## Broadcast Audio Scan Service Server (BASS) API {#sec:broadcast_audio_scan_service_server_api}


**broadcast_audio_scan_service_server.h** : 
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
    

## Volume Offset Control Service Server API {#sec:le_audio_api}


**le_audio.h** 



## Broadcast Audio Source Endpoint AD Builder API {#sec:le_audio_base_builder_api}


**le_audio_base_builder.h** 


## Broadcast Audio Source Endpoint AD Parser API {#sec:le_audio_base_parser_api}


**le_audio_base_parser.h** 


## LE Audio Util API {#sec:le_audio_util_api}


**le_audio_util.h** 



## Mesh Provisioning Service Server API {#sec:mesh_provisioning_service_server_api}


**mesh_provisioning_service_server.h** 


    
    /**
     * @brief Init Mesh Provisioning Service Server with ATT DB
     */
    void mesh_provisioning_service_server_init(void);
    
    /**
     * @brief Send a Proxy PDU message containing Provisioning PDU from a Provisioning Server to a Provisioning Client.
     * @param con_handle 
     * @param proxy_pdu 
     * @param proxy_pdu_size max lenght MESH_PROV_MAX_PROXY_PDU
     */
    void mesh_provisioning_service_server_send_proxy_pdu(uint16_t con_handle, const uint8_t * proxy_pdu, uint16_t proxy_pdu_size);
    
    /**
     * @brief Register callback for the PB-GATT.
     * @param callback
     */
    void mesh_provisioning_service_server_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Request can send now event to send PDU
     * Generates an MESH_SUBEVENT_CAN_SEND_NOW subevent
     * @param con_handle
     */
    void mesh_provisioning_service_server_request_can_send_now(hci_con_handle_t con_handle);

## Mesh Proxy Service Server API {#sec:mesh_proxy_service_server_api}


**mesh_proxy_service_server.h** 


    
    /**
     * @brief Init Mesh Proxy Service Server with ATT DB
     */
    void mesh_proxy_service_server_init(void);
    
    /**
     * @brief Send a Proxy PDU message containing proxy PDU from a proxy Server to a proxy Client.
     * @param con_handle 
     * @param proxy_pdu 
     * @param proxy_pdu_size max lenght MESH_PROV_MAX_PROXY_PDU
     */
    void mesh_proxy_service_server_send_proxy_pdu(uint16_t con_handle, const uint8_t * proxy_pdu, uint16_t proxy_pdu_size);
    
    /**
     * @brief Register callback for the PB-GATT.
     * @param callback
     */
    void mesh_proxy_service_server_register_packet_handler(btstack_packet_handler_t callback);
    
    /**
     * @brief Request can send now event to send PDU
     * Generates an MESH_SUBEVENT_CAN_SEND_NOW subevent
     * @param con_handle
     */
    void mesh_proxy_service_server_request_can_send_now(hci_con_handle_t con_handle);
