
# SDP Utils API {#sec:sdp_util_api}


**src/classic/sdp_util.h** 


        
    // OFFSETS FOR LOCALIZED ATTRIBUTES - BLUETOOTH_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST
    #define SDP_Offset_ServiceName        0x0000
    #define SDP_Offset_ServiceDescription 0x0001
    #define SDP_Offset_ProviderName       0x0002
    
    typedef enum {
        DE_NIL = 0,
        DE_UINT,
        DE_INT,
        DE_UUID,
        DE_STRING,
        DE_BOOL,
        DE_DES,
        DE_DEA,
        DE_URL
    } de_type_t;
    
    typedef enum {
        DE_SIZE_8 = 0,
        DE_SIZE_16,
        DE_SIZE_32,
        DE_SIZE_64,
        DE_SIZE_128,
        DE_SIZE_VAR_8,
        DE_SIZE_VAR_16,
        DE_SIZE_VAR_32
    } de_size_t;
    
    // MARK: DateElement
    void      de_dump_data_element(const uint8_t * record);
    uint32_t  de_get_len(const uint8_t * header);
    
    
    // @note returned "string" is not NULL terminated
    const uint8_t * de_get_string(const uint8_t * element);
    
    de_size_t de_get_size_type(const uint8_t * header);
    de_type_t de_get_element_type(const uint8_t * header);
    uint32_t  de_get_header_size(const uint8_t * header);
    // returns true if uint16 was read
    bool      de_element_get_uint16(const uint8_t * element, uint16_t * value);
    uint32_t  de_get_data_size(const uint8_t * header);
    uint32_t  de_get_uuid32(const uint8_t * element);
    // returns true if uint16 was read
    bool      de_get_normalized_uuid(uint8_t *uuid128, const uint8_t *element);
    
    void      de_create_sequence(uint8_t * header);
    void      de_store_descriptor_with_len(uint8_t * header, de_type_t type, de_size_t size, uint32_t len);
    uint8_t * de_push_sequence(uint8_t *sequence);
    void      de_pop_sequence(uint8_t * parent, uint8_t * child);
    void      de_add_number(uint8_t *sequence, de_type_t type, de_size_t size, uint32_t value);
    void      de_add_data(uint8_t *sequence, de_type_t type, uint16_t size, uint8_t *data);
    void      de_add_uuid128(uint8_t * sequence, uint8_t * uuid);
    
    // returns data element  len if date element is smaller than size
    uint32_t de_get_len_safe(const uint8_t * header, uint32_t size);
    
    // MARK: DES iterator
    typedef struct {
        uint8_t * element;
        uint16_t pos;
        uint16_t length;
    } des_iterator_t;
    
    bool      des_iterator_init(des_iterator_t * it, uint8_t * element);
    bool      des_iterator_has_more(des_iterator_t * it);
    de_type_t des_iterator_get_type (des_iterator_t * it);
    uint16_t  des_iterator_get_size (des_iterator_t * it);
    uint8_t * des_iterator_get_element(des_iterator_t * it);
    void      des_iterator_next(des_iterator_t * it);
    
    // MARK: SDP
    uint16_t  sdp_append_attributes_in_attributeIDList(uint8_t *record, uint8_t *attributeIDList, uint16_t startOffset, uint16_t maxBytes, uint8_t *buffer);
    uint8_t * sdp_get_attribute_value_for_attribute_id(uint8_t * record, uint16_t attributeID);
    bool      sdp_set_attribute_value_for_attribute_id(uint8_t * record, uint16_t attributeID, uint32_t value);
    bool      sdp_record_matches_service_search_pattern(uint8_t *record, uint8_t *serviceSearchPattern);
    uint16_t  sdp_get_filtered_size(uint8_t *record, uint8_t *attributeIDList);
    bool      sdp_filter_attributes_in_attributeIDList(uint8_t *record, uint8_t *attributeIDList, uint16_t startOffset, uint16_t maxBytes, uint16_t *usedBytes, uint8_t *buffer);
    bool      sdp_attribute_list_contains_id(uint8_t *attributeIDList, uint16_t attributeID);
    bool      sdp_attribute_list_valid(uint8_t *attributeIDList);
    bool      sdp_valid_service_search_pattern(uint8_t *service_search_pattern);
    /*
     * @brief Returns service search pattern for given UUID-16
     * @note Uses fixed buffer
     */
    uint8_t* sdp_service_search_pattern_for_uuid16(uint16_t uuid16);
    
    /*
     * @brief Returns service search pattern for given UUID-128
     * @note Uses fixed buffer
     */
    uint8_t* sdp_service_search_pattern_for_uuid128(const uint8_t * uuid128);
    
