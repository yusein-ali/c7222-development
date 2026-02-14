# Getting Started

This guide walks through first-time setup for this repository in VS Code with CMake Tools and the Raspberry Pi Pico extension.

For full build details (toolchain assumptions, options, and build variants), see [`building.md`](building.md).

## 1. Clone The Repository

```bash
git clone <your-repo-url>
cd c7222-development
```

## 2. Initialize Submodules

This project depends on Git submodules (for example external FreeRTOS-related sources and docs mirrors).

```bash
git submodule update --init --recursive
```

If you already cloned and need to refresh:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

## 3. Install Required VS Code Extensions

Install at minimum:

- `raspberry-pi.raspberry-pi-pico`
- `ms-vscode.cmake-tools`
- `marus25.cortex-debug`
- `ms-vscode.cpptools`

The Pico extension is expected to provision Pico SDK/toolchain content under `~/.pico-sdk`.

Useful optional extensions/tools:

- VS Code serial console: `ms-vscode.vscode-serial-monitor` (Serial Monitor)
- CLion serial console: `Serial Port Monitor` plugin (JetBrains)
- RTOS task/object inspection: `xRTOS` (VS Code) and related xRTOS support/plugin in CLion

This project configures FreeRTOS runtime stats/trace support for detailed RTOS views (tasks/objects), including settings from `libs/elec_c7222/config/FreeRTOSConfig.h` and runtime hooks under `libs/elec_c7222/utils/platform/*/freertos_hooks.c`.

## 4. What CMake Presets Are

CMake presets are named, version-controlled configuration profiles stored in [`CMakePresets.json`](../../CMakePresets.json).

In this repository:

- `Debug`: configures a debug build in `build/Debug`
- `Release`: configures a release build in `build/Release`

Why use presets:

- Same configuration across all developers.
- No need to remember long configure commands.
- Predictable build directories and cache values.

## 5. Enable And Use CMake Presets In VS Code

Open the repository folder in VS Code, then use Command Palette (`Ctrl/Cmd+Shift+P`):

1. `CMake: Select Configure Preset`
2. Choose `Pico - Debug` (or `Pico - Release`)
3. `CMake: Configure`

After configure completes, CMake Tools generates the selected build tree and target metadata.

## 6. Select Build And Run/Debug Targets

Use Command Palette actions in this order:

1. `CMake: Set Build Target`
2. Select the executable target you want (for example `getting-started-freertos-cpp`).
3. `CMake: Set Launch/Debug Target` (if shown in your CMake Tools version)
4. Pick the same executable as launch target.

Notes:

- Build target controls what `CMake: Build` builds by default.
- Launch/debug target controls what `CMake: Debug` or Run actions execute.

### Getting-Started Targets

When you configure the project, the getting-started executables are configured together with the rest of the available targets.

The two getting-started targets are:

- `getting-started-freertos-c`
- `getting-started-freertos-cpp`

For a first debug session, the natural starting point is `getting-started-freertos-c`, since it is the simpler FreeRTOS C setup.

## 7. Build, Clean, And Rebuild

Common actions from Command Palette:

- `CMake: Build`
- `CMake: Clean`
- `CMake: Clean Rebuild` (or run Clean then Build)

Recommended first-time flow:

1. Configure preset.
2. Set build target.
3. Build.

UART logging note:

- The project CMake configuration sets default stdio UART baud rate to `921600` via `PICO_DEFAULT_UART_BAUD_RATE`.

## 8. Debugging In VS Code (CMake Tools + Pico + Cortex-Debug)

### 8.1 Typical Debug Flow

1. `CMake: Select Configure Preset` -> choose `Pico - Debug`
2. `CMake: Configure`
3. `CMake: Set Build Target`
4. `CMake: Build`
5. Start debug from:
   - `CMake: Debug`, or
   - Run and Debug panel -> `Pico Debug (Cortex-Debug)`

### 8.2 How Pico Extension Helps

The Pico extension integrates SDK/toolchain and debug metadata into VS Code.
In this repository, `launch.json` uses Pico extension command variables such as:

- `${command:raspberry-pi-pico.launchTargetPath}`
- `${command:raspberry-pi-pico.getGDBPath}`
- `${command:raspberry-pi-pico.getTarget}`
- `${command:raspberry-pi-pico.getChip}`

This allows launch configurations to resolve correct ELF path, GDB binary, target config, and SVD without hard-coding per-machine values.

### 8.3 OpenOCD And Launch Configurations

Provided debug configurations include:

- `Pico Debug (Cortex-Debug)` (extension-managed OpenOCD)
- `Pico Debug (Cortex-Debug with external OpenOCD)`

`config.openocd` is provided primarily for CLion/OpenOCD workflows; VS Code generally uses `.vscode/launch.json` and Pico extension variables.

## 9. CLion Run/Debug Configurations

The included CLion project setup provides similar build and debug configurations.
You can select them from CLion's **Run/Debug Configurations** menu, and they should work out of the box for the standard project workflow.

## 10. Practical Workflow Summary

For daily use, this sequence is effective:

1. Pull latest + update submodules.
2. Select preset (`Debug` for development).
3. Configure once after CMake changes.
4. Set build target.
5. Build.
6. Debug with `CMake: Debug` or Run and Debug panel.

## 11. Next Reading

- Build details: [`building.md`](building.md)
- Repository overview: [`../../README.md`](../../README.md)
- Library docs overview: [`../../libs/elec_c7222/docs/markdown/overview.md`](../../libs/elec_c7222/docs/markdown/overview.md)
