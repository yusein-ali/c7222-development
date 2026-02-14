# C7222 Pico W Board Interfaces and Pin Assignments

The C7222 Pico W board is a very simple I/O board intended as a getting-started platform for Raspberry Pi Pico W development. It provides a UART connector, a reset button, four user buttons, and three red/green LED pairs so core embedded workflows can be tested quickly with minimal hardware complexity.

@image html c7222_pico2_board_schematic.png "C7222 Pico W Board Schematic"

The schematic is shown in the image above. The 3D board view below is for reference only.

@image html c7222_pico2_board_board.png "C7222 Pico W Board 3D View"

This document summarizes the external interfaces and GPIO assignments for the C7222 Pico W board, based on the schematic.

## Board Overview

The board is built around a Raspberry Pi Pico W. It exposes:
1. A debug UART header.
2. Four push buttons with pull-ups.
3. Three dual‑color LED pairs (red/green).
4. A reset switch tied to RUN.

## GPIO Assignments

### LEDs (active‑low)

Each LED is tied to 3V3 through a 470 Ω resistor. The GPIO drives the LED low to turn it on (active‑low).

1. `LED1_RED`  → `GP21`
2. `LED1_GREEN` → `GP20`
3. `LED2_RED`  → `GP19`
4. `LED2_GREEN` → `GP18`
5. `LED3_RED`  → `GP17`
6. `LED3_GREEN` → `GP16`

These match the C++ API enums:
1. `PicoWBoard::LedId::LED1_RED = 21`
2. `PicoWBoard::LedId::LED1_GREEN = 20`
3. `PicoWBoard::LedId::LED2_RED = 19`
4. `PicoWBoard::LedId::LED2_GREEN = 18`
5. `PicoWBoard::LedId::LED3_RED = 17`
6. `PicoWBoard::LedId::LED3_GREEN = 16`

### Buttons (active‑low)

Each button pulls the GPIO to GND when pressed, with a 100 kΩ pull‑up to 3V3.

1. `BUTTON_B1` → `GP2`
2. `BUTTON_B2` → `GP3`
3. `BUTTON_B3` → `GP4`
4. `BUTTON_B4` → `GP5`

These match the C++ API enums:
1. `PicoWBoard::ButtonId::BUTTON_B1 = 2`
2. `PicoWBoard::ButtonId::BUTTON_B2 = 3`
3. `PicoWBoard::ButtonId::BUTTON_B3 = 4`
4. `PicoWBoard::ButtonId::BUTTON_B4 = 5`

### Debug UART Header

The UART header exposes the Pico’s UART signals:

1. `DBG_TX` → `GP0`
2. `DBG_RX` → `GP1`
3. `GND`

Notes:
1. This is the default UART for Pico platforms and is enabled out of the box for `stdio`/`printf` output.
2. When using a Pico Debugger, the UART wires must be crossed: board `DBG_TX` goes to debugger `RX`, and board `DBG_RX` goes to debugger `TX`.
3. If using the Pico Debugger provided cables, the female and male pin cables can be attached as follows:
   - Female black ↔ male black
   - Female orange ↔ male yellow
   - Female yellow ↔ male orange

## Reset

The RESET switch ties the Pico `RUN` pin to GND to reset the board.

## Usage Notes

1. LEDs are active‑low: write `0` to turn on, `1` to turn off.
2. Buttons are active‑low: pressed = logic `0`, released = logic `1`.
3. The GPIO assignments are fixed by hardware; use the enums in `c7222_pico_w_board.hpp` for clarity.
4. LED drive relies on the Pico’s sink capability (drive strength). This is not optimized for low‑power operation.
