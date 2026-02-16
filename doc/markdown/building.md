# Building The Project

This document explains how the CMake build is structured in this repository, which options are available, and how examples/projects are converted into final firmware targets.

## 1. Build System Overview

The project uses a single top-level `CMakeLists.txt` to:

- initialize Pico SDK/toolchain integration
- import external dependencies (FreeRTOS Kernel and FreeRTOS-cpp11)
- collect application/example interface libraries
- generate one executable per selected app/example target

The default board is configured as:

- `PICO_BOARD = pico2_w`

Toolchain defaults are aligned with Pico VS Code extension paths via:

- `cmake/pico-compiler-settings.cmake`

### 1.1 Platform selection: `rpi_pico` vs `grader`

Several parts of the library (BLE, GAP, GATT, Security Manager, and some device code) select platform-specific source files based on whether `PICO_SDK_PATH` is available:

- If `PICO_SDK_PATH` is defined and non-empty, platform sources are taken from `platform/rpi_pico/`.
- Otherwise, the build falls back to `platform/grader/`.

This pattern is used in:

- `libs/elec_c7222/ble/ble.cmake` and its submodules (`gap.cmake`, `gatt.cmake`, `security_manager.cmake`)
- `libs/elec_c7222/devices/devices.cmake`
- `libs/elec_c7222/utils/utils.cmake`

## 2. Key CMake Options

Main options defined in the root CMake:

- `C7222_ENABLE_BLE` (default `ON`)
  - Enables BLE helpers in `ELEC_C7222`.
  - Controls whether BLE examples are included.
- `C7222_BLE_DEBUG` (default `OFF`)
  - Adds `C7222_BLE_DEBUG=1` compile definition to generated app targets.
- `C7222_EXAMPLES_BUILD` (default `ON`)
  - Enables example target collection from `libs/elec_c7222/examples`.
- `C7222_GETTING_STARTED_BUILD` (default `ON`)
  - Enables getting-started targets from `getting-started/getting-started.cmake`.
- `C7222_EXPORT_PICO_UF2` (default `ON`)
  - Adds a post-build step that copies each target UF2 image into
    `${CMAKE_BINARY_DIR}/images/<target>.uf2` for drag-and-drop flashing via
    Pico MSC (BOOTSEL mode), without debugger tooling.

Additional cache variables:

- `PROJECT_NAME` (default `ELEC-C7222-CPP-LIB`)
- `PICO_BOARD` (default `pico2_w`)
- `FREERTOS_KERNEL_PATH` (optional override path)
- `FREERTOS_CPP11_PATH` (optional override path)

## 3. Dependency Import Design

### 3.1 FreeRTOS Kernel

Imported via:

- `cmake/FREERTOS_KERNEL_import.cmake`

Behavior:

- Uses `FREERTOS_KERNEL_PATH` if defined.
- Otherwise prefers local `libs/FreeRTOS-Kernel`.
- If missing, clones `FreeRTOS-Kernel` and initializes required submodules.
- Adds the platform port subdirectory to the build.

### 3.2 FreeRTOS-cpp11

Imported/configured via:

- `cmake/FreeRTOS_CPP11_import.cmake`
- `cmake/FreeRTOS_Cpp11_lib.cmake`

Behavior:

- Uses `FREERTOS_CPP11_PATH` if defined.
- Otherwise prefers local `libs/FreeRTOS_cpp11` or clones it.
- Exposes:
  - `FREERTOS_CPP11_SOURCES`
  - `FREERTOS_CPP11_INCLUDE_DIRS`

These sources are added to every generated executable to provide C++ threading integration on top of FreeRTOS.

## 4. Target Assembly Pattern

The build uses a two-layer model:

1. Interface library layer (app/example descriptors)
2. Final executable layer (created in one loop in root CMake)

### 4.1 Interface Library Contract

Each selectable app/example is represented by an `INTERFACE` target that carries:

