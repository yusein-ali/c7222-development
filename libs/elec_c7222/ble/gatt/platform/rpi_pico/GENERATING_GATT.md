# Comprehensive GATT Development Guide for Pico BTstack

This document provides a full reference for defining, generating, and integrating BLE GATT services on the Raspberry Pi Pico using the BTstack framework. It assumes you are comfortable with embedded C/C++, binary layouts, and the BLE ATT/GATT model.

## 1. GATT File Syntax Reference (.gatt)

BTstack uses a simplified comma-separated values (CSV) Domain Specific Language to define the Attribute Protocol (ATT) database. Files are typically stored in:

`libs/elec_c7222/ble/gatt/platform/rpi_pico/services/custom/`.

### File Layout Rules

- Lines starting with `//` are comments.
- `#import <name.gatt>` loads built-in BTstack service definitions.
- `#import "name.gatt"` loads a local `.gatt` alongside your file.
- Values can be quoted strings (`"Pico"`) or space-separated hex bytes (`01 02 03`).
- UUIDs can be explicit 16-bit/128-bit values or predefined tokens from BTstack's assigned UUID list (see BTstack documentation and `bluetooth_gatt.h`).


### Core Keywords and Syntax

| Keyword | Syntax | Description |
| --- | --- | --- |
| PRIMARY_SERVICE | `PRIMARY_SERVICE, <uuid>` | Starts a new Primary Service. |
| SECONDARY_SERVICE | `SECONDARY_SERVICE, <uuid>` | Starts a Secondary Service. |
| INCLUDE_SERVICE | `INCLUDE_SERVICE, <uuid>` | Includes a previously defined service by UUID (uses its handle range). |
| CHARACTERISTIC | `CHARACTERISTIC, <uuid>, <properties>, <value>` | Defines a characteristic declaration + value (declaration + value handle). |
| CHARACTERISTIC_USER_DESCRIPTION | `CHARACTERISTIC_USER_DESCRIPTION, <properties>` | Adds the 0x2901 descriptor (dynamic descriptor in BTstack). |
| CLIENT_CHARACTERISTIC_CONFIGURATION | `CLIENT_CHARACTERISTIC_CONFIGURATION` | Reserved token; CCCD is auto-added when `NOTIFY`/`INDICATE` is set. |
| SERVER_CHARACTERISTIC_CONFIGURATION | `SERVER_CHARACTERISTIC_CONFIGURATION, <properties>` | Adds the 0x2903 SCCD descriptor (dynamic). |
| CHARACTERISTIC_FORMAT | `CHARACTERISTIC_FORMAT, <id>, <format>, <exponent>, <unit_uuid>, <namespace>, <description_uuid>` | Adds the 0x2904 presentation format descriptor. |
| CHARACTERISTIC_AGGREGATE_FORMAT | `CHARACTERISTIC_AGGREGATE_FORMAT, <id1>, <id2>, ...` | Aggregates multiple presentation formats (0x2905). |
| VALID_RANGE | `VALID_RANGE, <properties>` | Adds 0x2906 descriptor (dynamic, read-only). |
| EXTERNAL_REPORT_REFERENCE | `EXTERNAL_REPORT_REFERENCE, <properties>, <report_uuid16>` | Adds 0x2907 descriptor. |
| REPORT_REFERENCE | `REPORT_REFERENCE, <properties>, <report_id>, <report_type>` | Adds 0x2908 descriptor. |
| NUMBER_OF_DIGITALS | `NUMBER_OF_DIGITALS, <count>` | Adds 0x2909 descriptor. |
| VALUE_TRIGGER_SETTING | `VALUE_TRIGGER_SETTING, <properties>` | Adds 0x290A descriptor (dynamic). |
| ENVIRONMENTAL_SENSING_CONFIGURATION | `ENVIRONMENTAL_SENSING_CONFIGURATION, <properties>` | Adds 0x290B descriptor (dynamic). |
| ENVIRONMENTAL_SENSING_MEASUREMENT | `ENVIRONMENTAL_SENSING_MEASUREMENT, <properties>` | Adds 0x290C descriptor (dynamic, read-only). |
| ENVIRONMENTAL_SENSING_TRIGGER_SETTING | `ENVIRONMENTAL_SENSING_TRIGGER_SETTING, <properties>` | Adds 0x290D descriptor (dynamic). |

### Characteristic Properties

Properties control how a client interacts with the data. Multiple properties are separated by the pipe `|` symbol.

