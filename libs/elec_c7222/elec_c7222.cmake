set(ELEC_C7222_DIR ${CMAKE_CURRENT_LIST_DIR})

add_library(ELEC_C7222 INTERFACE)

set(C7222_PLATFORM "rpi_pico2w" CACHE STRING "ELEC_C7222 platform implementation")
set_property(CACHE C7222_PLATFORM PROPERTY STRINGS rpi_pico2w grader rp2350_custom_board)

set(C7222_SUPPORTED_PLATFORMS rpi_pico2w grader rp2350_custom_board)
list(FIND C7222_SUPPORTED_PLATFORMS "${C7222_PLATFORM}" C7222_PLATFORM_INDEX)
if(C7222_PLATFORM_INDEX EQUAL -1)
    message(FATAL_ERROR
        "Unsupported C7222_PLATFORM='${C7222_PLATFORM}'. "
        "Supported values are: ${C7222_SUPPORTED_PLATFORMS}")
endif()

if(C7222_PLATFORM STREQUAL "rpi_pico2w")
    set(C7222_PLATFORM_SOURCE_DIR rpi_pico)
    set(C7222_PLATFORM_USES_PICO_SDK TRUE)
    set(PLATFORM_HAS_CYW43 TRUE CACHE BOOL "Platform has a CYW43 wireless chip" FORCE)
elseif(C7222_PLATFORM STREQUAL "grader")
    set(C7222_PLATFORM_SOURCE_DIR grader)
    set(C7222_PLATFORM_USES_PICO_SDK FALSE)
    set(PLATFORM_HAS_CYW43 FALSE CACHE BOOL "Platform has a CYW43 wireless chip" FORCE)
elseif(C7222_PLATFORM STREQUAL "rp2350_custom_board")
    set(C7222_PLATFORM_SOURCE_DIR rp2350_custom_board)
    set(C7222_PLATFORM_USES_PICO_SDK TRUE)
    option(PLATFORM_HAS_CYW43 "Platform has a CYW43 wireless chip" ON)
endif()

message(STATUS "C7222_PLATFORM is defined: ${C7222_PLATFORM}")
message(STATUS "PLATFORM_HAS_CYW43 is defined: ${PLATFORM_HAS_CYW43}")

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
include(${ELEC_C7222_DIR}/freertos_wrappers/freertos_wrappers.cmake)

target_link_libraries(ELEC_C7222 INTERFACE
    ELEC_C7222_DEVICES
    ELEC_C7222_UTILS
    ELEC_C7222_FREERTOS_WRAPPERS
)

target_include_directories(ELEC_C7222 INTERFACE "${ELEC_C7222_DIR}/include")
target_include_directories(ELEC_C7222 INTERFACE "${ELEC_C7222_DIR}/config")

if(C7222_ENABLE_BLE)
    include(${ELEC_C7222_DIR}/ble/ble.cmake)
    target_link_libraries(ELEC_C7222 INTERFACE ELEC_C7222_BLE)
endif()

if(C7222_EXAMPLES_BUILD)
    include(${ELEC_C7222_DIR}/examples/examples.cmake)
    if(TARGET C7222_EXAMPLE_LIB)
        target_link_libraries(ELEC_C7222 INTERFACE C7222_EXAMPLE_LIB)
    endif()
endif()
