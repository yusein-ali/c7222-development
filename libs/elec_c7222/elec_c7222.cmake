set(ELEC_C7222_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(ELEC_C7222 INTERFACE)

# Determine whether BLE support is enabled
if (NOT DEFINED C7222_ENABLE_BLE)
    if(DEFINED PICO_BT_ENABLE_BLE)
        set(C7222_ENABLE_BLE ${PICO_BT_ENABLE_BLE})
    elseif(DEFINED ENABLE_BLE)
        set(C7222_ENABLE_BLE ${ENABLE_BLE})
    elseif(NOT DEFINED C7222_ENABLE_BLE)
        option(C7222_ENABLE_BLE "Enable BLE helpers in ELEC_C7222" OFF)
    endif()
endif()

message(STATUS "C7222_ENABLE_BLE is defined: ${C7222_ENABLE_BLE}")
message(STATUS "C7222_ENABLE_BLE_DEBUG is defined: ${C7222_BLE_DEBUG}" )
# Immediate child modules
include(${ELEC_C7222_DIR}/devices/devices.cmake)
include(${ELEC_C7222_DIR}/utils/utils.cmake)

target_link_libraries(ELEC_C7222 INTERFACE
    ELEC_C7222_DEVICES
    ELEC_C7222_UTILS
)

target_include_directories(ELEC_C7222 INTERFACE "${ELEC_C7222_DIR}/include")
target_include_directories(ELEC_C7222 INTERFACE "${ELEC_C7222_DIR}/config")

if(C7222_ENABLE_BLE)
    include(${ELEC_C7222_DIR}/ble/ble.cmake)
    target_link_libraries(ELEC_C7222 INTERFACE ELEC_C7222_BLE)
endif()