- GATT properties: `BROADCAST`, `READ`, `WRITE_WITHOUT_RESPONSE`, `WRITE`, `NOTIFY`, `INDICATE`, `AUTHENTICATED_SIGNED_WRITE`, `EXTENDED_PROPERTIES`.
- BTstack extensions: `DYNAMIC` (route through callbacks, no static value storage).
- Extended property flags: `RELIABLE_WRITE`, `AUTHENTICATION_REQUIRED`, `AUTHORIZATION_REQUIRED`.
- Read permissions: `READ_ANYBODY`, `READ_ENCRYPTED`, `READ_AUTHENTICATED`, `READ_AUTHENTICATED_SC`, `READ_AUTHORIZED`.
- Write permissions: `WRITE_ANYBODY`, `WRITE_ENCRYPTED`, `WRITE_AUTHENTICATED`, `WRITE_AUTHENTICATED_SC`, `WRITE_AUTHORIZED`.
- Encryption key size: `ENCRYPTION_KEY_SIZE_7` through `ENCRYPTION_KEY_SIZE_16`.
- Low-level permission bits (rare): `READ_PERMISSION_BIT_0`, `READ_PERMISSION_BIT_1`, `WRITE_PERMISSION_BIT_0`, `WRITE_PERMISSION_BIT_1`, `READ_PERMISSION_SC`, `WRITE_PERMISSION_SC`.

### Value Definitions

- Static String: `CHARACTERISTIC, 2A29, READ, "Manufacturer Name"`
- Static Hex: `CHARACTERISTIC, 2A04, READ, 06 00 08 00` (Connection Parameters)
- Empty/Dynamic: `CHARACTERISTIC, FF01, READ | DYNAMIC,` (Value is provided by C++ logic)

### Known UUID Tokens (BTstack)

BTstack lets you use well-known service/characteristic UUIDs as named tokens instead of raw hex. These come from the assigned numbers list in `bluetooth_gatt.h`.

Example:

```gatt
PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_HEART_RATE
CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_HEART_RATE_MEASUREMENT, DYNAMIC | NOTIFY,
```

### From Bluetooth.org Profile Spec to .gatt

BTstack’s docs also outline a practical way to build services from official Bluetooth SIG profile specs:

1. Go to `https://www.bluetooth.com/specifications/specs/` (or the GATT XML listing).
2. Find the service profile (for example, Heart Rate or Battery Service).
3. Open the service definition and its characteristic definitions.
4. Translate each characteristic and descriptor into `.gatt` entries:
   - Service UUID -> `PRIMARY_SERVICE`
   - Characteristic UUID + properties -> `CHARACTERISTIC`
   - Required descriptors (CCCD, user description, format, etc.) -> matching `.gatt` descriptor keywords
5. Save the `.gatt` file and generate the header with `compile_gatt.py`.

## 2. Example .gatt Files

### Minimal custom service with dynamic value and notifications

```gatt
// Example: custom temperature service
PRIMARY_SERVICE, 0000FFF0-0000-1000-8000-00805F9B34FB
CHARACTERISTIC, 0000FFF1-0000-1000-8000-00805F9B34FB, READ | NOTIFY | DYNAMIC,
```

### Mixed standard + custom, with descriptors and permissions

```gatt
// Use standard services
#import <gatt_service.gatt>
#import <battery_service.gatt>

// Device Information (static strings)
PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_DEVICE_INFORMATION
CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_MANUFACTURER_NAME_STRING, READ, "Pico Lab"

// Custom service with a dynamic value and a report reference
PRIMARY_SERVICE, 0000FF10-0000-1000-8000-00805F9B34FB
CHARACTERISTIC, 0000FF11-0000-1000-8000-00805F9B34FB, READ | WRITE | DYNAMIC | ENCRYPTION_KEY_SIZE_16,
REPORT_REFERENCE, READ, 1, 1
```

## 3. Best Practices

- UUID Stability: Changing a UUID or reordering services changes the handles. This will break bonded clients (like smartphones) unless you toggle Bluetooth on the phone to clear the GATT cache.
- Memory Management: Since Attribute objects point directly into the att_db blob, the generated header array must stay in memory for the entire duration of the application.
- Cleanup: If using CMake, delete any existing manual *.gatt.h files to ensure you are including the one generated in the build directory.

## 4. BTstack Documentation Workflow Summary

BTstack’s documented workflow for GATT services is:

1. Write a `.gatt` file that defines your services/characteristics.
2. Run `compile_gatt.py` (directly or via CMake) to generate a header containing the ATT DB blob (`profile_data`/`att_db`).
3. Include the generated header in your firmware.
4. Initialize the ATT server with the generated database via `att_server_init(...)` and register your read/write callbacks.
5. If you import BTstack standard services, call their `*_init(...)` functions after registering the ATT DB.
6. At runtime, use `att_server_notify(...)` / `att_server_indicate(...)` to send updates to subscribed clients.


