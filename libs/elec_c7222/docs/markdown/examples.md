# Examples

This document describes the example applications shipped with `libs/elec_c7222` and how to build them. The examples are intended for teaching and validation of the library on Raspberry Pico W platforms.

## Overview

The examples are organized under `libs/elec_c7222/examples` and are built through a common CMake selection mechanism. Exactly one example can be enabled at a time.

Examples currently provided:

- **FreeRTOS board validation** (`freertos-board-example`): exercises GPIO, LEDs, buttons, interrupts, and FreeRTOS primitives using the `c7222_pico_w_board` API. Intended for first-time hardware validation.
- **BLE GAP example** (`ble/gap`): focuses on GAP only. It initializes the BLE stack, configures advertising, registers a GAP event handler, and periodically updates manufacturer data.
- **BLE GATT server example** (`ble/gatt-server`): demonstrates an AttributeServer with a GATT profile, Security Manager configuration, characteristic discovery, and periodic temperature updates.

## Build selection model

The build system requires that exactly one example be enabled when `C7222_EXAMPLES_BUILD` is ON. Each example has its own option flag. The selected example provides its sources through an interface library that is linked to the final executable.

Key options (declared in `CMakeLists.txt`):

- `C7222_EXAMPLES_BUILD` (BOOL): enable example build selection logic.
- `C7222_EXAMPLE_FREERTOS_BOARD` (BOOL): enable FreeRTOS board example.
- `C7222_EXAMPLE_BLE_GAP` (BOOL): enable BLE GAP example.
- `C7222_EXAMPLE_BLE_GATT_SERVER` (BOOL): enable BLE GATT server example.
- `C7222_ENABLE_BLE` (BOOL): enable BLE support; required for BLE examples.
- `APP_NAME` (STRING): application name for the final executable.

When `C7222_EXAMPLES_BUILD` is OFF, the build falls back to sources from `APP_SOURCE_DIR` (default behavior). When it is ON, exactly one example must be enabled or CMake will emit a fatal error.

## How to configure and build

Use CMake presets or pass `-D` options manually. The following example configures and builds the BLE GAP example:

```bash
cmake -S . -B build \
  -DC7222_EXAMPLES_BUILD=ON \
  -DC7222_EXAMPLE_BLE_GAP=ON \
  -DC7222_EXAMPLE_BLE_GATT_SERVER=OFF \
  -DC7222_EXAMPLE_FREERTOS_BOARD=OFF \
  -DC7222_ENABLE_BLE=ON \
  -DAPP_NAME=PicoBleGapDemo

cmake --build build
```

Example configuration for the GATT server example:

```bash
cmake -S . -B build \
  -DC7222_EXAMPLES_BUILD=ON \
  -DC7222_EXAMPLE_BLE_GAP=OFF \
  -DC7222_EXAMPLE_BLE_GATT_SERVER=ON \
  -DC7222_EXAMPLE_FREERTOS_BOARD=OFF \
  -DC7222_ENABLE_BLE=ON \
  -DAPP_NAME=PicoBleGattDemo

cmake --build build
```

Example configuration for the FreeRTOS board example:

```bash
cmake -S . -B build \
  -DC7222_EXAMPLES_BUILD=ON \
  -DC7222_EXAMPLE_BLE_GAP=OFF \
  -DC7222_EXAMPLE_BLE_GATT_SERVER=OFF \
  -DC7222_EXAMPLE_FREERTOS_BOARD=ON \
  -DAPP_NAME=PicoBoardDemo

cmake --build build
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
    "C7222_EXAMPLE_BLE_GAP": "ON",
    "C7222_EXAMPLE_BLE_GATT_SERVER": "OFF",
    "C7222_EXAMPLE_FREERTOS_BOARD": "OFF",
    "C7222_ENABLE_BLE": "ON",
    "APP_NAME": "PicoBleGapDemo"
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
    "C7222_EXAMPLE_BLE_GAP": "OFF",
    "C7222_EXAMPLE_BLE_GATT_SERVER": "ON",
    "C7222_EXAMPLE_FREERTOS_BOARD": "OFF",
    "C7222_ENABLE_BLE": "ON",
    "APP_NAME": "PicoBleGattDemo"
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
    "C7222_EXAMPLES_BUILD": "ON",
    "C7222_EXAMPLE_BLE_GAP": "OFF",
    "C7222_EXAMPLE_BLE_GATT_SERVER": "OFF",
    "C7222_EXAMPLE_FREERTOS_BOARD": "ON",
    "APP_NAME": "PicoBoardDemo"
  }
}
```

### Creating new presets

To add a new preset:

1. Copy one of the example blocks above.
2. Give it a unique `name` and `displayName`.
3. Pick a unique `binaryDir` so builds do not collide.
4. Set exactly one example flag to `ON` and the others to `OFF`.
5. Set `C7222_ENABLE_BLE` to `ON` for BLE examples.
6. Optionally set `APP_NAME` to label the output ELF/UF2.

In VS Code, select a preset via **CMake: Select Configure Preset**, then run **CMake: Configure** and **CMake: Build**.
