# Example: Custom Service (NOTIFY)

This example demonstrates the minimal pattern for implementing a custom BLE service with a NOTIFY characteristic.

---

## Target

- `example-ble-custom-service-notify`

---

## Files

- `libs/elec_c7222/examples/ble/custom-service-notify/main_ble_custom_service_notify.cpp`
- `libs/elec_c7222/examples/ble/custom-service-notify/app_profile.gatt`

---

## What it demonstrates

- Defining a NOTIFY characteristic in a `.gatt` file
- Locating the characteristic by UUID
- Periodically updating the characteristic value
- Sending notifications to a subscribed client

---

## GATT layout

Custom vendor service: `0xFFD0`

| UUID | Name | Properties |
|------|------|------------|
| `0xFFD1` | NotifyValue | `READ` + `NOTIFY` |

---

## How to test (nRF Connect / LightBlue)

1. Connect to the device (`c7222-notify-demo`)
2. Open `NotifyValue`
3. Enable notifications (CCCD)
4. Observe periodic counter updates

---

## Notes

If notifications are not enabled on the client (CCCD not set), `Notify()` calls will typically have no effect.
