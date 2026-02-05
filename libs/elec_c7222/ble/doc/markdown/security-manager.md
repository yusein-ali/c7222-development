# Security Manager Module Overview

This module encapsulates BTstack’s Security Manager (SM) configuration and event handling. It provides a structured C++ interface for pairing, authentication, and authorization workflows.

## Core Classes

- `c7222::SecurityManager` (`libs/elec_c7222/ble/security_manager/include/security_manager.hpp`)
  - Singleton wrapper for BTstack SM.
  - Configures IO capability, authentication requirements, bonding, and key sizes.
  - Dispatches pairing and authorization events to registered handlers.
  - Provides APIs to accept/deny pairing prompts.

- `c7222::SecurityManager::EventHandler`
  - Event interface for Just Works, passkey, numeric comparison, pairing complete, and authorization events.
  - Stored as raw pointers; handlers must outlive the `SecurityManager` instance.

## Design Principles

- **Explicit configuration:** Security settings must be applied before connections are established.
- **Validation:** Configuration is checked to ensure it can satisfy GATT database requirements.
- **Event‑driven:** The application decides how to respond to pairing prompts.

## Assumed Usage Scenarios

- Devices that require encryption or authenticated access to specific characteristics.
- Devices that need bonding or Secure Connections policies.

## Typical Flow

```cpp
auto* sm = c7222::SecurityManager::GetInstance();

c7222::SecurityManager::SecurityParameters params;
params.io_capability = c7222::SecurityManager::IoCapability::kDisplayYesNo;
params.authentication = c7222::SecurityManager::AuthenticationRequirement::kMitmProtection;
params.min_encryption_key_size = 7;
params.max_encryption_key_size = 16;

sm->Configure(params);
```

## Key Notes

- Security configuration should match GATT permissions (READ/WRITE + READ_AUTHENTICATED/WRITE_AUTHENTICATED).
- Authorization decisions are tracked per connection and used by `AttributeServer` for descriptor writes.

## Configuration Validation

`SecurityManager` provides a validation step to ensure the requested security level is achievable with the current IO capability and pairing configuration. Use `ValidateConfiguration(authentication_required, authorization_required, encryption_required)` to check:

- Whether MITM protection can be satisfied given the selected IO capability.
- Whether Secure Connections requirements are compatible with the current configuration.
- Whether encryption and key size constraints can be met.

This is intended to be called before connections are established, so the application can fail fast or adjust configuration before clients connect.

## Interaction with AttributeServer

The Security Manager and AttributeServer cooperate at runtime to enforce GATT permissions:

- **SecurityManager** configures pairing policy (IO capability, MITM, SC, bonding) and handles pairing/authorization events.
- **AttributeServer** enforces the required security level for attribute access, especially for descriptor writes (CCCD/SCCD).

### Shared Security State

At runtime, the platform glue updates the AttributeServer with the current security state derived from SM/GAP events:

- **Connection handle** (`SetConnectionHandle`) is stored on the AttributeServer and propagated to characteristics.
- **Security level** (`SetSecurityLevel`) reflects encryption/authentication state.
- **Authorization status** (`SetAuthorizationGranted`) records application‑level authorization decisions.

This cached state is used by `Characteristic` descriptor handlers (e.g., CCCD/SCCD writes) to accept or reject operations based on the configured security requirements.

### Practical Flow

1. SecurityManager configures authentication requirements before connections.\n
2. When pairing completes or security level changes, the platform layer updates AttributeServer.\n
3. AttributeServer uses the cached security level to validate reads/writes against GATT permissions.

## Event Handling in Detail

### How to Register

- Implement `c7222::SecurityManager::EventHandler` and override the callbacks you need.
- Register with `c7222::SecurityManager::AddEventHandler(handler)` **before** pairing can be triggered (typically before `c7222::Ble::TurnOn()`).
- Handlers are stored as raw pointers. The handler object must outlive the `SecurityManager` singleton.

### When Handlers Are Called

Handlers are invoked when SM‑related HCI events are dispatched into the stack (via `c7222::Ble::DispatchBleHciPacket()`):

- Just Works confirmation requests
- Numeric comparison requests
- Passkey display/input requests
- Pairing complete events
- Re‑encryption complete events
- Authorization requests and results

If the platform layer does not forward HCI events, handlers will never be called.

### What Type of Objects Are Passed

`c7222::SecurityManager::EventHandler` callbacks receive strongly‑typed values such as:

- `c7222::ConnectionHandle` identifying the active connection
- `uint32_t` passkeys (display or comparison)
- `c7222::SecurityManager::PairingStatus` and `uint8_t` status codes
- `c7222::SecurityManager::AuthorizationResult` for authorization decisions

These values are only guaranteed to be valid during the callback.

### Multiple Handlers

- You can register **more than one** handler with `c7222::SecurityManager::AddEventHandler()`.
- Internally, the module stores a list of handler pointers and iterates through them in registration order.
- All registered handlers receive the event; no handler short‑circuits the others.
- You can remove a handler with `RemoveEventHandler()` if needed.

### Typical Pattern

```cpp
class MySmHandler : public c7222::SecurityManager::EventHandler {
 public:
  void OnJustWorksRequest(c7222::ConnectionHandle handle) const override {
    c7222::SecurityManager::GetInstance()->ConfirmJustWorks(handle);
  }
  void OnPasskeyDisplay(c7222::ConnectionHandle, uint32_t passkey) const override {
    (void)passkey;
  }
};

auto* sm = c7222::SecurityManager::GetInstance();
static MySmHandler handler;
sm->AddEventHandler(handler);
```
