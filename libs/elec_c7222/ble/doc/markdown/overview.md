# BLE Library Design Overview

<div style="display:none">
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2gap
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2gatt
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2security-manager
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2creating-profiles
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2comprehensive-gatt-guide
</div>

This library wraps BTstack with C++ classes to provide a more structured, object‑oriented BLE API while keeping BTstack’s behavior and constraints visible. It is intentionally thin: it does not hide the underlying ATT/GATT and HCI concepts, and it expects the application to manage event dispatch and object lifetimes carefully.

## BTstack Configuration (Pico W)

BTstack configuration for this library lives under `ble/platform/rpi_pico/config/`.
The main configuration file is `btstack_config.h`.

### Enabled Options in `btstack_config.h`

- **FreeRTOS integration**
  - `HAVE_FREERTOS_TASK_NOTIFICATIONS`
- **BLE roles**
  - `ENABLE_LE_PERIPHERAL`
  - `ENABLE_LE_CENTRAL` (when `RUNNING_AS_CLIENT` is set)
- **Logging**
  - `ENABLE_LOG_INFO`
  - `ENABLE_LOG_ERROR`
  - `ENABLE_PRINTF_HEXDUMP`
- **Security and crypto**
  - `ENABLE_SOFTWARE_AES128`
  - `ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS`
- **Flow control**
  - `ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL`
- **HAL/platform features**
  - `HAVE_EMBEDDED_TIME_MS`
  - `HAVE_ASSERT`
- **Resource limits / buffers**
  - `MAX_ATT_DB_SIZE` (fixed-size ATT DB)
  - `MAX_NR_HCI_CONNECTIONS`
  - `MAX_NR_GATT_CLIENTS` (depends on `RUNNING_AS_CLIENT`)
  - `MAX_NR_SM_LOOKUP_ENTRIES`
  - `MAX_NR_WHITELIST_ENTRIES`
  - `MAX_NR_LE_DEVICE_DB_ENTRIES`
  - `MAX_NR_CONTROLLER_ACL_BUFFERS`
  - `MAX_NR_CONTROLLER_SCO_PACKETS`
  - `HCI_OUTGOING_PRE_BUFFER_SIZE`
  - `HCI_ACL_PAYLOAD_SIZE`
  - `HCI_ACL_CHUNK_SIZE_ALIGNMENT`
  - `HCI_HOST_ACL_PACKET_LEN`, `HCI_HOST_ACL_PACKET_NUM`
  - `HCI_HOST_SCO_PACKET_LEN`, `HCI_HOST_SCO_PACKET_NUM`
  - `NVM_NUM_DEVICE_DB_ENTRIES`
  - `NVM_NUM_LINK_KEYS`
  - `HCI_RESET_RESEND_TIMEOUT_MS`

## Core Design Goals

- Encapsulate BTstack’s C APIs in C++ classes with clear ownership rules.
- Keep platform‑specific details isolated in `platform/` while the core logic stays platform‑agnostic.
- Preserve BTstack semantics (ATT database model, security levels, HCI events) rather than inventing a new abstraction layer.
- Favor explicit initialization order and a single, global BLE state.

## Top‑Level Architecture

- `c7222::Ble` is the top‑level singleton facade and owns access to the other singletons.
- `c7222::Gap` handles advertising, connection state, and GAP events. See \ref md_libs_2elec__c7222_2ble_2doc_2markdown_2gap "gap.md".
- `c7222::AttributeServer` parses the ATT database and routes attribute reads/writes. See \ref md_libs_2elec__c7222_2ble_2doc_2markdown_2gatt "gatt.md".
- `c7222::SecurityManager` configures pairing/encryption and dispatches security events. See \ref md_libs_2elec__c7222_2ble_2doc_2markdown_2security-manager "security-manager.md".
- Utility types (e.g., `BleAddress`, `BleError`, `Uuid`) provide shared protocol data types and error mapping.

## Singleton Model and Threading

