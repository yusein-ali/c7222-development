**Utils Overview**

This document summarizes the `libs/elec_c7222/utils` utilities layer. These helpers provide small, reusable building blocks used across the course codebase, especially in FreeRTOS and C++ applications.

**Purpose**

The utilities module exists to:

1. Provide safe base classes for copy/move semantics.
2. Wrap common FreeRTOS concepts in C++-friendly interfaces.
3. Supply platform hook implementations that enable debugging features such as runtime stats.

**Utility Classes**

**1) `NonCopyable`, `NonMovable`, `NonCopyableNonMovable`, `MovableOnly`, `CopyableOnly`**

Location: `libs/elec_c7222/utils/include/non_copyable.hpp`

Purpose:
1. `NonCopyable`: disables copy operations while allowing move.
2. `NonMovable`: disables move operations while allowing copy.
3. `NonCopyableNonMovable`: disables both copy and move.
4. `MovableOnly`: allows move but forbids copy (convenience base).
5. `CopyableOnly`: allows copy but forbids move (convenience base).

These base classes are intended for hardware handles, singleton-like services, and objects that must not be duplicated or relocated.

**2) `FreeRtosTimer`**

Location: `libs/elec_c7222/utils/include/freertos_timer.hpp`

Purpose:
1. RAII wrapper around FreeRTOS software timers.
2. Provides explicit `Initialize()` plus `Start()`, `Stop()`, `Reset()`, and `ChangePeriod()` APIs.
3. Uses a `std::function<void()>` callback invoked from the FreeRTOS timer service task.

**Platform-Specific Hooks**

**FreeRTOS Hooks for Pico Platform**

Location: `libs/elec_c7222/utils/platform/rpi_pico/freertos_hooks.c`

These hooks provide:
1. Stack overflow diagnostics in `vApplicationStackOverflowHook`.
2. Runtime stats support (`vConfigureTimerForRunTimeStats`, `ulGetRunTimeCounterValue`) for debugger views such as VS Code **FreeRTOS**/RTOS task inspection.

**FreeRTOS Hooks for Grader Platform**

Location: `libs/elec_c7222/utils/platform/grader/freertos_hooks.c`

These hooks mirror the Pico behavior, using `clock_gettime` for runtime stats in the simulated environment.

**FreeRTOS Configuration Notes**

The FreeRTOS configuration in `libs/elec_c7222/config/FreeRTOSConfig.h` and the associated hooks are intended to:

1. Enable runtime stats collection for IDE/debugger task views.
2. Support FreeRTOS‑CPP11 integration, including C++ standard library multithreading (e.g., `std::thread`) in course examples.
