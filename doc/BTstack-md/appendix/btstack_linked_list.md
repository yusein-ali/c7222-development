
# Linked List API {#sec:btstack_linked_list_api}


**src/btstack_linked_list.h** 


    	
    typedef struct btstack_linked_item {
        struct btstack_linked_item *next; // <-- next element in list, or NULL
    } btstack_linked_item_t;
    
    typedef btstack_linked_item_t * btstack_linked_list_t;
    
    typedef struct {
    	int advance_on_next;
        btstack_linked_item_t * prev;	// points to the item before the current one
        btstack_linked_item_t * curr;	// points to the current item (to detect item removal)
    } btstack_linked_list_iterator_t;
    
    
    /**
     * @brief Test if list is empty.
     * @param list
     * @return true if list is empty
     */
    bool btstack_linked_list_empty(btstack_linked_list_t * list);
    
    /**
     * @brief Add item to list as first element.
     * @param list
     * @param item
     * @return true if item was added, false if item already in list
     */
    bool btstack_linked_list_add(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Add item to list as last element.
     * @param list
     * @param item
     * @return true if item was added, false if item already in list
     */
    bool btstack_linked_list_add_tail(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Pop (get + remove) first element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_pop(btstack_linked_list_t * list);
    
    /**
     * @brief Remove item from list
     * @param list
     * @param item
     * @return true if item was removed, false if it is no't in list
     */
    bool btstack_linked_list_remove(btstack_linked_list_t * list, btstack_linked_item_t *item);
    
    /**
     * @brief Get first element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_get_first_item(btstack_linked_list_t * list);
    
    /**
     * @breif Get previous element
     * @param list
     * @param item
     * @return previous element or NULL if not found or element was first in liest
     */
    btstack_linked_item_t * btstack_linked_list_get_previous_item(btstack_linked_list_t * list, btstack_linked_item_t * item);
    
    /**
     * @brief Get last element.
     * @param list
     * @return first element or NULL if list is empty
     */
    btstack_linked_item_t * btstack_linked_list_get_last_item(btstack_linked_list_t * list);
    
    /**
     * @brief Counts number of items in list
     * @return number of items in list
     */
    int btstack_linked_list_count(btstack_linked_list_t * list);
    
    /**
     * @brief Split list into matching and not-matching items based on a criteria
     * @param input list of items
     * @param matches list of matching items
     * @param other all other items
     * @param predicate is called with linked list item and context, returns true if item matches
     * @param context
     */
    void btstack_linked_list_split(btstack_linked_list_t * input,
                                    btstack_linked_list_t * matches,
                                    btstack_linked_list_t * other,
                                    bool (*predicate)(const btstack_linked_item_t *item, void * context), void * context);
    
    /**
     * @brief Filter matching items into second list
     * @param input list of items
     * @param matches list of matching items
     * @param predicate is called with linked list item and context, returns true if item matches
     * @param context
     */
    void btstack_linked_list_filter(btstack_linked_list_t * input,
                                    btstack_linked_list_t * matches,
                                    bool (*predicate)(const btstack_linked_item_t *item, void * context), void * context);
    
    /**
     * @brief Initialize Linked List Iterator
     * @note robust against removal of current element by btstack_linked_list_remove
     * @param it iterator context
     * @param list
     */
    void btstack_linked_list_iterator_init(btstack_linked_list_iterator_t * it, btstack_linked_list_t * list);
    
    /**
     * @brief Has next element
     * @param it iterator context
     * @return true if next element is available
     */
    bool btstack_linked_list_iterator_has_next(btstack_linked_list_iterator_t * it);
    
    /**
     * @brief Get next list eleemnt
     * @param it iterator context
     * @return list element
     */
    btstack_linked_item_t * btstack_linked_list_iterator_next(btstack_linked_list_iterator_t * it);
    
    /**
     * @brief Remove current list element from list
     * @param it iterator context
     */
    void btstack_linked_list_iterator_remove(btstack_linked_list_iterator_t * it);
    
