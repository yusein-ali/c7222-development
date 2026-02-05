# GATT/ATT Module Overview

This module encapsulates BTstack’s ATT database and GATT‑level routing with C++ classes. It provides parsing, handle‑based routing, and characteristic‑level events while preserving BTstack’s ATT semantics.

## Core Classes

- `c7222::AttributeServer` (`libs/elec_c7222/ble/gatt/include/attribute_server.hpp`)
  - Singleton wrapper around the BTstack ATT server.
  - Parses the compiled ATT DB and builds `Service`/`Characteristic` objects.
  - Routes ATT read/write callbacks to the correct object.
  - Fans out HCI events for indications and flow control.

- `c7222::Service` (`libs/elec_c7222/ble/gatt/include/service.hpp`)
  - Groups characteristics and service‑level attributes.
  - Built from ordered `Attribute` entries produced by parsing.

- `c7222::Characteristic` (`libs/elec_c7222/ble/gatt/include/characteristic.hpp`)
  - Owns Declaration, Value, and optional descriptors.
  - Dispatches read/write operations to handlers.
  - Exposes `EventHandler` for semantic events (read/write, CCCD changes, indication completion).


- `c7222::Attribute` (`libs/elec_c7222/ble/gatt/include/attribute.hpp`) is the leaf object for a single ATT DB entry:
  - Stores UUID, properties, handle, and value storage.
  - Static values reference the DB blob; dynamic values are owned in memory.


## Design Principles

- **ATT‑first:** The module mirrors BTstack’s ATT database and handle‑based routing.
- **Stable memory:** Static attributes point into the compiled ATT DB blob; the blob must remain valid.
- **Separation of concerns:** Parsing builds structure; routing is handle‑based; application logic is attached via callbacks.
- **Event‑driven:** Higher‑level characteristic events are optional and layered on top of raw ATT access.

## Assumed Usage Scenarios

- Embedded GATT server with a compiled ATT DB generated from `.gatt` files.
- Applications that need to intercept read/write events or provide dynamic values.
- Scenarios that need CCCD/SCCD handling for notifications or indications.

## Runtime Behavior and Routing

- `c7222::AttributeServer::Init(att_db)` parses the ATT DB once and builds services.
- BTstack calls `att_read_callback` / `att_write_callback`.
- `AttributeServer` finds the target handle and routes to:
  - `c7222::Characteristic::HandleAttributeRead/Write()` if the handle belongs to a characteristic.
  - `c7222::Attribute::InvokeRead/WriteCallback()` for service‑level attributes.

## Key Notes

- The server assumes **one active connection**. Multiple connections are not supported reliably.
- Dynamic values require the `DYNAMIC` property in the `.gatt` file.
- CCCD is auto‑added by `NOTIFY`/`INDICATE` in `.gatt`.
- User Description text must be set at runtime via `SetUserDescription()` / `SetUserDescriptionText()`.



## UUIDs

### What a UUID Is

In GATT/ATT, a UUID identifies the **type** of an attribute. It is how BTstack distinguishes a Primary Service declaration from a Characteristic Value or a descriptor. UUIDs can be:

- **16‑bit** (Bluetooth SIG‑assigned)
- **128‑bit** (custom/vendor‑specific)

### How BTstack Handles UUIDs

In the compiled ATT database, each attribute stores its UUID. When parsing the DB, BTstack (and this library) reads either a 16‑bit or 128‑bit UUID based on the entry size and flags. This UUID is then used to classify the attribute and route it to the correct higher‑level object.\n
### How We Manage UUIDs

`c7222::Uuid` (`libs/elec_c7222/ble/gatt/include/uuid.hpp`) is the strongly‑typed wrapper used throughout parsing and lookup:

- Encapsulates 16‑bit and 128‑bit UUID forms.
- Provides helpers to check standard attribute types (e.g., Service Declaration, CCCD, User Description).
- Used as the canonical key for `Service`, `Characteristic`, and `Attribute` classification.\n

## Attributes

### BTstack ATT Attribute Concept

In BTstack, the ATT database is a linear list of **attributes**. Each attribute is a record that contains:

- **Handle**: 16‑bit identifier used by the ATT protocol.
- **UUID**: 16‑bit or 128‑bit type of the attribute (service declaration, characteristic value, descriptor, etc.).
- **Flags/Properties**: permission and behavior bits (read/write/security/dynamic).
- **Value bytes**: the data payload stored in the ATT DB.

An **attribute is the unit that the ATT protocol reads and writes**. GATT operations (read/write/notify/indicate) ultimately target an attribute handle.

The compiled `att_db` blob is a packed binary representation of these entries. The ATT server reads this list to answer read/write requests and calls user callbacks for dynamic entries.

### Encapsulation in `c7222::Attribute`

`c7222::Attribute` mirrors a single BTstack attribute entry and adds C++‑level safety and callbacks:

