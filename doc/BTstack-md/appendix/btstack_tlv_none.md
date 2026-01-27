
# Empty TLV Instance API {#sec:btstack_tlv_none_api}


**src/btstack_tlv_none.h** : 
Empty implementation for BTstack's Tag Value Length Persistent Storage implementations
No keys are stored. Can be used as placeholder during porting to new platform. 


    
    /**
     * Init Tag Length Value Store
     * @param context btstack_tlv_none_t 
     * @param hal_flash_bank_impl    of hal_flash_bank interface
     * @Param hal_flash_bank_context of hal_flash_bank_interface
     */
    const btstack_tlv_t * btstack_tlv_none_init_instance(void);
    
