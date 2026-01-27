
# Linked Queue API {#sec:btstack_linked_queue_api}


**src/btstack_linked_queue.h** 


    
    typedef struct {
        btstack_linked_item_t * head;
        btstack_linked_item_t * tail;
    } btstack_linked_queue_t;
    
    /**
     * @brief Tests if queue is empty
     * @return true if emtpy
     */
    bool btstack_linked_queue_empty(btstack_linked_queue_t * queue);
    
    /**
     * @brief Append item to queue
     * @param queue
     * @param item
     */
    void btstack_linked_queue_enqueue(btstack_linked_queue_t * queue, btstack_linked_item_t * item);
    
    /**
     * @brief Pop next item from queue
     * @param queue
     * @return item or NULL if queue empty
     */
    btstack_linked_item_t * btstack_linked_queue_dequeue(btstack_linked_queue_t * queue);
    
    /**
     * @brief Get first item from queue
     * @param queue
     * @return item or NULL if queue empty
     */
    btstack_linked_item_t * btstack_linked_queue_first(btstack_linked_queue_t * queue);
    
