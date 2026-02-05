# Creating GATT Profiles (BTstack + This Library)

This note summarizes the BTstack GATT Server guidance (from `doc/BTstack-md/profiles.md`, section "GATT Server") and adds **GATT‑file‑specific** caveats used in this project.

Warning: a detailed GATT guide is available in [comprehensive-gatt-guide.md](comprehensive-gatt-guide.md). Refer to it for deeper explanations and edge cases.

## BTstack GATT Server Basics

- BTstack does not provide a high-level GATT server implementation. Instead, you define a textual `.gatt` profile and the BTstack profile compiler converts it into a compact ATT database.
- The ATT server (`att_db.h` and `att_server.h`) answers requests using the compiled database and calls read/write callbacks for dynamic attributes.
- The `.gatt` format is a simple CSV-like syntax.

Example structure:

```gatt
#import <service_name.gatt>

PRIMARY_SERVICE, {SERVICE_UUID}
CHARACTERISTIC, {ATTRIBUTE_TYPE_UUID}, {PROPERTIES}, {VALUE}
CHARACTERISTIC, {ATTRIBUTE_TYPE_UUID}, {PROPERTIES}, {VALUE}
```

## UUIDs and Values

- UUIDs may be 16-bit (`1800`) or 128-bit (`00001234-0000-1000-8000-00805F9B34FB`).
- Values can be a string (`"text"`) or a sequence of hex bytes (`01 02 03`).

## Properties and Dynamic Attributes

- Properties are combined with `|`.
- If a characteristic uses `DYNAMIC`, reads/writes are routed to the application callbacks. Without `DYNAMIC`, the value is static and not writable.
- Adding `NOTIFY` or `INDICATE` automatically adds a CCCD in the compiled database.

Core properties:

- `READ`
- `WRITE`
- `WRITE_WITHOUT_RESPONSE`
- `NOTIFY`
- `INDICATE`
- `DYNAMIC`

Security-related properties:

- `AUTHENTICATION_REQUIRED` (read and write)
- `READ_ENCRYPTED`
- `READ_AUTHENTICATED`
- `WRITE_ENCRYPTED`
- `WRITE_AUTHENTICATED`
- `ENCRYPTION_KEY_SIZE_7` through `ENCRYPTION_KEY_SIZE_16`

**Important:** security flags do not imply access. You must still include `READ` and/or `WRITE` to allow the operation. For example, `READ_AUTHENTICATED` without `READ` will still reject reads. If authentication is required, it must be explicitly stated on the corresponding read and/or write permission flags.

## Implementing Standard Services

BTstack’s standard process:

1. Get the service specification from Bluetooth SIG.
2. Extract the Service Characteristics table and properties.
3. Create the `.gatt` file from that table.
4. Implement the service server logic (read/write callbacks).

BTstack provides many standard UUIDs in `bluetooth_gatt.h`. Missing UUIDs can be taken from the Bluetooth SIG assigned numbers.

### Creating a Service from Bluetooth SIG Specs (BTstack documentation excerpt)

**Copied and adapted from BTstack documentation** (doc/BTstack-md/profiles.md, section “Implementing Standard GATT Services”). The wording below is clarified but the steps and example follow BTstack.

1. **Get the service specification** from the Bluetooth SIG website.
2. **Find the Service Characteristics table** and note the required characteristics and their properties.
3. **Translate the table into a `.gatt` file** with the correct UUID tokens and properties.
4. **Implement the server logic** by handling read/write requests for the characteristic handles.

Example (Battery Service):

- The Battery Service has one mandatory characteristic: **Battery Level**.
- Properties for Battery Level: **Read** (mandatory) and **Notify** (optional).

Example `.gatt` snippet:

```gatt
// Battery Service v1.0
PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_BATTERY_SERVICE
CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_BATTERY_LEVEL, DYNAMIC | READ | NOTIFY,
```

BTstack includes most standard UUID tokens in `bluetooth_gatt.h`. If a UUID is missing, you can pull it from the Bluetooth SIG assigned numbers list.

BTstack also ships several pre‑implemented service `.gatt` files. In this project, some examples live under `ble/gatt/platform/rpi_pico/services/`.

