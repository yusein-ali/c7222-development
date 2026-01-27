
# HID Parser API {#sec:btstack_hid_parser_api}


**src/btstack_hid_parser.h** : 
Single-pass HID Report Parser: HID Report is directly parsed without preprocessing HID Descriptor to minimize memory.



    
    // HID Descriptor Iterator
    
    /**
     * @brief Init HID Descriptor iterator
     * @param iterator
     * @param hid_descriptor
     * @param hid_descriptor_len
     */
    void btstack_hid_descriptor_iterator_init(btstack_hid_descriptor_iterator_t * iterator, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len);
    
    /**
     * @brief Check if HID Descriptor Items are available
     * @param iterator
     * @return
     */
    bool btstack_hid_descriptor_iterator_has_more(btstack_hid_descriptor_iterator_t * iterator);
    
    /**
     * @brief Get next HID Descriptor Item
     * @param iterator
     * @return
     */
    const hid_descriptor_item_t * btstack_hid_descriptor_iterator_get_item(btstack_hid_descriptor_iterator_t * iterator);
    
    /**
     * @brief Returns if HID Descriptor was well formed
     * @param iterator
     * @return
     */
    bool btstack_hid_descriptor_iterator_valid(btstack_hid_descriptor_iterator_t * iterator);
    
    
    // HID Descriptor Usage Iterator
    
    /**
     * @brief Initialize usages iterator for HID Descriptor and report type
     * @param parser
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @param hid_report_type
     */
    void btstack_hid_usage_iterator_init(btstack_hid_usage_iterator_t * iterator, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len, hid_report_type_t hid_report_type);
    
    /**
     * @brief Checks if more usages are available
     * @param parser
     */
    bool btstack_hid_usage_iterator_has_more(btstack_hid_usage_iterator_t * iterator);
    
    /**
     * @brief Get current usage item
     * @param parser
     * @param item
     */
    void btstack_hid_usage_iterator_get_item(btstack_hid_usage_iterator_t * iterator, btstack_hid_usage_item_t * item);
    
    
    // HID Report Iterator
    
    /**
     * @brief Initialize HID Parser.
     * @param parser state 
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @param hid_report_type
     * @param hid_report
     * @param hid_report_len
     */
    void btstack_hid_parser_init(btstack_hid_parser_t * parser, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len, hid_report_type_t hid_report_type, const uint8_t * hid_report, uint16_t hid_report_len);
    
    /**
     * @brief Checks if more fields are available
     * @param parser
     */
    bool btstack_hid_parser_has_more(btstack_hid_parser_t * parser);
    
    /**
     * @brief Get next field
     * @param parser
     * @param usage_page
     * @param usage
     * @param value provided in HID report
     */
    void btstack_hid_parser_get_field(btstack_hid_parser_t * parser, uint16_t * usage_page, uint16_t * usage, int32_t * value);
    
    
    /**
     * @brief Parses descriptor and returns report size for given report ID and report type
     * @param report_id or HID_REPORT_ID_UNDEFINED
     * @param report_type
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return report size in bytes or 0 on parsing error
     */
    int btstack_hid_get_report_size_for_id(uint16_t report_id, hid_report_type_t report_type, const uint8_t *hid_descriptor,
                                           uint16_t hid_descriptor_len);
    
    /**
     * @brief Parses descriptor and returns status for given report ID
     * @param report_id
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return status for report id
     */
    hid_report_id_status_t btstack_hid_report_id_valid(uint16_t report_id, const uint8_t * hid_descriptor, uint16_t hid_descriptor_len);
    
    /**
     * @brief Parses descriptor and returns true if report ID found
     * @param hid_descriptor
     * @param hid_descriptor_len
     * @return true if report ID declared in descriptor
     */
    bool btstack_hid_report_id_declared(const uint8_t *hid_descriptor, uint16_t hid_descriptor_len);
    
    
