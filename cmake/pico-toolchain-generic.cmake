# Generic Pico toolchain file that relies on PICO_TOOLCHAIN_PATH.
# This file is intentionally SDK-agnostic; it only selects the compiler.

if(NOT DEFINED ENV{PICO_TOOLCHAIN_PATH} OR "$ENV{PICO_TOOLCHAIN_PATH}" STREQUAL "")
    message(WARNING "PICO_TOOLCHAIN_PATH environment variable is not set. Skipping toolchain setup.")
    return()
endif()

set(PICO_TOOLCHAIN_PATH "$ENV{PICO_TOOLCHAIN_PATH}" CACHE PATH "Pico toolchain path")

# Accept both a toolchain root (with /bin) and a direct bin directory.
set(_pico_toolchain_bin "${PICO_TOOLCHAIN_PATH}")
if(EXISTS "${PICO_TOOLCHAIN_PATH}/bin")
    set(_pico_toolchain_bin "${PICO_TOOLCHAIN_PATH}/bin")
endif()

set(_pico_gcc "${_pico_toolchain_bin}/arm-none-eabi-gcc")
set(_pico_gxx "${_pico_toolchain_bin}/arm-none-eabi-g++")
set(_pico_as "${_pico_toolchain_bin}/arm-none-eabi-gcc")

if(NOT EXISTS "${_pico_gcc}")
    message(WARNING "arm-none-eabi-gcc not found in ${_pico_toolchain_bin}. Skipping toolchain setup.")
    return()
endif()
if(NOT EXISTS "${_pico_gxx}")
    message(WARNING "arm-none-eabi-g++ not found in ${_pico_toolchain_bin}. Skipping toolchain setup.")
    return()
endif()

message(STATUS "Found Pico toolchain in ${_pico_toolchain_bin}")

set(CMAKE_SYSTEM_NAME Generic CACHE STRING "Target system" FORCE)
set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "Target CPU" FORCE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "Try-compile target type" FORCE)

# Pin toolchain to the exact binaries under PICO_TOOLCHAIN_PATH.
set(CMAKE_C_COMPILER "${_pico_gcc}" CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${_pico_gxx}" CACHE FILEPATH "C++ compiler" FORCE)
set(CMAKE_ASM_COMPILER "${_pico_as}" CACHE FILEPATH "ASM compiler" FORCE)

# Keep find_* from picking up host tools/libraries by default.
set(CMAKE_FIND_ROOT_PATH "${PICO_TOOLCHAIN_PATH}" CACHE PATH "Toolchain root" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "" FORCE)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "" FORCE)
