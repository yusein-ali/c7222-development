
# In-Memory Link Key Storage API {#sec:btstack_link_key_db_memory_api}


**src/classic/btstack_link_key_db_memory.h** 


    
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
    