## Project‑Specific GATT File Caveats

### User Description Descriptor

- The `.gatt` keyword `CHARACTERISTIC_USER_DESCRIPTION` adds the 0x2901 descriptor to the database.
- In this library, the descriptor text is provided at runtime via `c7222::Characteristic::SetUserDescription()` or `SetUserDescriptionText()`.
- The User Description descriptor is treated as read‑only and served by a shared internal read handler.
- `CHARACTERISTIC_USER_DESCRIPTION` does **not** take a default value in the `.gatt` file; it must be set in code.

Example (User Description set in code):

```gatt
PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING
CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE, DYNAMIC | READ | NOTIFY,
CHARACTERISTIC_USER_DESCRIPTION, READ
```

```cpp
// After AttributeServer is initialized and characteristics are parsed
auto* server = c7222::AttributeServer::GetInstance();
auto* ch = server->FindCharacteristicByUuid(c7222::Uuid(0x2A6E)); // Temperature
if(ch != nullptr) {
    ch->SetUserDescription("Temperature (C)");
}
```

### Dynamic Characteristic Syntax

- For dynamic characteristics without a static value, include a trailing comma in the `.gatt` line.
- Example:

```gatt
CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE, DYNAMIC | READ | NOTIFY,
```

### Handle Stability and Client Caches

- Any change in `.gatt` order or content changes ATT handles.
- BLE clients (phones) cache handle maps. Clear the cache or "forget" the device after profile changes.

## Examples with Authentication Required

**Read authentication required:**

```gatt
PRIMARY_SERVICE, 0000FFF0-0000-1000-8000-00805F9B34FB
CHARACTERISTIC, 0000FFF1-0000-1000-8000-00805F9B34FB, DYNAMIC | READ | READ_AUTHENTICATED,
```

**Read and write authentication required:**

```gatt
PRIMARY_SERVICE, 0000FFF0-0000-1000-8000-00805F9B34FB
CHARACTERISTIC, 0000FFF2-0000-1000-8000-00805F9B34FB, DYNAMIC | READ | WRITE | READ_AUTHENTICATED | WRITE_AUTHENTICATED,
```

## Complete Example (from `examples/ble/gatt-server/app_profile.gatt`)

```gatt
PRIMARY_SERVICE, GAP_SERVICE
CHARACTERISTIC, GAP_DEVICE_NAME, READ, "picow_temp"

PRIMARY_SERVICE, GATT_SERVICE
CHARACTERISTIC, GATT_DATABASE_HASH, READ,

PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING
CHARACTERISTIC, fc930f88-1a30-45d7-8c17-604c1a036b9f, DYNAMIC | READ | WRITE | READ_AUTHENTICATED | READ_ENCRYPTED | WRITE_AUTHENTICATED | WRITE_ENCRYPTED,
CHARACTERISTIC_USER_DESCRIPTION, READ

CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE, READ | NOTIFY | INDICATE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED,
CHARACTERISTIC_USER_DESCRIPTION, READ
```

Line‑by‑line explanation:

1. `PRIMARY_SERVICE, GAP_SERVICE`  
   Adds the standard GAP service (device name, appearance, etc.).
2. `CHARACTERISTIC, GAP_DEVICE_NAME, READ, "picow_temp"`  
   Defines the Device Name characteristic with a static value.
3. `PRIMARY_SERVICE, GATT_SERVICE`  
   Adds the standard GATT service.
4. `CHARACTERISTIC, GATT_DATABASE_HASH, READ,`  
   Adds the Database Hash characteristic (static, compiler‑generated value).
5. `PRIMARY_SERVICE, ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING`  
   Starts the Environmental Sensing service.
6. `CHARACTERISTIC, <custom UUID>, DYNAMIC | READ | WRITE | READ_AUTHENTICATED | READ_ENCRYPTED | WRITE_AUTHENTICATED | WRITE_ENCRYPTED,`  
   Adds a custom, dynamic configuration characteristic with authenticated/encrypted read and write.
7. `CHARACTERISTIC_USER_DESCRIPTION, READ`  
   Adds the 0x2901 User Description descriptor (text provided in code).
