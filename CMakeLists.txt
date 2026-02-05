# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

set(PICO_BOARD pico2_w CACHE STRING "Board type")
set(C7222_ENABLE_BLE ON CACHE BOOL "Enable BLE helpers in ELEC_C7222")
option(C7222_BLE_DEBUG "Enable BLE debug logging" OFF)

include(cmake/pico-compiler-settings.cmake)
# set(FREERTOS_KERNEL_SMP 1 CACHE BOOL "Disable SMP FreeRTOS port for single-core scheduling")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(cmake/pico_sdk_import.cmake)
# pull other libraries if they do not exist
include(cmake/FREERTOS_KERNEL_import.cmake)
include(cmake/FreeRTOS_CPP11_import.cmake)

# Include FreeRTOS-cpp11 library configuration
include(cmake/FreeRTOS_Cpp11_lib.cmake)

set(APP_NAME "PicoBleGattTest" CACHE STRING "Application name")
set(APP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/examples/ble/gatt-server" CACHE PATH "Application source directory")

project(${APP_NAME} C CXX ASM)




# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# elec_c7222 helpers
include(${CMAKE_SOURCE_DIR}/libs/elec_c7222/elec_c7222.cmake)

# Add executable. Default name is the project name, version 0.1

file(GLOB APP_SOURCES
    "${APP_SOURCE_DIR}/*.cpp"
    "${APP_SOURCE_DIR}/*.c")

message(STATUS "APP_NAME: ${APP_NAME}")
message(STATUS "APP_DIR: ${APP_SOURCE_DIR}")
message(STATUS "APP_SOURCES: ${APP_SOURCES}")

add_executable(${APP_NAME}
                    ${APP_SOURCES}
                    ${FREERTOS_CPP11_SOURCES}
                )

# Set language for C++ sources
set_source_files_properties(${FREERTOS_CPP11_SOURCES} PROPERTIES LANGUAGE CXX)

# Ensure C++17 standard
set_target_properties(${APP_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    file(GLOB_RECURSE ELEC_C7222_SOURCE_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/libs/elec_c7222/*.c"
        "${CMAKE_SOURCE_DIR}/libs/elec_c7222/*.cpp"
        "${CMAKE_SOURCE_DIR}/libs/elec_c7222/*.cc"
    )
    set_source_files_properties(${ELEC_C7222_SOURCE_FILES} PROPERTIES
        COMPILE_OPTIONS "-Wall;-Wpedantic"
    )
endif()

# Search this source dir for project headers
target_include_directories(${APP_NAME} PRIVATE 
                            ${CMAKE_SOURCE_DIR}
                            ${FREERTOS_CPP11_INCLUDE_DIRS}
                            ${CMAKE_SOURCE_DIR}/libs/elec_c7222/ble/gatt/platform/rpi_pico/config
                            )

target_link_options(${APP_NAME} PRIVATE
    "LINKER:--wrap=malloc"
    "LINKER:--wrap=free"
    "LINKER:--wrap=calloc"
    "LINKER:-Map=${CMAKE_BINARY_DIR}/${APP_NAME}.map"
)
# pull in common dependencies
target_link_libraries(${APP_NAME} PRIVATE
    pico_stdlib
    FreeRTOS-Kernel-Heap4
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_adc
    ELEC_C7222
)

# Enable libstdc++ thread support so <mutex> works for the FreeRTOS C++ wrappers.
target_compile_definitions(${APP_NAME} PRIVATE
    _GLIBCXX_HAS_GTHREADS=1
    _GLIBCXX_USE_C99_STDINT_TR1=1
)

if(C7222_BLE_DEBUG)
    target_compile_definitions(${APP_NAME} PRIVATE C7222_BLE_DEBUG=1)
endif()

# Set the default UART baudrate to 921600 (or any other standard rate)
add_compile_definitions(PICO_DEFAULT_UART_BAUD_RATE=921600)

# enable usb output, disable uart output
pico_enable_stdio_usb(${APP_NAME} 0)
pico_enable_stdio_uart(${APP_NAME}  1)
pico_add_extra_outputs(${APP_NAME})

file(GLOB APP_GATT_FILES CONFIGURE_DEPENDS "${APP_SOURCE_DIR}/*.gatt")
foreach(APP_GATT_FILE IN LISTS APP_GATT_FILES)
    pico_btstack_make_gatt_header(${APP_NAME} PRIVATE "${APP_GATT_FILE}")
endforeach()

# add url via pico_set_program_url

if (CMAKE_SIZE_UTIL)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:${APP_NAME}>
    )
else()
    message(WARNING "CMAKE_SIZE_UTIL not found; size report disabled.")
endif()
