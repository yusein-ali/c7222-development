set(ELEC_C7222_EXAMPLES_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT DEFINED C7222_EXAMPLES_BUILD)
    set(C7222_EXAMPLES_BUILD OFF)
endif()

message(STATUS "C7222_EXAMPLES_BUILD is defined: ${C7222_EXAMPLES_BUILD}")

if(C7222_EXAMPLES_BUILD)
    set(C7222_ENABLED_EXAMPLES "")

    if(C7222_EXAMPLES_BUILD)
        include(${ELEC_C7222_EXAMPLES_DIR}/freertos-board-example/freertos-board-example.cmake)
        include(${ELEC_C7222_EXAMPLES_DIR}/ble/ble-examples.cmake)
    endif()

    list(LENGTH C7222_ENABLED_EXAMPLES _c7222_examples_count)
    if(_c7222_examples_count EQUAL 0)
        message(FATAL_ERROR "C7222_EXAMPLES_BUILD is ON but no example is enabled. Enable exactly one example.")
    elseif(_c7222_examples_count GREATER 1)
        message(FATAL_ERROR "Only one C7222 example may be enabled at a time. Enabled: ${C7222_ENABLED_EXAMPLES}")
    endif()

    # Create a single alias interface library for the selected example
    list(GET C7222_ENABLED_EXAMPLES 0 _c7222_selected_example)
    add_library(C7222_EXAMPLE_LIB INTERFACE)
    target_link_libraries(C7222_EXAMPLE_LIB INTERFACE ${_c7222_selected_example})
endif()
