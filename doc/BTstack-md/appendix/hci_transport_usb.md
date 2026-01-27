
# HCI Transport USB API {#sec:hci_transport_usb_api}


**src/hci_transport_usb.h** 


    
    /*
     * @brief
     */
    const hci_transport_t * hci_transport_usb_instance(void);
    
    /**
     * @brief Specify USB Bluetooth device via port numbers from root to device
     */
    void hci_transport_usb_set_path(int len, uint8_t * port_numbers);
    
    /**
     * @brief Specify USB Bluetooth device via bus and port numbers from hub to device
     */
    void hci_transport_usb_set_bus_and_path(uint8_t bus, int len, uint8_t* port_numbers);
    
    /**
     * @brief Add device to list of known Bluetooth USB Controller
     * @param vendor_id
     * @param product_id
     */
    void hci_transport_usb_add_device(uint16_t vendor_id, uint16_t product_id);
    