- `Ble`, `Gap`, `AttributeServer`, and `SecurityManager` are singletons.
- The library is **not thread‑safe**. It is designed for single‑threaded or carefully serialized access. You must ensure all BLE calls happen from a consistent execution context.
- Event handlers are stored as raw pointers. Handler instances must outlive the BLE components that store them.

## ATT/GATT Database Flow

- BTstack uses a compiled ATT database (`att_db.h/att_db.c`) generated from `.gatt` files. See \ref md_libs_2elec__c7222_2ble_2doc_2markdown_2creating-profiles "creating-profiles.md" for the project’s GATT‑file guidance.
- `c7222::AttributeServer::Init(context)` expects a platform‑specific context pointer. On Pico W, this is the ATT database blob.
- `AttributeServer` parses the ATT database into `Service`, `Characteristic`, and `Attribute` objects.
- Static attributes reference bytes inside the ATT database blob. That blob must remain valid for the lifetime of the server.

## GATT Object Model

- `Attribute` represents a single ATT database entry (UUID, properties, handle, value).
- `Characteristic` wraps a Declaration, Value, and optional descriptors (CCCD/SCCD/User Description, etc.).
- `Service` groups characteristics and service‑level attributes.
- `Characteristic` uses internal read/write handlers and forwards higher‑level events to registered `EventHandler` instances.

## Security Model

- `SecurityManager` configures IO capability, authentication requirements, bonding, and key size ranges.
- `AttributeServer` caches security state and uses it to validate descriptor writes (e.g., CCCD/SCCD).
- Security configuration should be set before connections are established.
- Validation helpers exist to check whether the configured security can satisfy the GATT database requirements.

## Event Dispatch and Platform Glue

- The platform layer must forward HCI events into `c7222::Ble::DispatchBleHciPacket()`.
- `Ble` fans out events to GAP, AttributeServer, and SecurityManager as appropriate.
- On Raspberry Pi Pico W, BTstack runs its event processing inside a FreeRTOS task, so HCI events are dispatched from the BLE stack task context.
- On Raspberry Pi Pico W, the platform integration lives in:
  - `libs/elec_c7222/ble/platform/rpi_pico/`
  - `libs/elec_c7222/ble/gap/platform/rpi_pico/`
  - `libs/elec_c7222/ble/gatt/platform/rpi_pico/`
  - `libs/elec_c7222/ble/security_manager/platform/rpi_pico/`


## Key Limitations and Design Assumptions

- Single active connection is assumed by `AttributeServer`. Multiple connections are not supported reliably.
- ATT database entries are treated as immutable static data unless marked dynamic.
- Callbacks must be registered after the AttributeServer parses the ATT database and before BLE is turned on.
- Application code is responsible for error handling and for ensuring Bluetooth clients clear cached handle maps when the GATT layout changes.

## Typical Initialization Flow

- Configure security (optional) via `SecurityManager`.
- Enable the Attribute Server with the compiled ATT DB.
- Register GAP, SecurityManager, and Characteristic handlers.
- Turn on BLE via `c7222::Ble::TurnOn()`.
- Dispatch HCI events into `c7222::Ble::DispatchBleHciPacket()`.

## Platform Targets

- `platform/rpi_pico` provides the real BTstack integration for Pico W.
- `platform/grader` offers a lightweight stub for grading/testing.

## Usage Notes to Avoid Common Pitfalls

- Ensure `.gatt` changes regenerate the ATT database header and that clients clear their GATT cache.
- Use trailing commas for dynamic characteristics without static values in `.gatt` files.
- Keep all handler instances alive for the duration of BLE use.
- Avoid calling BLE APIs concurrently from multiple threads.


## Project Internal Mapping & C++ Integration

The project uses a custom C++ layer to wrap the raw BTstack ATT callbacks into an object-oriented structure.
See module‑specific documentation for full details:

- GAP module: `libs/elec_c7222/ble/doc/markdown/gap.md`
- GATT/ATT module: `libs/elec_c7222/ble/doc/markdown/gatt.md`
- Security Manager module: `libs/elec_c7222/ble/doc/markdown/security-manager.md`

### The Pipeline

