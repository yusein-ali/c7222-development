set(ELEC_C7222_BLE_DIR ${CMAKE_CURRENT_LIST_DIR})

if (DEFINED PICO_SDK_PATH AND NOT PICO_SDK_PATH STREQUAL "")
    file(GLOB ELEC_C7222_BLE_SOURCES "${ELEC_C7222_BLE_DIR}/rpi_pico/*.cpp")
else()
    file(GLOB ELEC_C7222_BLE_SOURCES "${ELEC_C7222_BLE_DIR}/grader/*.cpp")
endif()

add_library(ELEC_C7222_BLE INTERFACE)
target_sources(ELEC_C7222_BLE INTERFACE ${ELEC_C7222_BLE_SOURCES})
target_include_directories(ELEC_C7222_BLE INTERFACE "${ELEC_C7222_BLE_DIR}/include")

# Submodules (only GAP for now)
include(${ELEC_C7222_BLE_DIR}/gap/gap.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GAP)