- interface sources/includes
- required metadata properties:
  - `TARGET_NAME` (final executable name)
  - `TARGET_PATH` (source directory, for diagnostics)
  - optional `GATT_FILES` (for BLE profile header generation)

This pattern is used by:

- `libs/elec_c7222/examples/.../*.cmake`
- `getting-started/getting-started.cmake`

### 4.2 APP_LIBS Collection

The root CMake builds a unified `APP_LIBS` list:

- if `C7222_EXAMPLES_BUILD=ON`: append `C7222_EXAMPLES`
- if `C7222_GETTING_STARTED_BUILD=ON`: append `C7222_GETTING_STARTED`

Then it iterates over `APP_LIBS` and creates one executable per interface library.

### 4.3 ELEC_C7222 library composition

The helper library is defined in:

- `libs/elec_c7222/elec_c7222.cmake`

It creates the interface target:

- `ELEC_C7222`

and links the main subcomponents:

- `ELEC_C7222_DEVICES` (from `libs/elec_c7222/devices/devices.cmake`)
- `ELEC_C7222_UTILS` (from `libs/elec_c7222/utils/utils.cmake`)
- optionally `ELEC_C7222_BLE` (from `libs/elec_c7222/ble/ble.cmake`) when `C7222_ENABLE_BLE=ON`

Most apps/examples only need to link against `ELEC_C7222` to inherit the full library stack.


## 5. How Executables Are Generated

For each entry in `APP_LIBS`, root CMake:

1. Reads metadata (`TARGET_NAME`, `TARGET_PATH`, optional `GATT_FILES`)
2. Creates executable:
   - `add_executable(<TARGET_NAME> ${FREERTOS_CPP11_SOURCES})`
3. Links common dependencies:
   - Pico SDK libs (`pico_stdlib`, hardware libs)
   - FreeRTOS kernel heap/port target
   - BTstack/CYW43 libs
   - `ELEC_C7222`
   - selected interface library itself
4. Applies compile definitions for libstdc++ gthread support:
   - `_GLIBCXX_HAS_GTHREADS=1`
   - `_GLIBCXX_USE_C99_STDINT_TR1=1`
5. Enables Pico outputs (`.elf`, `.uf2`, etc.) via `pico_add_extra_outputs`
6. If BLE and `GATT_FILES` are present, runs `pico_btstack_make_gatt_header`

### 5.2 Post-build UF2 export for Pico MSC flashing

When `C7222_EXPORT_PICO_UF2=ON` (default), each built executable adds a
post-build copy step:

- source UF2: `${CMAKE_CURRENT_BINARY_DIR}/<TARGET_NAME>.uf2`
- exported UF2: `${CMAKE_BINARY_DIR}/images/<TARGET_NAME>.uf2`

This exported file is intended for direct copy to the Pico mass-storage drive
that appears when the board is connected in BOOTSEL mode.

### 5.1 BLE `.gatt` → generated header

When an app/example interface library defines the `GATT_FILES` property, the root CMake iterates over those `.gatt` files and invokes the Pico SDK helper:

- `pico_btstack_make_gatt_header(<TARGET_NAME> PRIVATE "<file.gatt>")`

This generates a C header representation of the GATT database and wires it into the build of the executable target.

Overall, each app/example inherits a consistent platform/runtime baseline while contributing only its own sources through the interface library.

Default UART stdio behavior:

- The root CMake applies `PICO_DEFAULT_UART_BAUD_RATE=921600`, so default UART stdio output is configured for 921600 baud.

## 6. Examples And Getting-Started Targets

### 6.1 Examples

When `C7222_EXAMPLES_BUILD=ON`, examples are loaded from:

- `libs/elec_c7222/examples/examples.cmake`

Current set includes:

- `example-freertos-board`
- `example-freertos-device-cpp`
- BLE examples (when BLE enabled), such as:
  - `example-ble-gap`
  - `example-ble-gatt-server`