## 5. Generation Workflow (This Project)

### Automated Generation (CMake)

Add your service definition under `services/custom/your_service.gatt`. In `CMakeLists.txt` (after `pico_sdk_init()`):

```cmake
pico_btstack_make_gatt_header(picoboard_freertos_test PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/libs/elec_c7222/ble/gatt/platform/rpi_pico/services/custom/your_service.gatt)
```

### Manual Generation (CLI)

Run from the repository root to generate the header for manual inspection or check-in:

```bash
python lib/btstack/tool/compile_gatt.py \
  libs/elec_c7222/ble/gatt/platform/rpi_pico/services/custom/your_service.gatt \
  > libs/elec_c7222/ble/gatt/platform/rpi_pico/services/custom/your_service.gatt.h
```

## 6. Project Internal Mapping & C++ Integration

The project uses a custom C++ layer to wrap the raw BTstack ATT callbacks into an object-oriented structure.

### Class Organization (C++ Layer)

- `AttributeServer` (`libs/elec_c7222/ble/gatt/include/attribute_server.hpp`)
  - Singleton that owns the parsed `Service` list.
  - Caches the platform context (on Pico W, the `att_db` blob).
  - Routes ATT read/write requests to `Characteristic` or service-level `Attribute` handlers.
  - Fans out ATT-related HCI events to characteristics (for indication completion and flow control).
- `Service` (`libs/elec_c7222/ble/gatt/include/service.hpp`)
  - Owns the Service Declaration `Attribute`, optional Included Service Declarations, and a list of `Characteristic` objects.
  - `ParseFromAttributes()` consumes the ordered attribute list and builds services in discovery order.
- `Characteristic` (`libs/elec_c7222/ble/gatt/include/characteristic.hpp`)
  - Owns its Declaration `Attribute`, Value `Attribute`, and optional descriptors (CCCD, SCCD, User Description, Extended Properties, and custom descriptors).
  - Implements handle-based routing so the AttributeServer can dispatch to the correct internal attribute.
  - Stores a list of `EventHandlers` for high-level application callbacks.
- `Attribute` (`libs/elec_c7222/ble/gatt/include/attribute.hpp`)
  - Leaf object representing a single ATT DB entry: UUID, properties/flags, handle, and value storage.
  - Static values point into the DB blob; dynamic values use owned `std::vector` storage.
  - Provides read/write callbacks for direct data access or validation.

### The Pipeline

- Binary Blob Initialization: `compile_gatt.py` produces a binary array `att_db`. `AttributeServer::Init` (`libs/elec_c7222/ble/gatt/platform/rpi_pico/attribute_server.cpp`) receives this pointer and registers it with the BTstack core via `att_server_init(...)`.
- Binary Parsing (BTstack ATT DB -> Attributes):
  - The parser starts at `db + 1` (BTstack reserves the first byte).
  - Each entry begins with `entry_size` (uint16, little-endian). A size of 0 terminates the list.
  - Next fields are: `flags` (uint16), `handle` (uint16), then UUID (2 or 16 bytes).
  - The remaining bytes in the entry are the value payload.
  - `ParseEntry()` constructs an `Attribute` from `(uuid, flags, value_ptr, value_len, handle)`.
  - Static attributes keep pointers into the DB blob, so the DB must outlive all parsed objects.
- Object Assembly (Attributes -> Services/Characteristics):
  - `InitServices()` calls `Service::ParseFromAttributes()`, which walks the ordered list.
  - Each Service block starts at a Service Declaration attribute and consumes attributes until the next Service Declaration.
  - Within a Service block, `Characteristic::ParseFromAttributes()` groups:
    - Declaration attribute
    - Value attribute
    - Descriptor attributes (CCCD/SCCD/User Description/Extended Properties/custom)
- Runtime Routing (ATT -> C++ objects):
  - BTstack calls `att_read_callback` / `att_write_callback`.
  - `AttributeServer` finds the matching handle:
    - If it belongs to a `Characteristic`, it calls `Characteristic::HandleAttributeRead/Write()`.
    - Otherwise it calls the service-level `Attribute::InvokeRead/WriteCallback()`.
  - Errors are mapped through `BleError` back to BTstack ATT error codes.

### Handler Types and Purposes

There are two main handler families: **low-level attribute callbacks** and **high-level characteristic events**.

**1) Attribute callbacks (data access + validation)**

Registered on `Attribute` instances:

- `Attribute::SetReadCallback(ReadCallback)`  
  Purpose: Provide data for reads (or block reads via ATT error codes). Used for dynamic values or service-level attributes.
- `Attribute::SetWriteCallback(WriteCallback)`  
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

Registered via `Characteristic::AddEventHandler(EventHandlers&)`:

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