- Binary Blob Initialization: `compile_gatt.py` produces a binary array `profile_db`. `c7222::AttributeServer::Init` (`libs/elec_c7222/ble/gatt/platform/rpi_pico/attribute_server.cpp`) receives this pointer and registers it with the BTstack core via `att_server_init(...)`.
- Binary Parsing (BTstack ATT DB -> Attributes):
  - The parser starts at `db + 1` (BTstack reserves the first byte).
  - Each entry begins with `entry_size` (uint16, little-endian). A size of 0 terminates the list.
  - Next fields are: `flags` (uint16), `handle` (uint16), then UUID (2 or 16 bytes).
  - The remaining bytes in the entry are the value payload.
  - `ParseEntry()` constructs an `Attribute` from `(uuid, flags, value_ptr, value_len, handle)`.
  - Static attributes keep pointers into the DB blob, so the DB must outlive all parsed objects.
- Object Assembly (Attributes -> Services/Characteristics):
  - `InitServices()` calls `c7222::Service::ParseFromAttributes()`, which walks the ordered list.
  - Each Service block starts at a Service Declaration attribute and consumes attributes until the next Service Declaration.
  - Within a Service block, `c7222::Characteristic::ParseFromAttributes()` groups:
    - Declaration attribute
    - Value attribute
    - Descriptor attributes (CCCD/SCCD/User Description/Extended Properties/custom)
- Runtime Routing (ATT -> C++ objects):
  - BTstack calls `att_read_callback` / `att_write_callback`.
  - `AttributeServer` finds the matching handle:
    - If it belongs to a `Characteristic`, it calls `c7222::Characteristic::HandleAttributeRead/Write()`.
    - Otherwise it calls the service-level `c7222::Attribute::InvokeRead/WriteCallback()`.
  - Errors are mapped through `BleError` back to BTstack ATT error codes.

### Handler Types and Purposes

There are two main handler families: **low-level attribute callbacks** and **high-level characteristic events**.

**1) Attribute callbacks (data access + validation)**

Registered on `Attribute` instances:

- `c7222::Attribute::SetReadCallback(ReadCallback)`  
  Purpose: Provide data for reads (or block reads via ATT error codes). Used for dynamic values or service-level attributes.
- `c7222::Attribute::SetWriteCallback(WriteCallback)`  
  Purpose: Validate/accept writes and optionally update dynamic storage.

Where used:
- Service-level attributes (e.g., Included Service Declaration attributes).
- Characteristic value or descriptor attributes if you want to bypass default behavior.
- Any custom descriptor with dynamic storage.

Notes:
- If no read callback is installed, `InvokeReadCallback()` returns stored bytes for static or dynamic attributes.
- If no write callback is installed on a **dynamic** attribute, the default storage still updates and the write succeeds.
- Read callbacks return a byte count (or ATT error code) to match BTstack semantics; write callbacks return `BleError`.

**2) Characteristic EventHandlers (semantic events)**

Registered via `c7222::Characteristic::AddEventHandler(EventHandlers&)`:

- `OnUpdatesEnabled(bool is_indication)` / `OnUpdatesDisabled()`  
  Fired on CCCD writes when a client enables/disables notifications/indications.
- `OnBroadcastEnabled()` / `OnBroadcastDisabled()`  
  Fired on SCCD writes for broadcast configuration.
- `OnRead()`  
  Fired during a value read (from the default value read handler). Use to refresh dynamic data with `SetValue()`.
- `OnWrite(const std::vector<uint8_t>& data)`  
  Fired after a successful value write (default write handler).
- `OnIndicationComplete(uint8_t status)` / `OnConfirmationReceived(bool status)`  
  Fired after HCI indication completion events. These are driven by `DispatchBleHciPacket()` being fed HCI packets.

Notes:
- These are **notifications** about events, not the raw data path. They do not replace
  the value attribute read/write callbacks.
- If you override the value attribute with `SetReadCallback()`/`SetWriteCallback()`,
  the default handlers (and therefore `OnRead`/`OnWrite`) are bypassed.


