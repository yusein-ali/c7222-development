set(ELEC_C7222_UTILS_DIR ${CMAKE_CURRENT_LIST_DIR})

if(DEFINED PICO_SDK_PATH AND NOT PICO_SDK_PATH STREQUAL "")
    file(GLOB ELEC_C7222_UTILS_PLATFORM_SOURCES
        "${ELEC_C7222_UTILS_DIR}/platform/rpi_pico/*.cpp"
        "${ELEC_C7222_UTILS_DIR}/platform/rpi_pico/*.c")
else()
    file(GLOB ELEC_C7222_UTILS_PLATFORM_SOURCES
        "${ELEC_C7222_UTILS_DIR}/platform/grader/*.cpp"
        "${ELEC_C7222_UTILS_DIR}/platform/grader/*.c")
endif()

file(GLOB ELEC_C7222_UTILS_SOURCES
    "${ELEC_C7222_UTILS_DIR}/src/*.cpp"
    "${ELEC_C7222_UTILS_DIR}/src/*.c")

add_library(ELEC_C7222_UTILS INTERFACE)

target_sources(ELEC_C7222_UTILS
    INTERFACE
    ${ELEC_C7222_UTILS_SOURCES}
    ${ELEC_C7222_UTILS_PLATFORM_SOURCES})

target_include_directories(ELEC_C7222_UTILS
    INTERFACE
    "${ELEC_C7222_UTILS_DIR}/include")
