# C7222 development workspace entrypoint.

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize Pico SDK path from installed location
# (can come from environment/CMake cache).

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
# ==========================================================================

set(PICO_BOARD pico2_w CACHE STRING "Board type")

# ====================================================================================
# OPTIONS
set(PROJECT_NAME "ELEC-C7222-CPP-LIB" CACHE STRING "Project name")

option(C7222_ENABLE_BLE "Enable BLE helpers in ELEC_C7222" ON)
option(C7222_BLE_DEBUG "Enable BLE debug logging" OFF)
option(C7222_EXAMPLES_BUILD "Enable C7222 examples build" ON)
option(C7222_GETTING_STARTED_BUILD "Enable getting-started build targets" ON)
option(C7222_EXPORT_PICO_UF2 "Export UF2 image copy in build/images after build" ON)

# Internal list for app-level GATT files.
if(C7222_ENABLE_BLE)
    set(APP_GATT_FILES "" CACHE INTERNAL "List of .gatt files to compile for the app; populated by examples")
endif()
# ====================================================================================
# Centralized toolchain + SDK + FreeRTOS setup.
include(${CMAKE_CURRENT_LIST_DIR}/cmake/c7222_development.cmake)
c7222_prepare_pre_project()
# set(FREERTOS_KERNEL_SMP 1 CACHE BOOL "Disable SMP FreeRTOS port for single-core scheduling")

# ====================================================================================
# PROJECT
project(${PROJECT_NAME} C CXX ASM)

# ====================================================================================
# Project component setup that depends on project()/pico_sdk_init().
c7222_prepare_post_project()

# Build app libraries from examples/getting-started and configure final executables.
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
# Configure compiler/linker settings for each app target.
# Force FreeRTOS cpp11 wrapper sources to be treated as C++.
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

    c7222_configure_app_target(${APP_NAME} ${_lib})
endforeach()
