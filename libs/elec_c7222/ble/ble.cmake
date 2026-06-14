set(ELEC_C7222_BLE_DIR ${CMAKE_CURRENT_LIST_DIR})

file(GLOB ELEC_C7222_BLE_SOURCES
    "${ELEC_C7222_BLE_DIR}/${C7222_PLATFORM_SOURCE_DIR}/*.cpp"
    "${ELEC_C7222_BLE_DIR}/platform/${C7222_PLATFORM_SOURCE_DIR}/*.cpp"
)

file(GLOB ELEC_C7222_BLE_COMMON_SOURCES "${ELEC_C7222_BLE_DIR}/src/*.cpp")

add_library(ELEC_C7222_BLE INTERFACE)

target_sources(ELEC_C7222_BLE INTERFACE
    ${ELEC_C7222_BLE_SOURCES}
    ${ELEC_C7222_BLE_COMMON_SOURCES}
)

target_include_directories(ELEC_C7222_BLE INTERFACE
    "${ELEC_C7222_BLE_DIR}/include"
)

if(C7222_PLATFORM_USES_PICO_SDK)
    target_include_directories(ELEC_C7222_BLE INTERFACE
        "${ELEC_C7222_BLE_DIR}/platform/${C7222_PLATFORM_SOURCE_DIR}/config"
    )
endif()

include(${ELEC_C7222_BLE_DIR}/gap/gap.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GAP)

include(${ELEC_C7222_BLE_DIR}/gatt/gatt.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GATT)

include(${ELEC_C7222_BLE_DIR}/security_manager/security_manager.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_SECURITY_MANAGER)
