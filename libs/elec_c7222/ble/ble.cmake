set(ELEC_C7222_BLE_DIR ${CMAKE_CURRENT_LIST_DIR})

if (DEFINED PICO_SDK_PATH AND NOT PICO_SDK_PATH STREQUAL "")
    file(GLOB ELEC_C7222_BLE_SOURCES
        "${ELEC_C7222_BLE_DIR}/rpi_pico/*.cpp"
        "${ELEC_C7222_BLE_DIR}/platform/rpi_pico/*.cpp"
    )
else()
    file(GLOB ELEC_C7222_BLE_SOURCES
        "${ELEC_C7222_BLE_DIR}/grader/*.cpp"
        "${ELEC_C7222_BLE_DIR}/platform/grader/*.cpp"
    )
endif()
file(GLOB ELEC_C7222_BLE_COMMON_SOURCES "${ELEC_C7222_BLE_DIR}/src/*.cpp")

add_library(ELEC_C7222_BLE INTERFACE)
target_sources(ELEC_C7222_BLE INTERFACE 
                    ${ELEC_C7222_BLE_SOURCES} 
                    ${ELEC_C7222_BLE_COMMON_SOURCES})
                    
target_include_directories(ELEC_C7222_BLE INTERFACE "${ELEC_C7222_BLE_DIR}/include")

# Submodules (only GAP for now)
include(${ELEC_C7222_BLE_DIR}/gap/gap.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GAP)
include(${ELEC_C7222_BLE_DIR}/gatt/gatt.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GATT)
include(${ELEC_C7222_BLE_DIR}/security_manager/security_manager.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_SECURITY_MANAGER)
