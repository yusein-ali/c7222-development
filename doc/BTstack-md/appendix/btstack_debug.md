
# Debug Messages API {#sec:btstack_debug_api}


**src/btstack_debug.h** : 
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
    