BLE examples may append `.gatt` files to `GATT_FILES` for header generation.

#### 6.1.1 How to add a new example target

Examples are registered as `INTERFACE` libraries and then collected into the global `C7222_EXAMPLES` list.

A typical example `.cmake` file does the following:

1) Create an interface library:
- `add_library(<EXAMPLE_LIB> INTERFACE)`

2) Set required metadata properties (used by the root executable-generation loop):
- `TARGET_NAME`: the final executable name
- `TARGET_PATH`: directory path for diagnostics/logging

3) Add sources/includes:
- `target_sources(<EXAMPLE_LIB> INTERFACE ...)`
- `target_include_directories(<EXAMPLE_LIB> INTERFACE ...)`

4) (Optional) Register `.gatt` files:
- append them to the target property `GATT_FILES`

5) Add the example to the global list:
- `list(APPEND C7222_EXAMPLES <EXAMPLE_LIB>)`

See existing examples for the exact pattern:
- `libs/elec_c7222/examples/ble/gatt-server/ble-gatt-server-example.cmake`
- `libs/elec_c7222/examples/ble/gap/ble-gap-example.cmake`


### 6.2 Getting Started

When `C7222_GETTING_STARTED_BUILD=ON`, targets are loaded from:

- `getting-started/getting-started.cmake`

Targets:

- `getting-started-freertos-c`
- `getting-started-freertos-cpp`

These are configured along with the rest of enabled targets and are typically the first targets to debug.

## 7. Common Configure/Build Commands

Using CMake presets:

```bash
cmake --preset Debug
cmake --build build/Debug -j
```

Or explicit configure/build:

```bash
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug -j
```

Examples of option overrides:

```bash
cmake --preset Debug \
  -DC7222_EXAMPLES_BUILD=ON \
  -DC7222_GETTING_STARTED_BUILD=ON \
  -DC7222_ENABLE_BLE=ON \
  -DC7222_BLE_DEBUG=OFF
```

Build a specific target:

```bash
cmake --build build/Debug --target getting-started-freertos-c
cmake --build build/Debug --target getting-started-freertos-cpp
```

## 8. Design Rationale Summary

This CMake design is intentionally centered on interface-library descriptors and one executable-generation loop to keep:

- target registration modular
- common linker/runtime setup centralized
- examples/getting-started extensible without duplicating root build logic

## 9. Serial And RTOS Debug Views

Serial console viewing options:

- VS Code extension: `ms-vscode.vscode-serial-monitor` (Serial Monitor)
- CLion plugin: `Serial Port Monitor` (JetBrains)

RTOS task/object visualization options:

- `xRTOS` extension support in VS Code
- related xRTOS support/plugin in CLion for detailed RTOS object/task views

The project FreeRTOS configuration is set up to support these debugger views, including runtime-stats and trace-related options in `libs/elec_c7222/config/FreeRTOSConfig.h` together with runtime hooks in `libs/elec_c7222/utils/platform/*/freertos_hooks.c`.

## 10. Generating Local Documentation (Doxygen)

To generate local API/docs output from this repository, install:

- `doxygen`
- Graphviz `dot` tool

`dot` is needed because the Doxygen configuration enables call/include/collaboration graphs (`HAVE_DOT=YES`).

### 10.1 Install Tools

macOS (Homebrew):

```bash
brew install doxygen graphviz
```

Ubuntu/Debian:

```bash
sudo apt update
sudo apt install -y doxygen graphviz
```

Windows (winget):

```powershell
winget install --id DimitriVanHeesch.Doxygen -e
winget install --id Graphviz.Graphviz -e
```

### 10.2 Verify Installation

```bash
doxygen --version
dot -V
```

### 10.3 Generate Documentation

From repository root:

```bash
doxygen Doxyfile
```

Output is generated under:

- `doc/elec-c7222/html/`
