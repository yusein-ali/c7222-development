
# Human Interface Device (HID) API {#sec:btstack_hid_api}


**src/btstack_hid.h** 


    
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
    
