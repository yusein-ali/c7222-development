# This is a copy of <FREERTOS_KERNEL_PATH>/portable/ThirdParty/GCC/RP2040/FREERTOS_KERNEL_import.cmake

# This can be dropped into an external project to help locate the FreeRTOS kernel
# It should be include()ed prior to project(). Alternatively this file may
# or the CMakeLists.txt in this directory may be included or added via add_subdirectory
# respectively.

if (DEFINED ENV{FREERTOS_KERNEL_PATH} AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
    message("Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
endif ()

# If no path provided, prefer a workspace-local checkout under libs/FreeRTOS-Kernel; clone and init submodules when absent
if ((NOT FREERTOS_KERNEL_PATH OR FREERTOS_KERNEL_PATH STREQUAL "") AND DEFINED CMAKE_SOURCE_DIR)
    set(_freertos_kernel_local_path ${CMAKE_SOURCE_DIR}/libs/FreeRTOS-Kernel)
    if (EXISTS ${_freertos_kernel_local_path})
        set(FREERTOS_KERNEL_PATH ${_freertos_kernel_local_path})
        message(STATUS "Using existing FreeRTOS-Kernel at ${_freertos_kernel_local_path}")
    else()
        file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
        execute_process(
            COMMAND git clone --recurse-submodules --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git ${_freertos_kernel_local_path}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE freertos_clone_result
            ERROR_VARIABLE freertos_clone_error
        )
        if (NOT freertos_clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone FreeRTOS-Kernel: ${freertos_clone_error}")
        endif()
        set(FREERTOS_KERNEL_PATH ${_freertos_kernel_local_path})
        message(STATUS "Cloned FreeRTOS-Kernel into ${_freertos_kernel_local_path}")
    endif()

    # Some ports (e.g. RP2350) live in submodules; ensure they are present
    set(_freertos_port_dir ${FREERTOS_KERNEL_PATH}/portable/ThirdParty/Community-Supported-Ports)
    if (EXISTS ${FREERTOS_KERNEL_PATH} AND NOT EXISTS ${_freertos_port_dir}/GCC)
        execute_process(
            COMMAND git submodule update --init --recursive --depth 1
            WORKING_DIRECTORY ${FREERTOS_KERNEL_PATH}
            RESULT_VARIABLE freertos_submodule_result
            ERROR_VARIABLE freertos_submodule_error
        )
        if (NOT freertos_submodule_result EQUAL 0)
            message(FATAL_ERROR "Failed to init FreeRTOS submodules: ${freertos_submodule_error}")
        else()
            message(STATUS "Initialized FreeRTOS submodules for community-supported ports")
        endif()
    endif()
    unset(_freertos_port_dir)
    unset(_freertos_kernel_local_path)
endif()

# first pass we look in old tree; second pass we look in new tree
foreach(SEARCH_PASS RANGE 0 1)
    if (SEARCH_PASS)
        # ports may be moving to submodule in the future
        set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "portable/ThirdParty/Community-Supported-Ports/GCC")
        set(FREERTOS_KERNEL_RP2040_BACK_PATH "../../../../..")
    else()
        set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "portable/ThirdParty/GCC")
        set(FREERTOS_KERNEL_RP2040_BACK_PATH "../../../..")
    endif()

    if(PICO_PLATFORM STREQUAL "rp2040")
        set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}/RP2040")
    else()
        if (PICO_PLATFORM STREQUAL "rp2350-riscv")
            set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}/RP2350_RISC-V")
        else()
            set(FREERTOS_KERNEL_RP2040_RELATIVE_PATH "${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}/RP2350_ARM_NTZ")
        endif()
    endif()

    if (NOT FREERTOS_KERNEL_PATH)
        # check if we are inside the FreeRTOS kernel tree (i.e. this file has been included directly)
        get_filename_component(_ACTUAL_PATH ${CMAKE_CURRENT_LIST_DIR} REALPATH)
        get_filename_component(_POSSIBLE_PATH ${CMAKE_CURRENT_LIST_DIR}/${FREERTOS_KERNEL_RP2040_BACK_PATH}/${FREERTOS_KERNEL_RP2040_RELATIVE_PATH} REALPATH)
        if (_ACTUAL_PATH STREQUAL _POSSIBLE_PATH)
            get_filename_component(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/${FREERTOS_KERNEL_RP2040_BACK_PATH} REALPATH)
        endif()
        if (_ACTUAL_PATH STREQUAL _POSSIBLE_PATH)
            get_filename_component(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_LIST_DIR}/${FREERTOS_KERNEL_RP2040_BACK_PATH} REALPATH)
            message("Setting FREERTOS_KERNEL_PATH to ${FREERTOS_KERNEL_PATH} based on location of FreeRTOS-Kernel-import.cmake")
            break()
        elseif (PICO_SDK_PATH AND EXISTS "${PICO_SDK_PATH}/../FreeRTOS-Kernel")
            set(FREERTOS_KERNEL_PATH ${PICO_SDK_PATH}/../FreeRTOS-Kernel)
            message("Defaulting FREERTOS_KERNEL_PATH as sibling of PICO_SDK_PATH: ${FREERTOS_KERNEL_PATH}")
            break()
        endif()
    endif ()

    if (NOT FREERTOS_KERNEL_PATH)
        foreach(POSSIBLE_SUFFIX Source FreeRTOS-Kernel FreeRTOS/Source)
            # check if FreeRTOS-Kernel exists under directory that included us
            set(SEARCH_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
            get_filename_component(_POSSIBLE_PATH ${SEARCH_ROOT}/${POSSIBLE_SUFFIX} REALPATH)
            if (EXISTS ${_POSSIBLE_PATH}/${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}/CMakeLists.txt)
                get_filename_component(FREERTOS_KERNEL_PATH ${_POSSIBLE_PATH} REALPATH)
                message("Setting FREERTOS_KERNEL_PATH to '${FREERTOS_KERNEL_PATH}' found relative to enclosing project")
                break()
            endif()
        endforeach()
        if (FREERTOS_KERNEL_PATH)
            break()
        endif()
    endif()

    # user must have specified
    if (FREERTOS_KERNEL_PATH)
        if (EXISTS "${FREERTOS_KERNEL_PATH}/${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}")
            break()
        endif()
    endif()
endforeach ()

if (NOT FREERTOS_KERNEL_PATH)
    message(FATAL_ERROR "FreeRTOS location was not specified. Please set FREERTOS_KERNEL_PATH.")
endif()

set(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" CACHE PATH "Path to the FreeRTOS Kernel")

get_filename_component(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${FREERTOS_KERNEL_PATH})
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' not found")
endif()
if (NOT EXISTS ${FREERTOS_KERNEL_PATH}/${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}/CMakeLists.txt)
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' does not contain a '${PICO_PLATFORM}' port here: ${FREERTOS_KERNEL_RP2040_RELATIVE_PATH}")
endif()
set(FREERTOS_KERNEL_PATH ${FREERTOS_KERNEL_PATH} CACHE PATH "Path to the FreeRTOS_KERNEL" FORCE)

add_subdirectory(${FREERTOS_KERNEL_PATH}/${FREERTOS_KERNEL_RP2040_RELATIVE_PATH} FREERTOS_KERNEL)
