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
    target_compile_definitions(ELEC_C7222_BLE INTERFACE
        C7222_BLE_HAS_BTSTACK=1
    )

    if(NOT DEFINED PICO_BTSTACK_PATH AND DEFINED PICO_SDK_PATH)
        set(PICO_BTSTACK_PATH "${PICO_SDK_PATH}/lib/btstack")
    endif()
endif()

if(C7222_PLATFORM_USES_PICO_SDK
   AND DEFINED PICO_BTSTACK_PATH
   AND EXISTS "${PICO_BTSTACK_PATH}/platform/freertos/btstack_run_loop_freertos.c")
    target_sources(ELEC_C7222_BLE INTERFACE
        "${PICO_BTSTACK_PATH}/platform/freertos/btstack_run_loop_freertos.c"
    )
    target_include_directories(ELEC_C7222_BLE INTERFACE
        "${PICO_BTSTACK_PATH}/platform/freertos"
    )
    target_compile_definitions(ELEC_C7222_BLE INTERFACE
        C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP=1
    )
    target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_FREERTOS_WRAPPERS)
endif()

set(ELEC_C7222_BLE_PLATFORM_CONFIG_DIR
    "${ELEC_C7222_BLE_DIR}/platform/${C7222_PLATFORM_SOURCE_DIR}/config")

if(EXISTS "${ELEC_C7222_BLE_PLATFORM_CONFIG_DIR}/btstack_config.h")
    target_include_directories(ELEC_C7222_BLE INTERFACE
        "${ELEC_C7222_BLE_PLATFORM_CONFIG_DIR}"
    )
elseif(C7222_PLATFORM_USES_PICO_SDK)
    message(FATAL_ERROR
        "C7222_ENABLE_BLE=ON requires btstack_config.h for platform "
        "'${C7222_PLATFORM}' at ${ELEC_C7222_BLE_PLATFORM_CONFIG_DIR}.")
endif()

include(${ELEC_C7222_BLE_DIR}/gap/gap.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GAP)

include(${ELEC_C7222_BLE_DIR}/gatt/gatt.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_GATT)

include(${ELEC_C7222_BLE_DIR}/security_manager/security_manager.cmake)
target_link_libraries(ELEC_C7222_BLE INTERFACE ELEC_C7222_BLE_SECURITY_MANAGER)
