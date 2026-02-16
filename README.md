# ELEC-C7222 Embedded Development Project

<!--
<div style="display:none">
\subpage md_doc_2markdown_2getting-started "Getting Started"
\subpage md_doc_2markdown_2building "Building The Project"
\subpage md_libs_2elec__c7222_2docs_2markdown_2overview "ELEC-C7222 Library Overview"
</div>
-->

Complete documentation is available at: https://yusein-ali.github.io/c7222-development/

## Overview

This repository is a Raspberry Pi Pico 2 W focused embedded development workspace for:

- FreeRTOS-based C and C++ applications.
- Course-oriented embedded communication examples.
- Reusable platform and BLE helper libraries.

The project brings together application examples, reusable libraries, build infrastructure, and local copies of key reference documentation so development and learning can happen in one place.

**This project assumes the Raspberry Pi Pico 2 W variant. Other Pico variants may be supported, but they are not tested in this repository workflow.**

## Purpose

The project is intended to:

- Provide a practical base for developing and testing embedded software on Pico/Pico W hardware.
- Demonstrate both classic FreeRTOS C tasking and modern C++ threading workflows on top of FreeRTOS.
- Offer course-ready reusable components (`elec_c7222`) for device access and Bluetooth workflows.
- Keep important external documentation available locally in Markdown form for offline and IDE-native reading.


## Design Principles

- Modular composition: functionality is split into small CMake interface libraries and composable modules.
- Build-time configurability: features/examples are enabled through CMake options instead of hard-coded build scripts.
- RTOS-first development: FreeRTOS is the scheduling foundation for both C and C++ runtime models.
- Practical C/C++ interoperability: the repository supports both direct FreeRTOS C APIs and C++11 thread abstractions.
- Documentation-in-repo: major third-party references are mirrored under `doc/` for quick access during implementation and debugging.

## Provided Components

- [`getting-started/`](getting-started)
  - Minimal FreeRTOS bring-up applications:
  - [`freertos-c/main_freertos.c`](getting-started/freertos-c/main_freertos.c): C example using `xTaskCreate` + `vTaskDelay`.
  - [`freertos-cpp/main_freertos.cpp`](getting-started/freertos-cpp/main_freertos.cpp): C++ example using `std::thread` mapped onto FreeRTOS.
- [`libs/elec_c7222/`](libs/elec_c7222)
  - Main project library source tree (actual course/library implementation).
  - Course helper library with device abstractions, utilities, and BLE modules.
  - Includes examples that are integrated as CMake interface libraries.
- [`libs/FreeRTOS-Kernel/`](libs/FreeRTOS-Kernel)
  - External FreeRTOS Kernel source used by the build.
- [`libs/FreeRTOS_cpp11/`](libs/FreeRTOS_cpp11)
  - Course fork of FreeRTOS-cpp11 integration layer.
  - Enables C++11 threading facilities in embedded FreeRTOS systems (`std::thread`, mutex/condition variable support, `sleep_for`, etc.).
- [`cmake/`](cmake)
  - Import/configuration logic for Pico SDK, FreeRTOS Kernel, and FreeRTOS-cpp11.
- [`doc/`](doc)
  - Local documentation bundles (details below).
- [`.clang-format`](.clang-format)
  - Project formatting rules for `clang-format`-based source formatting.
- [`config.openocd`](config.openocd)
  - OpenOCD configuration used for CLion-based debug sessions.
- [`Doxyfile`](Doxyfile)
  - Root Doxygen configuration file for generating merged project documentation (HTML output under `doc/elec-c7222/html/`).

## Documentation Bundles in `doc/`

