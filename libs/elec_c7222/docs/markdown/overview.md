# ELEC-C7222 Library Overview

<div style="display:none">
\subpage md_libs_2elec__c7222_2devices_2doc_2markdown_2overview
\subpage md_libs_2elec__c7222_2ble_2doc_2markdown_2overview
\subpage md_libs_2elec__c7222_2utils_2doc_2markdown_2overview
\subpage md_libs_2elec__c7222_2docs_2markdown_2examples
</div>

The `libs/elec_c7222` library provides shared course infrastructure. It includes three major subsystems:

1. Devices: hardware abstraction for GPIO, LEDs, buttons, and board helpers.
2. BLE: a C++ wrapper around BTstack with GAP/GATT helpers and profile tooling.
3. Utils: common utilities (copy/move policy helpers, FreeRTOS timer wrapper, and platform hooks).

## Navigation

**Devices**

- Location: `libs/elec_c7222/devices`
- Overview: [Devices Overview](../../devices/doc/markdown/overview.md)
- Board Reference: [C7222 Board Reference](../../devices/doc/markdown/c7222_board.md)

**BLE**

- Location: `libs/elec_c7222/ble`
- Overview: [BLE Overview](../../ble/doc/markdown/overview.md)
- Guides:
  1. [Creating GATT Profiles](../../ble/doc/markdown/creating-profiles.md)
  2. [GAP Module](../../ble/doc/markdown/gap.md)
  3. [GATT/ATT Module](../../ble/doc/markdown/gatt.md)
  4. [Security Manager Module](../../ble/doc/markdown/security-manager.md)

**Utils**

- Location: `libs/elec_c7222/utils`
- Overview: [Utils Overview](../../utils/doc/markdown/overview.md)
