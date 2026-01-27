
# Device Database API {#sec:le_device_db_api}


**src/ble/le_device_db.h** 


    
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
    
