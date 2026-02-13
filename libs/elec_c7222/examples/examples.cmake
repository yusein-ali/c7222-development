set(ELEC_C7222_EXAMPLES_DIR ${CMAKE_CURRENT_LIST_DIR})

if(NOT DEFINED C7222_EXAMPLES_BUILD)
    set(C7222_EXAMPLES_BUILD OFF)
endif()

message(STATUS "C7222_EXAMPLES_BUILD is defined: ${C7222_EXAMPLES_BUILD}")

if(C7222_EXAMPLES_BUILD)
    set(C7222_EXAMPLES "")

    include(${ELEC_C7222_EXAMPLES_DIR}/freertos-board-example/freertos-board-example.cmake)
    include(${ELEC_C7222_EXAMPLES_DIR}/freertos-device-cpp/freertos-device-cpp.cmake)
    include(${ELEC_C7222_EXAMPLES_DIR}/ble/ble-examples.cmake)

    list(LENGTH C7222_EXAMPLES _c7222_examples_count)
    if(_c7222_examples_count EQUAL 0)
        message(FATAL_ERROR "C7222_EXAMPLES_BUILD is ON but no examples were added.")
    endif()
endif()