- [`doc/BTstack-md/`](doc/BTstack-md)
  - Markdown documentation from the original BlueKitchen BTstack project.
  - This is intended to make extensive BTstack references available directly in your local development environment/IDE.
  - BlueKitchen website: [https://bluekitchen-gmbh.com/](https://bluekitchen-gmbh.com/)
  - BTstack GitHub repository: [https://github.com/bluekitchen/btstack](https://github.com/bluekitchen/btstack)
- [`doc/FreeRTOS-Kernel-Book/`](doc/FreeRTOS-Kernel-Book)
  - Markdown fork/copy of the FreeRTOS Kernel Book.
  - Included so users can read full kernel concepts and API guidance without leaving the development environment.

## Project Library Documentation

The actual project libraries reside under:

- [`libs/elec_c7222/`](libs/elec_c7222)

The primary project documentation for these libraries is under:

- [`libs/elec_c7222/docs/`](libs/elec_c7222/docs)

Library board support targets the **ELEC-C7222 Pico2W board**. Board-specific details are documented in:

- [`libs/elec_c7222/devices/doc/markdown/c7222_board.md`](libs/elec_c7222/devices/doc/markdown/c7222_board.md)

Reference:

- [`libs/elec_c7222/docs/markdown/overview.md`](libs/elec_c7222/docs/markdown/overview.md)

## External FreeRTOS-Related Developments in `libs/`

- [`libs/FreeRTOS-Kernel/`](libs/FreeRTOS-Kernel): FreeRTOS Kernel source tree used by this project.
- [`libs/FreeRTOS_cpp11/`](libs/FreeRTOS_cpp11): FreeRTOS-cpp11 fork used by this project to provide C++ threading integration on FreeRTOS.
  - Original author: Piotr Grygorczuk
  - Original repository: [https://github.com/grygorek/FreeRTOS_cpp11](https://github.com/grygorek/FreeRTOS_cpp11)

## Build System

The entire project is built with CMake.

Key points:

- Top-level configuration is driven by [`CMakeLists.txt`](CMakeLists.txt) and [`CMakePresets.json`](CMakePresets.json).
- App/example targets are assembled from interface libraries and expanded into final executables in the root build logic.
- Optional feature sets are toggled through CMake options (for example examples/getting-started selection).

## Environment Assumptions

This project assumes the Pico SDK is installed by the **Raspberry Pi Pico Visual Studio Code extension**.

Before building, install the extension, then run its Pico setup/bootstrap flow so the SDK/toolchain paths are available in your environment.
The cross-compiler toolchain and required build-system components are downloaded during that setup/first initialization flow (not during extension installation itself).

## IDE Notes

- VS Code is supported for standard workflow and SDK setup.
- For cross-platform/cross-toolchain projects, the `clangd` language server often provides more reliable code intelligence than default IntelliSense.
- How to enable `clangd` in VS Code:
  - Extension: [vscode-clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
  - Setup guide: [https://clangd.llvm.org/installation](https://clangd.llvm.org/installation)
- VS Code embedded debugging in this workflow is typically based on `cortex-debug`, which has limited C++ debugging support, especially around pretty-printing of C++ standard library objects.
- These limitations can often be worked around with `stdio`-based debug prints, and that approach is practical in many embedded workflows.
- For richer C++ debugging, multi-arch GDB provides built-in capabilities, and CLion provides out-of-box, platform-independent debugger integration.
  - This is the reason CLion project files (in [`.idea/`](.idea)) are provided in this repository.

## Build and Getting Started Guides

For detailed, step-by-step instructions, use the local guides:

- Build guide: [`doc/markdown/building.md`](doc/markdown/building.md)
- Getting started guide: [`doc/markdown/getting-started.md`](doc/markdown/getting-started.md)

## License

This repository is licensed under the terms described in the [`LICENSE`](LICENSE) file.

## Credits

- This project was started by **Yusein R. Ali**, responsible teacher for the course **ELEC-C7222 - Embedded Programming with Communication Devices** (Aalto University, School of Electrical Engineering).
- **Reza Heidari** (Head TA in 2026) contributed to the project.

Comments, feedback, and suggestions are welcome.
