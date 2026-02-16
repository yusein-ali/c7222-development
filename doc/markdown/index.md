# ELEC-C7222 Documentation

## Overview

This repository contains the source code required for completing the ELEC-C7222 Embedded Programming with Communication Devices course at Aalto University School of Electrical Engineering.

The project brings together application examples, reusable libraries, build infrastructure, and local copies of key reference documentation so development and learning can happen in one place.

**This project assumes the Raspberry Pi Pico 2 W variant. Other Pico variants may be supported, but they are not tested in this repository workflow.**

Detailed interfaces and module-level documentation are available in the [API Reference](api/index.html).

## Purpose

This project is intended to:

- Provide a practical base for developing and testing embedded software on Pico/Pico W hardware.
- Demonstrate both classic FreeRTOS C tasking and modern C++ threading workflows on top of FreeRTOS.
- Offer course-ready reusable components for device access and Bluetooth workflows.
- Keep important external documentation available locally in Markdown form for offline and IDE-native reading.

## Design Principles

- Modular composition: functionality is split into CMake interface libraries and composable modules.
- Build-time configurability: features and examples are enabled through CMake options.
- RTOS-first development: FreeRTOS is the scheduling foundation for both C and C++ runtime models.
- Practical C/C++ interoperability: both direct FreeRTOS C APIs and C++11 thread abstractions are supported.
- Documentation-in-repo: key third-party references are mirrored under `doc/` for quick access during implementation and debugging.

## Provided Components

- `getting-started/`: minimal FreeRTOS bring-up applications in C and C++.
- `libs/elec_c7222/`: main project library source tree with device abstractions, utilities, and BLE modules.
- `libs/FreeRTOS-Kernel/`: external FreeRTOS Kernel source used by the build.
- `libs/FreeRTOS_cpp11/`: FreeRTOS-cpp11 fork that enables C++11 threading facilities in embedded FreeRTOS systems.
- `cmake/`: import and configuration logic for Pico SDK, FreeRTOS Kernel, and FreeRTOS-cpp11.
- `doc/`: local documentation bundles and project guides.

## Documentation Bundles

The `doc/` directory includes:

- BTstack documentation in Markdown from the BlueKitchen repository.
- A Markdown fork/copy of the FreeRTOS Kernel Book.

These resources are included so users can access extensive documentation directly from their development environment.

## Build and Tooling

The entire project is built with CMake and is configured through top-level project files and CMake presets.

The project assumes Pico SDK setup through the Raspberry Pi Pico Visual Studio Code extension. During setup and first environment initialization, the extension workflow downloads required toolchains and build tooling.

## IDE Notes

- VS Code development is based on Cortex-Debug and Pico extension tooling.
- For cross-platform embedded C++ projects, `clangd` is recommended for language-server features.
- VS Code C++ debug pretty-printing can be limited for some STL objects, while CLion and multi-arch GDB workflows typically provide richer out-of-box debugging support.
- CLion project files are provided for improved embedded C++ debug experience.

## Start Here

- [Getting Started](markdown/getting-started.md)
- [Build Guide](markdown/building.md)
- [API Reference](api/index.html)

The API reference includes detailed documentation for all major modules, components, and interfaces described on this page.

## Credits

- This project was started by **Yusein R. Ali**, responsible teacher for the course **ELEC-C7222 - Embedded Programming with Communication Devices** (Aalto University, School of Electrical Engineering).
- **Reza Heidari** (Head TA in 2026) contributed to the project.

Comments, feedback, and suggestions are welcome.
