# Custom Board BLE Setup (RP2350)

This page documents how to port the BLE library to a custom RP2350 board that connects a
separate BLE controller over UART, without using the CYW43 module. The reference
implementation is `libs/elec_c7222/examples/ble/rp2350-custom-board-gatt-server/`.

## Architecture Overview

On Pico W, BTstack is driven by the CYW43 chip and its SDK integration owns the run loop
and transport. On a custom board with an external BLE controller, you supply the transport
layer yourself through a `BtstackPort` subclass. The `Ble` singleton accepts a
`BtstackPort*` on first construction and delegates all BTstack lifecycle calls through it.

```
main()
  └─ Platform::Initialize()          // stdio, clocks, hardware init
  └─ FreeRtosTask "BLE_App"
       └─ Ble::GetInstance(&port)    // registers the custom port
            └─ BtstackPort           // abstract: Init / Apply / ExecuteRunLoop
                 └─ RpiBdBtstackPort // concrete: H4 UART + FreeRTOS run loop
```

The `BtstackPort` base class owns a `BtstackPortTables` bundle of non-owning pointers to
BTstack C function tables (`hci_transport_t`, `btstack_run_loop_t`, `btstack_chipset_t`,
`btstack_tlv_t`). The concrete port fills this bundle and `Apply()` writes it to BTstack.

## BtstackPort Contract

```
virtual bool Init()    — initialize hardware and BTstack memory pools; call SetInitialized(true)
virtual void Deinit()  — release resources; call SetInitialized(false)
bool Apply()           — non-virtual; writes tables to BTstack (called by Ble after Init)
bool UsesFreeRtosRunLoop() — returns true when tables carry the FreeRTOS run loop
void ExecuteRunLoop()  — blocks forever running btstack_run_loop_execute(); called from a dedicated task
```

Derived classes call `SetTables()` in their constructor or `Init()` to register the bundle.

## RpiBdBtstackPort

`RpiBdBtstackPort` is the concrete port for the custom RP2350 board. It configures:

- **Transport:** HCI H4 over UART (`hci_transport_h4_instance` wrapping
  `btstack_uart_block_embedded_instance`)
- **Run loop:** BTstack FreeRTOS run loop (`btstack_run_loop_freertos`)
- **Persistence:** none (`btstack_tlv_none_init_instance`) — bonding keys are not stored
- **Chipset driver:** none — the external controller uses a standard H4 protocol with no
  vendor extension

### UART Configuration

Hardware pin assignments come from `rpi_bd_hub_mcu.h`:

| Signal | GPIO |
|--------|------|
| TX     | 4    |
| RX     | 5    |
| CTS    | 6    |
| RTS    | 7    |

Baud rate: **921600 bps** (`RPI_BD_BLE_UART_BAUD`).  
Hardware flow control (CTS/RTS) is enabled. The CMake flag
`HAVE_UART_DMA_SET_FLOWCONTROL=1` must be defined so BTstack's embedded UART driver
activates flow control at startup.

### Init Sequence

```cpp
bool RpiBdBtstackPort::Init() {
    if (IsInitialized()) return true;
    btstack_memory_init();
    SetInitialized(true);
    return true;
}
```

`btstack_memory_init()` allocates BTstack's internal memory pools from its own static
buffers. It must be called before `Apply()` writes tables into BTstack.

### Table Creation

```cpp
// Inside RpiBdBtstackPort constructor (simplified)
const btstack_uart_block_t* uart    = btstack_uart_block_embedded_instance();
const hci_transport_t*      h4      = hci_transport_h4_instance(uart);
const btstack_tlv_t*        tlv     = btstack_tlv_none_init_instance();

SetTables(BtstackPortTables::CreateWithFreeRtosRunLoop(
    h4,              // HCI transport
    &ble_uart_config,// UART baud / pins / flow control
    nullptr,         // no chipset driver
    tlv,             // no-op TLV storage
    nullptr));       // no TLV context
```

`CreateWithFreeRtosRunLoop` is a factory on `BtstackPortTables` that pre-fills the
`run_loop` field with `btstack_run_loop_freertos_get_instance()`.

## FreeRTOS Run Loop Task

Because `UsesFreeRtosRunLoop()` returns true, `Ble` does not call `btstack_run_loop_execute()`
inline. Instead, when `EnableAttributeServer()` (or `TurnOn()`) triggers
`EnsureBtstackPortInitialized()`, a dedicated **BTstack** FreeRTOS task is created:

```
Task name:     "BTstack"
Stack depth:   2048 words
Priority:      IdlePriority() + 2   (one above BLE_App)
Body:          Ble::BtstackRunLoopTaskBody()
               → btstack_port->Init()
               → btstack_port->Apply()
               → btstack_port->ExecuteRunLoop()   // never returns
```

`Ble` uses two `std::atomic<bool>` flags (`btstack_run_loop_ready_`, `btstack_run_loop_failed_`)
to synchronize startup. `BLE_App` spins with 1 ms delays until the BTstack task signals
readiness or failure.

## Application Task Layout

| Task        | Stack (words) | Priority            | Notes                          |
|-------------|---------------|---------------------|--------------------------------|
| BLE_App     | 4096          | Idle + 1            | GATT setup, advertising loop  |
| BTstack     | 2048          | Idle + 2            | BTstack run loop (blocks)      |
| Timer svc   | 1024          | configMAX_PRIORITIES - 1 | Fires timer callbacks     |

> The BLE_App stack must be large enough for `std::cout` with complex operator<< chains
> (Service, Characteristic, AttributeServer). 4096 words is the minimum comfortable size.
> The Timer Service stack must be at least 1024 words when timer callbacks use floating-point
> `printf` (`%.2f` on Cortex-M33 drives significant stack depth through `dtoa`).

