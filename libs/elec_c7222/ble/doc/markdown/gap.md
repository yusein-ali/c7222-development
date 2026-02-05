# GAP Module Overview

This module wraps the BTstack GAP APIs with C++ classes and event handlers. It is designed for embedded BLE peripherals where advertising and connection state are managed by a single owner.

## Core Classes

- `c7222::Gap` (`libs/elec_c7222/ble/gap/include/gap.hpp`)
  - Singleton wrapper for BTstack GAP.
  - Owns advertising parameters/data, scan response data, and connection state.
  - Parses HCI events and dispatches them to registered `c7222::Gap::EventHandler` instances.
  - Provides legacy advertising configuration and convenience builders.

- `c7222::Gap::EventHandler`
  - Event interface for advertising start/stop, connection/disconnection, and scan reports.
  - Stored as raw pointers; handlers must outlive the `Gap` instance.

## Utility Types Used by GAP

- `c7222::BleAddress` (`libs/elec_c7222/ble/include/ble_address.hpp`)
  - Strongly typed BLE address, used in connection and scan events.

- `c7222::BleError` (`libs/elec_c7222/ble/include/ble_error.hpp`)
  - Unified error codes used across GAP/ATT/SM mappings.
## Design Principles

- **Singleton ownership:** GAP is a singleton because the controller exposes a single GAP state machine.
- **Event‑driven:** The module relies on HCI event dispatch to drive state and user callbacks.
- **Thin wrapper:** GAP retains BTstack semantics; it does not hide advertising or connection details.
- **Runtime configurability:** Advertising data and parameters can be updated without rebooting.

## Assumed Usage Scenarios

- Embedded BLE peripheral that advertises a custom service and accepts a single connection.
- Applications that need to update advertising/scan response data based on runtime state (sensor readings, device name updates, etc.).
## Event Dispatch Requirement

Your platform glue must forward HCI packets to `c7222::Gap::DispatchBleHciPacket()` (or via `c7222::Ble::DispatchBleHciPacket()` which fans out to GAP). Without this, callbacks will never fire.

## Event Handling in Detail

### How to Register

- Implement `c7222::Gap::EventHandler` and override the callbacks you need.
- Register with `c7222::Gap::AddEventHandler(handler)` **before** calling `c7222::Ble::TurnOn()` or starting advertising.
- Handlers are stored as raw pointers. The handler object must outlive the `Gap` singleton.

### When Handlers Are Called

Handlers are invoked when GAP‑related HCI events are dispatched into `c7222::Gap::DispatchBleHciPacket()` (directly or via `c7222::Ble::DispatchBleHciPacket()`):

- Advertising start/stop status
- Connection complete and disconnection events
- Advertising reports (scanning events) when enabled on the controller
- Connection parameter updates (if supported by the platform glue)

If the platform layer does not forward HCI events, handlers will never be called.

### What Type of Objects Are Passed

`c7222::Gap::EventHandler` callbacks receive strongly‑typed values such as:

- `c7222::ConnectionHandle` for active connections
- `c7222::BleAddress` for peer identity
- `uint8_t` status or reason codes from the controller
- Raw advertising payloads (as byte arrays) for scan reports

These are plain value types or references to stack‑owned buffers that are only valid during the callback.

### Multiple Handlers

- You can register **more than one** handler with `c7222::Gap::AddEventHandler()`.
- Internally, the module stores a list of handler pointers and iterates through them in registration order.
- All registered handlers receive the event; no handler short‑circuits the others.
- You can remove a handler with `c7222::Gap::RemoveEventHandler()` if needed.

### Typical Pattern

```cpp
class MyGapHandler : public c7222::Gap::EventHandler {
 public:
  void OnConnectionComplete(uint8_t status,
                            c7222::ConnectionHandle handle,
                            const c7222::BleAddress& address,
                            uint16_t, uint16_t, uint16_t) const override {
    (void)handle;
    if (status == 0) {
      // Connected
    }
  }
};

auto& gap = c7222::Gap::GetInstance();
static MyGapHandler handler;
gap.AddEventHandler(handler);
```

## Why This Event Handler Design

The GAP module uses an OOP event handler interface instead of a flat list of function pointers or free‑function callbacks for practical embedded reasons:

- **Cohesive state:** A handler can hold state (counters, configuration, references) without global variables. This keeps GAP‑related logic together and testable.
- **Multiple listeners:** A list of handler objects allows more than one subsystem to observe the same event stream (e.g., logging + application logic) without a custom multiplexer.
- **Extensibility:** Adding new events only requires adding virtual methods with default no‑ops, preserving source compatibility for existing handlers.
- **Lifecycle clarity:** Handlers are registered and removed explicitly, which is easier to reason about than scattered callback function pointers.
- **Consistent API:** The same pattern is used for GAP, GATT, and Security Manager, reducing cognitive load and integration mistakes.

