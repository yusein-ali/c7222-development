
# AVRCP Media Item Iterator API {#sec:avrcp_media_item_iterator_api}


**src/classic/avrcp_media_item_iterator.h** 


    
    typedef struct avrcp_media_item_context {
         const uint8_t * data;
         uint16_t   offset;
         uint16_t   length;
    } avrcp_media_item_context_t;
    
    // Media item data iterator
    void avrcp_media_item_iterator_init(avrcp_media_item_context_t *context, uint16_t avrcp_media_item_len, const uint8_t * avrcp_media_item_data);
    int  avrcp_media_item_iterator_has_more(const avrcp_media_item_context_t * context);
    void avrcp_media_item_iterator_next(avrcp_media_item_context_t * context);
    
    // Access functions
    uint32_t          avrcp_media_item_iterator_get_attr_id(const avrcp_media_item_context_t * context);
    uint16_t         avrcp_media_item_iterator_get_attr_charset(const avrcp_media_item_context_t * context);
    uint16_t         avrcp_media_item_iterator_get_attr_value_len(const avrcp_media_item_context_t * context);
    const uint8_t *  avrcp_media_item_iterator_get_attr_value(const avrcp_media_item_context_t * context);
    
