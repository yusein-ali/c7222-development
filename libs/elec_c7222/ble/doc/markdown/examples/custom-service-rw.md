# Example: Custom Service (READ + WRITE)

This example demonstrates the minimal pattern for implementing a custom BLE service with a readable characteristic and a writable characteristic.

---

## Target

- `example-ble-custom-service-rw`

---

## Files

- `libs/elec_c7222/examples/ble/custom-service-rw/main_ble_custom_service_rw.cpp`
- `libs/elec_c7222/examples/ble/custom-service-rw/app_profile.gatt`

---

## What it demonstrates

- Defining a custom service in a `.gatt` file
- Finding services/characteristics by UUID using `AttributeServer`
- Handling writes using `Characteristic::EventHandler`
- Updating a readable characteristic in response to a write

---

## GATT layout

Custom vendor service: `0xFFE0`

| UUID | Name | Properties |
|------|------|------------|
| `0xFFE1` | PublicValue | `READ` |
| `0xFFE2` | WriteValue | `WRITE` / `WRITE_WITHOUT_RESPONSE` |

---

## How to test (nRF Connect / LightBlue)

1. Connect to the device (`c7222-rw-demo`)
2. Read `PublicValue` -> returns the current value
3. Write any bytes to `WriteValue`
4. Read `PublicValue` again -> it becomes whatever you wrote

---

## Notes

The `.gatt` file uses `DYNAMIC` for both characteristics so the application can update values and receive write events.
