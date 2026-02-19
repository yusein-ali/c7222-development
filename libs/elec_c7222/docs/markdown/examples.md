# ELEC-C7222 Library Examples

<div style="display:none">
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2examples_2security-manager
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2examples_2custom-service-rw
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2examples_2custom-service-notify
</div>


This document describes the example applications shipped with `libs/elec_c7222` and how to build them. The examples are intended for teaching and validation of the library on Raspberry Pico W platforms.

## Overview

The examples are organized under `libs/elec_c7222/examples` and are built through a common CMake selection mechanism. When `C7222_EXAMPLES_BUILD` is ON, all example targets are created and can be selected in your build/IDE.

Examples currently provided:

- **FreeRTOS board validation** (`freertos-board-example`): exercises GPIO, LEDs, buttons, interrupts, and FreeRTOS primitives using the `c7222_pico_w_board` API. Intended for first-time hardware validation.
- **FreeRTOS device C++ example** (`freertos-device-cpp`): demonstrates C++ device wrappers, SafeLed/ButtonEvent helpers, and ISR-to-task dispatch using FreeRTOS wrapper classes (`FreeRtosTimer`, `FreeRtosTask`).
- **BLE GAP example** (`ble/gap`): focuses on GAP only. It initializes the BLE stack, configures advertising, registers a GAP event handler, and periodically updates manufacturer data using FreeRTOS wrapper classes (`FreeRtosTask`).
- **BLE GATT server example** (`ble/gatt-server`): demonstrates an AttributeServer with a GATT profile, Security Manager configuration, characteristic discovery, and periodic temperature updates using FreeRTOS wrapper classes (`FreeRtosTask`, `FreeRtosTimer`).

## Build selection model

The build system creates an interface library per example (under `libs/elec_c7222/examples`) and the top-level CMake assembles the list when `C7222_EXAMPLES_BUILD` is ON. Each example target carries metadata properties such as `TARGET_NAME`, `TARGET_PATH`, and optional `GATT_FILES` for BLE apps.

Key options (declared in `CMakeLists.txt`):

- `C7222_EXAMPLES_BUILD` (BOOL): enable example build targets.
- `C7222_ENABLE_BLE` (BOOL): enable BLE support; required for BLE examples.
- `APP_NAME` (STRING): application name for the final executable when examples are not built.

When `C7222_EXAMPLES_BUILD` is OFF, the build falls back to sources from `APP_SOURCE_DIR` (default behavior). When it is ON, all example targets are available and you can select the desired executable in your IDE or via CMake target selection.

## How to configure and build

Use CMake presets or pass `-D` options manually. The following example configures and builds all examples, then builds the BLE GAP target:

```bash
cmake -S . -B build \
  -DC7222_EXAMPLES_BUILD=ON \
  -DC7222_ENABLE_BLE=ON

cmake --build build --target example-ble-gap
```

Example build for the GATT server example:

```bash
cmake --build build --target example-ble-gatt-server
```

Example build for the FreeRTOS board example:

```bash
cmake --build build --target example-freertos-board

Example build for the FreeRTOS device C++ example:

```bash
cmake --build build --target example-freertos-device-cpp
```
```

## Clearing cache and rebuilding

CMake caches configuration in the build directory. To switch examples or change options cleanly, remove the build directory or the cache file, then reconfigure.

Option A: remove the build directory:

```bash
rm -rf build
cmake -S . -B build -D<options...>
cmake --build build
```

Option B: remove only the cache file:

```bash
rm -f build/CMakeCache.txt
cmake -S . -B build -D<options...>
cmake --build build
```

If you use CMake Tools in VS Code, you can also run **CMake: Delete Cache and Reconfigure** from the command palette, then rebuild.

## Using CMake Presets

This project already includes `CMakePresets.json` at the repository root. You can modify that file to add or adjust presets as needed, then select them in VS Code or on the command line.

Example preset for the BLE GAP example:

```json
{
  "name": "ble-gap-example",
  "displayName": "BLE GAP Example",
  "generator": "Ninja",
  "binaryDir": "${sourceDir}/build/ble-gap",
  "cacheVariables": {
    "C7222_EXAMPLES_BUILD": "ON",
    "C7222_ENABLE_BLE": "ON"
  }
}
```

Example preset for the BLE GATT server example:

```json
{
  "name": "ble-gatt-server-example",
  "displayName": "BLE GATT Server Example",
  "generator": "Ninja",
  "binaryDir": "${sourceDir}/build/ble-gatt-server",
  "cacheVariables": {
    "C7222_EXAMPLES_BUILD": "ON",
    "C7222_ENABLE_BLE": "ON"
  }
}
```

Example preset for the FreeRTOS board example:

```json
{
  "name": "freertos-board-example",
  "displayName": "FreeRTOS Board Example",
  "generator": "Ninja",
  "binaryDir": "${sourceDir}/build/freertos-board",
  "cacheVariables": {
    "C7222_EXAMPLES_BUILD": "ON"
  }
}
```

### Creating new presets

To add a new preset:

1. Copy one of the example blocks above.
2. Give it a unique `name` and `displayName`.
3. Pick a unique `binaryDir` so builds do not collide.
4. Set `C7222_ENABLE_BLE` to `ON` for BLE examples.
5. Optionally set `APP_NAME` to label the output ELF/UF2.

In VS Code, select a preset via **CMake: Select Configure Preset**, then run **CMake: Configure** and **CMake: Build**.
