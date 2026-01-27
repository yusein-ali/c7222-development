
# Static Link Key Storage API {#sec:btstack_link_key_db_static_api}


**src/classic/btstack_link_key_db_static.h** : 
Static Link Key Storage implementation to use during development/porting:
- Link keys have to be manually added to this file to make them usable
+ Link keys are preserved on reflash in constrast to the program flash based link key store



    
    /*
     * @brief
     */
    const btstack_link_key_db_t * btstack_link_key_db_static_instance(void);
    
