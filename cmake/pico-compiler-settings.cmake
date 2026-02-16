# Pico toolchain defaults derived from .vscode/settings.json.
if(NOT DEFINED PICO_SDK_PATH)
    if(DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}" CACHE PATH "Pico SDK path")
    elseif(WIN32)
        set(PICO_SDK_PATH "$ENV{USERPROFILE}/.pico-sdk/sdk/2.2.0" CACHE PATH "Pico SDK path")
    else()
        set(PICO_SDK_PATH "$ENV{HOME}/.pico-sdk/sdk/2.2.0" CACHE PATH "Pico SDK path")
    endif()
endif()

# Allow either a direct SDK version directory or the ~/.pico-sdk root.
if(PICO_SDK_PATH AND EXISTS "${PICO_SDK_PATH}/sdk")
    if(DEFINED sdkVersion AND EXISTS "${PICO_SDK_PATH}/sdk/${sdkVersion}")
        set(PICO_SDK_PATH "${PICO_SDK_PATH}/sdk/${sdkVersion}" CACHE PATH "Pico SDK path" FORCE)
    elseif(EXISTS "${PICO_SDK_PATH}/sdk/2.2.0")
        set(PICO_SDK_PATH "${PICO_SDK_PATH}/sdk/2.2.0" CACHE PATH "Pico SDK path" FORCE)
    endif()
endif()

if(NOT DEFINED PICO_TOOLCHAIN_PATH)
    if(DEFINED ENV{PICO_TOOLCHAIN_PATH})
        set(PICO_TOOLCHAIN_PATH "$ENV{PICO_TOOLCHAIN_PATH}" CACHE PATH "Pico toolchain path")
    elseif(WIN32)
        set(PICO_TOOLCHAIN_PATH "$ENV{USERPROFILE}/.pico-sdk/toolchain/14_2_Rel1" CACHE PATH "Pico toolchain path")
    else()
        set(PICO_TOOLCHAIN_PATH "$ENV{HOME}/.pico-sdk/toolchain/14_2_Rel1" CACHE PATH "Pico toolchain path")
    endif()
endif()

# Recover from stale cache path when a valid local compiler exists.
if(PICO_TOOLCHAIN_PATH AND NOT EXISTS "${PICO_TOOLCHAIN_PATH}" AND EXISTS "/usr/bin/arm-none-eabi-gcc")
    set(PICO_TOOLCHAIN_PATH "/usr" CACHE PATH "Pico toolchain path" FORCE)
endif()

# Pico SDK's compiler finder appends PATH_SUFFIXES bin; if user gives /usr/bin,
# normalize it to /usr to avoid false warnings.
if(PICO_TOOLCHAIN_PATH
   AND EXISTS "${PICO_TOOLCHAIN_PATH}/arm-none-eabi-gcc"
   AND PICO_TOOLCHAIN_PATH MATCHES "/bin/?$")
    get_filename_component(_pico_toolchain_root "${PICO_TOOLCHAIN_PATH}" DIRECTORY)
    set(PICO_TOOLCHAIN_PATH "${_pico_toolchain_root}" CACHE PATH "Pico toolchain path" FORCE)
endif()

# Accept both a toolchain root (with /bin) and a direct bin directory.
set(PICO_TOOLCHAIN_BIN_PATH "${PICO_TOOLCHAIN_PATH}")
if(PICO_TOOLCHAIN_PATH AND EXISTS "${PICO_TOOLCHAIN_PATH}/bin")
    set(PICO_TOOLCHAIN_BIN_PATH "${PICO_TOOLCHAIN_PATH}/bin")
endif()

if(PICO_TOOLCHAIN_PATH)
    if(WIN32)
        set(_pico_path_sep ";")
    else()
        set(_pico_path_sep ":")
    endif()
    if(DEFINED ENV{PATH})
        set(ENV{PATH} "${PICO_TOOLCHAIN_BIN_PATH}${_pico_path_sep}$ENV{PATH}")
    else()
        set(ENV{PATH} "${PICO_TOOLCHAIN_BIN_PATH}")
    endif()
endif()

if(NOT DEFINED PICO_PLATFORM)
    set(PICO_PLATFORM rp2350 CACHE STRING "Pico platform")
endif()

if(NOT DEFINED PICO_COMPILER)
    set(PICO_COMPILER pico_arm_cortex_m33_gcc CACHE STRING "Pico compiler")
endif()

set(PICO_GCC_TRIPLE arm-none-eabi CACHE STRING "Pico GCC triple")

# Recover from stale cache values such as ~/.pico-sdk/sdk/<version> when
# the SDK is actually checked out directly at ~/.pico-sdk.
if(PICO_SDK_PATH
   AND NOT EXISTS "${PICO_SDK_PATH}/cmake/preload/toolchains/${PICO_COMPILER}.cmake"
   AND EXISTS "$ENV{HOME}/.pico-sdk/cmake/preload/toolchains/${PICO_COMPILER}.cmake")
    set(PICO_SDK_PATH "$ENV{HOME}/.pico-sdk" CACHE PATH "Pico SDK path" FORCE)
endif()

if(PICO_SDK_PATH AND EXISTS "${PICO_SDK_PATH}/cmake/preload/toolchains/${PICO_COMPILER}.cmake")
    include("${PICO_SDK_PATH}/cmake/preload/toolchains/${PICO_COMPILER}.cmake")
else()
    message(FATAL_ERROR
        "Could not find Pico toolchain preload file at "
        "'${PICO_SDK_PATH}/cmake/preload/toolchains/${PICO_COMPILER}.cmake'. "
        "Set PICO_SDK_PATH to your SDK version directory (for example: "
        "~/.pico-sdk/sdk/2.2.0) or ~/.pico-sdk with sdkVersion set."
    )
endif()

# Tell CMake we're cross-compiling for bare metal to avoid host link flags.
set(CMAKE_SYSTEM_NAME PICO CACHE STRING "Target system" FORCE)
set(CMAKE_SYSTEM_PROCESSOR arm CACHE STRING "Target CPU" FORCE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY CACHE STRING "Try-compile target type")


# Locate the correct size utility from the ARM toolchain
if(CMAKE_C_COMPILER)
    get_filename_component(PICO_TOOLCHAIN_BIN_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
else()
    set(PICO_TOOLCHAIN_BIN_DIR "${PICO_TOOLCHAIN_BIN_PATH}")
endif()
find_program(CMAKE_SIZE_UTIL NAMES ${PICO_GCC_TRIPLE}-size HINTS "${PICO_TOOLCHAIN_BIN_DIR}" REQUIRED)
message(STATUS "Found size tool: ${CMAKE_SIZE_UTIL}")
