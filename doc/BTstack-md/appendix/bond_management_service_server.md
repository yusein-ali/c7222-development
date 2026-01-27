
# Bond Management Service Server API {#sec:bond_management_service_server_api}


**src/ble/gatt-service/bond_management_service_server.h** 


    #define BOND_MANAGEMENT_CONTROL_POINT_OPCODE_NOT_SUPPORTED 0x80
    #define BOND_MANAGEMENT_OPERATION_FAILED 0x81
    
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_AND_LE                   0x00001
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_AND_LE_WITH_AUTH         0x00002
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC                          0x00004
    #define BMF_DELETE_ACTIVE_BOND_CLASSIC_WITH_AUTH                0x00008
    #define BMF_DELETE_ACTIVE_BOND_LE                               0x00010
    #define BMF_DELETE_ACTIVE_BOND_LE_WITH_AUTH                     0x00020
    #define BMF_DELETE_ALL_BONDS_CLASSIC_AND_LE                     0x00040
    #define BMF_DELETE_ALL_BONDS_CLASSIC_AND_LE_WITH_AUTH           0x00080
    #define BMF_DELETE_ALL_BONDS_CLASSIC                            0x00100
    #define BMF_DELETE_ALL_BONDS_CLASSIC_WITH_AUTH                  0x00200
    #define BMF_DELETE_ALL_BONDS_LE                                 0x00400
    #define BMF_DELETE_ALL_BONDS_LE_WITH_AUTH                       0x00800
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE           0x01000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE_WITH_AUTH 0x02000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC                  0x04000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_WITH_AUTH        0x08000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_LE                       0x10000
    #define BMF_DELETE_ALL_BUT_ACTIVE_BOND_LE_WITH_AUT              0x20000
    
    typedef enum {
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_CLASSIC_AND_LE = 0x01,
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ACTIVE_BOND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_CLASSIC_AND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BONDS_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC_AND_LE,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_CLASSIC,
        BOND_MANAGEMENT_CMD_DELETE_ALL_BUT_ACTIVE_BOND_LE
    } bond_management_cmd_t;
    
    /**
     * @brief Init Bond Management Service with ATT DB
     * @param supported_features
     */
    void bond_management_service_server_init(uint32_t supported_features);
    
    /**
     * @brief Set authorisation string
     * @note String is not copied
     * @param authorisation_string
     */
    void bond_management_service_server_set_authorisation_string(const char * authorisation_string);
    
