# ELEC-C7222 Library Examples: GAP Example

This example demonstrates the minimal pattern for BLE advertising and GAP event handling,
without any GATT server or security configuration.

---

## Target

- `example-ble-gap`

---

## Files

- `libs/elec_c7222/examples/ble/gap/main_ble_gap.cpp`

---

## What it demonstrates

- Configuring advertising flags, device name, and manufacturer-specific data
- Starting advertising via `Gap::StartAdvertising()`
- Registering a `Gap::EventHandler` for logging GAP events
- Updating manufacturer-specific advertising data in the main loop
- Using `OnBoardLED` as a heartbeat to confirm task activity

---

## How to test (nRF Connect / LightBlue)

1. Scan for `Pico2_BLE++`
2. Observe the device in the scan list — manufacturer data updates every 1 second
3. Connect to the device
4. The GAP event handler logs `OnConnectionComplete` to the UART console
5. Disconnect — the handler logs `OnDisconnectionComplete` and advertising restarts

---

## Expected UART output

```
Platform initialized successfully.
Starting FreeRTOS BLE GAP Example...
BLE Stack ON is requested!
Bluetooth Turned On
Advertising started as 'Pico2_BLE++'...
GAP event: AdvertisingStart (status=0x00)
GAP event: ConnectionComplete (status=0x00, handle=0x0000)
GAP event: DisconnectionComplete (status=0x00, handle=0x0000)
GAP event: AdvertisingStart (status=0x00)
```

---

## Notes

- No `AttributeServer` or `SecurityManager` is involved. This is purely a GAP demo.
- The manufacturer-specific payload is overwritten each second with the current tick
  count (seconds since boot) using `AdvertisementDataBuilder::Pop()` + `Push()`.
- The LED toggles each time advertising data is refreshed, providing a visual heartbeat.
