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

# ====================================================================================
# OPTIONS
set(PROJECT_NAME "ELEC-C7222-CPP-LIB" CACHE STRING "Project name")

option(C7222_ENABLE_BLE "Enable BLE helpers in ELEC_C7222" ON)
option(C7222_BLE_DEBUG "Enable BLE debug logging" OFF)
option(C7222_EXAMPLES_BUILD "Enable C7222 examples build" ON)
option(C7222_GETTING_STARTED_BUILD "Enable getting-started build targets" ON)

# internal list for gatt files
if(C7222_ENABLE_BLE)
    set(APP_GATT_FILES "" CACHE INTERNAL "List of .gatt files to compile for the app; populated by examples")
endif()
# ====================================================================================
# COMPILER SETTINGS
include(cmake/pico-compiler-settings.cmake)
# set(FREERTOS_KERNEL_SMP 1 CACHE BOOL "Disable SMP FreeRTOS port for single-core scheduling")
# ====================================================================================
# COMPONENTS
# Pull in Raspberry Pi Pico SDK (must be before project)
include(cmake/pico_sdk_import.cmake)
# pull other libraries if they do not exist
include(cmake/FREERTOS_KERNEL_import.cmake)
include(cmake/FreeRTOS_CPP11_import.cmake)

# Include FreeRTOS-cpp11 library configuration
include(cmake/FreeRTOS_Cpp11_lib.cmake)

# ====================================================================================
# PROJECT
project(${PROJECT_NAME} C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# ====================================================================================
# THE LAST COMPNENT: elec_c7222 helpers -- this must be included after defining the project and options
include(${CMAKE_SOURCE_DIR}/libs/elec_c7222/elec_c7222.cmake)
include(${CMAKE_SOURCE_DIR}/getting-started/getting-started.cmake)

# Add executable. Default name is the project name, version 0.1
if(NOT C7222_EXAMPLES_BUILD)
    set(APP_NAME ${PROJECT_NAME})
    # if we are not building examples, just build the app from the main source dir
    set(APP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src" CACHE PATH "Application source directory")
    set(APP_SOURCE_DIR "${CMAKE_SOURCE_DIR}/examples/ble/gatt-server" CACHE PATH "Application source directory")
    file(GLOB APP_SOURCES
        "${APP_SOURCE_DIR}/*.cpp"
        "${APP_SOURCE_DIR}/*.c")
    file(GLOB APP_GATT_FILES
        "${APP_SOURCE_DIR}/*.gatt")


    add_library(APP_LIB INTERFACE)
    target_add_library(${APP_NAME} INTERFACE)
    target_sources(APP_LIB INTERFACE 
                    ${APP_SOURCES})

    target_include_directories(APP_LIB INTERFACE 
                    ${APP_SOURCE_DIR})

    set_property(TARGET APP_LIB PROPERTY TARGET_NAME ${APP_NAME})
    set_property(TARGET APP_LIB PROPERTY GATT_FILES ${APP_GATT_FILES})

    list(APPEND APP_LIBS ${APP_LIB})
else()
    # if we are building examples, the example library will pull in the app sources
    set(APP_LIBS "")
    list(APPEND APP_LIBS ${C7222_EXAMPLES})
endif()

if(C7222_GETTING_STARTED_BUILD)
    list(APPEND APP_LIBS ${C7222_GETTING_STARTED})
endif()


# ====================================================================================
# CONFIGURE COMPILER AND LINKER SETTINGS
# Set language for C++ sources
set_source_files_properties(${FREERTOS_CPP11_SOURCES} PROPERTIES LANGUAGE CXX)

foreach(_lib IN LISTS APP_LIBS)
    message(STATUS "APP_LIB: ${_lib}")
    # Example: link each lib to the app
    # target_link_libraries(${APP_NAME} PRIVATE ${_lib})

    get_property(APP_NAME  TARGET ${_lib} PROPERTY TARGET_NAME)
    if(NOT APP_NAME OR APP_NAME STREQUAL "NOTFOUND")
        message(FATAL_ERROR "TARGET_NAME property must be defined!")
    endif()
    get_property(GATT_FILES TARGET ${_lib} PROPERTY GATT_FILES)
    if(NOT GATT_FILES OR GATT_FILES STREQUAL "NOTFOUND")
        set(GAT_FILE "")
    endif()
    get_property(APP_PATH  TARGET ${_lib} PROPERTY TARGET_PATH)
    if(NOT APP_PATH OR APP_PATH STREQUAL "NOTFOUND")
        message(FATAL_ERROR "TARGET_PATH property must be defined!")
    endif()

    message(STATUS "  Configuring app ${APP_NAME}") 
    message(STATUS "    with app sources in ${APP_PATH}") 
    message(STATUS "    and GATT files ${GATT_FILES}")

    add_executable(${APP_NAME} 
                   ${FREERTOS_CPP11_SOURCES})
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


    target_link_options(${APP_NAME} PRIVATE
        "LINKER:--wrap=malloc"
        "LINKER:--wrap=free"
        "LINKER:--wrap=calloc"
        "LINKER:-Map=${CMAKE_BINARY_DIR}/${APP_NAME}.map"
    )

    # Search this source dir for project headers
    target_include_directories(${APP_NAME} PRIVATE 
                                ${CMAKE_SOURCE_DIR}
                                ${FREERTOS_CPP11_INCLUDE_DIRS}
                                )

    # pull in common dependencies
    target_link_libraries(${APP_NAME} PRIVATE
        pico_stdlib
        hardware_adc
        hardware_clocks
        hardware_pwm
        FreeRTOS-Kernel-Heap4
        pico_btstack_ble
        pico_btstack_cyw43
        pico_cyw43_arch_none
        ELEC_C7222
        ${_lib}
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


    if(C7222_ENABLE_BLE AND GATT_FILES) 
        foreach(APP_GATT_FILE IN LISTS GATT_FILES)
            pico_btstack_make_gatt_header(${APP_NAME} PRIVATE "${APP_GATT_FILE}")
        endforeach()
    endif()

    # add url via pico_set_program_url

    if (CMAKE_SIZE_UTIL)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:${APP_NAME}>
        )
    else()
        message(WARNING "CMAKE_SIZE_UTIL not found; size report disabled.")
    endif()
endforeach()
