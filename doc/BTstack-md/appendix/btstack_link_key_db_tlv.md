
# Link Key TLV Storage API {#sec:btstack_link_key_db_tlv_api}


**src/classic/btstack_link_key_db_tlv.h** : 
Interface to provide link key storage via BTstack's TLV storage.

@TLV 'BLT' x


    
    /**
     * Init Link Key DB using TLV
     * @param btstack_tlv_impl of btstack_tlv interface
     * @Param btstack_tlv_context of btstack_tlv_interface
     */
    const btstack_link_key_db_t * btstack_link_key_db_tlv_get_instance(const btstack_tlv_t * btstack_tlv_impl, void * btstack_tlv_context);
    
