include_guard(GLOBAL)

# Resolve stable paths once for all helpers in this module.
set(C7222_DEVELOPMENT_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(C7222_DEVELOPMENT_ROOT_DIR "${C7222_DEVELOPMENT_CMAKE_DIR}/.." ABSOLUTE)

# -----------------------------------------------------------------------------
# c7222_prepare_pre_project
# Intended use:
#   Run once in the top-level CMakeLists.txt before project().
#   This centralizes all "pre-project" setup needed for Pico + FreeRTOS builds.
#
# Inputs:
#   None (reads standard cache/env variables consumed by imported files, e.g.
#   PICO_SDK_PATH / PICO_TOOLCHAIN_PATH).
#
# Outputs / side effects:
#   - Includes compiler/toolchain setup.
#   - Imports Pico SDK and FreeRTOS dependency paths.
#   - Defines FreeRTOS C++ wrapper variables such as:
#       FREERTOS_CPP11_SOURCES
#       FREERTOS_CPP11_INCLUDE_DIRS
#   - Must run in caller scope; this is why it is a macro, not a function.
#
# Typical usage:
#   include(${CMAKE_CURRENT_LIST_DIR}/cmake/c7222_development.cmake)
#   c7222_prepare_pre_project()
#   project(my_app C CXX ASM)
macro(c7222_prepare_pre_project)
    # Configure compiler/toolchain defaults and PATH handling.
    include("${C7222_DEVELOPMENT_CMAKE_DIR}/pico-compiler-settings.cmake")
    # Import Pico SDK.
    include("${C7222_DEVELOPMENT_CMAKE_DIR}/pico_sdk_import.cmake")
    # Import FreeRTOS kernel + C++ wrappers.
    include("${C7222_DEVELOPMENT_CMAKE_DIR}/FreeRTOS_Kernel_import.cmake")
    include("${C7222_DEVELOPMENT_CMAKE_DIR}/FreeRTOS_CPP11_import.cmake")
    # Collect FreeRTOS C++ wrapper source/include variables.
    include("${C7222_DEVELOPMENT_CMAKE_DIR}/FreeRTOS_Cpp11_lib.cmake")
endmacro()

# -----------------------------------------------------------------------------
# c7222_prepare_post_project
# Intended use:
#   Run once after project() and pico_sdk_init().
#   Includes project-local component registration that requires project context.
#
# Inputs:
#   None.
#
# Outputs / side effects:
#   - Includes elec_c7222 and getting-started component CMake files.
#   - Populates example/app interface targets used later in target creation.
#
# Typical usage:
#   project(my_app C CXX ASM)
#   pico_sdk_init()
#   c7222_prepare_post_project()
macro(c7222_prepare_post_project)
    # Register project-local component targets after project()/SDK init.
    include("${C7222_DEVELOPMENT_ROOT_DIR}/libs/elec_c7222/elec_c7222.cmake")
    include("${C7222_DEVELOPMENT_ROOT_DIR}/getting-started/getting-started.cmake")
endmacro()

# -----------------------------------------------------------------------------
# c7222_define_development_interface
# Intended use:
#   Create one reusable INTERFACE target carrying common include dirs, libraries,
#   and compile definitions for all C7222 app executables.
#
# Inputs:
#   None directly.
#   Preconditions:
#   - Target ELEC_C7222 already exists.
#   - Variable FREERTOS_CPP11_INCLUDE_DIRS is defined.
#
# Outputs / side effects:
#   - Creates target: c7222_development (INTERFACE)
#   - Creates alias:  C7222::development
#   - Adds shared include directories, link libraries, and compile definitions.
#   - Idempotent: if C7222::development already exists, returns immediately.
#
# Typical usage:
#   c7222_define_development_interface()
#   target_link_libraries(my_target PRIVATE C7222::development)
function(c7222_define_development_interface)
    # Reuse previously created interface target.
    if(TARGET C7222::development)
        return()
    endif()

    # Validate required dependency targets/variables.
    if(NOT TARGET ELEC_C7222)
        message(FATAL_ERROR "ELEC_C7222 target must be defined before calling c7222_define_development_interface().")
    endif()

    if(NOT DEFINED FREERTOS_CPP11_INCLUDE_DIRS)
        message(FATAL_ERROR "FREERTOS_CPP11_INCLUDE_DIRS must be set before calling c7222_define_development_interface().")
    endif()

    # Define a shared interface target for all app executables.
    add_library(c7222_development INTERFACE)
    add_library(C7222::development ALIAS c7222_development)

    # Export common include directories.
    target_include_directories(c7222_development INTERFACE
        ${CMAKE_SOURCE_DIR}
        ${FREERTOS_CPP11_INCLUDE_DIRS}
    )

    # Export common dependency link set.
    target_link_libraries(c7222_development INTERFACE
        pico_stdlib
        pico_cyw43_arch_none
        hardware_adc
        hardware_clocks
        hardware_pwm
        FreeRTOS-Kernel-Heap4
        ELEC_C7222
    )

    if(C7222_ENABLE_BLE)
        target_link_libraries(c7222_development INTERFACE
            pico_btstack_ble
            pico_btstack_cyw43
        )
    endif()

    # Export shared compile definitions for FreeRTOS C++ and board defaults.
    target_compile_definitions(c7222_development INTERFACE
        _GLIBCXX_HAS_GTHREADS=1
        _GLIBCXX_USE_C99_STDINT_TR1=1
        PICO_DEFAULT_UART_BAUD_RATE=921600
        $<$<BOOL:${C7222_BLE_DEBUG}>:C7222_BLE_DEBUG=1>
    )
endfunction()

# -----------------------------------------------------------------------------
# c7222_configure_app_target(target_name app_lib)
# Intended use:
#   One-stop helper to create/configure an app executable with all standard
#   C7222 build settings and post-build behavior.
#
# Inputs:
#   target_name (required):
#     Executable target name. If it does not exist, this function creates it
#     with FREERTOS_CPP11_SOURCES.
#   app_lib (required):
#     Interface/object/static library target that carries app sources/includes
#     and optional target properties used by this project (e.g. GATT_FILES).
#
# Outputs / side effects:
#   - Ensures executable target exists and uses C++17.
#   - Applies warning options to elec_c7222 sources for GNU/Clang.
#   - Links target with C7222::development and app_lib.
#   - Adds linker wrap/map options.
#   - Enables/disables stdio backends and Pico extra outputs.
#   - Optionally exports UF2 into ${CMAKE_BINARY_DIR}/images.
#   - Optionally generates BLE GATT headers from app_lib GATT_FILES.
#   - Adds post-build size reporting when CMAKE_SIZE_UTIL is available.
#
# Typical usage (simple):
#   # app_lib must define TARGET_NAME/TARGET_PATH properties in this repo style
#   c7222_configure_app_target(example_app C7222_EXAMPLE_BLE_GAP)
#
# Typical usage (loop over app libs):
#   foreach(_lib IN LISTS APP_LIBS)
#       get_property(APP_NAME TARGET ${_lib} PROPERTY TARGET_NAME)
#       c7222_configure_app_target(${APP_NAME} ${_lib})
#   endforeach()
function(c7222_configure_app_target target_name app_lib)
    # Create the app executable on first use.
    if(NOT TARGET ${target_name})
        # Ensure wrapper sources are known before creating target.
        if(NOT DEFINED FREERTOS_CPP11_SOURCES)
            message(FATAL_ERROR "FREERTOS_CPP11_SOURCES must be set before calling c7222_configure_app_target().")
        endif()

        # Build app target with FreeRTOS C++ runtime sources.
        add_executable(${target_name}
            ${FREERTOS_CPP11_SOURCES}
        )

        # Enforce project C++ language level.
        set_target_properties(${target_name} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )

        # Apply stricter warnings to local C/C++ library sources.
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            file(GLOB_RECURSE _elec_c7222_source_files CONFIGURE_DEPENDS
                "${C7222_DEVELOPMENT_ROOT_DIR}/libs/elec_c7222/*.c"
                "${C7222_DEVELOPMENT_ROOT_DIR}/libs/elec_c7222/*.cpp"
                "${C7222_DEVELOPMENT_ROOT_DIR}/libs/elec_c7222/*.cc"
            )
            set_source_files_properties(${_elec_c7222_source_files} PROPERTIES
                COMPILE_OPTIONS "-Wall;-Wpedantic"
            )
        endif()
    endif()

    # Validate per-app source container target.
    if(NOT TARGET ${app_lib})
        message(FATAL_ERROR "App library '${app_lib}' does not exist.")
    endif()

    # Ensure shared interface exists before linking.
    c7222_define_development_interface()

    # Link app-specific sources plus common development dependencies.
    target_link_libraries(${target_name} PRIVATE
        C7222::development
        ${app_lib}
    )

    # Apply linker wrappers and generate map file.
    target_link_options(${target_name} PRIVATE
        "LINKER:--wrap=malloc"
        "LINKER:--wrap=free"
        "LINKER:--wrap=calloc"
        "LINKER:-Map=${CMAKE_BINARY_DIR}/${target_name}.map"
    )

    # Standard stdio/output settings for Pico targets.
    pico_enable_stdio_usb(${target_name} 0)
    pico_enable_stdio_uart(${target_name} 1)
    pico_add_extra_outputs(${target_name})

    # Optionally copy built UF2 to a stable output folder.
    if(C7222_EXPORT_PICO_UF2)
        set(_exported_uf2_dir "${CMAKE_BINARY_DIR}/images")
        set(_exported_uf2_path "${_exported_uf2_dir}/${target_name}.uf2")
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_exported_uf2_dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_BINARY_DIR}/${target_name}.uf2"
                "${_exported_uf2_path}"
            COMMAND ${CMAKE_COMMAND} -E echo "UF2 image: ${_exported_uf2_path}"
        )
    endif()

    # Optionally compile app GATT profiles into generated headers.
    get_property(_gatt_files TARGET ${app_lib} PROPERTY GATT_FILES)
    if(C7222_ENABLE_BLE AND _gatt_files AND NOT _gatt_files STREQUAL "NOTFOUND")
        foreach(_app_gatt_file IN LISTS _gatt_files)
            pico_btstack_make_gatt_header(${target_name} PRIVATE "${_app_gatt_file}")
        endforeach()
    endif()

    # Emit firmware size after link when the tool is available.
    if(CMAKE_SIZE_UTIL)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_SIZE_UTIL} $<TARGET_FILE:${target_name}>
        )
    else()
        message(WARNING "CMAKE_SIZE_UTIL not found; size report disabled.")
    endif()
endfunction()
