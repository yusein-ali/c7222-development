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
set(FREERTOS_KERNEL_SMP 1 CACHE BOOL "Disable SMP FreeRTOS port for single-core scheduling")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(FREERTOS_KERNEL_import.cmake)
include(FreeRTOS_CPP11_import.cmake)

project(picoboard_freertos_test C CXX ASM)

# Include FreeRTOS-cpp11 library configuration
include(FreeRTOS_Cpp11_lib.cmake)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(picoboard_freertos_test
                    main_free_rtos.cpp
                    ${FREERTOS_CPP11_SOURCES}
                )

# Set language for C++ sources
set_source_files_properties(${FREERTOS_CPP11_SOURCES} PROPERTIES LANGUAGE CXX)

# Ensure C++17 standard
set_target_properties(picoboard_freertos_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Search this source dir for project headers
target_include_directories(picoboard_freertos_test PRIVATE 
                            ${CMAKE_SOURCE_DIR}
                            ${FREERTOS_CPP11_INCLUDE_DIRS}
                            )

target_link_options(picoboard_freertos_test PRIVATE
    "LINKER:--wrap=malloc"
    "LINKER:--wrap=free"
    "LINKER:--wrap=calloc"
)
# pull in common dependencies
target_link_libraries(picoboard_freertos_test PRIVATE
    pico_stdlib
    FreeRTOS-Kernel-Heap4
)
if (PICO_CYW43_SUPPORTED)
    target_link_libraries(picoboard_freertos_test PRIVATE pico_cyw43_arch_none)
endif()

# Enable libstdc++ thread support so <mutex> works for the FreeRTOS C++ wrappers.
target_compile_definitions(picoboard_freertos_test PRIVATE
    _GLIBCXX_HAS_GTHREADS=1
    _GLIBCXX_USE_C99_STDINT_TR1=1
)

# enable usb output, disable uart output
pico_enable_stdio_usb(picoboard_freertos_test 1)
pico_enable_stdio_uart(picoboard_freertos_test  0)
pico_add_extra_outputs(picoboard_freertos_test)

# add url via pico_set_program_url