- **UUID + properties + handle** are stored directly and used for lookups and routing.
- **Static values** keep a pointer/size into the ATT DB blob (no copy, minimal RAM).
- **Dynamic values** are stored in an owned `std::vector<uint8_t>` to allow runtime updates.
- **Read/Write callbacks** allow custom logic or validation and are registered as C++ function objects (`std::function`).
  - For standard attribute types (identified by UUID), higher‑level classes set the appropriate handlers during construction/parsing.
  - For example, when a descriptor UUID is recognized, the owning `Characteristic` installs the correct read/write handler for that descriptor.
  - Reads return a byte count or ATT error code.
  - Writes return `BleError` and update the dynamic storage on success.

### Static vs Dynamic Attributes

- **Static** (no `DYNAMIC` flag): value is immutable at runtime. Reads return the bytes embedded in the ATT DB blob. Writes are rejected.
- **Dynamic** (`DYNAMIC` flag): reads and writes are forwarded to the application (if callbacks are installed). If no write callback is installed, the value is still stored and the write succeeds.

### Why This Encapsulation Matters

- **Memory efficiency:** static attributes reference the DB blob directly without copying.
- **Type‑safe routing:** the Attribute class exposes explicit callbacks with `BleError` semantics.
- **Consistency with BTstack:** handles and properties map directly to BTstack flags so the ATT server’s behavior remains predictable.

## Characteristics

### Characteristics in BTstack

In BTstack’s ATT database, a **Characteristic** is represented as a *group of attributes* in a fixed order:

1. **Characteristic Declaration** (UUID 0x2803)  
   Contains the properties bitfield, the value handle, and the characteristic UUID.
2. **Characteristic Value** (UUID = characteristic type)  
   The actual data attribute that clients read/write.
3. **Optional descriptors**  
   Examples: CCCD (0x2902), User Description (0x2901), Extended Properties (0x2900), etc.

This is why ATT operations always target a **handle**: each characteristic is really multiple attributes, and each attribute has its own handle.

### Characteristics in this library

The `c7222::Characteristic` class groups these related attributes into a single C++ object, eliminating the need to manually track individual handles and descriptors across the ATT DB. This abstraction lets applications work with high-level characteristic concepts (read, write, notify) rather than juggling raw handles and callbacks, making the code more maintainable and less error-prone.

### Construction and Parsing

`c7222::Characteristic` can be created in two ways:

- **Direct construction** using UUID + properties + handles (manual assembly).
- **Parsing** from the ATT DB: `c7222::Characteristic::ParseFromAttributes()` groups a Declaration, Value, and Descriptors into a single object.

During parsing, descriptor UUIDs determine which descriptor objects are created and which handlers they receive.

### Properties and Security Requirements

Characteristic properties are a bitfield (read, write, notify, indicate, etc.) derived from the ATT DB. Security permissions (read/write encrypted/authenticated/authorized, SC requirement, and key size) are encoded in the attribute properties and evaluated at runtime by helper methods such as:

- `ReadRequiresEncryption()` / `WriteRequiresEncryption()`
- `ReadRequiresAuthentication()` / `WriteRequiresAuthentication()`
- `ReadRequiresAuthorization()` / `WriteRequiresAuthorization()`
- `ReadRequiresSecureConnections()` / `WriteRequiresSecureConnections()`

These permissions influence CCCD/SCCD writes and read/write access to the value attribute.

### Default Attributes Created

Every characteristic includes:

- **Declaration attribute** (mandatory, identifies the characteristic).
- **Value attribute** (mandatory, holds the data).

Optional descriptors are created when present in the ATT DB:

- **CCCD** when `NOTIFY` or `INDICATE` is set.
- **SCCD** for broadcast configuration.
- **User Description** (0x2901).
- **Extended Properties** (0x2900).
- Custom descriptors (any other UUIDs).

### Access and Attribute Read/Write Callbacks

Access is managed through the underlying `Attribute` objects:

- Each attribute can have a read callback and/or write callback (`std::function`).
- Value reads and writes are routed through `c7222::Characteristic::HandleAttributeRead/Write()`.
- If you override the value attribute’s callbacks, you bypass the default behavior.

Descriptor‑level behavior also uses attribute read/write callbacks:

- **CCCD (0x2902)** writes are routed to a dedicated handler that validates security requirements and toggles notifications/indications.
- **SCCD (0x2903)** writes are routed to a dedicated handler that toggles broadcasts.
- **User Description (0x2901)** reads are routed to the shared read handler; writes are rejected.

Default behavior:

- **Read:** returns the stored static/dynamic value (unless rejected by permissions).
- **Write:** updates dynamic storage and fires `OnWrite` if using the default handler.

### Why Event Handlers Exist

`c7222::Characteristic::EventHandler` provides a semantic, higher‑level event interface on top of raw attribute access. It exists to:

- Separate **application logic** from low‑level ATT handling.
- Provide **semantic callbacks** (e.g., notifications enabled, indication complete).
- Allow **multiple listeners** without custom fan‑out logic.

### Registering and Using Event Handlers

```cpp
class MyCharHandler : public c7222::Characteristic::EventHandler {
 public:
  void OnRead() override { /* refresh dynamic value */ }
  void OnWrite(const std::vector<uint8_t>& data) override { (void)data; }
};

auto* ch = server->FindCharacteristicByUuid(c7222::Uuid(0x2A6E));
if(ch) {
  static MyCharHandler handler;
  ch->AddEventHandler(handler);
}
```

