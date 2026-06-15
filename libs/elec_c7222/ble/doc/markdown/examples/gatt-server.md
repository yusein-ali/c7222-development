# ELEC-C7222 Library Examples: GATT Server Example (Pico W)

This example demonstrates a full GATT server on the Raspberry Pi Pico W, combining the
AttributeServer, SecurityManager, and a periodic timer that updates a temperature
characteristic from the on-chip ADC sensor.

---

## Target

- `example-ble-gatt-server`

---

## Files

- `libs/elec_c7222/examples/ble/gatt-server/main_ble_att_server.cpp`
- `libs/elec_c7222/examples/ble/gatt-server/app_profile.gatt`
- `libs/elec_c7222/examples/ble/gatt-server/ble_onchip_temperature.cpp`

---

## What it demonstrates

- Enabling the SecurityManager with MITM protection and `DisplayOnly` IO capability
  (passkey pairing)
- Enabling the AttributeServer with a compiled GATT database
- Locating characteristics by UUID and by generated ATT handle constant
- Setting user description descriptors at runtime
- Periodically updating a `NOTIFY` + `INDICATE` characteristic from a FreeRTOS timer
- Updating manufacturer-specific advertising data in the main loop
- Toggling the on-board LED from the timer callback as a visual heartbeat

---

## GATT layout

Environmental Sensing Service (`0x181A`)

| UUID | Name | Properties |
|------|------|------------|
| `fc930f88-1a30-45d7-8c17-604c1a036b9f` | Configuration | `READ` + `WRITE` + `READ_AUTHENTICATED` + `READ_ENCRYPTED` + `WRITE_AUTHENTICATED` + `WRITE_ENCRYPTED` |
| `0x2A6E` (Temperature) | Temperature | `READ` + `NOTIFY` + `INDICATE` + `READ_AUTHENTICATED` + `READ_ENCRYPTED` |

Both characteristics require an authenticated, encrypted link. Reads and writes are
rejected until pairing with MITM protection is complete.

---

## How to test (nRF Connect)

1. Scan and connect to `Pico2_BLE++`
2. The device requests pairing. Enter the 6-digit passkey displayed on the UART console.
3. After pairing, open the **Environmental Sensing** service.
4. Enable notifications on **Temperature** (0x2A6E) — the value updates every 2 seconds
   in units of °C × 100 (e.g., `0x09C4` = 25.00 °C).
5. Read or write **Configuration** (custom UUID) — reads return the last written uint16;
   writes are accepted and stored.

---

## Expected UART output

```
Starting FreeRTOS BLE Example...
Attribute server initialized.
Environmental Sensing Service found in ATT DB.
Configuration Characteristic found in ATT DB.
Found Temperature Service!
Initializing BleOnchipTemperature manager with characteristics...
Printing Attribute Server
...
CYW43 init complete. Powering up BTstack...
BLE Stack is ON!
Timer started and will fire in 100 ticks!
Bluetooth Turned On
Advertising started as 'Pico2_BLE'...
GAP event: AdvertisingStart (status=0x00)
Timer Callback: Updated temperature to 25.34 C (raw: 0x09E2)
...
```

---

## Notes

- This is the Pico W version of the GATT server. BLE transport is provided by the CYW43
  module — no custom `BtstackPort` is needed.
- For the equivalent example targeting a custom RP2350 board with an external UART BLE
  controller, see `examples/rp2350-custom-board-gatt-server/` and
  `libs/elec_c7222/ble/doc/markdown/examples/rp2350-custom-board-gatt-server.md`.
- Security Manager must be enabled before `EnableAttributeServer` — the attribute server
  asserts that a configured security manager is present when it finds
  authenticated/encrypted characteristics in the profile.
