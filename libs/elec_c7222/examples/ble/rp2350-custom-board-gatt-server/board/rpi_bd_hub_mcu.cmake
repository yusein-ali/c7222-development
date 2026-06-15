# This example configures a custom RP2350 board. It does not select a Pico SDK
# board identity; clear any incoming board override and select only the platform.
unset(PICO_BOARD CACHE)
unset(PICO_BOARD)
set(PICO_PLATFORM rp2350 CACHE STRING "Pico SDK platform used by the RPi BD Hub MCU example" FORCE)

# RP2354A is the RP2350A package with stacked 2 MB QSPI flash.
set(PICO_FLASH_SIZE_BYTES "(2 * 1024 * 1024)" CACHE STRING "Primary flash size" FORCE)
set(PICO_RP2350_A2_SUPPORTED 1 CACHE BOOL "Include RP2350 A2 support" FORCE)

list(APPEND PICO_CONFIG_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/rpi_bd_hub_mcu.h")