8. `CHARACTERISTIC, ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE, READ | NOTIFY | INDICATE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED,`  
   Adds the standard temperature characteristic, dynamic, readable, and supports notify/indicate.
9. `CHARACTERISTIC_USER_DESCRIPTION, READ`  
   Adds a User Description descriptor for the temperature characteristic.

## What the Parsed Profile Looks Like

When the ATT DB is parsed, the library builds `Service`, `Characteristic`, and `Attribute` objects. A conceptual view looks like:

```
Service: GAP (0x1800)
  Characteristic: Device Name (0x2A00)
    Value: "picow_temp"

Service: GATT (0x1801)
  Characteristic: Database Hash (0x2B2A)
    Value: <16-byte hash>

Service: Environmental Sensing (0x181A)
  Characteristic: <custom UUID>
    Properties: READ | WRITE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED | WRITE_AUTHENTICATED | WRITE_ENCRYPTED
    Descriptor: User Description (0x2901)
  Characteristic: Temperature (0x2A6E)
    Properties: READ | NOTIFY | INDICATE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED
    Descriptor: CCCD (auto‑added by NOTIFY/INDICATE)
    Descriptor: User Description (0x2901)
```

## Generated ATT DB Array (from build output)

This is the `profile_data[]` array generated by BTstack for the current `app_profile.gatt`:

```c
// att db format version 1
// binary attribute representation:
// - size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

const uint8_t profile_data[] =
{
    // ATT DB Version
    1,

    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18,
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME - READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a,
    // 0x0003 VALUE CHARACTERISTIC-GAP_DEVICE_NAME - READ -'picow_temp'
    // READ_ANYBODY
    0x12, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x70, 0x69, 0x63, 0x6f, 0x77, 0x5f, 0x74, 0x65, 0x6d, 0x70,
    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18,
    // 0x0005 CHARACTERISTIC-GATT_DATABASE_HASH - READ
    0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x2a, 0x2b,
    // 0x0006 VALUE CHARACTERISTIC-GATT_DATABASE_HASH - READ -''
    // READ_ANYBODY
    0x18, 0x00, 0x02, 0x00, 0x06, 0x00, 0x2a, 0x2b, 0x30, 0xa8, 0x8b, 0xe2, 0x85, 0x1b, 0x11, 0xcd, 0x8e, 0xd2, 0x42, 0x15, 0xd7, 0x71, 0x83, 0x20,
    // 0x0007 PRIMARY_SERVICE-ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING
    0x0a, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x1a, 0x18,
    // 0x0008 CHARACTERISTIC-fc930f88-1a30-45d7-8c17-604c1a036b9f - DYNAMIC | READ | WRITE | READ_AUTHENTICATED | READ_ENCRYPTED | WRITE_AUTHENTICATED | WRITE_ENCRYPTED
    0x1b, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x0a, 0x09, 0x00, 0x9f, 0x6b, 0x03, 0x1a, 0x4c, 0x60, 0x17, 0x8c, 0xd7, 0x45, 0x30, 0x1a, 0x88, 0x0f, 0x93, 0xfc,
    // 0x0009 VALUE CHARACTERISTIC-fc930f88-1a30-45d7-8c17-604c1a036b9f - DYNAMIC | READ | WRITE | READ_AUTHENTICATED | READ_ENCRYPTED | WRITE_AUTHENTICATED | WRITE_ENCRYPTED
    // READ_AUTHENTICATED, WRITE_AUTHENTICATED
    0x16, 0x00, 0x1a, 0x0b, 0x09, 0x00, 0x9f, 0x6b, 0x03, 0x1a, 0x4c, 0x60, 0x17, 0x8c, 0xd7, 0x45, 0x30, 0x1a, 0x88, 0x0f, 0x93, 0xfc,
    // 0x000a USER_DESCRIPTION-READ
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x0a, 0x00, 0x01, 0x29,
    // 0x000b CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE - READ | NOTIFY | INDICATE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED
    0x0d, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x03, 0x28, 0x32, 0x0c, 0x00, 0x6e, 0x2a,
    // 0x000c VALUE CHARACTERISTIC-ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE - READ | NOTIFY | INDICATE | DYNAMIC | READ_AUTHENTICATED | READ_ENCRYPTED
    // READ_AUTHENTICATED
    0x08, 0x00, 0x02, 0x09, 0x0c, 0x00, 0x6e, 0x2a,
    // 0x000d CLIENT_CHARACTERISTIC_CONFIGURATION
    // READ_ANYBODY, WRITE_ANYBODY
    0x0a, 0x00, 0x0e, 0x01, 0x0d, 0x00, 0x02, 0x29, 0x00, 0x00,
    // 0x000e USER_DESCRIPTION-READ
    // READ_ANYBODY, WRITE_ANYBODY
    0x08, 0x00, 0x0a, 0x01, 0x0e, 0x00, 0x01, 0x29,
    // END
    0x00, 0x00,
};
```