Handlers are stored as raw pointers. Multiple handlers can be registered and will be invoked in registration order. Remove with `RemoveEventHandler()` if needed.

## Services

### Service in BTstack

In BTstack, a **service** is a logical grouping of attributes that begins with a Service Declaration attribute and continues until the next Service Declaration. The service is identified by the UUID stored in that declaration.

In BTstack, a **Service** is represented by a Service Declaration attribute (UUID 0x2800 for Primary, 0x2801 for Secondary), followed by all characteristic and descriptor attributes that belong to that service until the next Service Declaration.

### C++ Encapsulation (`c7222::Service`)

`c7222::Service` groups the service‑level attributes and its characteristics into a single object:

- **Service declaration attribute** (`declaration_attr_`) stores the service UUID and identifies Primary vs Secondary.
- **Included service declarations** are stored both as:
  - `included_service_declarations_` (raw ATT attributes, used for accurate DB representation)
  - `included_services_` (higher‑level `Service` objects for easier access)
- **Characteristics list** (`characteristics_`) stores all characteristic objects that belong to the service.

`Service` is a **container** and router for characteristics; the actual functional behavior of a service (business logic, data production/validation) must be implemented by the application using characteristic callbacks and event handlers.

### Construction and Parsing

Services are created either by:

- **Manual construction** using the UUID and type, then adding characteristics via `AddCharacteristic()` or `CreateCharacteristic()`.
- **Parsing from the ATT DB**, where `c7222::Service::ParseFromAttributes()` consumes attributes in order and builds the service and all its characteristics.

During parsing, characteristics are extracted in discovery order using `c7222::Characteristic::ParseFromAttributes()`.

### Service Attributes and Operations

Service objects provide:

- **Accessors** for declaration handles and characteristic lists.
- **Routing helpers** (`FindServiceAttributeByHandle`) so the AttributeServer can dispatch reads/writes that are not part of a characteristic value.
- **Security aggregation** (`HasCharacteristicsRequiringAuthentication/Encryption/Authorization`) to inform security configuration decisions before connections.

### Connection Handle Propagation

When `SetConnectionHandle()` is called on a service, the handle is propagated to all characteristics and included services. This enables notifications/indications from value updates without each characteristic needing to be updated manually.

### Why This Encapsulation Matters

- **Correct handle grouping:** Services map directly to the ATT DB ordering rules and keep related attributes together.
- **Security planning:** Aggregated security requirements are computed once at the service level.
- **Clean routing:** AttributeServer can route non‑characteristic attributes through the service object without special cases.

## AttributeServer in Detail

### Platform‑Agnostic Core

`c7222::AttributeServer` is the C++ wrapper around BTstack’s ATT server model. The core (platform‑agnostic) responsibilities are:

- **ATT DB parsing orchestration:** calls into the platform parser to obtain `Attribute` objects, then builds `Service` and `Characteristic` objects in discovery order.
- **Handle‑based routing:** maps ATT read/write requests to either a `Characteristic` (value or descriptor) or a service‑level `Attribute`.
- **Security state cache:** stores the current connection handle, security level, and authorization status for runtime permission checks.
- **HCI event fan‑out:** forwards indication completion and flow‑control events to characteristics via `DispatchBleHciPacket()`.

The public API is intentionally thin: it exposes `Init()`, lookup helpers (by UUID/handle), and connection/security setters. All read/write semantics follow BTstack’s ATT expectations (read returns byte count or error; write returns ATT error codes).

### Raspberry Pi Pico W Port (BTstack Integration)

The Pico W port lives in `libs/elec_c7222/ble/gatt/platform/rpi_pico/attribute_server.cpp` and wires the C++ layer into BTstack’s C callbacks:

1. **ATT DB registration**  
   `att_server_init(att_db, att_read_callback, att_write_callback)` registers the compiled DB and the C callbacks.

2. **ATT DB parsing**  
   The port parses the binary `att_db` blob into `Attribute` objects and calls `InitServices()` to build services/characteristics.

3. **Read/Write callbacks**  
   - `att_read_callback` calls `c7222::AttributeServer::ReadAttribute()` and maps the result back to BTstack’s ATT status codes.  
   - `att_write_callback` calls `c7222::AttributeServer::WriteAttribute()` and returns 0 on success or an ATT error code.

4. **Security updates**  
   The platform glue updates the server’s cached security level and authorization state based on GAP/SM events so CCCD/SCCD writes are validated correctly.

### Why the C++ Encapsulation Helps with BTstack

BTstack provides a low‑level C interface: raw ATT DB bytes, handles, and callback functions. The C++ wrapper adds:

- **Structure:** parsed Services/Characteristics map handles to intent, reducing manual handle tables.
- **Safety:** `Attribute` separates static vs dynamic storage and centralizes permission checks.
- **Extensibility:** `c7222::Characteristic::EventHandler` provides semantic hooks (CCCD changes, indication complete) without re‑implementing ATT logic.
- **Consistency:** the same routing logic works across platforms; only the parsing/registration glue is platform‑specific.
