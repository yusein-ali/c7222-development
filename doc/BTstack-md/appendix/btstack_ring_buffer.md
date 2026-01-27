
# Ring Buffer API {#sec:btstack_ring_buffer_api}


**src/btstack_ring_buffer.h** 


    
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
    
