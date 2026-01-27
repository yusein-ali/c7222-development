
# HCI Transport EM9304 API API {#sec:hci_transport_em9304_spi_api}


**src/hci_transport_em9304_spi.h** : 
The EM9304 uses an extended SPI interface and this HCI Transport is based on the the btstack_em9304.h interface.



    
    /*
     * @brief Setup H4 over SPI instance for EM9304 with em9304_spi_driver
     * @param em9304_spi_driver to use
     */
    const hci_transport_t * hci_transport_em9304_spi_instance(const btstack_em9304_spi_t * em9304_spi_driver);
    
