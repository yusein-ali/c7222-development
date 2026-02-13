# FreeRTOS-cpp11 Sources and Includes
# Defines sources and include directories for FreeRTOS C++11 wrapper

if (NOT FREERTOS_CPP11_PATH)
    message(FATAL_ERROR "FREERTOS_CPP11_PATH is not set. Include FreeRTOS_CPP11_import.cmake first.")
endif()

if (NOT FREERTOS_KERNEL_PATH)
    message(FATAL_ERROR "FREERTOS_KERNEL_PATH is not set. Include FREERTOS_KERNEL_import.cmake first.")
endif()

# Source directory
set(FREERTOS_CPP11_SOURCE_DIR ${FREERTOS_CPP11_PATH}/FreeRTOS/cpp11_gcc)

# GLOB all .cpp files in the cpp11_gcc directory
file(GLOB FREERTOS_CPP11_SOURCES "${FREERTOS_CPP11_SOURCE_DIR}/*.cpp")

# libstdc++ concurrency sources (needed for std::condition_variable)
set(FREERTOS_CPP11_LIBSTDCPP_DIR ${FREERTOS_CPP11_PATH}/libstdc++_gcc/v13)
list(APPEND FREERTOS_CPP11_SOURCES
    "${FREERTOS_CPP11_LIBSTDCPP_DIR}/condition_variable.cc"
    # "${FREERTOS_CPP11_LIBSTDCPP_DIR}/future.cc"
    # "${FREERTOS_CPP11_LIBSTDCPP_DIR}/libatomic.c"
)

# Set include directories as a list
set(FREERTOS_CPP11_INCLUDE_DIRS
    ${FREERTOS_CPP11_SOURCE_DIR}
    ${FREERTOS_KERNEL_PATH}/include
)

message(STATUS "FreeRTOS-cpp11 configured")
message(STATUS "  Sources: ${FREERTOS_CPP11_SOURCES}")
message(STATUS "  Include dirs: ${FREERTOS_CPP11_INCLUDE_DIRS}")