## Initialization Order in BLE_App Task

The order below is enforced by assertions inside `Ble` and `AttributeServer`.

```
1. OnChipTemperatureSensor::GetInstance()->Initialize()
2. FreeRtosTimer::Initialize(...)           // timer is created but not started
3. Ble::GetInstance(false, &btstack_port)   // registers port; ensures platform initialized
4. Ble::EnableSecurityManager(sm_params)    // must come before EnableAttributeServer
5. Ble::EnableAttributeServer(profile_data) // triggers BtstackPort Init + Apply
6. att_server->FindServiceByUuid(...)       // resolve handles after DB is parsed
7. characteristic->SetUserDescription(...)  // configure descriptors
8. BleOnchipTemperature::GetInstance(...)   // attaches event handlers
9. Ble::SetOnBleStackOnCallback(on_turn_on)
10. Ble::TurnOn()                           // powers controller; on_turn_on() fires on HCI_STATE_WORKING
11. app_timer.Start(100)                    // begin periodic temperature updates
```

`EnableSecurityManager` must precede `EnableAttributeServer` because the attribute server
checks whether a security manager is configured when it finds secured characteristics
(attributes flagged `READ_AUTHENTICATED`, `WRITE_ENCRYPTED`, etc.).

## Security Configuration

The example uses MITM protection with display-only IO capability, which triggers passkey
display pairing. The peer must enter the displayed 6-digit passkey to complete pairing.

```cpp
SecurityManager::SecurityParameters sm_params;
sm_params.authentication        = AuthenticationRequirement::kMitmProtection;
sm_params.io_capability         = IoCapability::kDisplayOnly;
sm_params.gatt_client_required_security_level = GattClientSecurityLevel::kLevel2;
//                                              ^ Authenticated + encrypted
```

`kLevel2` means the GATT server rejects reads/writes from an unauthenticated client.
BTstack enforces this at the ATT layer before any characteristic handler is invoked.

## Advertising Setup

Advertising is deferred to `on_turn_on()`, the callback fired when BTstack reaches
`HCI_STATE_WORKING`. This avoids races between the controller power-on sequence and the
advertising API.

```cpp
ble->SetRandomAddress(BleAddress(BleAddress::AddressType::kLeRandom, kStaticRandomAddress));
ble->SetAdvertisementFlags(Flags::kLeGeneralDiscoverableMode | Flags::kBrEdrNotSupported);
ble->SetDeviceName("RP2350_Custom_BLE");
```

The static random address must have its two most significant bits set (`0xC0` in the MSB),
enforced by a `static_assert` at the call site.

Advertising interval is 320–400 slots (200–250 ms), using `kAdvInd` (connectable,
undirected). The main loop updates manufacturer-specific advertising data with the current
tick count every 100 ms while advertising is active, using `AdvertisementDataBuilder::Pop()`
and `Push()` to replace the last element in-place.

## GATT Profile

The profile is defined in `app_profile.gatt` and compiled by `pico_btstack_make_gatt_header`
into `app_profile.h` (the `profile_data` blob). It contains:

| Service | UUID | Characteristics |
|---------|------|----------------|
| GAP | 0x1800 | Device Name (read) |
| GATT | 0x1801 | Database Hash (read) |
| Environmental Sensing | 0x181A | Configuration (custom 128-bit UUID, read/write, authenticated + encrypted) |
| Environmental Sensing | 0x181A | Temperature (0x2A6E, read/notify/indicate, authenticated + encrypted) |

Both characteristics under the Environmental Sensing service require authentication and
encryption; this is what forces Security Manager to be enabled and configured.

The configuration characteristic is located by **handle** (the generated constant
`ATT_CHARACTERISTIC_fc930f88_..._USER_DESCRIPTION_HANDLE`) rather than UUID, because the
128-bit UUID lookup is slower and the handle is stable within a fixed compiled profile.
The temperature characteristic is located by **UUID** via
`Service::FindCharacteristicByUuid(Uuid(0x2A6E))`.

## CMake Build Notes

The example target is guarded by a platform check:

```cmake
if(NOT "${C7222_PLATFORM}" STREQUAL "rp2350_custom_board")
    return()
endif()
```

Additional link dependencies beyond `C7222::development`:

- `hardware_uart` — UART peripheral driver
- `hardware_dma` — DMA-backed UART TX (required for `btstack_uart_block_embedded`)

Additional BTstack sources added directly (not part of `pico_btstack_ble`):

- `${PICO_BTSTACK_PATH}/src/hci_transport_h4.c`
- `${PICO_BTSTACK_PATH}/platform/embedded/btstack_uart_block_embedded.c`

These are the H4 framing layer and the embedded blocking UART driver. The Pico W path uses
a different transport (HCI over SPI via the CYW43 driver), so these files are not included
in the standard `pico_btstack_ble` target.

## stdio Thread Safety

The project uses `configUSE_NEWLIB_REENTRANT 1` and `configSUPPORT_PICO_SYNC_INTEROP 1`
(see `libs/elec_c7222/config/FreeRTOSConfig.h`). Together these make:

- **newlib `printf`/`scanf`** reentrant per task (each task has its own `_reent` struct).
- **Pico SDK `mutex_t`** (used internally by `pico_stdio`) FreeRTOS-aware: a task that
  cannot acquire the stdio mutex yields to the FreeRTOS scheduler rather than busy-spinning.

Despite these flags being set, `printf` calls that use floating-point formats (`%.2f`) in
the Timer Service Task still require adequate stack depth. Stack overflow in the timer task
corrupts the in-progress output buffer, producing truncated lines and garbage bytes.
Minimum recommended `configTIMER_TASK_STACK_DEPTH` when using float printf in any timer
callback is **1024 words**.
