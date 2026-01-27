
# Device Database TLV API {#sec:le_device_db_tlv_api}


**src/ble/le_device_db_tlv.h** : 
@TLV 'BTD' x


    
    /**
     * @brief configure le device db for use with btstack tlv instance
     * @param btstack_tlv_impl to use
     * @param btstack_tlv_context
     */
    
    void le_device_db_tlv_configure(const btstack_tlv_t * btstack_tlv_impl, void * btstack_tlv_context);
    
