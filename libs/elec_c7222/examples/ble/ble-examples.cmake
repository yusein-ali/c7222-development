set(ELEC_C7222_BLE_EXAMPLES_DIR ${CMAKE_CURRENT_LIST_DIR})

include(${ELEC_C7222_BLE_EXAMPLES_DIR}/gap/ble-gap-example.cmake)
include(${ELEC_C7222_BLE_EXAMPLES_DIR}/gatt-server/ble-gatt-server-example.cmake)
include(${ELEC_C7222_BLE_EXAMPLES_DIR}/security-manager/ble-security-manager-example.cmake)
include(${ELEC_C7222_BLE_EXAMPLES_DIR}/custom-service-rw/ble-custom-service-rw-example.cmake)