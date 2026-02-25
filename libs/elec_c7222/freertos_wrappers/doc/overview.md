# FreeRTOS Wrappers Overview

## Purpose

`freertos_wrappers` provides a consistent C++ API for RTOS synchronization,
communication, and tasking primitives across two targets:

- `rpi_pico`: wrappers call native FreeRTOS APIs.
- `grader`: wrappers call grader-facing `extern "C"` hooks (and in some cases
  hosted fallback behavior), so tests can store/inspect state and drive events.

The goal is to keep application code mostly platform-agnostic while preserving
core FreeRTOS semantics (ticks, ISR variants, explicit start/stop/take/give),
including critical section enter/exit behavior.

## Design model

- Most classes are ownership-based wrappers over an underlying handle, with
  destructor cleanup for that handle.
- Default constructors are lightweight; resources are created by `Initialize()`
  or convenience constructors.
- Re-initialization typically deletes/unregisters previous resources first.
- Copy/move are disabled to avoid duplicating ownership of RTOS handles.

## FreeRTOS Configuration Notes

The FreeRTOS configuration in `libs/elec_c7222/config/FreeRTOSConfig.h` and the associated hooks are intended to:

1. Enable runtime stats collection for IDE/debugger task views.
2. Support FreeRTOS‑CPP11 integration, including C++ standard library multithreading (e.g., `std::thread`) in course examples.

## RAII: when it helps and where to be careful

RAII is useful for deterministic cleanup:

- If an object goes out of scope, wrapper destructor releases the RTOS/grader
  resource.
- This reduces leaks and stale-handle bugs.
- For synchronization operations, acquisition/release is still explicit
  (`Lock/Unlock`, `Take/Give`, `Enter/Exit`); wrappers are not scoped guard
  types that auto-acquire in constructors.

RAII does **not** replace runtime lifecycle planning:

- Do not destroy wrappers while other tasks/ISRs may still access them.
- For long-lived primitives shared globally, static/storage-duration objects or
  explicit shutdown order is usually safer than short stack scope.
- For `FreeRtosTask`, deletion in destructor is convenient but you still must
  ensure the task body and any referenced data are safe to stop at that point.

## Wrapper usage by class

### `FreeRtosTimer`

Use for one-shot/periodic deferred callbacks.

Common pattern:

- Configure callback with `Initialize(...)` or constructor.
- Start with `Start(...)` (or `StartFromISR(...)`).
- Stop/reset/change period as needed.

Notes:

- Callback runs in timer-service context (`rpi_pico`) or grader equivalent.
- Keep callback short and non-blocking.

### `FreeRtosTask`

Use to create/manage an execution context.

Common pattern:

- Provide task function and optional argument.
- Start via constructor/`Initialize(...)`.
- Control with `Suspend/Resume`, priority setters, `Delete`.

Notes:

- The task function is stored as `std::function<void(void*)>`.
- Keep referenced data alive for the task lifetime.

#### Static current-thread and scheduler helpers

`FreeRtosTask` also provides static utilities for operations tied to the
current execution context and scheduler:

- `Delay(ticks)`: block the current task for a number of ticks.
- `Yield()`: yield the CPU from the current task.
- `GetTickCount()`: read the current scheduler tick counter.
- `MsToTicks(ms)`: convert milliseconds to scheduler ticks.
- `IdlePriority()`: return the platform idle-priority baseline.
- `StartScheduler()`: start the scheduler (typically from `main`).

These helpers let examples avoid direct `FreeRTOS.h` macro/API calls in
application code while preserving FreeRTOS semantics.

Typical usage:

```cpp
#include "freertos_task.hpp"

[[noreturn]] void worker_task(void* arg) {
	(void)arg;
	while(true) {
		// Do periodic work.
		c7222::FreeRtosTask::Delay(c7222::FreeRtosTask::MsToTicks(100));
	}
}

int main() {
	static c7222::FreeRtosTask worker;
	(void)worker.Initialize("Worker",
							1024,
							c7222::FreeRtosTask::IdlePriority() + 1,
							worker_task,
							nullptr);
	c7222::FreeRtosTask::StartScheduler();
	for(;;) {}
}
```

### `FreeRtosBinarySemaphore`

Use for event signaling (single token available/unavailable).

Common pattern:

- `Take(...)` in waiter task.
- `Give()` or `GiveFromISR()` in producer/ISR.

### `FreeRtosCountingSemaphore`

Use for bounded resource/token counting.

Common pattern:

- Initialize with `(max_count, initial_count)`.
- `Take(...)` to consume token, `Give()`/`GiveFromISR()` to return token.
- `GetCount()` for observability.

### `FreeRtosMutex` and `FreeRtosRecursiveMutex`

Use for mutual exclusion of shared state.

Common pattern:

- `Lock(...)` before critical section.
- `Unlock()` after critical section.
- Use recursive variant only when same owner must re-enter lock.

Notes:

- Prefer mutexes (not semaphores) for protecting shared data ownership.

### `FreeRtosCriticalSection`

Use for short non-blocking regions that require interrupt/scheduler exclusion.

Common pattern:

- `Enter()` immediately before the protected instructions.
- `Exit()` immediately after the protected instructions.
- `IsEntered()` reports whether this wrapper object currently owns the critical section.

Notes:

- Keep critical sections very short.
- Do not perform blocking RTOS operations while entered.
- The wrapper tracks a binary entered state (entered/not-entered), and destructor cleanup provides RAII if `Exit()` is missed.

### `FreeRtosEventGroup`

Use for bitmask-based synchronization across tasks.

Common pattern:

- Producer sets bits via `SetBits(...)` / `SetBitsFromISR(...)`.
- Consumer waits via `WaitBits(...)` with clear/wait-all options.

### `FreeRtosQueue`

Use for fixed-size item passing with backpressure.

Common pattern:

- Initialize with queue length and item size.
- `Send(...)`/`Receive(...)` for task context.
- ISR counterparts for interrupt context.

Notes:

- `Overwrite(...)` is intended for mailbox-style queues (length 1).

### `FreeRtosTaskNotification`

Use for direct task-to-task signaling/value update.

Common pattern:

- Notify target task with `Notify*`.
- Target task blocks with `Wait*` or `Take*`.

Notes:

- Generally lower overhead than separate queue/semaphore in one-to-one cases.

### `FreeRtosStreamBuffer`

Use for byte-stream transfer where message boundaries are not required.

Common pattern:

- Producer sends bytes with `Send*`.
- Consumer reads available bytes with `Receive*`.

### `FreeRtosMessageBuffer`

Use for variable-size discrete message transfer.

Common pattern:

- Producer sends one message at a time with `Send*`.
- Consumer receives full next message with `Receive*`.

## Structure

- Public API: `freertos_wrappers/include/`
- Implementations:
  - `freertos_wrappers/platform/rpi_pico/`
  - `freertos_wrappers/platform/grader/`
- Module build file: `freertos_wrappers/freertos_wrappers.cmake`
