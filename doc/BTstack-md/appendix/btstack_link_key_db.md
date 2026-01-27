
# Link Key DB API {#sec:btstack_link_key_db_api}


**src/classic/btstack_link_key_db.h** : 
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
    
