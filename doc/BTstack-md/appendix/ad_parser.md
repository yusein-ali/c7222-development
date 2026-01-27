
# AD Data Parser API {#sec:ad_parser_api}


**src/ad_parser.h** : 
AD Data (Advertisements and EIR) Parser


    
    typedef struct ad_context {
         const uint8_t * data;
         uint8_t   offset;
         uint8_t   length;
    } ad_context_t;
    
    // Advertising or Scan Response data iterator
    void ad_iterator_init(ad_context_t *context, uint8_t ad_len, const uint8_t * ad_data);
    bool ad_iterator_has_more(const ad_context_t * context);
    void ad_iterator_next(ad_context_t * context);
    
    // Access functions
    uint8_t         ad_iterator_get_data_type(const ad_context_t * context);
    uint8_t         ad_iterator_get_data_len(const ad_context_t * context);
    const uint8_t * ad_iterator_get_data(const ad_context_t * context);
    
    // convenience function on complete advertisements
    bool ad_data_contains_uuid16(uint8_t ad_len, const uint8_t * ad_data, uint16_t uuid16);
    bool ad_data_contains_uuid128(uint8_t ad_len, const uint8_t * ad_data, const uint8_t * uuid128);
    
