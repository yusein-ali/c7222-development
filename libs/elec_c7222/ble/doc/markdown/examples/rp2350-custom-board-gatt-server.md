# ELEC-C7222 Library Examples: RP2350 Custom Board GATT Server

This example demonstrates a full GATT server on a custom RP2350 board with a separate BLE
controller connected over UART. It combines the AttributeServer, SecurityManager, and a
custom `BtstackPort` subclass, and periodically notifies a temperature characteristic using
the on-chip ADC sensor.

---

## Target

- `example-ble-rp2350-custom-board-gatt-server`

> Only built when `C7222_PLATFORM=rp2350_custom_board`.

---

## Files

- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/main_ble_att_server.cpp`
- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/app_profile.gatt`
- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/ble_onchip_temperature.cpp`
- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/board/rpi_bd_btstack_port.cpp`
- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/board/rpi_bd_hub_mcu.h`
- `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/board/hal_uart_dma.c`

---

## What it demonstrates

- Implementing a `BtstackPort` subclass for a custom board (HCI H4 over UART, hardware
  flow control, FreeRTOS run loop, no CYW43)
- Enabling the Security Manager with MITM protection and `DisplayOnly` IO capability
  (passkey pairing)
- Locating characteristics by UUID and by generated ATT handle constant
- Setting user description descriptors at runtime
- Periodically updating a `NOTIFY` + `INDICATE` characteristic from a FreeRTOS timer
- Updating manufacturer-specific advertising data in the main loop

---

## GATT layout

Environmental Sensing Service (`0x181A`)

| UUID | Name | Properties |
|------|------|------------|
| `fc930f88-1a30-45d7-8c17-604c1a036b9f` | Configuration | `READ` + `WRITE` + `READ_AUTHENTICATED` + `READ_ENCRYPTED` + `WRITE_AUTHENTICATED` + `WRITE_ENCRYPTED` |
| `0x2A6E` (Temperature) | Temperature | `READ` + `NOTIFY` + `INDICATE` + `READ_AUTHENTICATED` + `READ_ENCRYPTED` |

Both characteristics require an authenticated, encrypted link. The device will not serve
reads or accept writes until pairing with MITM protection is complete.

---

## How to test (nRF Connect)

1. Scan and connect to `RP2350_Custom_BLE`
2. The device will request pairing. Enter the 6-digit passkey displayed on the UART
   console into the nRF Connect passkey prompt.
3. After pairing, open the **Environmental Sensing** service.
4. Enable notifications on **Temperature** (0x2A6E) — the value updates every 2 seconds
   in units of °C × 100 (e.g., `0x09C4` = 25.00 °C).
5. Read or write **Configuration** (custom UUID) — reads return the last written uint16;
   writes are accepted and stored in the characteristic value.

---

## Expected UART output

```
Starting RP2350 custom board FreeRTOS BLE GATT server example...
Attribute server initialized.
Environmental Sensing Service found in ATT DB.
Configuration Characteristic found in ATT DB.
Found Temperature Service!
Initializing BleOnchipTemperature manager with characteristics...
Printing Attribute Server
...
Platform init complete. Powering up BTstack...
BLE Stack is ON!
Timer started and will fire in 100 ticks!
Bluetooth Turned On
Advertising started as 'RP2350_Custom_BLE'...
GAP event: AdvertisingStart (status=0x00)
Timer Callback: Updated temperature to 25.34 C (raw: 0x09E2)
...
```

---

## Notes

- `RpiBdBtstackPort` configures UART1 at 921600 bps with CTS/RTS flow control on GPIOs
  4/5/6/7 (defined in `rpi_bd_hub_mcu.h`). No bonding persistence — pairing keys are
  discarded on reset.
- The BLE_App task stack must be at least **4096 words** due to deep `operator<<` chains
  when printing the ATT database to the console.
- The FreeRTOS Timer Service task stack must be at least **1024 words** to safely run
  `printf("%.2f", ...)` in the timer callback.
- Security Manager must be enabled **before** `EnableAttributeServer` — the attribute
  server asserts that a configured security manager is present when it finds
  authenticated/encrypted characteristics in the profile.
- See `libs/elec_c7222/ble/doc/markdown/custom-board-setup.md` for a detailed explanation
  of the `BtstackPort` design and initialization order.