Alternative designs (global function callbacks, single function pointer tables, or switch‑based dispatch) are smaller but become harder to scale as events grow, and they complicate multi‑consumer use cases. The OOP approach is a deliberate tradeoff favoring maintainability and composability.

## Advertisement Data Types and Builders

### AdvertisementData (AD Structure)

`c7222::AdvertisementData` represents a **single AD structure**: a length byte, a type byte, and a payload. This is the unit the BLE controller expects inside the 31‑byte legacy advertising payload.

Key ideas:

- **One AD structure per instance.** The class builds `length + type + value` into a contiguous byte vector.
- **Type‑safe payload construction.** Overloads accept:
  - `std::vector<uint8_t>` for raw payload bytes.
  - Pointer to a concrete POD object (payload is `sizeof(T)`).
  - Pointer + count for arrays of POD objects.
  - Reference to a concrete object (payload is `sizeof(T)`).
- **Type identification.** The AD type is one of `AdvertisementDataType` (Flags, Local Name, Manufacturer Specific, etc.).

This class is **not** the full advertising payload; it is a single fragment you can combine with others.

### AdvertisementDataBuilder (Payload Builder)

`c7222::AdvertisementDataBuilder` assembles multiple `AdvertisementData` objects into a **complete advertising payload** and enforces the 31‑byte legacy limit.

Key behaviors:

- **Add / ReplaceOrAdd.** You can add multiple AD structures or replace an existing one of the same type.
- **Validation.** The builder validates total size and AD structure correctness.
- **Build + decode.** It can also decode a raw payload back into AD structures for inspection.
- **Uniqueness enforcement.** `operator+=` and merge operators assert if duplicate AD structures are added.

### How They Interact

1. Build one or more `AdvertisementData` structures (flags, name, manufacturer data, etc.).
2. Add them to an `AdvertisementDataBuilder`.
3. Pass the builder to `c7222::Gap::SetAdvertisingData()` or `c7222::Gap::SetScanResponseData()`.

### Runtime Configuration Updates

The `Gap` class supports live updates to advertising and scan response data. When you call:

- `SetAdvertisingData(...)`
- `SetScanResponseData(...)`

it will:

1. Stop advertising if it is currently active.
2. Apply the new data through the BTstack API.
3. Restart advertising if it was previously running.

This ensures that the controller always sees a consistent payload without requiring the application to manually stop and restart advertising.

### Typical Flow

```cpp
auto& gap = c7222::Gap::GetInstance();

// Build advertising data
c7222::AdvertisementDataBuilder ad;
ad.add(c7222::AdvertisementDataType::kFlags,
       c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
       c7222::AdvertisementData::Flags::kBrEdrNotSupported);
ad.add(c7222::AdvertisementDataType::kCompleteLocalName, "PicoW-BLE");

// Apply at runtime
const bool ok = gap.SetAdvertisingData(ad);
if(ok) {
    gap.StartAdvertising();
}
```

### Updating While Advertising

```cpp
// Update the advertised name while running
c7222::AdvertisementDataBuilder ad;
ad.add(c7222::AdvertisementDataType::kCompleteLocalName, "PicoW-BLE-v2");

gap.SetAdvertisingData(ad); // internally stops + restarts if needed
```

### Basic Types and Payload Support

`AdvertisementData` supports:

- **Flags:** using `c7222::AdvertisementData::Flags` bitmask (discoverable mode, BR/EDR not supported, etc.).
- **Local Names:** shortened or complete, as UTF‑8 strings.
- **Manufacturer Specific Data:** raw bytes (company ID + payload).
- **Service Data:** raw bytes for 16‑bit UUID service data.
- **Custom binary payloads:** via POD structs or raw byte buffers.

These are all represented as AD structures with a specific `AdvertisementDataType` and a payload you supply.

### Example

```cpp
// Build flags structure
const uint8_t flags = static_cast<uint8_t>(
    c7222::AdvertisementData::Flags::kLeGeneralDiscoverableMode |
    c7222::AdvertisementData::Flags::kBrEdrNotSupported);
c7222::AdvertisementData ad_flags(c7222::AdvertisementDataType::kFlags, &flags);

// Build name structure
const std::string name = "PicoW-BLE";
c7222::AdvertisementData ad_name(
    c7222::AdvertisementDataType::kCompleteLocalName,
    reinterpret_cast<const uint8_t*>(name.data()),
    name.size());

// Assemble payload
c7222::AdvertisementDataBuilder builder;
builder.Add(ad_flags);
builder.Add(ad_name);

auto& gap = c7222::Gap::GetInstance();
gap.SetAdvertisingData(builder);
```
