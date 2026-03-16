# FreeRTOS-cpp11 import cmake
# Downloads FreeRTOS-cpp11 repository to the shared c7222-development libs directory.

if (DEFINED ENV{FREERTOS_CPP11_PATH} AND (NOT FREERTOS_CPP11_PATH))
    set(FREERTOS_CPP11_PATH $ENV{FREERTOS_CPP11_PATH})
    message("Using FREERTOS_CPP11_PATH from environment ('${FREERTOS_CPP11_PATH}')")
endif()

# If no path provided, download to the shared c7222-development libs directory.
if ((NOT FREERTOS_CPP11_PATH OR FREERTOS_CPP11_PATH STREQUAL "") AND DEFINED CMAKE_SOURCE_DIR)
    if(DEFINED C7222_DEVELOPMENT_LIBS_DIR)
        set(_freertos_cpp11_libs_dir ${C7222_DEVELOPMENT_LIBS_DIR})
    else()
        get_filename_component(_freertos_cpp11_libs_dir "${CMAKE_CURRENT_LIST_DIR}/../libs" ABSOLUTE)
    endif()
    set(_freertos_cpp11_local_path ${_freertos_cpp11_libs_dir}/FreeRTOS_cpp11)
    if (EXISTS ${_freertos_cpp11_local_path})
        set(FREERTOS_CPP11_PATH ${_freertos_cpp11_local_path})
        message(STATUS "Using existing FreeRTOS-cpp11 at ${_freertos_cpp11_local_path}")
    else()
        file(MAKE_DIRECTORY ${_freertos_cpp11_libs_dir})
        message(STATUS "Cloning FreeRTOS-cpp11 into ${_freertos_cpp11_local_path}")
        execute_process(
            COMMAND git clone https://github.com/yusein-ali/FreeRTOS_cpp11.git ${_freertos_cpp11_local_path}
            WORKING_DIRECTORY ${_freertos_cpp11_libs_dir}
            RESULT_VARIABLE freertos_cpp11_clone_result
            ERROR_VARIABLE freertos_cpp11_clone_error
        )
        if (NOT freertos_cpp11_clone_result EQUAL 0)
            message(FATAL_ERROR "Failed to clone FreeRTOS-cpp11: ${freertos_cpp11_clone_error}")
        endif()
        set(FREERTOS_CPP11_PATH ${_freertos_cpp11_local_path})
        message(STATUS "Cloned FreeRTOS-cpp11 into ${_freertos_cpp11_local_path}")
    endif()
    unset(_freertos_cpp11_libs_dir)
    unset(_freertos_cpp11_local_path)
endif()

if (NOT FREERTOS_CPP11_PATH)
    message(STATUS "FreeRTOS-cpp11 not found, skipping")
else()
    set(FREERTOS_CPP11_PATH "${FREERTOS_CPP11_PATH}" CACHE PATH "Path to FreeRTOS-cpp11")
    get_filename_component(FREERTOS_CPP11_PATH "${FREERTOS_CPP11_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
    if (NOT EXISTS ${FREERTOS_CPP11_PATH})
        message(FATAL_ERROR "Directory '${FREERTOS_CPP11_PATH}' not found")
    endif()
    message(STATUS "FreeRTOS-cpp11 path set to: ${FREERTOS_CPP11_PATH}")
endif()
