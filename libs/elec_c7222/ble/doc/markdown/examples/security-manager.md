# Example: Security Manager (Encrypted GATT)

This example demonstrates how the BLE Security Manager integrates with the GATT server and how encrypted characteristic access behaves in practice.

---

## Target

- `example-ble-security-manager`

---

## Files

- `libs/elec_c7222/examples/ble/security-manager/main_ble_security_manager.cpp`
- `libs/elec_c7222/examples/ble/security-manager/app_profile.gatt`

---

## What it demonstrates

- Enabling the Security Manager
- "Just Works" pairing configuration
- A secure characteristic (`READ_ENCRYPTED`) that fails before pairing and succeeds after pairing

---

## GATT layout

Custom vendor service: `0xFFF0`

| UUID | Name | Properties |
|------|------|------------|
| `0xFFF1` | PublicValue | `READ` |
| `0xFFF2` | SecureValue | `READ_ENCRYPTED` |

---

## How to test (nRF Connect / LightBlue)

1. Connect to the device (`c7222-sec-demo`)
2. Read `PublicValue` -> succeeds immediately
3. Read `SecureValue` -> fails before pairing and/or triggers pairing
4. Pair with the device ("Just Works")
5. Read `SecureValue` again -> succeeds

---

## Expected UART output

You should see:

- advertising started
- security events (pairing/encryption)
- normal GATT read logs from the characteristic handlers

---

## Notes

This example keeps the pairing configuration intentionally simple:

- `IoCapability = NoInputNoOutput`
- no MITM requirement

The secure behavior is enforced by the `.gatt` characteristic flags, not by application logic.