## Generated Handle Macros (from the same header)

The generated header also provides macros that map services and characteristics to ATT handles. These are useful when you need to reference handles directly (e.g., for notifications, indications, or for raw ATT access).

Examples from `app_profile.h`:

```c
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING_START_HANDLE 0x0007
#define ATT_SERVICE_ORG_BLUETOOTH_SERVICE_ENVIRONMENTAL_SENSING_END_HANDLE   0x000e

#define ATT_CHARACTERISTIC_GAP_DEVICE_NAME_01_VALUE_HANDLE 0x0003
#define ATT_CHARACTERISTIC_GATT_DATABASE_HASH_01_VALUE_HANDLE 0x0006
#define ATT_CHARACTERISTIC_fc930f88_1a30_45d7_8c17_604c1a036b9f_01_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_fc930f88_1a30_45d7_8c17_604c1a036b9f_01_USER_DESCRIPTION_HANDLE 0x000a
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_VALUE_HANDLE 0x000c
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_CLIENT_CONFIGURATION_HANDLE 0x000d
#define ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_USER_DESCRIPTION_HANDLE 0x000e
```

Typical uses:

- **Notifications/Indications:** use the value handle when calling `att_server_notify()` or `att_server_indicate()` in BTstack.
- **Descriptor writes:** use the CCCD handle (e.g., `_CLIENT_CONFIGURATION_HANDLE`) for enabling notifications/indications.
- **Service range checks:** use the service start/end handle macros for service‑level read/write dispatch if needed.

## Using the Parsed DB with Our Classes

```cpp
auto* ble = c7222::Ble::GetInstance();
ble->EnableAttributeServer(att_db);  // att_db from generated header

auto* server = c7222::AttributeServer::GetInstance();

// Find and configure the temperature characteristic
auto* temp = server->FindCharacteristicByUuid(c7222::Uuid(0x2A6E));
if(temp != nullptr) {
    temp->SetUserDescription("Temperature (C)");
    // Add handlers for read/write/notify if needed
    // temp->AddEventHandler(my_handler);
}

// Find and configure the custom characteristic (128‑bit UUID)
auto* cfg = server->FindCharacteristicByUuid(
    c7222::Uuid::FromString("fc930f88-1a30-45d7-8c17-604c1a036b9f"));
if(cfg != nullptr) {
    cfg->SetUserDescription("Configuration");
}
```

### Example: Using Handle Macros to Locate Characteristics

If you already have a handle (or want to use the generated macros directly), you can use the handle‑based lookup:

```cpp
auto* server = c7222::AttributeServer::GetInstance();

// Find the temperature characteristic via its value handle
auto* temp = server->FindCharacteristicByHandle(
    ATT_CHARACTERISTIC_ORG_BLUETOOTH_CHARACTERISTIC_TEMPERATURE_01_VALUE_HANDLE);

// Find the custom configuration characteristic via its value handle
auto* cfg = server->FindCharacteristicByHandle(
    ATT_CHARACTERISTIC_fc930f88_1a30_45d7_8c17_604c1a036b9f_01_VALUE_HANDLE);
```

You can also use the CCCD handle macro when enabling notifications/indications from client code, or when checking if a write targets the CCCD descriptor.
