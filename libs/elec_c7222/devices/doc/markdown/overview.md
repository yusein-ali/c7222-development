**Devices Overview**

This document introduces the ELEC_C7222 devices layer, its design principles, and how to use it across the two supported platforms. It is intended to help students and teaching staff understand what each device abstraction does, how platform-specific behavior is isolated, and how to use the APIs correctly.

For board-level interfaces and pin assignments, see [Board Reference](c7222_board.md).

**Purpose**

The devices module provides a small, consistent set of abstractions for GPIO, LEDs, buttons, on-board LED control, and the on-chip temperature sensor. It aims to:

1. Hide hardware-specific details behind clear C++ interfaces.
2. Keep APIs beginner-friendly and explicit.
3. Offer the same logical behavior across the real Pico W platform and the grader (simulated) platform.

**Design Principles**

1. **Separation of Roles**
`GpioIn` and `GpioOut` are distinct classes. Input-only and output-only operations are deliberately separated to prevent misuse and reduce ambiguous configuration.

2. **Single Ownership Per Pin**
Each GPIO pin can only be owned by a single `GpioIn` or `GpioOut` instance at a time. Platform backends enforce this via internal maps and assertions.

3. **Explicit Initialization**
Platform initialization is explicit (`Platform::Initialize()`). Device initialization is also explicit for `OnBoardLED` and `OnChipTemperatureSensor`. This avoids hidden side effects and keeps ordering clear.

4. **Platform Isolation**
Platform-dependent behavior is confined to `platform/rpi_pico` (real hardware) and `platform/grader` (simulated). Device headers and `src/` code remain platform-agnostic.

**Platform-Agnostic Layer (Common API)**

Key headers in `libs/elec_c7222/devices/include`:

1. `gpio.hpp`  
Defines `GpioIn`, `GpioOut`, and configuration enums. This is the core GPIO abstraction.

2. `led.hpp`  
Defines `Led`, an output-only wrapper that provides `On()`, `Off()`, and `Toggle()`.

3. `button.hpp`  
Defines `Button`, an input-only wrapper that provides `IsPressed()` and optional IRQ usage through `GpioIn`.

4. `onboard_led.hpp`  
Defines `OnBoardLED`, a singleton for the built-in board LED.

5. `onchip_temperature_sensor.hpp`  
Defines `OnChipTemperatureSensor`, a singleton for the RP2040 temperature sensor.

6. `c7222_pico_w_board.hpp`  
Defines `PicoWBoard`, which maps logical LED and button IDs to board GPIOs.

7. `platform.hpp`  
Defines `Platform`, a singleton that coordinates platform initialization and offers convenience accessors for LEDs/buttons.

**Platform Implementations**

**1) Raspberry Pi Pico W (hardware)**

Located in `libs/elec_c7222/devices/platform/rpi_pico/`.

- `gpio.cpp` implements `GpioIn`/`GpioOut` using the Pico SDK.
- `onboard_led.cpp` drives the on-board LED (either default LED pin or CYW43 LED).
- `onchip_temperature_sensor.cpp` configures the ADC and reads temperature values.
- `platform.cpp` performs architecture initialization (e.g., CYW43) and exposes device accessors.

**2) Grader Platform (simulated)**

Located in `libs/elec_c7222/devices/platform/grader/`.

- `gpio.cpp` provides stub behavior so code compiles without hardware.
- `onboard_led.cpp` and `onchip_temperature_sensor.cpp` return simulated results.
- `platform.cpp` mirrors the interface and init flow without hardware dependencies.

**Usage Examples**

**1) Basic Platform Initialization**

```cpp
auto* platform = c7222::Platform::GetInstance();
platform->Initialize();
```

**2) On-Board LED (explicit initialization)**

```cpp
auto* led = c7222::OnBoardLED::GetInstance();
led->Initialize();
led->On();
```

**3) Temperature Sensor (explicit initialization)**

```cpp
auto* sensor = c7222::OnChipTemperatureSensor::GetInstance();
sensor->Initialize();
float temp_c = sensor->GetCelsius();
```

**4) PicoWBoard LEDs and Buttons**

```cpp
auto* platform = c7222::Platform::GetInstance();
platform->Initialize();

// LED usage
platform->LedOn(c7222::PicoWBoard::LedId::LED1_RED);
platform->ToggleLed(c7222::PicoWBoard::LedId::LED1_GREEN);

// Button usage
bool pressed = platform->IsButtonPressed(c7222::PicoWBoard::ButtonId::BUTTON_B1);
```

**5) GPIO Direct Usage**

```cpp
c7222::GpioOut out_pin(15);
out_pin.Write(true);

c7222::GpioIn in_pin(14);
bool level = in_pin.Read();
```

**Notes and Constraints**

1. Assertions are used heavily to enforce valid configuration and single-pin ownership. This assumes debug builds. Release builds will not enforce these checks.
2. `OnBoardLED` and `OnChipTemperatureSensor` are not auto-initialized. Users must call `Initialize()` explicitly.
3. `PicoWBoard` construction performs its own initialization, so it should only be created after platform initialization.

**C API Note**

There is also a simple C-only API for board GPIOs in `c7222_pico_w_board.h`. It is intended for **C code**; C++ users should prefer the `c7222_pico_w_board.hpp` API and device classes.
