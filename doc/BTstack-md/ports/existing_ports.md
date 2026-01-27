#

## Existing ports

- [BTstack Port for Ambiq Apollo2 with EM9304](#sec:apollo2-em9304Port)
- [Archive of earlier ports](#sec:archivePort)
- [BTstack Port for the Espressif ESP32 Platform](#sec:esp32Port)
- [BTstack Port for FreeBSD Systems](#sec:freebsd-netgraphPort)
- [BTstack Port for POSIX Systems with libusb Library](#sec:libusbPort)
- [BTstack Port for POSIX Systems with Intel Wireless 8260/8265 Controllers](#sec:libusb-intelPort)
- [BTstack Port for Linux Systems](#sec:linuxPort)
- [BTstack Port for the Maxim MAX32630FTHR ARM Cortex-M4F](#sec:max32630-fthrPort)
- [BTstack Port for MSP432P401 Launchpad with CC256x](#sec:msp432p401lp-cc256xPort)
- [BTstack Port with Cinnamon for Nordic nRF5 Series ](#sec:nrf5-cinnamonPort)
- [BTstack Port for POSIX Systems with H4 Bluetooth Controller](#sec:posix-h4Port)
- [BTstack Port for POSIX Systems with modern Infineon (CYW) H4 Bluetooth Controller](#sec:posix-h4-airocPort)
- [BTstack Port for POSIX Systems with Atmel ATWILC3000 Controller](#sec:posix-h4-atwilc3000Port)
- [BTstack Port for POSIX Systems with Dialog Semiconductor DA14531 Controller](#sec:posix-h4-da14531Port)
- [BTstack Port for POSIX Systems with Dialog Semiconductor DA14581 Controller](#sec:posix-h4-da14581Port)
- [BTstack Port for POSIX Systems with Dialog Semiconductor DA14585 Controller](#sec:posix-h4-da14585Port)
- [BTstack Port for POSIX Systems with NXP/Marvel H4 Bluetooth Controller](#sec:posix-h4-nxpPort)
- [BTstack Port for POSIX Systems with Zephyr-based Controller](#sec:posix-h4-zephyrPort)
- [BTstack Port for QT with H4 Bluetooth Controller](#sec:qt-h4Port)
- [BTstack Port for QT with USB Bluetooth Dongle](#sec:qt-usbPort)
- [BTstack Port for Raspberry Pi 3 with BCM4343 Bluetooth/Wifi Controller](#sec:raspiPort)
- [BTstack Port for Renesas Eval Kit EK-RA6M4 with DA14531](#sec:renesas-ek-ra6m4a-da14531Port)
- [BTstack Port for Renesas Target Board TB-S1JA with CC256x](#sec:renesas-tb-s1ja-cc256xPort)
- [BTstack Port for Ezurio Vela IF820 Series](#sec:rp2040-vela-if820Port)
- [BTstack Port for SAMV71 Ultra Xplained with ATWILC3000 SHIELD](#sec:samv71-xplained-atwilc3000Port)
- [BTstack Port for STM32 F4 Discovery Board with CC256x](#sec:stm32-f4discovery-cc256xPort)
- [BTstack Port for STM32 F4 Discovery Board with USB Bluetooth Controller](#sec:stm32-f4discovery-usbPort)
- [BTstack Port for STM32 Nucleo L073RZ Board with EM9304 Controller](#sec:stm32-l073rz-nucleo-em9304Port)
- [BTstack Port with Cinnamon for Semtech SX1280 Controller on Miromico FMLR-80](#sec:stm32-l451-miromico-sx1280Port)
- [BTstack Port with Cinnamon for Semtech SX1280 Controller on STM32L476 Nucleo ](#sec:stm32-l476rg-nucleo-sx1280Port)
- [BTstack Port for STM32WB55 Nucleo Boards using FreeRTOS](#sec:stm32-wb55xx-nucleo-freertosPort)
- [BTstack on Web using WebSerial](#sec:web-h4Port)
- [BTstack Port for WICED platform](#sec:wiced-h4Port)
- [BTstack Port for Windows Systems with Bluetooth Controller connected via Serial Port](#sec:windows-h4Port)
- [BTstack Port for Windows Systems with DA14585 Controller connected via Serial Port](#sec:windows-h4-da14585Port)
- [BTstack Port for Windows Systems with Zephyr-based Controller](#sec:windows-h4-zephyrPort)
- [BTstack Port for Windows Systems using the WinUSB Driver](#sec:windows-winusbPort)
- [BTstack Port for Windows Systems with Intel Wireless 8260/8265 Controllers](#sec:windows-winusb-intelPort)
- [BTstack Port for Zephyr RTOS](#sec:zephyrPort)

## BTstack Port for Ambiq Apollo2 with EM9304 {#sec:apollo2-em9304Port}

This port uses the Ambiq Apollo2 EVB and the Ambiq EM9304 (AM BLE) shield.
HAL and BSP from Ambiq Suite 1.2.11 were used together with the regular ARM GCC toolchain.
Firmware upload is possible via the internal J-Link interface or the 10-pin Mini ARM-JTAG Interface.

### Hardware {#sec:apollo2-em9304Port-hardware}

Ambiq Apollo2 EVB + AM_BLE Shield 
- http://ambiqmicro.com/apollo-ultra-low-power-mcus/apollo2-mcu/

### Software {#sec:apollo2-em9304Port-software}

AmbiqSuite:
- http://ambiqmicro.com/apollo-ultra-low-power-mcus/apollo2-mcu/

Please clone BTstack as AmbiqSuite/third-party/bstack folder into the AmbiqSuite.

### Create Example Projects {#sec:apollo2-em9304Port-create-example-projects}

To create example GCC projects, go to the Apollo2-EM9304 folder

	$ cd port/apollo2-em9304

and run make

	$ ./create_examples.py

All examples are placed in the boards/apollo2_evb_am_ble/examples folder with btstack_ prefix.


### Compile & Run Example Project {#sec:apollo2-em9304Port-compile-run-example-project}

Go to to the gcc folder of one of the example folders and run make

    $ make

To upload, please follow the instructions in the Apollo Getting Started documents.

### Debug output {#sec:apollo2-em9304Port-debug-output}

printf is routed over the USB connector of the EVB at 115200.

In port/apollo2-em9304/btstack_config.h additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/btstack_port.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### TODO {#sec:apollo2-em9304Port-todo}
- BTstack's TLV persisten storage via Flash memory is not implemented yet.
- SPI Fullduplex: Newer Apollo 2 revisions supports SPI Full Duplex. The Ambiq Suite 1.2.11 does not cover Full Duplex with IRQ callback. It could be emulated by setting the Full Duplex mode and doing a regular write operation. When the write is complete, the received data can be read from the IOM FIFO.
- During MCU sleep without an ongoing SPI operation, the SPI could be fully disabled, which would reduce enrgey consumption.
## Archive of earlier ports {#sec:archivePort}

The ports in this folder are not recommended for new designs. This does not mean that the target hardware is not
suitable for new designs, just that the ports would need to reworked/refreshed.

List of ports with reason for move to this archive:

- MSP430 ports: The ports used the not-maintained version of the community MSP430 gcc port, which does not support more than 64 kB of FLASH RAM.
  A current port should use the official GCC version sponsored by TI. In addition, the MSP430 UART does not support hardware RTS/CTS.
  For a new port, our [ringbuffer approach](https://bluekitchen-gmbh.com/msp432p401r-cc2564c-port-uart-without-hardware-rtscts-flow-control/) should be used. Individual ports:
    - [EZ430-RF256x Bluetooth Evaluation Tool for MSP430](https://www.element14.com/community/docs/DOC-72027/l/ez430-rf256x-bluetooth-evaluation-too)
    - [MSP430F5438 Experimenter Board for MSP430](https://www.element14.com/community/docs/DOC-40373/l/msp430f5438-based-experimenter-board) with [Bluetooth CC2564 Module Evaluation Board](https://www.ti.com/tool/cc2564modnem)
    - [MSP-EXP430F5529LP LaunchPad](https://www.ti.com/ww/en/launchpad/launchpads-msp430-msp-exp430f5529lp.html#tabs) with [Bluetooth CC2564 Module Evaluation Board](https://www.ti.com/tool/cc2564modnem) and [EM Adapter BoosterPack](https://www.ti.com/tool/boost-ccemadapter) with additional 32768Hz quartz oscillator

- Ports for Broadcom/Cypress Controllers using HCI Transport H5: The PatchRAM cannot be uploaded via H5. This requires to upload the PatchRAM
  using H4 mode and then to start the stack using H5 transport. In addition, several bugs have been observed in H5 mode, e.g. LE Encrypt not wokring. Unless absolutely neccessary, it's better to use H4 mode.
    - Unix-based system connected to Broadcom/Cypress Bluetooth module via H5 over serial port
    - Broadcom platforms that support the WICED SDK via H5 UART, see wiced-h4

- Port for MicroChip PIC32 with Harmony Framework: the original port was for Harmony v1, while there's a Harmony V3 out since 2019:
    - [Microchip's PIC32 Bluetooth Audio Development Kit](https://www.microchip.com/Developmenttools/ProductDetails/DV320032)

- iOS port:
    - BTstack on iOS is not supported anymore.

## BTstack Port for the Espressif ESP32 Platform {#sec:esp32Port}

### Setup {#sec:esp32Port-setup}

- Follow [Espressif IoT Development Framework (ESP-IDF) setup](https://github.com/espressif/esp-idf) to install XTensa toolchain and the ESP-IDF. 
- Currently used for testing: ESP-IDF v5.4

### Usage {#sec:esp32Port-usage}

Please note: Since BTstack v1.7, BTstack sources are not and should not be copied into `${ESP_IDF}/components` folder. 
Please remove `${ESP_IDF}/components/btstack`

The current build system allows to build all provided examples for all supported ESP32 versions.
By default, the 'gatt_counter' example will be build for the original ESP32.

To select example 'gatt_streamer_server' and the ESP32-C3:

   EXAMPLE='gatt_streamer_server' idf.py set-target esp32c3

To get a list of valid targets:

   idf.py set-target

To get a list of BTstack examples:

    idf.py list-examples


To change the example:

    idf.py fullclean

To compile the example, run:

    idf.py build


To upload the binary to your device, run:

	idf.py -p PATH-TO-DEVICE flash


To get debug output, run:

	idf.py monitor

You can quit the monitor with CTRL-].


### Configuration

The configuration file 'btstack_config.h' is provided by the main project via the `btstack_config` component in
`components/btstack_config/btstack_config.h`

### Integration into custom projects

The esp32 port (this folder) contains the components 'btstack' and 'btstack_config'. 
It can be compiled out-of-tree by setting the environment variable BTSTACK_ROOT, example:

    BTSTACK_ROOT=/path/to/btstack EXAMPLE='gatt_streamer_server' idf.py build


### Limitations {#sec:esp32Port-limitations}

### Issues with the Bluetooth Controller Implementation

There are different issues in the Bluetooth Controller of the ESP32 that is provided in binary. We've submitted appropriate issues on the GitHub Issues page here: https://github.com/espressif/esp-idf/issues/created_by/mringwal

### Audio playback

Audio playback is implemented by `btstack_audio_esp32_v4.c` resp. `btstack_audio_esp32_v5.c` and supports generic I2S codecs as well as the ES8388 on the 
[ESP32 LyraT v4.3](https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/board-esp32-lyrat-v4.3.html) devkit.

Due to the various ESP32 variants, please double check the I2S Configuration in the mentioned audio file and verify that the correct menuconfig options are set.

For a generic ESP32, the first I2S interface is used with with the following pin out:

ESP32 pin | I2S Pin
----------|---------
GPIO0     | MCK
GPIO5     | BCLK
GPIO25    | LRCK
GPIO26    | DOUT
GPIO35    | DIN

If support for the LyraT v4.3 is enabled via menuconfig - Example Board Configuration --> ESP32 board --> ESP32-LyraT V4.3, CONFIG_ESP_LYRAT_V4_3_BOARD gets defined and the ES8388 will be configured as well.

We've also used the MAX98357A on the [Adafruit breakout board](https://www.adafruit.com/product/3006). 
The simplest audio example is the `mod_player`, which plays back an 8 kB sound file and the a2dp_sink_demo that implements a basic Bluetooth loudspeaker.
You can test the audio input with the `audio_duplex` example.

### ESP32 printf/log {#sec:esp32Port-esp32-printf-log}

The BTstack port setups a buffered output for printf/ESP log macros. However, if that gets full, the main thread will get blocked. If you're playing audio, e.g. a2dp_sink_demo, and a lot of text
is sent over the UART, this will cause audio glitches as the I2S DMA driver isn't re-filled on time. If this happens, you can increase the UART baudrate, reduce the debug output or increase the output
buffer size - or any combination of these. :)

### Multi-Threading

BTstack is not thread-safe, but you're using a multi-threading OS. Any function that is called from BTstack, e.g. packet handlers, can directly call into BTstack without issues. For other situations, you need to provide some general 'do BTstack tasks' function and trigger BTstack to execute it on its own thread.

To call a function from the BTstack thread, you can use *btstack_run_loop_execute_on_main_thread* allows to directly schedule a function callback, 
i.e. 'do BTstack tasks' function, from the BTstack thread. The called function then checks for pending BTstack tasks and executes them.
## BTstack Port for FreeBSD Systems {#sec:freebsd-netgraphPort}

### Overview {#sec:freebsd-netgraphPort-overview}
This port assumes that FreeBSD provides an ng_hci netgraph node for a connected Bluetooth Controller. 
In most cases, these are Bluetooth USB dongles or built-in Bluetooth Controller connected via USB.

For Bluetooth Controllers connected via UART, the POSX-H4 port might be a better option als 

### Implementation details {#sec:freebsd-netgraphPort-implementation-details}
In FreeBSD 13.2, the hci node is connected to a l2cap node and a btsock_hci_raw node. In order to take control, this 
port create a custom netgraph ng_socket node and connect to the 'acl' and 'raw' hooks of the hci node. The OS Bluetooth
functionality will be interrupted.

### Compilation {#sec:freebsd-netgraphPort-compilation}

BTstack's FeeeBSD port does not have additional dependencies. To compile the cmake project with Make

    mkdir build
    cd build
    cmake ..
    make

or using Ninja:

    mkdir build
    cd build
    cmake -G Ninja ..
    ninja

### Running the examples {#sec:freebsd-netgraphPort-running-the-examples}

As the port needs to reconfigure the Bluetooth netgraph node, it needs to run with root privileges.
It tries to connect to 'ubt0hci' by default. If your Bluetooth Controller is different, you can select it with '-u node'
On start, BTstack prints the path to the packet log and prints the information on the detected Buetooth Controller.

	$ sudo gatt_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	BTstack up and running on 00:1A:7D:DA:71:13.

### ToDO {#sec:freebsd-netgraphPort-todo}
- drop privileges after startup
- auto-detect ng_hci node
- support for profiles that require SCO: HFP & HSP
## BTstack Port for POSIX Systems with libusb Library {#sec:libusbPort}

### Compilation {#sec:libusbPort-compilation}
The quickest way to try BTstack is on a Linux or OS X system with an
additional USB Bluetooth dongle. It requires
[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/)
and [libusb-1.0](http://libusb.info) or higher to be
installed.

On a recent Debian-based system, all you need is:

	sudo apt-get install gcc git cmake ninja-build pkg-config libusb-1.0 portaudio19-dev


When everything is ready, you compile all examples with make:

	make

or using CMake + Ninja:

    mkdir build
    cd build
    cmake -G Ninja ..
    ninja

### Environment Setup {#sec:libusbPort-environment-setup}

### Linux

On Linux, the USB Bluetooth dongle is usually not accessible to a regular user.

You can add a udev rule for your dongle to extend access rights to user processes.

For this, create `/etc/udev/rules.d/btstack.rules` and add this

	# Match all devices from CSR
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a12", MODE="0666"

	# Match all devices from Realtek
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0bda", MODE="0666"

	# Match Cypress Semiconductor / Broadcom BCM20702A, e.g. DeLOCK Bluetooth 4.0 dongle
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a5c", ATTRS{idProduct}=="21e8", MODE="0666"

	# Match Asus BT400
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0b05", ATTRS{idProduct}=="17cb", MODE="0666"

	# Match Laird BT860 / Cypress Semiconductor CYW20704A2
	SUBSYSTEM=="usb", ATTRS{idVendor}=="04b4", ATTRS{idProduct}=="f901", MODE="0666"

### macOS

On macOS, the OS will try to use a plugged-in Bluetooth Controller if one is available. 
It's best to to tell the OS to always use the internal Bluetooth Contoller. 

For this, execute:

    sudo nvram bluetoothHostControllerSwitchBehavior=never

and then reboot to activate the change. 

Note: if you get this error,

	libusb: warning [darwin_open] USBDeviceOpen: another process has device opened for exclusive access
	libusb: error [darwin_reset_device] ResetDevice: device not opened for exclusive access

and you didn't start another instance and you didn't assign the USB Controller to a virtual machine,
macOS uses the plugged-in Bluetooth Controller. Please configure NVRAM as explained and try again after a reboot.

### Broadcom/Cypress/Infineon Controllers
During startup BTstack queries the Controlle for the Local Name, which is set to the Controller type (e.g. 'BCM20702A).
The chipset support uses this information to look for a local PatchRAM file of that name and uploads it.

### Realtek Controllers
During startup, the libusb HCI transport implementations reports the USB Vendor/Product ID, which is then forwarded to the Realtek chipset support.
The chipset support contains a mapping between USB Product ID and ( Patch, Configuration ) files. If found, these are
uploaded.


### Running the examples {#sec:libusbPort-running-the-examples}

BTstack's HCI USB transport will try to find a suitable Bluetooth module and use it. 

On start, BTstack will try to find a suitable Bluetooth module. It will also print the path to the packet log as well as the USB path.

	$ ./le_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	Packet Log: /tmp/hci_dump_6.pklg
	USB device 0x0a12/0x0001, path: 06
    Local version information:
    - HCI Version    0x0006
    - HCI Revision   0x22bb
    - LMP Version    0x0006
    - LMP Subversion 0x22bb
    - Manufacturer 0x000a
	BTstack up and running on 00:1A:7D:DA:71:01.

If you want to run multiple examples at the same time, it helps to fix the path to the used Bluetooth module by passing -u usb-path to the executable.

Example running le_streamer and le_streamer_client in two processes, using CSR Bluetooth dongles at USB path 6 and 4:

	./le_streamer -u 6
	Specified USB Path: 06
	Packet Log: /tmp/hci_dump_6.pklg
	USB device 0x0a12/0x0001, path: 06
    Local version information:
    - HCI Version    0x0006
    - HCI Revision   0x22bb
    - LMP Version    0x0006
    - LMP Subversion 0x22bb
    - Manufacturer 0x000a
	BTstack up and running on 00:1A:7D:DA:71:01.
	To start the streaming, please run the le_streamer_client example on other device, or use some GATT Explorer, e.g. LightBlue, BLExplr.

	$ ./le_streamer_client -u 4
	Specified USB Path: 04
	Packet Log: /tmp/hci_dump_4.pklg
	USB device 0x0a12/0x0001, path: 04
    Local version information:
    - HCI Version    0x0006
    - HCI Revision   0x22bb
    - LMP Version    0x0006
    - LMP Subversion 0x22bb
    - Manufacturer 0x000a
	BTstack up and running on 00:1A:7D:DA:71:02.
	Start scanning!
## BTstack Port for POSIX Systems with Intel Wireless 8260/8265 Controllers {#sec:libusb-intelPort}

Same as port/libusb, but customized for Intel Wireless 8260 and 8265 Controllers.
These controller require firmware upload and configuration to work. Firmware and config is downloaded from the Linux firmware repository.

### Compilation {#sec:libusb-intelPort-compilation}

Requirements:
- [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/)
- [libusb-1.0](http://libusb.info)

On a recent Debian-based system, all you need is:

	apt-get install gcc git libusb-1.0 pkg-config


When everything is ready, you compile all examples with:

	make

### Environment {#sec:libusb-intelPort-environment}

On Linux, the USB Bluetooth dongle is usually not accessible to a regular user. You can either:
- run the examples as root
- add a udev rule for your dongle to extend access rights to user processes

To add an udev rule, please create `/etc/udev/rules.d/btstack.rules` and add this

	# Match all devices from CSR
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a12", MODE="0666"

	# Match DeLOCK Bluetooth 4.0 dongle
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a5c", ATTRS{device}=="21e8", MODE="0666"

	# Match Asus BT400
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0b05", ATTRS{device}=="17cb", MODE="0666"

	# Match Laird BT860 / Cypress Semiconductor CYW20704A2
	SUBSYSTEM=="usb", ATTRS{idVendor}=="04b4", ATTRS{device}=="f901", MODE="0666"

	# Match Intel Wireless 8260 8265
	SUBSYSTEM=="usb", ATTRS{idVendor}=="8027", ATTRS{device}=="0a2b, MODE="0666"

On macOS, the OS will try to use a plugged-in Bluetooth Controller if one is available. 
It's best to to tell the OS to always use the internal Bluetooth Contoller. 

For this, execute:

    sudo nvram bluetoothHostControllerSwitchBehavior=never

and then reboot to activate the change. 

### Running the examples {#sec:libusb-intelPort-running-the-examples}

BTstack's HCI USB transport will try to find a suitable Bluetooth module and use it. 

On start, BTstack will try to find a suitable Bluetooth module. It will also print the path to the packet log as well as the USB path.

	$ ./le_counter
	Packet Log: /tmp/hci_dump.pklg
	USB Path: 03-01-04-03
	Firwmare ./ibt-12-16.sfi
	Firmware upload complete
	Firmware operational
	Done 1
	BTstack counter 0001
	USB Path: 03-01-04-03
BTstack up and running on F8:34:41:D5:BE:6F.

If you want to run multiple examples at the same time, it helps to fix the path to the used Bluetooth module by passing -u usb-path to the executable.

Example running le_streamer and le_streamer_client in two processes, using Bluetooth dongles at USB path 6 and 4:

	./le_streamer -u 6
	Specified USB Path: 06
	Packet Log: /tmp/hci_dump_6.pklg
	USB Path: 06
	BTstack up and running on 00:1A:7D:DA:71:13.
	To start the streaming, please run the le_streamer_client example on other device, or use some GATT Explorer, e.g. LightBlue, BLExplr.

	$ ./le_streamer_client -u 4
	Specified USB Path: 04
	Packet Log: /tmp/hci_dump_4.pklg
	USB Path: 04
	BTstack up and running on 00:1A:7D:DA:71:13.
	Start scanning!
## BTstack Port for Linux Systems {#sec:linuxPort}

While BTstack can directly work on Linux with most Bluetooth Controllers that are connected via UART (port/posix-h4) or USB (port/libusb), it might be convenient to use the Linux Bluetooth Subsystem in some cases, e.g. if the Bluetooth Controller uses other transports (such as SDIO) or the Controller is already fully configured by the distributions.


### Compilation {#sec:linuxPort-compilation}

In addition to regular C build tools, you also need the Bluetooth development package installed.

	sudo apt install libbluetooth-dev

Now you can compile it as usual with CMake

	mkdir build
	cd build
	cmake ..
	make


### Running the examples {#sec:linuxPort-running-the-examples}

Please make sure that BlueZ is not installed or at least disabled

	sudo systemctl stop bluetooth
	sudo systemctl disable bluetooth
	sudo systemctl mask bluetooth

Also make sure that the chosen device (here, hci0) is down

	sudo hciconfig hci0 down

To check

    hciconfig hci0

    hci0:	Type: Primary  Bus: USB
	        BD Address: 00:1A:7D:DA:71:13  ACL MTU: 1021:8  SCO MTU: 64:1
	        DOWN 
	        RX bytes:566359 acl:0 sco:0 events:40 errors:0
	        TX bytes:2174059 acl:2694 sco:0 commands:329 errors:0


To access the Bluetooth Controller, you can either run the examples as root, or, set the necessary permissions for a compiled example, e.g.

	sudo setcap 'cap_net_raw,cap_net_admin+eip' gatt_counter

Now, you can run this example as a regular user

	$ ./gatt_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	BTstack up and running on 00:1A:7D:DA:71:13.


### Status {#sec:linuxPort-status}

When running gatt_counter, a basic LE Peripheral with a GATT Service, the first two connections fail, while the third and later work as expected. There's no difference when looking at the HCI Trace with btmon, just that no ACL packets are received in the failing attempts.
## BTstack Port for the Maxim MAX32630FTHR ARM Cortex-M4F {#sec:max32630-fthrPort}

This port uses the [MAX32630FTHR ARM Cortex M4F Board](https://www.maximintegrated.com/en/products/microcontrollers/MAX32630FTHR.html) with the onboard TI CC2564B Bluetooth controller. It usually comes with the [DAPLINK Programming Adapter](https://developer.mbed.org/teams/MaximIntegrated/wiki/MAXREFDES100HDK). 
The DAPLINK allows to upload firmware via a virtual mass storage device (like mbed), provides a virtual COM port for a console, and enables debugging via the SWD interface via OpenOCD.

The port uses non-blocking polling UART communication with hardware flow control for Bluetooth controller. It was tested and achieved up to 1.8 Mbps bandwidth between two Max32630FTHR boards.

### Prerequisites {#sec:max32630-fthrPort-prerequisites}

### Maxim SDK

The Maxim has dropped the support for MAX32630 and is not recommended for new designs and it is also dropped from the latest SDK.

#### Maxim SDK Mirror

The required minimum set of files from the SDK are included in the port files [port/max32630-fthr/maxim](https://github.com/bluekitchen/btstack/tree/develop/port/max32630-fthr/maxim) for convenience and to support linux build.

>#### (For reference) ARM Cortex Toolchain - a.k.a. the old Maxim SDK
>The [ARM Cortex Toolchain](https://download.analog.com/sds/exclusive/SFW0001500A/ARMCortexToolchain.exe) is free software that provides peripheral libraries, linker files, initial code and some board files. It also provides Eclipse Neon and Maxim modified OpenOCD to program the microcontroller together with various examples for Maxim Cortex M4F ARM processors.

>For debugging, OpenOCD can be used. The regular OpenOCD does not support Maxim ARM microcontrollers yet, but a modified OpenOCD for use with Maxim devices can be found in the Maxim ARM Toolchain.

>Toolchain and Eclipse guide can be found in README.pdf file where the Maxim Toolchain is installed. Please note that this port was done using Makefiles.

### ARM Toolchain Setup

Download and extract arm toolchain. Record extract folder as `TOOLCHAIN_PATH`.

Windows msys:
[arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi.zip](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-mingw-w64-x86_64-arm-none-eabi.zip)

WSL Linux:
[arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz](https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz)

### Python - 3.12 or newer

Download and install python and make sure the alias `python` is accessible in the build terminal.

### MSYS2 setup - (windows only)
The [ARM Cortex Toolchain](https://download.analog.com/sds/exclusive/SFW0001500A/ARMCortexToolchain.exe) contains msys1.0, but it is complete (lacking unzip) and it is recommended that to use a more recent msys2.

Download and install msys2 - https://github.com/msys2/msys2-installer/releases/
- Install with option to Run msys2 after completion.
- Record install folder as `MSYS_PATH`
- Install required packages make and unzip.
  `pacman -S make unzip`
- close msys2

### Environment Variables

The `BTSTACK_ROOT`, `MAXIM_PATH`, and `PATH` must point to the previously setup tools as absolute paths.

The `BTSTACK_ROOT` is provided as relative path for examples in `port/max32630-fthr/example/project/Makefile`, but must be provided otherwise.

If not provided the `MAXIM_PATH` is set based on `BTSTACK_ROOT`.

Windows msys:

Run scripts/setenv.bat or set manually
```batch
## TOOLCHAIN_PATH=%EXTRACT_PATH% {#sec:max32630-fthrPort-toolchain-path-extract-path}
## MSYS_BIN=%MSYS_PATH%\usr\bin {#sec:max32630-fthrPort-msys-bin-msys-path-usr-bin}
set PATH=%TOOLCHAIN_PATH%/bin;%MSYS_BIN%;%PATH%
```

WSL Linux:
```bash
PATH="${TOOLCHAIN_PATH}/bin:${PATH}"
```

### Build {#sec:max32630-fthrPort-build}

The examples can be compiled using `make`. The build is implemented as a combination of several make and python scripts. For details see the scripts `port/max32630-fthr`, `port/max323630-fthr/scripts`, and `port/max32630-fthr/example/template` folders.

Running the make script `port/max32630-fthr/Makefile` will create Makefiles for the projects. And the projects are built running make scripts `port/max32630-fthr/example/Makefile` or `port/max32630-fthr/example/project/Makefile`.

```bash
cd %BTSTACK_ROOT%/port/max32630-fthr
.../port/max32630-fthr$ make
> Creating example projects:
> project - a2dp_sink_demo
> project - a2dp_source_demo
> project - ancs_client_demo
> ...
.../port/max32630-fthr$ cd example
## build a make target default(all), dual, general, classic, or ble {#sec:max32630-fthrPort-build-a-make-target-default-all-dual-general-classic-or-ble}
.../port/max32630-fthr/example$ make
## or build individual project e.g. gatt_counter {#sec:max32630-fthrPort-or-build-individual-project-e-g-gatt-counter}
.../port/max32630-fthr/example$ cd gatt_counter
.../port/max32630-fthr/example/gatt_counter$ make
```

In each example/project folder the make target is to build .elf file in the build folder which is convenient for debugging using Eclipse or GDB. 

For flashing via the virtual USB drive, the make also builds .bin file relative to the project ../bin/project.bin e.g. which is for the examples `example/bin`.

### Flashing Max32630 ARM Processor {#sec:max32630-fthrPort-flashing-max32630-arm-processor}

A firmware binary can be flashed either by copying the .bin file to the DAPLINK mass storage drive, or by using OpenOCD on the command line, or from Eclipse CDT.

The simplest way is drag and drop the generated .bin file to the DAPLINK mass storage drive. Once the file is copied to the mass storage device, the DAPLINK should program and then run the new firmware.

Alternatively, OpenOCD can be used to flash and debug the device. A suitable programming script can be found in the scripts folder.

### Debugging {#sec:max32630-fthrPort-debugging}

OpenOCD can also be used for developing and especially for debugging. Eclipse or GDB via OpenOCD could be used for step by step debugging.

### Debug output {#sec:max32630-fthrPort-debug-output}

printf messages are redirected to UART2. UART2 is accessible via the DAPLINK Programming Adapter as a virtual COM port at 115200 baud with no flow control. If this doesn't work for you, you can connect P3_1 (UART TX) of the MAX32630FTHR board to a USB-to-UART adapter.

Additional debug information can be enabled by uncommenting ENABLE_LOG_INFO in the src/btstack_config.h header file and a clean rebuild.

### TODOs {#sec:max32630-fthrPort-todos}
  - Support for BTSTACK_STDIN
  - Add flash-openocd to Makefile template
  - Add Eclipse CDT projects for max32630fthr
  - Implement hal_led.h to control LED on board
 
## BTstack Port for MSP432P401 Launchpad with CC256x {#sec:msp432p401lp-cc256xPort}

This port is for the the TI MSP432P401R Launchpad with TI's CC256x Bluetooth Controller using TI's DriverLib (without RTOS). 
For easy development, Ozone project files are generated as well.

As the MSP432P401 does not have support for hardware RTS/CTS, this port makes use of Ping Pong DMA transfer mode
(similar to circular DMA on other MCUs) to use two adjacent receive buffers and raise RTS until a completed buffer is processed. 

### Hardware {#sec:msp432p401lp-cc256xPort-hardware}

[TI MSP432P401R LaunchPad](https://www.ti.com/tool/MSP-EXP432P401R)

As Bluetooth Controller, there are two BoosterPacks that can be use:
1. [BOOST-CC2564MODA CC2564B BoosterPack](https://www.ti.com/tool/BOOST-CC2564MODA) (USD 20)
2. [Evaluation Module (EM) Adapto](https://www.ti.com/tool/TIDM-LPBP-EMADAPTER) (USD 20) with one of the CC256x modules:
    - [CC2564B Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/cc2564modnem.aspx) (USD 20)
    - [CC2564C Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/CC256XCQFN-EM-CC2564C-Dual-Mode-Bluetooth-Controller-Evaluation-Module-P51277.aspx) (USD 60)

The CC2564B Booster pack is around USD 20 while thhe EM Adapter with the CC2564C module is around USD 80.

The project in the BTstack repo `port/msp432p401lp-cc256x' is configured for the EM Adapter + newer CC2564C module.

When using the CC2564B (either as BOOST-CC2564MODA or CC2564B Dual-mode Bluetooth(R) Controller Evaluation Module), the *bluetooth_init_cc2564B_1.8_BT_Spec_4.1.c* must be used as cc256x_init_script. See Makefile variable INIT_SCRIPT.

When using the CC2564B Booster Pack, please use uncomment the defines for the GPIO definition (search for `BOOST-CC2564MODA`)

When using the EM Adapter Booster Pack, please make sure to solder a 32.768 kHz quarz oscillator as explained in 4.7 of the [EM Wireless Booster Pack User Guide](http://www.ti.com/lit/ug/swru338a/swru338a.pdf). If you don't have an oscillator of that size, you might solder one upside done (turtle-on-back style) to the unused upper right pad and wire GCC, VCC, and clock with thin wires.


### Software {#sec:msp432p401lp-cc256xPort-software}

To build all examples, you need the regular ARM GCC toolcahin installed. Run make

	$ make

All examples and the .jdebug Ozone project files are placed in the 'gcc' folder.


### Flash And Run The Examples {#sec:msp432p401lp-cc256xPort-flash-and-run-the-examples}

The Makefile builds different versions: 
- example.elf: .elf file with all debug information
- example.bin: .bin file that can be used for flashing

There are different options to flash and debug the MSP432P401R LaunchPad. If all but the jumpers for power (the left three) are removed on J101, an external JTAG like SEGGER's J-Link can be connected via J8 'MSP432 IN'.

### Run Example Project using Ozone {#sec:msp432p401lp-cc256xPort-run-example-project-using-ozone}

When using an external J-Link programmer, you can flash and debug using the cross-platform [SEGGER Ozone Debugger](https://www.segger.com/products/development-tools/ozone-j-link-debugger/). It is included in some J-Link programmers or can be used for free for evaluation usage.

Just start Ozone and open the .jdebug file in the build folder. When compiled with `ENABLE_SEGGER_RTT`, the debug output shows up in the Terminal window of Ozone. 


### Debug output {#sec:msp432p401lp-cc256xPort-debug-output}

All debug output is send via SEGGER RTT or via USART2. To get the console from USART2, remove `ENABLE_SEGGER_RTT` from btstack_config.h and open a terminal to the virtual serial port of the Launchpad at 115200.

In btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be disabled/enabled via ENABLE_LOG_INFO.

Also, the full packet log can be enabled in main.c  by uncommenting the hci_dump_init(..) line. The output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py


### GATT Database {#sec:msp432p401lp-cc256xPort-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. The Makefile contains rules to update the .h file when the .gatt was modified.
## BTstack Port with Cinnamon for Nordic nRF5 Series  {#sec:nrf5-cinnamonPort}

*Cinnamon* is BlueKitchen's minimal, yet robust Controller/Link Layer implementation for use with BTstack.

In contrast to common Link Layer implementations, our focus is on a robust and compact implementation for production use,
where code size matters (e.g. current code size about 8 kB).

### Status {#sec:nrf5-cinnamonPort-status}
The current implementation supports a single Peripheral role, or, passive scanning in Observer role. In the Peripheral role,
channel map updates, as well as connection param updates are supported.

Support for LE Central Role as well as Encryption is planned but not supported yet. 

### Requirements {#sec:nrf5-cinnamonPort-requirements}
- arm-none-eabi toolchain
- Nordic's nRF5-SDK

### Supported Hardware {#sec:nrf5-cinnamonPort-supported-hardware}
All nNRF5x SOCs. Built files are provided for PCA10040 (52832 DK), but others can be supported with minimal changes.

### Use {#sec:nrf5-cinnamonPort-use}
- Provide path to nRF5-SDK either in `NRF5_SDK_ROOT` environment variable or directly in `pca10040/armgcc/Makefile`.
- run make
- All supported examples are built in the `build` folder.
- You can use Segger's OZONE with the provided `EXAMPLE.jdebug` project file to flash and run the examples.
## BTstack Port for POSIX Systems with H4 Bluetooth Controller {#sec:posix-h4Port}

### Configuration {#sec:posix-h4Port-configuration}
Most Bluetooth Bluetooth Controllers connected via UART/H4 require some special configuration, e.g. to set the UART baud rate, and/or require firmware patches during startup. In this port, we've tried to do most of these automatically based on information gathered from the Bluetooth Controller. Here's some Controller specific details:

### TI CC256x {#sec:posix-h4Port-ti-cc256x}
The CC2564x needs the correct init script to start up. The Makfile already has entries for most silicon revisions:

- CC2560:  bluetooth_init_cc2564_2.14.c
- CC2564B: bluetooth_init_cc2564B_1.8_BT_Spec_4.1.c
- CC2564C: bluetooth_init_cc2564C_1.5.c

Please pick the correct one. The main.c verifies that the correct script is loaded, but the init script is linked to the executable.

### Broadcom BCM/CYW 43430 {#sec:posix-h4Port-broadcom-bcm-cyw-43430}

The correct firmware file needs to be provided in the current working directory. The Makefile downloads the one for the BCM43430 e.g. found on later Raspberry Pi editions. Please see the separate port/raspi, too.


### Compilation {#sec:posix-h4Port-compilation}

BTstack's POSIX-H4 does not have additional dependencies. You can directly run make.

	make


### Running the examples {#sec:posix-h4Port-running-the-examples}

On start, BTstack prints the path to the packet log and prints the information on the detected Buetooth Controller.

	$ ./le_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	BTstack up and running on 00:1A:7D:DA:71:13.

Please note that BTstack will increase the baudrate. Before starting again, you should reset or power-cycle the Bluetooth Controller.
## BTstack Port for POSIX Systems with modern Infineon (CYW) H4 Bluetooth Controller {#sec:posix-h4-airocPort}

### Configuration {#sec:posix-h4-airocPort-configuration}
Newer Infineon Airoc (tm) Controllers like the CYW5557x series accept PatchRAM upload only in a so-called
'auto-baud mode' which is entered by asserting CTS (low) and starting/resetting the controller via BT_REG_EN.
This port currently only supports the CYW5557x Controllers.

### Compilation {#sec:posix-h4-airocPort-compilation}

BTstack's posix-h4-bcm port does not have additional dependencies. You can directly run cmake and then your default
build system. E.g. with Ninja:

	mkdir build
    cd build
    cmake -G Ninja ..
    ninja
 
### Running the examples {#sec:posix-h4-airocPort-running-the-examples}

On start, BTstack opens the serial port, which asserts CTS, and requests you to reset the Controller with a countdown.

	$ ./gatt_counter
    Packet Log: /tmp/hci_dump.pklg
    Phase 1: Download firmware
    Please reset Bluetooth Controller, e.g. via RESET button. Firmware download starts in:
    3
    2
    1
    Firmware download started
    Local version information:
    - HCI Version    0x000a
    - HCI Revision   0x0b73
    - LMP Version    0x000a
    - LMP Subversion 0x2257
    - Manufacturer 0x0009
    Phase 2: Main app
    BTstack up and running at 55:56:0A:0A:76:93
    ...

### ToDo {#sec:posix-h4-airocPort-todo}
- select PatchRAM file based on HCI Read Local Version Information
## BTstack Port for POSIX Systems with Atmel ATWILC3000 Controller {#sec:posix-h4-atwilc3000Port}
This port allows to use the ATWILC3000 connected via UART with BTstack running on a POSIX host system, see test setup below (which lacks a proper RESET button).

### Compilation {#sec:posix-h4-atwilc3000Port-compilation}

$ make

The Makefile downloads the wilc3000_bt_firmware.bin firmware from the [GitHub atwilc3000/firmware](https://github.com/atwilc3000/firmware) repo.

### Usage {#sec:posix-h4-atwilc3000Port-usage}

Just run any of the provided examples, e.g.

$ ./le_counter

At start, the firmware file is first uploaded to the ATWILC3000, before BTstack start up.

Please note that it configures the ATWILC3000 for a higher baud rate it does not detect if the firmware has already been downloaded, so you need to reset the ATWILC3000 before starting an example.

Tested with the official [ATWILC3000 SHIELD](http://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATWILC3000-SHLD) on OS X.

![ATWILC3000 Test Setup](ATWILC3000-Breakout.jpg)
## BTstack Port for POSIX Systems with Dialog Semiconductor DA14531 Controller {#sec:posix-h4-da14531Port}

This port allows to use the DA14531 connected via UART with BTstack running on a POSIX host system.

Instead of storing the HCI firmware in the OTP, it first downloads the hci_531_active_uart_460800.hex firmware 
from the 6.0.16.1144 SDK, before BTstack starts up.

After Power Cycle, please start one of the test applications and press the Reset button to trigger firmware download.

Please note that it does not detect if the firmware has already been downloaded, so you need to Power Cycle
the DA14531 before starting an example again. 

Alternatively, after running one of the examples once to upload the firmware, you can use the regular 'posix-h4' port
and change the initial UART baud rate to 460800 as long as you don't power cycle the dev kit.

For production use, the DA14531 could be power cycled from the main CPU during startup, e.g. after the call
to btstack_chipset_da145xx_download_firmware_with_uart, or, the HCI firmware could be burned into the OTP.

## Software Setup / Firmware {#sec:posix-h4-da14531Port-software-setup-firmware}

On the [DA14531 USB Development Kit](https://www.dialog-semiconductor.com/products/bluetooth-low-energy/da14531-development-kit-usb),
the UART is configured via DIP switched. By this, the mapping to the DA14531 GPIOs is fixed. In SDK 6.0.6.1144, the
GPIO mapping of RTS and CTS is flipped. In order to be able to us the same HCI firmware on both dev kits, we've 
used the following configuration in `user_perip_setup.h`

```C
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_0

    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_1

    #define UART1_RTSN_PORT         GPIO_PORT_0
    #define UART1_RTSN_PIN          GPIO_PIN_4

    #define UART1_CTSN_PORT         GPIO_PORT_0
    #define UART1_CTSN_PIN          GPIO_PIN_3
```

We also increased the UART baudrate to 460800

```C
#define UART1_BAUDRATE              UART_BAUDRATE_460800
```

We also disabled the SLEEP mode in user_config.h:

```C
static const sleep_state_t app_default_sleep_mode = ARCH_SLEEP_OFF;
```

After compilation with Keil uVision 5, the generated .hex file is copied into btstack/chipset/da145xx as
`hci_531_active_uart_460800.hex`, and then
`convert_hex_files" is used to convert it into a C data array.


## Hardware Setup - Dev Kit Pro {#sec:posix-h4-da14531Port-hardware-setup-dev-kit-pro}

To use the [DA14531 Dev Kit Pro](https://www.dialog-semiconductor.com/products/bluetooth-low-energy/da14530-and-da14531-development-kit-pro)
with BTstack, please make the following modifications:
- Follow Chapter 4.1 and Figure 4 in the [DA14531 Development Kit Pro Hardware User Manual
  UM-B-114](https://www.dialog-semiconductor.com/sites/default/files/2021-06/UM-B-114_DA14531_Devkit_Pro_Hardware_User%20manual_1v5.pdf)
  and set SW1 of the 14531 daughter board into position "BUCK" position marked with an "H" on the left side.
- configure the dev kit for Full UART (4-wire) Configuration by adding jumper wires between J1 and J2

## Hardware Setup - Dev Kit USB {#sec:posix-h4-da14531Port-hardware-setup-dev-kit-usb}

To use the [Dev Kit USB](https://www.dialog-semiconductor.com/products/bluetooth-low-energy/da14531-development-kit-usb#tab-field_tab_content_overview)
with BTstack, please make the following modifications:
- Follow Chapter 5.6 in the [DA14531 USB Development Kit Hardware UM-B-125](https://www.dialog-semiconductor.com/sites/default/files/um-b-125_da14531_usb_development_kit_hw_manual_1v1.pdf)
  and set the DIP switches as described.
 
 # Example Run

```
$ ./gatt_counter
Packet Log: /tmp/hci_dump.pklg
Phase 1: Download firmware
Phase 2: Main app
BTstack counter 0001
BTstack up and running on 80:EA:CA:70:00:08.
```
## BTstack Port for POSIX Systems with Dialog Semiconductor DA14581 Controller {#sec:posix-h4-da14581Port}

This port allows to use the DA14581 connected via UART with BTstack running on a POSIX host system.

It first downloads the hci_581_active_uart.hex firmware from the  DA14581_HCI_3.110.2.12 SDK packet, before BTstack starts up.

Please note that it does not detect if the firmware has already been downloaded, so you need to reset the DA14581 before starting an example.

For production use, the HCI firmware could be flashed into the OTP and the firmware download could be skipped.

Tested with the official DA14581 Dev Kit on OS X.
## BTstack Port for POSIX Systems with Dialog Semiconductor DA14585 Controller {#sec:posix-h4-da14585Port}

This port allows to use the DA14585 connected via UART with BTstack running on a POSIX host system.

It first downloads the hci_581.hex firmware from the 6.0.8.509 SDK, before BTstack starts up.

Please note that it does not detect if the firmware has already been downloaded, so you need to reset the DA14585 before starting an example.

For production use, the HCI firmware could be flashed into the OTP and the firmware download could be skipped.

Tested with the official DA14585 Dev Kit Basic on OS X.
## BTstack Port for POSIX Systems with NXP/Marvel H4 Bluetooth Controller {#sec:posix-h4-nxpPort}

### Configuration {#sec:posix-h4-nxpPort-configuration}
Most Bluetooth Bluetooth Controllers connected via UART/H4 require some special configuration, e.g. to set the UART baud rate, and/or require firmware patches during startup.
In this port, we've show how a NXP/Marvell Controller can be configured for use with BTstack. It's unclear if the required firmware file for older Controllers,
e.g. 88W8997, can be detected during the firmware upload. This port selects the firmware for NXP 88W8997. 
For newer Controllers, e.g. IW416 or IW612, the firmware can be selected automatically.

### Compilation {#sec:posix-h4-nxpPort-compilation}

BTstack's posix-h4-nxp port does not have additional dependencies. You can directly run cmake and then your default build system. E.g. with Ninja:

	mkdir build
    cd build
    cmake -G Ninja ..
    ninja
 
### Running the examples {#sec:posix-h4-nxpPort-running-the-examples}

Please reset the Controller first. On start, BTstack prints the path to the packet log.

	$ ./gatt_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	BTstack up and running on 00:1A:7D:DA:71:13.

### Issues {#sec:posix-h4-nxpPort-issues}
- NXP 88W8997 does not support SCO Flow Control which causes glitches when sending audio

### ToDo {#sec:posix-h4-nxpPort-todo}
- increase baud rate for firmware upload
- skip firmware upload if firmware already present
- increase baud rate for application
## BTstack Port for POSIX Systems with Zephyr-based Controller {#sec:posix-h4-zephyrPort}

The main difference to the regular posix-h4 port is that that the Zephyr Controller uses 1000000 as baud rate.
In addition, the port defaults to use the fixed static address stored during production.

### Prepare Zephyr Controller {#sec:posix-h4-zephyrPort-prepare-zephyr-controller}

Please follow [this](https://devzone.nordicsemi.com/blogs/1059/nrf5x-support-within-the-zephyr-project-rtos/) blog post about how to compile and flash `samples/bluetooth/hci_uart` to a connected nRF5 dev kit.

In short: you need to install an arm-none-eabi gcc toolchain and the nRF5x Command Line Tools incl. the J-Link drivers, checkout the Zephyr project, and flash an example project onto the chipset:

  * Install [J-Link Software and documentation pack](https://www.segger.com/jlink-software.html).
  * Get nrfjprog as part of the [nRFx-Command-Line-Tools](http://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF52-DK). Click on Downloads tab on the top and look for your OS.
  * [Checkout Zephyr and install toolchain](https://www.zephyrproject.org/doc/getting_started/getting_started.html). We recommend using the [arm-non-eabi gcc binaries](https://launchpad.net/gcc-arm-embedded) instead of compiling it yourself. At least on OS X, this failed for us.

  * In *samples/bluetooth/hci_uart*, compile the firmware for nRF52 Dev Kit

      $ make BOARD=nrf52_pca10040

   * Upload the firmware

      $ make flash

   * For the nRF51 Dev Kit, use `make BOARD=nrf51_pca10028`.

### Configure serial port {#sec:posix-h4-zephyrPort-configure-serial-port}

To set the serial port of your Zephyr Controller, you can either update config.device_name in main.c or
always start the examples with the `-u /path/to/serialport` option.

### Compile Examples {#sec:posix-h4-zephyrPort-compile-examples}

    $ make

### Run example {#sec:posix-h4-zephyrPort-run-example}

Just run any of the created binaries, e.g.

    $ ./le_counter

The packet log will be written to /tmp/hci_dump.pklg

## BTstack Port for QT with H4 Bluetooth Controller {#sec:qt-h4Port}

Windows is supported with the MinGW Kit.

Windows with MSVC or Embedded (bare metal) platforms not supported yet.


### Configuration {#sec:qt-h4Port-configuration}

Most Bluetooth Bluetooth Controllers connected via UART/H4 require some special configuration, e.g. to set the UART baud rate, and/or require firmware patches during startup. In this port, we've tried to do most of these automatically based on information gathered from the Bluetooth Controller. Here's some Controller specific details:

### TI CC256x {#sec:qt-h4Port-ti-cc256x}
The CC2564x needs the correct init script to start up. The Makfile already has entries for most silicon revisions:

- CC2560:  bluetooth_init_cc2564_2.14.c
- CC2564B: bluetooth_init_cc2564B_1.8_BT_Spec_4.1.c
- CC2564C: bluetooth_init_cc2564C_1.5.c

Please pick the correct one. The main.c verifies that the correct script is loaded, but the init script is linked to the executable.

### Broadcom BCM/CYW 43430 {#sec:qt-h4Port-broadcom-bcm-cyw-43430}

The correct firmware file needs to be provided in the current working directory. The Makefile downloads the one for the BCM43430 e.g. found on later Raspberry Pi editions. Please see the separate port/raspi, too.

### Compilation {#sec:qt-h4Port-compilation}

On all platforms, you'll need Qt Python 3 installed.
On macOS/Linux [libusb-1.0](http://libusb.info) or higher is required, too.

When everything is ready, you can open the provided CMakelists.txt project in Qt Creator and run any of the provided examples.
See Qt documentation on how to compile on the command line or with other IDEs

You can also compile the project in a shell if you provide the path to the cmake folder of your Qt installation, e.g. on macOS with brew providing QT 5.15.16
	
	cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/Cellar/qt@5/5.15.16/lib/cmake/Qt5 ..


### Running the examples {#sec:qt-h4Port-running-the-examples}

BTstack's HCI USB transport will try to find a suitable Bluetooth module and use it. 

On start, BTstack will try to find a suitable Bluetooth module. It will also print the path to the packet log as well as the USB path.

	$ ./gattcounter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	USB Path: 06
	BTstack up and running on 00:1A:7D:DA:71:13.

## BTstack Port for QT with USB Bluetooth Dongle {#sec:qt-usbPort}

Uses libusb Library on macOS and Linux and WinUSB on Windows.
Windows is supported with the MinGW Kit.

Windows with MSVC or Embedded (bare metal) platforms not supported yet.

### Compilation {#sec:qt-usbPort-compilation}

On all platforms, you'll need Qt Python 3 installed.
On macOS/Linux [libusb-1.0](http://libusb.info) or higher is required, too.

When everything is ready, you can open the provided CMakelists.txt project in Qt Creator and run any of the provided examples.
See Qt documentation on how to compile on the command line or with other IDEs

You can also compile the project in a shell if you provide the path to the cmake folder of your Qt installation, e.g. on macOS with brew providing QT 5.15.16
	
	cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/Cellar/qt@5/5.15.16/lib/cmake/Qt5 ..

### Environment Setup {#sec:qt-usbPort-environment-setup}

### Windows {#sec:qt-usbPort-windows}

To allow WinUSB to access an USB Bluetooth dongle, you need to install a special device driver to make it accessible to user space processes.

It works like this:

-  Download [Zadig](http://zadig.akeo.ie)
-  Start Zadig
-  Select Options -> "List all devices"
-  Select USB Bluetooth dongle in the big pull down list
-  Select WinUSB in the right pull down list
-  Select "Replace Driver"

### Linux

On Linux, the USB Bluetooth dongle is usually not accessible to a regular user. You can either:
- run the examples as root
- add a udev rule for your dongle to extend access rights to user processes

To add an udev rule, please create `/etc/udev/rules.d/btstack.rules` and add this

	# Match all devices from CSR
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a12", MODE="0666"

	# Match DeLOCK Bluetooth 4.0 dongle
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0a5c", ATTRS{device}=="21e8", MODE="0666"

	# Match Asus BT400
	SUBSYSTEM=="usb", ATTRS{idVendor}=="0b05", ATTRS{device}=="17cb", MODE="0666"

	# Match Laird BT860 / Cypress Semiconductor CYW20704A2
	SUBSYSTEM=="usb", ATTRS{idVendor}=="04b4", ATTRS{device}=="f901", MODE="0666"

### macOS

On macOS, the OS will try to use a plugged-in Bluetooth Controller if one is available. 
It's best to to tell the OS to always use the internal Bluetooth Contoller. 

For this, execute:

    sudo nvram bluetoothHostControllerSwitchBehavior=never

and then reboot to activate the change. 

Note: if you get this error,

	libusb: warning [darwin_open] USBDeviceOpen: another process has device opened for exclusive access
	libusb: error [darwin_reset_device] ResetDevice: device not opened for exclusive access

and you didn't start another instance and you didn't assign the USB Controller to a virtual machine,
macOS uses the plugged-in Bluetooth Controller. Please configure NVRAM as explained and try again after a reboot.


### Running the examples {#sec:qt-usbPort-running-the-examples}

BTstack's HCI USB transport will try to find a suitable Bluetooth module and use it. 

On start, BTstack will try to find a suitable Bluetooth module. It will also print the path to the packet log as well as the USB path.

	$ ./gatt_counter
	Packet Log: /tmp/hci_dump.pklg
	BTstack counter 0001
	USB Path: 06
	BTstack up and running on 00:1A:7D:DA:71:13.

If you want to run multiple examples at the same time, it helps to fix the path to the used Bluetooth module by passing -u usb-path to the executable.

Example running le_streamer and le_streamer_client in two processes, using Bluetooth dongles at USB path 6 and 4:

	./gatt_streamer_server -u 6
	Specified USB Path: 06
	Packet Log: /tmp/hci_dump_6.pklg
	USB Path: 06
	BTstack up and running on 00:1A:7D:DA:71:13.
	To start the streaming, please run the le_streamer_client example on other device, or use some GATT Explorer, e.g. LightBlue, BLExplr.

	$ ./le_streamer_client -u 4
	Specified USB Path: 04
	Packet Log: /tmp/hci_dump_4.pklg
	USB Path: 04
	BTstack up and running on 00:1A:7D:DA:71:13.
	Start scanning!
## BTstack Port for Raspberry Pi 3 with BCM4343 Bluetooth/Wifi Controller {#sec:raspiPort}

Tested with Raspberry Pi 3 Model B V1.2, Raspberry Pi 3 Model B+, and Raspberry Pi Zero W V1.1.

With minor fixes, the port should also work with older Raspberry Pi models that use the [RedBear pHAT](https://redbear.cc/product/rpi/iot-phat.html). See TODO at the end.

### Raspberry Pi 3 / Zero W Setup {#sec:raspiPort-raspberry-pi-3-zero-w-setup}

There are various options for setting up the Raspberry Pi, have a look at the Internet. Here's what we did:

### Install Raspian Stretch Lite:

- Insert empty SD Card
- Download Raspian Stretch Lite from https://www.raspberrypi.org/downloads/raspbian/
- Install Etcher from https://etcher.io
- Run Etcher:
  - Select the image you've download before
  - Select your SD card
  - Flash!

### Configure Wifi

Create the file wpa_supplicant.conf in the root folder of the SD Card with your Wifi credentials:

	ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
	network={
	    ssid="YOUR_NETWORK_NAME"
	    psk="YOUR_PASSWORD"
	    key_mgmt=WPA-PSK
	}

Alternatively, just plug-it in via Ethernet - unless you have a Raspberry Pi Zero W.

### Enable SSH

Create an empty file called 'ssh' in the root folder of the SD Card to enable SSH.

### Boot

If everything was setup correctly, it should now boot up and join your Wifi network. You can reach it via mDSN as 'raspberrypi.local' and log in with user: pi, password: raspberry.

### Disable bluez

By default, bluez will start up using the the BCM4343. To make it available to BTstack, you can disable its system services:

	$ sudo systemctl disable hciuart
    $ sudo systemctl disable bthelper
	$ sudo systemctl disable bluetooth

and if you don't want to restart, you can stop them right away. Otherwise, please reboot here.

	$ sudo systemctl stop hciuart
    $ sudo systemctl stop bthelper
	$ sudo systemctl stop bluetooth

If needed, they can be re-enabled later as well.

### Compilation {#sec:raspiPort-compilation}

The Makefile assumes cross-compilation using the regular GCC Cross Toolchain for gnueabihf: arm-linux-gnueabihf-gcc. This should be available in your package manager. Read on for a heavy, but easy-to-use approach.

### Compile using Docker

For non-Linux users, we recommend to use a [Raspberry Pi Cross-Compiler in a Docker Container](https://github.com/sdt/docker-raspberry-pi-cross-compiler).
Please follow the installation instructions in the README. 

Then, setup a shared folder in Docker that contains the BTstack repository.
Now, go to the BTstack repository and 'switch' to the Raspberry Pi Cross-Compiler container:

	$ rpxc bash

The default images doesn't have a Python installation, so we manually install it:

	$ sudo apt-get install python

Change to the port/raspi folder inside the BTstack repo:

	$ cd btstack/port/raspi

and compile as usual:

	$ make

For regular use, it makes sense to add Python permanently to the Docker container. See documentation at GitHub.

### Running the examples {#sec:raspiPort-running-the-examples}

Copy one of the examples to the Rasperry Pi and just run them. BTstack will power cycle the Bluetooth Controller on models without hardware flowcontrol, i.e., Pi 3 A/B. With flowcontrol, e.g Pi Zero W and Pi 3 A+/B+, the firmware will only uploaded on first start. 

	pi@raspberrypi:~ $ ./le_counter
	Packet Log: /tmp/hci_dump.pklg
	Hardware UART without flowcontrol
	Phase 1: Download firmware
	Phase 2: Main app
	BTstack counter 0001
	BTstack up and running at B8:27:EB:27:AF:56

### Bluetooth Hardware Overview {#sec:raspiPort-bluetooth-hardware-overview}

Model                    | Bluetooth Controller                                    | UART Type | UART Flowcontrol | BT_REG_EN   | HCI Transport | Baudrate
-------------------------|---------------------------------------------------------|-----------|------------------|-------------|---------------|----------
Older                    | None                                                    |           |                  |             |               |
Pi 3 Model A, Model B    | [CYW43438](http://www.cypress.com/file/298076/download) | Hardware  | No               | 128         | H5            | 921600
Pi 3 Model A+, Model B + | [CYW43455](http://www.cypress.com/file/358916/download) | Hardware  | Yes              | 129 (See 1) | H4            | 921600 (See 2)
Pi Zero W                | [CYW43438](http://www.cypress.com/file/298076/download) | Hardware  | Yes              | 45          | H4            | 921600

1. Model A+/B+ have BT_REG_EN AND WL_REG_EN on the same (virtual) GPIO 129. A Bluetooth Controller power cycle also shuts down Wifi (temporarily). BTstack avoids a power cycle on A+/B+.
2. Model A+/B+ support 3 mbps baudrate. Not enabled/activated yet.

### TODO {#sec:raspiPort-todo}
- Raspberry Pi Zero W: Check if higher baud rate can be used, 3 mbps does not work.
- Raspberry + RedBear IoT pHAT (AP6212A = BCM4343) port: IoT pHAT need to get detected and the UART configured appropriately.
## BTstack Port for Renesas Eval Kit EK-RA6M4 with DA14531 {#sec:renesas-ek-ra6m4a-da14531Port}

This port uses the [Renesas EK-RA6M4](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra6m4-evaluation-kit-ra6m4-mcu-group) and a Renesas DA14531 Controller on the [MikroeE BLE Tiny Click board](https://www.mikroe.com/ble-tiny-click)

Renesas e2 Studio (Eclise-based) was used with the FSP HAL and without an RTOS to generate project sources.
Then, a new CMake buildfile was created to allow for cross-platform development and compilation of all examples.
For easy debugging, Ozone project files are generated as well.

### Hardware {#sec:renesas-ek-ra6m4a-da14531Port-hardware}

### Renesas Eval Kit EK-RA6M4:
- The RA6 contains a build in J-Link programmer which supports debut output via SEGGER RTT.
- It uses the MikroBus port for the DA1451 

| MikroBus | MCU   | Function            |
|----------|-------|---------------------|
| J21/2    | P115  | RESET (active high) | 
| P21/3    | P205  | RTS                 |
| J21/4    | P204  | CTS                 |
| J22/4    | P613  | TX                  |
| J22/3    | P614  | RX                  |

- UART RTS: Manual RTS control in UART callback handler. MikroBus slot with UART 7 does not have RTSCTS7
  on the pin used by BLE Tiny Click module.
- BSP
```
// 0x1000 main stack
#define BSP_CFG_STACK_MAIN_BYTES (0x1000)

// printf allocates memory from the heap
#define BSP_CFG_HEAP_BYTES (0x800)
```

### Renesas DA14531 Module on MikroE BLE Tiny Click board with 
- The board comes with some demo application and needs to be programmed with an HCI firmware to use it with a regular Bluetooth stack.
- Firmware details:
  - Keil uVision project `DA145xx_SDK/x.x.xx.xxxx/projects/target_apps/hci` on Windows

```
// Config: user_periph_setup.h
#define UART1_TX_PORT   GPIO_PORT_0
#define UART1_TX_PIN    GPIO_PIN_6
#define UART1_RX_PORT   GPIO_PORT_0
#define UART1_RX_PIN    GPIO_PIN_5
#define UART1_RTSN_PORT GPIO_PORT_0
#define UART1_RTSN_PIN  GPIO_PIN_7
#define UART1_CTSN_PORT GPIO_PORT_0
#define UART1_CTSN_PIN  GPIO_PIN_8
#define UART1_BAUDRATE  UART_BAUDRATE_460800
#define UART1_DATABITS  UART_DATABITS_8

// Config: user_config.h
static const sleep_state_t app_default_sleep_mode = ARCH_SLEEP_OFF;
```

- Firmware installation:
  - Connect GND (pin 5) and VCC (pin 6) with jumper wires to the RA6 dev board. 
  - Connect it with a ARM-Cortex 10-pin connector to a J-Link device
  - Start [SmartBond Flash Programmer](https://www.renesas.com/kr/en/software-tool/smartbond-flash-programmer)
  - The Programmer should auto-detect the DA14531 via the J-Link.
  - Select `firmware/hci_531_rx05_tx06_rts07_cts08_468000.hex` as firmware file and click `Program`

### Software {#sec:renesas-ek-ra6m4a-da14531Port-software}

The port provides a CMake project file that uses the installed Arm Gnu Toolchain.

- Install [Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
- Install [CMake](https://cmake.org)
- Install [Ninja](https://ninja-build.org)
- To compile, go to the port folder:

    `cd btstack/port/renesas-ek-ra6me4a-da14531`

- Create a build folder and go to build folder

    `mkdir build && cd build`

- Create Ninja build files

   `cmake -G Ninja ..`

- Build all examples

    `ninja`

This will build all examples as .elf files as well as .jdebug Ozone debug project files
Alternatively, the CMakeLists.txt can be used to compile using Make (`cmake -G "Unix Makefiles" ..` and `make`) or
or use the project in most modern IDEs (CLion, Visual Studio, Visual Studio Code, ...)


### Run Example Project using Ozone {#sec:renesas-ek-ra6m4a-da14531Port-run-example-project-using-ozone}

After building the examples, the generated .elf file can be used with Ozone.
Start Ozone and open the provided .jdebug file. The debug output is readily available in the RTT Terminal.


### Debug output {#sec:renesas-ek-ra6m4a-da14531Port-debug-output}

All debug output is send via SEGGER RTT.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects.

Also, the full packet log with addtional log information can be enabled in src/hal_entry.c by uncommenting the hci_dump_init(...) call.

The console output can then be converted into .pklg files by running tool/create_packet_log.py. The .pklg file can be
analyzed with the macOS X PacketLogger or WireShark.


### Setup {#sec:renesas-ek-ra6m4a-da14531Port-setup}

![Renesas Eval Kit EK-RA6M4 with DA14531](ra6m4-and-ble-tiny-click.jpg)

### Updating HAL Configuration {#sec:renesas-ek-ra6m4a-da14531Port-updating-hal-configuration}
- Start Renesas RA v3.7.0/e2-studio on Windows and open `e2-project`
- Open `configuration.xml` to get to "FSP Configuration" perspective
    - to add modules, click "New Stack"
    - module is configured in "Properties" view (usually below next to 'Problems' etc)
- Press button re-generates sources
- Copy folder `e2-project` into this port
- Check diff for unexpected changes
- If needed:
  - Update CMakeLists.txt to add new modules
  - Add code to enable ('open') new module in `R_BSP_WarmStart` of `port/hal_entry.c`
## BTstack Port for Renesas Target Board TB-S1JA with CC256x {#sec:renesas-tb-s1ja-cc256xPort}

This port uses the Renesas TB-S1JA with TI's CC256XEM ST Adapter Kit that allows to plug in a CC256xB or CC256xC Bluetooth module.
Renesas e2 Studio (Eclise-based) was used with the SSP HAL and without an RTOS.
For easy debugging, Ozone project files are generated as well.

### Hardware {#sec:renesas-tb-s1ja-cc256xPort-hardware}

Renesas Target Board TB-S1JA:
- [TB-S1JA Target Board Kit](https://www.renesas.com/eu/en/products/synergy/hardware/kits/tb-s1ja.html)

- CC2564B Bluetooth Controller:
  1. The best option is to get it as a BoostPack 
     - Info: BOOST-CC2564MODA: http://www.ti.com/tool/BOOST-CC2564MODA
  2. Alternatively, get the evaluation module together with the EM Wireless Booster pack and a 32.768 kHz oscillator
     - EM Wireless Booster Pack:
       - [Info](http://www.ti.com/tool/BOOST-CCEMADAPTER)
       - [User Guide](http://www.ti.com/lit/pdf/swru338)
    - CC256x Bluetooth module:
       - [CC2564B Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/cc2564modnem.aspx)
       - [CC2564C Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/CC256XCQFN-EM-CC2564C-Dual-Mode-Bluetooth-Controller-Evaluation-Module-P51277.aspx)
       - The module with the older CC2564B is around USD 20, while the one with the new CC2564C costs around USD 60

The projects are configured for the CC2564C. When using the CC2564B, *bluetooth_init_cc2564B_1.8_BT_Spec_4.1.c* should be used as cc256x_init_script. You can update this in the create_examples.py script.

Connct the Target Board to the TI Boosterpack, see Booster Pack Pinout: http://www.ti.com/ww/en/launchpad/dl/boosterpack-pinout-v2.pdf


J2 PIN | S1JA PORT | S1JA Signal | Boosterpack
-------|-----------|-------------|------------
2      | P301      | RXD0        | 3  (LP1)
4      | P302      | TXD0        | 4  (LP1)
6      | P304      | CTS0        | 36 (LP2)
8      | P303      | RTS0        | 37 (LP2)
10     | VCC       | VCC         |  1 (LP1)
12     | VSS       | GND         | 20 (LP2)
14     | P112      | nShutdown   | 19 (LP1)

### Software {#sec:renesas-tb-s1ja-cc256xPort-software}

Generate example projects

    $ python create_examples.py

This will generate an e2 Studio project for each example. 

### Excluded Examples

The a2dp examples (a2dp_source_demo and a2dp_sink_demo) were disabled as the C open-source SBC codec
with compile option -O2 wasn't fast enough to provide real-time encoding/decoding. 


### Build, Flash And Run The Examples in e2 Studio {#sec:renesas-tb-s1ja-cc256xPort-build-flash-and-run-the-examples-in-e2-studio}

Open the e2 Studio project and press the 'Debug' button. Debug output is only available via SEGGER RTT. You can run SEGGER's JLinkRTTViewer or use Ozone as described below.


### Run Example Project using Ozone {#sec:renesas-tb-s1ja-cc256xPort-run-example-project-using-ozone}

After compiling the project with e2 Studio, the genereated .elf file can be used with Ozone (also e.g. on macOS). 
In Ozone, the debug output is readily available in the terminal. A .jdebug file is provided in the project folder.


### Debug output {#sec:renesas-tb-s1ja-cc256xPort-debug-output}

All debug output is send via SEGGER RTT.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/hal_entry.c by uncommenting the hci_dump_init(...) call.
The console output can then be converted into .pklg files by running tool/create_packet_log.py. The .pklg file can be 
analyzed with the macOS X PacketLogger or WireShark.

### GATT Database {#sec:renesas-tb-s1ja-cc256xPort-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. The create_examples.py script converts the .gatt files into a corresponding .h for the project. After updating a .gatt file, the .h can be updated manually by running the provided update_gatt_db.sh or update_gatt_db.bat scripts. 

Note: In theory, this can be integrated into the e2 Studio/Eclipse project.


### Notes {#sec:renesas-tb-s1ja-cc256xPort-notes}
- HCI UART is set to 2 mbps. Using 3 or 4 mbps causes hang during startup


### Nice to have {#sec:renesas-tb-s1ja-cc256xPort-nice-to-have}
- Allow compilation using Makefile/CMake without the e2 Studio, e.g. on the Mac.
## BTstack Port for Ezurio Vela IF820 Series {#sec:rp2040-vela-if820Port}

The Vela IF820 Development kit and USB dongle use an  Raspberry Pi RP2040 MCU as a combined USB-to-UART adapter and SWD programmer. 

This port uses the RP2040 of the development kit as the main target with a connected Vela IF820 / CYW20820 Bluetooth Controller. 

In addition, the console is routed over USB CDC for easy integration and an USB audio interface is provided for sound output.

### Hardware {#sec:rp2040-vela-if820Port-hardware}

As this port targets the RP2040 on the development kit or the USB dongle, the Vela IF820 in module format cannot be used.

[More info on the Vela IF820 series](
https://www.ezurio.com/wireless-modules/bluetooth-modules/bluetooth-5-modules/vela-if820-series-bluetooth-5-2-dual-mode-modules)

### Loading HCI Firmware into the Vela IF820/CYW20820 {#sec:rp2040-vela-if820Port-loading-hci-firmware-into-the-vela-if820-cyw20820}
To use the Bluetooth Controller with BTstack, it needs to be flashed with the HCI Firmware provided by Ezurio.
Please use their [Flasher tool with the provided HCI firmware](https://github.com/Ezurio/Vela_IF820_Firmware).

To flash the HCI firmware, hold the "Module Recovery" button while pressing and releasing "Module Reset". 
Then start the Windows GUI and select the correct HCI firmware.

### Software {#sec:rp2040-vela-if820Port-software}

The port for the RP2040 requires the official [pico-sdk](https://github.com/raspberrypi/pico-sdk).
Please follow the [Getting Started Guide](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) and make sure to point `PICO_SDK_PATH` to the root folder of the SDK.

The current `pico-sdk` v2.2.0 uses TinyUSB v0.18, which has an issue with opening the virtual soundcard more than once.
Please update TinyUSB in `lib/tinyusb` to the current master branch or at least commit 5b200c4:

    $ cd ${PICO_SDK_PATH}/lib/tinyusb
    $ git checkout 5b200c4

After pointing `PICO_SDK_PATH` to the SDK, all examples can be compiled like this: 
     
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    $ make

All examples are placed in the `build` folder. Please make sure to specify the Release build type.
By default, pico-sdk uses Debug built type and the generated code isn't fast enough for the SBC Codec in the 
A2DP demos.

You can compile a single example by calling make with the name of the target .elf file, e.g. `make gatt_counter.elf`

### Flash And Run The Examples {#sec:rp2040-vela-if820Port-flash-and-run-the-examples}

By default, different versions of each example are build including:
- example.elf: .elf file with all debug information
- example.uf2: binary file that can be used to flash via the mass storage interface.

### Flash using MSC Interface
Hold the BOOTSEL button then press and release the "Debug Reset" button to enter bootloader mode. The RP2040 should 
get mounted by the OS. You can now copy any .uf2 into the root folder of the virtual RP2040 device. 
Press "Debug Reset" to start it.

### Flash using Picotool
If you still have the picoprobe firmware installed, hold the BOOTSEL button then press and release the "Debug Reset"
button to enter bootloader mode. You can now comfortably load any .elf file with: `picotool load gatt_streamer_server.elf`.
After that press the "Debug Reset" button once or run `picotool reboot`. 

The port contains the custom Reset logic to enter bootloader mode and you can directly load a new target with
`picotool load -f a2dp_sink_demo.elf`

### Flash and Debug using SWD Interface {#sec:rp2040-vela-if820Port-flash-and-debug-using-swd-interface}
As the Vela IF820 has a TagConnect 2030 interface, the RP2040 can be programmed and debugged with a suitable adapter 
and any SWD programmer, e.g. SEGGER J-Link or Raspberry Pi's Picoprobe. 

### A2DP Sink Demo Example {#sec:rp2040-vela-if820Port-a2dp-sink-demo-example}
The most interesting example for this port is the A2DP Sink Demo as it allows to stream music in Hifi quality to your computer.

After flashing the demo onto the RP2040, you can connect from a mobile phone to the `A2DP Sink Demo xx:xx:xx:xx:xx:xx`
device and play music. 
The music is available via the USB soundcard implemented on the RP2040 as part of this port.

On macOS, you can start `Quicktime Player.app` and select `New Audio Recording` from the File menu. 
The RP2040 shows up as "BTstack Audio" interface.
Another option is to use `Audacity`, enable `Software playthrough of interface` in `Settings->Recording` and start recording.

### Console / Debug Output {#sec:rp2040-vela-if820Port-console-debug-output}

All debug output is sent over the USB CDC. You can interact with the BTstack examples with a regular terminal program 
like [picocom](https://github.com/npat-efault/picocom)

### GATT Database {#sec:rp2040-vela-if820Port-gatt-database}
In BTstack, the GATT Database is defined via the `.gatt` file in the example folder. The `CMakeLists.txt` contains rules
to update the `.h` file when the `.gatt` was modified.

### HCI Traces {#sec:rp2040-vela-if820Port-hci-traces}

To enable HCI traces, you can uncomment the `WANT_HCI_DUMP=1` line in `CMakeLists.txt`. If you capture the output into
a `log.txt` file, you can use `tool/create_packet_log.py log.txt` to convert it into `log.pklg` which can be opened and
analyzed with [Wireshark](https://www.wireshark.org).

### Uninstall {#sec:rp2040-vela-if820Port-uninstall}
To restore the Vela IF820 dev kit to its original form, or to update the firmware on the CYW20820 module, you can 
re-flash `picoprobe` either with `picotool` or by entering the recovery mode (hold BOOSEL, press and release Debug Reset).
## BTstack Port for SAMV71 Ultra Xplained with ATWILC3000 SHIELD {#sec:samv71-xplained-atwilc3000Port}

This port uses the [SAMV71 Ultra Xplained Ultra](http://www.atmel.com/tools/atsamv71-xult.aspx) evaluation kit with an [ATWILC3000 SHIELD](http://www.microchip.com/DevelopmentTools/ProductDetails.aspx?PartNO=ATWILC3000-SHLD). The code is based on the Advanced Software Framework (ASF) (previously know as Atmel Software Framework). It uses the GCC Makefiles provided by the ASF. OpenOCD is used to upload the firmware to the device.

### Create Example Projects {#sec:samv71-xplained-atwilc3000Port-create-example-projects}

To create all example projects in the example folder, you can run:

    $ make

### Compile Example {#sec:samv71-xplained-atwilc3000Port-compile-example}

In one of the example folders:

    $ make

To upload the firmware:

    $ make flash

You need to connect the the Debug USB to your computer.

### Debug output {#sec:samv71-xplained-atwilc3000Port-debug-output}
printf is routed to USART1, which is connected to the virtual serial port. To get the console output, open a terminal at 115200.

In btstack_config.h, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in the main() function on main.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### TODOs {#sec:samv71-xplained-atwilc3000Port-todos}
    - Implement hal_flash_sector.h to persist link keys

### Issues {#sec:samv71-xplained-atwilc3000Port-issues}
    - Bluetooth UART driver uses per-byte interrupts and doesn't work reliable at higher baud rates (921600 seems ok, 2 mbps already causes problems).
    - An older XDMA-based implementation only sends 0x00 bytes over the UART. It might be better to just read incoming data into two buffers, (e.g. using a two element linked list with XDMA), and raising RTS when one buffer is full.
## BTstack Port for STM32 F4 Discovery Board with CC256x {#sec:stm32-f4discovery-cc256xPort}

This port uses the STM32 F4 Discovery Board with TI's CC256XEM ST Adapter Kit that allows to plug in a CC256xB or CC256xC Bluetooth module.
STCubeMX was used to provide the HAL, initialize the device, and create an initial CMakeLists.txt. For easy development, Ozone project files are generated as well.

### Hardware {#sec:stm32-f4discovery-cc256xPort-hardware}

STM32 Development kit and adapter for CC256x module:
- [STM32 F4 Discovery Board](http://www.st.com/en/evaluation-tools/stm32f4discovery.html)
- [CC256xEM Bluetooth Adatper Kit for ST](https://store.ti.com/CC256XEM-STADAPT-CC256xEM-Bluetooth-Adapter-Kit-P45158.aspx)

CC256x Bluetooth module:
- [CC2564B Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/cc2564modnem.aspx)
- [CC2564C Dual-mode Bluetooth(R) Controller Evaluation Module](https://store.ti.com/CC256XCQFN-EM-CC2564C-Dual-Mode-Bluetooth-Controller-Evaluation-Module-P51277.aspx)

The module with the older CC2564B is around USD 20, while the one with the new CC2564C costs around USD 60. The projects are configured for the CC2564C. When using the CC2564B, *bluetooth_init_cc2564B_1.8_BT_Spec_4.1.c* should be used as cc256x_init_script.

### Software {#sec:stm32-f4discovery-cc256xPort-software}

The build system uses CMake. To build all examples, run `cmake` and `make` in the build folder. The examples are built with
    
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

All examples and the .jedbug Ozone project files are placed in the 'build' folder.

### Flash And Run The Examples {#sec:stm32-f4discovery-cc256xPort-flash-and-run-the-examples}

Cmake builds different versions: 
- example.elf: .elf file with all debug information
- example.bin: .bin file that can be used for flashing

There are different options to flash and debug the F4 Discovery board. The F4 Discovery boards comes with an on-board [ST-Link programmer and debugger](https://www.st.com/en/development-tools/st-link-v2.html). As an alternative, the ST-Link programmer can be replaced by an [SEGGER J-Link OB](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/). Finally, the STM32 can be programmed with any ARM Cortex JTAG or SWD programmer via the SWD jumper.

### Run Example Project using Ozone {#sec:stm32-f4discovery-cc256xPort-run-example-project-using-ozone}

When using an external J-Link programmer or after installing J-Link OB on the F4 Discovery board, you can flash and debug using the cross-platform [SEGGER Ozone Debugger](https://www.segger.com/products/development-tools/ozone-j-link-debugger/). It is included in some J-Link programmers or can be used for free for evaluation usage.

Just start Ozone and open the .jdebug file in the build folder. When compiled with "ENABLE_SEGGER_RTT", the debug output shows up in the Terminal window of Ozone. 


### Debug output {#sec:stm32-f4discovery-cc256xPort-debug-output}

All debug output can be either send via SEGGER RTT or via USART2. To get the console from USART2, connect PA2 (USART2 TX) of the Discovery board to an USB-2-UART adapter and open a terminal at 115200.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/port.c resp. btstack/port/stm32-f4discovery-cc256x/src/port.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### GATT Database {#sec:stm32-f4discovery-cc256xPort-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. The CMakeLists.txt contains rules to update the .h file when the .gatt was modified.


### Maintainer Notes - Updating The Port {#sec:stm32-f4discovery-cc256xPort-maintainer-notes-updating-the-port}

The Audio BSP is from the STM32F4Cube V1.16 firmware and not generated from STM32CubeMX. To update the HAL, run 'generate code' in CubeMX. After that, make sure to re-apply the patches to the UART and check if the hal config was changed.

## BTstack Port for STM32 F4 Discovery Board with USB Bluetooth Controller {#sec:stm32-f4discovery-usbPort}

This port uses the STM32 F4 Discovery Board with an USB Bluetooth Controller plugged into its USB UTG port.
See [blog post](https://bluekitchen-gmbh.com/btstack-stm32-usb-port/) for details.

STCubeMX was used to provide the HAL, initialize the device, and the Makefile. For easy development, Ozone project files are generated as well.

### Hardware {#sec:stm32-f4discovery-usbPort-hardware}

STM32 Development kit with USB OTG adapter and USB CSR8510 Bluetooth Controller
- [STM32 F4 Discovery Board](http://www.st.com/en/evaluation-tools/stm32f4discovery.html)

### Software {#sec:stm32-f4discovery-usbPort-software}

To build all examples, run make

	$ make

All examples and the .jedbug Ozone project files are placed in the 'build' folder.


### Flash And Run The Examples {#sec:stm32-f4discovery-usbPort-flash-and-run-the-examples}

The Makefile builds different versions: 
- example.elf: .elf file with all debug information
- example.bin: .bin file that can be used for flashing

There are different options to flash and debug the F4 Discovery board. The F4 Discovery boards comes with an on-board [ST-Link programmer and debugger](https://www.st.com/en/development-tools/st-link-v2.html). As an alternative, the ST-Link programmer can be replaced by an [SEGGER J-Link OB](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/). Finally, the STM32 can be programmed with any ARM Cortex JTAG or SWD programmer via the SWD jumper.

### Run Example Project using Ozone {#sec:stm32-f4discovery-usbPort-run-example-project-using-ozone}

When using an external J-Link programmer or after installing J-Link OB on the F4 Discovery board, you can flash and debug using the cross-platform [SEGGER Ozone Debugger](https://www.segger.com/products/development-tools/ozone-j-link-debugger/). It is included in some J-Link programmers or can be used for free for evaluation usage.

Just start Ozone and open the .jdebug file in the build folder. When compiled with "ENABLE_SEGGER_RTT", the debug output shows up in the Terminal window of Ozone. 


### Debug output {#sec:stm32-f4discovery-usbPort-debug-output}

The debug output can send via SEGGER RTT.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/port.c resp. btstack/port/stm32-f4discovery-cc256x/src/port.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### GATT Database {#sec:stm32-f4discovery-usbPort-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. The Makefile contains rules to update the .h file when the .gatt was modified.


### Maintainer Notes - Updating The Port {#sec:stm32-f4discovery-usbPort-maintainer-notes-updating-the-port}

The Audio BSP is from the STM32F4Cube V1.16 firmware and not generated from STM32CubeMX. To update the HAL, run 'generate code' in CubeMX. After that, make sure to re-apply the patches to the UART and check if the hal config was changed.

## BTstack Port for STM32 Nucleo L073RZ Board with EM9304 Controller {#sec:stm32-l073rz-nucleo-em9304Port}

This port uses the STM32 Nucleo-L073RZ Board with EM's EM9304 Shield. 

The STM32CubeMX tool was used to provide the HAL, initialize the device, and create a basic Makefile. The Makefile has been exteneded to compile all BTstack LE examples. 
For easy development, Ozone project files are generated as well.

### Hardware {#sec:stm32-l073rz-nucleo-em9304Port-hardware}

In this port, the EM9304 is conencted via the SPI1 interface and configured for 8 Mhz. [Datasheet for the EM9304](http://www.emmicroelectronic.com/sites/default/files/public/products/datasheets/9304-ds_0.pdf)

It assumes that the EM9304 does not contain any patches and uploads the latest Metapatch during startup.

### Software {#sec:stm32-l073rz-nucleo-em9304Port-software}

To build all examples, run make

	$ make

All examples and the .jedbug Ozone project files are placed in the 'build' folder.

### Flash And Run The Examples {#sec:stm32-l073rz-nucleo-em9304Port-flash-and-run-the-examples}

The Makefile builds different versions: 
- example.elf: .elf file with all debug information
- example.bin: .bin file that can be used for flashing

There are different options to flash and debug the F4 Discovery board. The F4 Discovery boards comes with an on-board [ST-Link programmer and debugger](https://www.st.com/en/development-tools/st-link-v2.html). As an alternative, the ST-Link programmer can be replaced by an [SEGGER J-Link OB](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/). Finally, the STM32 can be programmed with any ARM Cortex JTAG or SWD programmer via the SWD jumper.

### Run Example Project using Ozone {#sec:stm32-l073rz-nucleo-em9304Port-run-example-project-using-ozone}

When using an external J-Link programmer or after installing J-Link OB on the F4 Discovery board, you can flash and debug using the cross-platform [SEGGER Ozone Debugger](https://www.segger.com/products/development-tools/ozone-j-link-debugger/). It is included in some J-Link programmers or can be used for free for evaluation usage.

Just start Ozone and open the .jdebug file in the build folder. When compiled with "ENABLE_SEGGER_RTT", the debug output shows up in the Terminal window of Ozone. 

### Debug output {#sec:stm32-l073rz-nucleo-em9304Port-debug-output}

All debug output can be either send via SEGGER RTT or via USART2. To get the console from USART2, connect PA2 (USART2 TX) of the Discovery board to an USB-2-UART adapter and open a terminal at 115200.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/port.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### GATT Database {#sec:stm32-l073rz-nucleo-em9304Port-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. During the build, the .gatt file is converted into a .h file with a binary representation of the GATT Database and useful defines for the application.

### Image {#sec:stm32-l073rz-nucleo-em9304Port-image}
![EM9304 DVK](EM9304DVK.jpg)
## BTstack Port with Cinnamon for Semtech SX1280 Controller on Miromico FMLR-80 {#sec:stm32-l451-miromico-sx1280Port}

*Cinnamon* is BlueKitchen's minimal, yet robust Controller/Link Layer implementation for use with BTstack.

In contrast to common Link Layer implementations, our focus is on a robust and compact implementation for production use,
where code size matters (e.g. current code size about 8 kB).

### Overview {#sec:stm32-l451-miromico-sx1280Port-overview}

This port targets the Semtech SX1280 radio controller. The Host Stack and the Controller (incl. Link Layer) run on a STM32 MCU, with the SX1280 connected via SPI.

It uses the SX1280 C-Driver from Semtech to communicate with the SX1280. The main modification was to the SPI driver that uses DMA for full packets.

### Status {#sec:stm32-l451-miromico-sx1280Port-status}

Tested with the [Miromico FMLR-80-P-STL4E module](https://miromico.ch/portfolio/fmlr-8x-x-stlx/) 
and our [SX1280 Shield](https://github.com/bluekitchen/sx1280-shield) - see (port/stm32-l476rg-nucleo-sx1280).
On the FMLR-80-P-STL4E module, the 52 Mhz clock for the SX1280 is controlled by the MCU.

SEGGER RTT is used for debug output, so a Segger J-Link programmer is required.

Uses 32.768 kHz crytstal as LSE for timing

Support for Broadcast and Peripheral roles.

The Makefile project compiles gatt_counter, gatt_streamer_server, hog_mouse and hog_keyboard examples.

### Limitation {#sec:stm32-l451-miromico-sx1280Port-limitation}

### Advertising State:
- Only Connectable Advertising supported
- Only fixed public BD_ADDR of 33:33:33:33:33:33 is used

### Connection State:
- Encryption not implemented
- Some LL PDUs not supported

### Central Role:
- Not implemented

### Observer Role:
- Not implemented

### Low power mode - basically not implemented:
- MCU does not sleep
- SPI could be disabled during sleep
- 1 ms SysTick is used for host stack although 16-bit tick time is provided by LPTIM1 based on 32768 Hz LSE.

### Getting Started {#sec:stm32-l451-miromico-sx1280Port-getting-started}

For the FMLR-80-P-STL4E module, just run make. You can upload the EXAMPLE.elf file created in build folder,
e.g. with Ozone using the provided EXAMPLE.jdebug, and run it.

### TODO {#sec:stm32-l451-miromico-sx1280Port-todo}

### General
- indicate random address in advertising pdus
- allow to set random BD_ADDR via HCI command and use in Advertisements
- support other regular adv types
- handle Encryption

### Low Power {#sec:stm32-l451-miromico-sx1280Port-low-power}
- enter STANDY_RC mode when idle
- implement MCU sleep (if next wakeup isn't immediate)
- sleep after connection request until first packet from Central
- replace SysTick with tick counter based on LPTIM1
- disable SPI on MCU sleep

## BTstack Port with Cinnamon for Semtech SX1280 Controller on STM32L476 Nucleo  {#sec:stm32-l476rg-nucleo-sx1280Port}

*Cinnamon* is BlueKitchen's minimal, yet robust Controller/Link Layer implementation for use with BTstack.

In contrast to common Link Layer implementations, our focus is on a robust and compact implementation for production use,
where code size matters (e.g. current code size about 8 kB).

### Overview {#sec:stm32-l476rg-nucleo-sx1280Port-overview}

This port targets the Semtech SX1280 radio controller. The Host Stack and the Controller (incl. Link Layer) run on a STM32 MCU, with the SX1280 connected via SPI.

It uses the SX1280 C-Driver from Semtech to communicate with the SX1280. The main modification was to the SPI driver that uses DMA for full packets.

### Status {#sec:stm32-l476rg-nucleo-sx1280Port-status}

Tested with the[Miromico FMLR-80-P-STL4E module](https://miromico.ch/portfolio/fmlr-8x-x-stlx/) 
(see port/stm32-l451-miromico-sx1280) and the
[STM32 L476 Nucleo dev kit](https://www.st.com/en/evaluation-tools/nucleo-l476rg.html)
with our [SX1280 Shield](https://github.com/bluekitchen/sx1280-shield). 

SEGGER RTT is used for debug output, so a Segger J-Link programmer is required, but the on-board
[ST-Link programmer and debugger](https://www.st.com/en/development-tools/st-link-v2.html) can be replaced by an
[SEGGER J-Link OB](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/). 

Uses 32.768 kHz crytstal as LSE for timing

Support for Broadcast and Peripheral roles.

The Makefile project compiles gatt_counter, gatt_streamer_server, hog_mouse and hog_keyboard examples.

### Limitation {#sec:stm32-l476rg-nucleo-sx1280Port-limitation}

### Advertising State:
- Only Connectable Advertising supported
- Only fixed public BD_ADDR of 33:33:33:33:33:33 is used

### Connection State:
- Encryption not implemented
- Some LL PDUs not supported

### Central Role:
- Not implemented

### Observer Role:
- Not implemented

### Low power mode - basically not implemented:
- MCU does not sleep
- SPI could be disabled during sleep
- 1 ms SysTick is used for host stack although 16-bit tick time is provided by LPTIM1 based on 32768 Hz LSE.

### Getting Started {#sec:stm32-l476rg-nucleo-sx1280Port-getting-started}

Just run make. You can upload the EXAMPLE.elf file created in build folder, 
e.g. with Ozone using the provided EXAMPLE.jdebug, and run it.

### TODO {#sec:stm32-l476rg-nucleo-sx1280Port-todo}

### General
- indicate random address in advertising pdus
- allow to set random BD_ADDR via HCI command and use in Advertisements
- support other regular adv types
- handle Encryption

### Low Power {#sec:stm32-l476rg-nucleo-sx1280Port-low-power}
- enter STANDY_RC mode when idle
- implement MCU sleep (if next wakeup isn't immediate)
- sleep after connection request until first packet from Central
- replace SysTick with tick counter based on LPTIM1
- disable SPI on MCU sleep

![STM32 L476RG Nucleo with SX1280 Shield](nucleo-with-sx1280-shield.jpg)
## BTstack Port for STM32WB55 Nucleo Boards using FreeRTOS {#sec:stm32-wb55xx-nucleo-freertosPort}

This port supports the Nucleo68 and the USB dongle of the [P-NUCLEO-WB55 kit](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html). Both have 1 MB of Flash memory.

The STM32Cube_FW_WB_V1.3.0 provides the HAL and WPAN, and initializes the device and the initial Makefile.
For easy development, Ozone project files are generated as well.

### Hardware {#sec:stm32-wb55xx-nucleo-freertosPort-hardware}

In this port, the Nucelo68 or the USB Dongle from the P-NUCLEO-WB55 can be used.

Last test was done using FUS v1.2 and HCI BLE Firmware v1.13 on Nucleo68
See STM32Cube_FW_WB_V1.13.0/Projects/STM32WB_Copro_Wireless_Binaries/Release_Notes.html for firmware install instructions.

Note: Segger RTT is currently not really usable. When sleep modes ared disabled for debuggin (see port_thread())
RTT works, but the output buffer quickly overflows. In Block mode, radio stops working.

### Nucleo68

The debug output is sent over USART1 and is available via the ST-Link v2.

### USB Dongle

To flash the dongle, SWD can be used via the lower 6 pins on CN1:
  - 3V3
  - PB3 - SWO (semi hosting not used)
  - PA14 - SCLK
  - PA13 - SWDIO
  - NRST
  - GND

The debug output is sent over USART1 and is available via PB6.

### Software {#sec:stm32-wb55xx-nucleo-freertosPort-software}

FreeRTOS V10.2.0 is used to run stack, you can get this example version by checking out official repo:

	$ cd Middlewares
	$ git submodule add https://github.com/aws/amazon-freertos.git
	$ git submodule update
	& cd amazon-freertos && git checkout v1.4.8

Or by specifying path to FreeRTOS

	$ make FREERTOS_ROOT=path_to_freertos

To build all examples, run make

	$ make

All examples and the .jedbug Ozone project files are placed in the 'build' folder.

### Flash And Run The Examples {#sec:stm32-wb55xx-nucleo-freertosPort-flash-and-run-the-examples}

The Makefile builds different versions: 
- example.elf: .elf file with all debug information
- example.bin: .bin file that can be used for flashing

### Nucleo68

There are different options to flash and debug the Nucleo68 board. The Nucleo68 boards comes with an on-board [ST-Link programmer and debugger](https://www.st.com/en/development-tools/st-link-v2.html). As an alternative, the ST-Link programmer can be replaced by an [SEGGER J-Link OB](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/). Finally, the STM32 can be programmed with any ARM Cortex JTAG or SWD programmer via the SWD jumper.

### USB Dongle

Please use any ARM Cortex SWD programmer via the SWD interface desribed in the hardware section.

### Run Example Project using Ozone {#sec:stm32-wb55xx-nucleo-freertosPort-run-example-project-using-ozone}

When using an external J-Link programmer or after installing J-Link OB on the Nucleo68 board, you can flash and debug using the cross-platform [SEGGER Ozone Debugger](https://www.segger.com/products/development-tools/ozone-j-link-debugger/). It is included in some J-Link programmers or can be used for free for evaluation usage.

Just start Ozone and open the .jdebug file in the build folder. When compiled with "ENABLE_SEGGER_RTT", the debug output shows up in the Terminal window of Ozone. Note: as mentioned before, Segger RTT currently stops working when CPU2 has started up.

### Debug output {#sec:stm32-wb55xx-nucleo-freertosPort-debug-output}

All debug output can be either send via SEGGER RTT or via USART1. To get the console from USART1, simply connect your board under STLink-v2 to your PC or connect PB6 (USART1 TX) of the Nucleo board to an USB-2-UART adapter and open a terminal at 115200.

In src/btstack_config.h resp. in example/btstack_config.h of the generated projects, additional debug information can be enabled by uncommenting ENABLE_LOG_INFO.

Also, the full packet log can be enabled in src/btstack_port.c by uncommenting the hci_dump_init(..) line. The console output can then be converted into .pklg files for OS X PacketLogger or WireShark by running tool/create_packet_log.py

### GATT Database {#sec:stm32-wb55xx-nucleo-freertosPort-gatt-database}
In BTstack, the GATT Database is defined via the .gatt file in the example folder. During the build, the .gatt file is converted into a .h file with a binary representation of the GATT Database and useful defines for the application.
## BTstack on Web using WebSerial {#sec:web-h4Port}

This ports compiles BTstack into WebAssembly and allows to run it in a Chrome-based Browser.
It communicates to the Bluetooth Controller via the WebSerial API.

### Requirements {#sec:web-h4Port-requirements}
- Emscripten from https://emscripten.org

### Build instructions {#sec:web-h4Port-build-instructions}
- Get Emscripten repository somewhere:
    - `git clone https://github.com/emscripten-core/emsdk.git`
- make sure EMSDK points to the root of your Emscripten installation:
    - `export EMSDK=<path/to/your/emscripten/repository>`
- create build folder and change to it:
    - `mkdir ninja`
    - `cd ninja`
- configure:
    - `cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=$EMSDK/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
        -DCMAKE_BUILD_TYPE=Release`
- build using your default CMake generator:
    - `ninja` or `make`

### Run Local Web Server {#sec:web-h4Port-run-local-web-server}
- `python3 -m http.server 8080`

### Test {#sec:web-h4Port-test}
- [Open local URL](http://localhost:8080/myprog.html)
- Force refresh of .js files: CMD+Alt+R on Safari, CMD+Shift+R on Chrome

### Supported Bluetooth Controllers {#sec:web-h4Port-supported-bluetooth-controllers}
The baudrate is currently set to 115200 and there's no chipset detection.
Suitable Controllers are therefore:
- Controllers with USB CDC and no need for firmware upload:
  - e.g. BL654 with Bluetooth SIG PTS Firmware
- Controllers with regular UART that start up at 115200 and no need for firmware upload:
  - e.g. Ezurio IF820/CYW20820 with HCI Firmware.

### Status {#sec:web-h4Port-status}
Supported Controller start up and all examples work as expected. The 115200 baud are to low for A2DP.
There's no audio driver yet as well as no support for STDIO. In addition, TLV is not supported, so
bonding information cannot be stored. A better UI will come, too. :)
## BTstack Port for WICED platform {#sec:wiced-h4Port}

Tested with:
- WICED SDK 3.4-6.2.1
- [RedBear Duo](https://redbear.cc/product/wifi-ble/redbear-duo.html): Please install [RedBear WICED Add-On](https://github.com/redbear/WICED-SDK) 
- [Inventek Systems ISM4334x](https://www.inventeksys.com/products-page/wifi-modules/serial-wifi/ism43341-m4g-l44-cu-embedded-serial-to-wifi-ble-nfc-module/) - Please contact Inventek Systems for WICED platform files
- [Inventek Systems ISM4343](https://www.inventeksys.com/wifi/wifi-modules/ism4343-wmb-l151/) (BCM43438 A1) - Please contact Inventek Systems for WICED platform files

To integrate BTstack into the WICED SDK, please move the BTstack project into WICED-SDK-6.2.1/libraries.

Then create projects for BTstack examples in WICED/apps/btstack by running:

	./create_examples.py

Now, the BTstack examples can be build from the WICED root in the same way as other examples, e.g.:

	./make btstack.spp_and_le_counter-RB_DUO

to build the SPP-and-LE-Counter example for the RedBear Duo (or use ISM43340_M4G_L44 / ISM4343_WBM_L151 for the Inventek Systems devices).

See WICED documentation about how to upload the firmware.

It should work with all WICED platforms that contain a Broadcom Bluetooth chipset.

The maximal baud rate is currenty limited to 1 mbps. 

The port uses the generated WIFI address plus 1 as Bluetooth MAC address.
It persists the LE Device DB and Classic Link Keys via the DCT mechanism.

All examples that rovide a GATT Server use the GATT DB in the .gatt file. Therefore you need to run ./update_gatt_db.sh in the apps/btstack/$(EXAMPLE) folder after modifying the .gatt file.
## BTstack Port for Windows Systems with Bluetooth Controller connected via Serial Port {#sec:windows-h4Port}

The Windows-H4 port uses the native run loop and allows to use Bluetooth Controllers connected via Serial Port.

Make sure to manually reset the Bluetooth Controller before starting any of the examples.

The port provides both a regular Makefile as well as a CMake build file. It uses native Win32 APIs for file access and does not require the Cygwin or mingw64 build/runtine. All examples can also be build with Visual Studio 2022 (e.g. Community Edition).

### Visual Studio 2022 {#sec:windows-h4Port-visual-studio-2022}

Visual Studio can directly open the provided `port/windows-windows-h4/CMakeLists.txt` and allows to compile and run all examples.

### mingw64  {#sec:windows-h4Port-mingw64}

It can also be compiles with a regular Unix-style toolchain like [mingw-w64](https://www.mingw-w64.org).
mingw64-w64 is based on [MinGW](https://en.wikipedia.org/wiki/MinGW), which '...provides a complete Open Source programming tool set which is suitable for the development of native MS-Windows applications, and which do not depend on any 3rd-party C-Runtime DLLs.'

We've used the Msys2 package available from the [downloads page](https://www.mingw-w64.org/downloads/) on Windows 10, 64-bit and use the MSYS2 MinGW 64-bit start menu item to compile 64-bit binaries.

In the MSYS2 shell, you can install everything with pacman:

    $ pacman -S git
    $ pacman -S cmake
    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-toolchain
    $ pacman -S mingw-w64-x86_64-portaudio
    $ pacman -S python
    $ pacman -S winpty

### Compilation with CMake

With mingw64-w64 installed, just go to the port/windows-h4 directory and use CMake as usual

    $ cd port/windows-h4
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Note: When compiling with msys2-32 bit and/or the 32-bit toolchain, compilation fails
as `conio.h` seems to be mission. Please use msys2-64 bit with the 64-bit toolchain for now.

### Console Output {#sec:windows-h4Port-console-output}

When running the examples in the MSYS2 shell, the console input (via btstack_stdin_support) doesn't work. It works in the older MSYS and also the regular CMD.exe environment. Another option is to install WinPTY and then start the example via WinPTY like this:

    $ winpty ./gatt_counter.exe

The packet log will be written to hci_dump.pklg

## BTstack Port for Windows Systems with DA14585 Controller connected via Serial Port {#sec:windows-h4-da14585Port}

This port allows to use the DA14585 connected via Serial Port with BTstack running on a Win32 host system.

It first downloads the hci_585.hex firmware from the 6.0.8.509 SDK, before BTstack starts up.

Please note that it does not detect if the firmware has already been downloaded, so you need to reset the DA14585 before starting an example.

For production use, the HCI firmware could be flashed into the OTP and the firmware download could be skipped.

Tested with the official DA14585 Dev Kit Basic on OS X and Windows 10.

The port provides both a regular Makefile as well as a CMake build file. It uses native Win32 APIs for file access and does not require the Cygwin or mingw64 build/runtine. All examples can also be build with Visual Studio 2022 (e.g. Community Edition).

### Visual Studio 2022 {#sec:windows-h4-da14585Port-visual-studio-2022}

Visual Studio can directly open the provided `port/windows-windows-h4-da14585/CMakeLists.txt` and allows to compile and run all examples.

### mingw64  {#sec:windows-h4-da14585Port-mingw64}

It can also be compiles with a regular Unix-style toolchain like [mingw-w64](https://www.mingw-w64.org).
mingw64-w64 is based on [MinGW](https://en.wikipedia.org/wiki/MinGW), which '...provides a complete Open Source programming tool set which is suitable for the development of native MS-Windows applications, and which do not depend on any 3rd-party C-Runtime DLLs.'

We've used the Msys2 package available from the [downloads page](https://www.mingw-w64.org/downloads/) on Windows 10, 64-bit and use the MSYS2 MinGW 64-bit start menu item to compile 64-bit binaries.

In the MSYS2 shell, you can install everything with pacman:

    $ pacman -S git
    $ pacman -S cmake
    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-toolchain
    $ pacman -S mingw-w64-x86_64-portaudio
    $ pacman -S python
    $ pacman -S winpty

### Compilation with CMake

With mingw64-w64 installed, just go to the port/windows-h4 directory and use CMake as usual

    $ cd port/windows-h4
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Note: When compiling with msys2-32 bit and/or the 32-bit toolchain, compilation fails
as `conio.h` seems to be mission. Please use msys2-64 bit with the 64-bit toolchain for now.

### Console Output {#sec:windows-h4-da14585Port-console-output}

When running the examples in the MSYS2 shell, the console input (via btstack_stdin_support) doesn't work. It works in the older MSYS and also the regular CMD.exe environment. Another option is to install WinPTY and then start the example via WinPTY like this:

    $ winpty ./gatt_counter.exe

The packet log will be written to hci_dump.pklg
## BTstack Port for Windows Systems with Zephyr-based Controller {#sec:windows-h4-zephyrPort}

The main difference to the regular windows-h4 port is that that the Zephyr Contoller uses 1000000 as baud rate.
In addition, the port defaults to use the fixed static address stored during production.

The port provides both a regular Makefile as well as a CMake build file. It uses native Win32 APIs for file access and does not require the Cygwin or mingw64 build/runtine. All examples can also be build with Visual Studio 2022 (e.g. Community Edition).

### Prepare Zephyr Controller {#sec:windows-h4-zephyrPort-prepare-zephyr-controller}

Please follow [this](https://devzone.nordicsemi.com/blogs/1059/nrf5x-support-within-the-zephyr-project-rtos/) blog post about how to compile and flash `samples/bluetooth/hci_uart` to a connected nRF5 dev kit.

In short: you need to install an arm-none-eabi gcc toolchain and the nRF5x Command Line Tools incl. the J-Link drivers, checkout the Zephyr project, and flash an example project onto the chipset:

  * Install [J-Link Software and documentation pack](https://www.segger.com/jlink-software.html).
  * Get nrfjprog as part of the [nRFx-Command-Line-Tools](http://www.nordicsemi.com/eng/Products/Bluetooth-low-energy/nRF52-DK). Click on Downloads tab on the top and look for your OS.
  * [Checkout Zephyr and install toolchain](https://www.zephyrproject.org/doc/getting_started/getting_started.html). We recommend using the [arm-non-eabi gcc binaries](https://launchpad.net/gcc-arm-embedded) instead of compiling it yourself. At least on OS X, this failed for us.

  * In *samples/bluetooth/hci_uart*, compile the firmware for nRF52 Dev Kit

      $ make BOARD=nrf52_pca10040

   * Upload the firmware

      $ make flash

   * For the nRF51 Dev Kit, use `make BOARD=nrf51_pca10028`.

### Configure serial port {#sec:windows-h4-zephyrPort-configure-serial-port}

To set the serial port of your Zephyr Controller, you can either update config.device_name in main.c or
always start the examples with the correct `-u COMx` option.

### Visual Studio 2022 {#sec:windows-h4-zephyrPort-visual-studio-2022}

Visual Studio can directly open the provided `port/windows-windows-h4-zephyr/CMakeLists.txt` and allows to compile and run all examples.

### mingw64  {#sec:windows-h4-zephyrPort-mingw64}

It can also be compiles with a regular Unix-style toolchain like [mingw-w64](https://www.mingw-w64.org).
mingw64-w64 is based on [MinGW](https://en.wikipedia.org/wiki/MinGW), which '...provides a complete Open Source programming tool set which is suitable for the development of native MS-Windows applications, and which do not depend on any 3rd-party C-Runtime DLLs.'

In the MSYS2 shell, you can install everything with pacman:

    $ pacman -S git
    $ pacman -S cmake
    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-toolchain
    $ pacman -S mingw-w64-x86_64-portaudio
    $ pacman -S python
    $ pacman -S winpty

### Compilation with CMake

With mingw64-w64 installed, just go to the port/windows-h4 directory and use CMake as usual

    $ cd port/windows-h4
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Note: When compiling with msys2-32 bit and/or the 32-bit toolchain, compilation fails
as `conio.h` seems to be mission. Please use msys2-64 bit with the 64-bit toolchain for now.

### Console Output {#sec:windows-h4-zephyrPort-console-output}

When running the examples in the MSYS2 shell, the console input (via btstack_stdin_support) doesn't work. It works in the older MSYS and also the regular CMD.exe environment. Another option is to install WinPTY and then start the example via WinPTY like this:

    $ winpty ./gatt_counter.exe

The packet log will be written to hci_dump.pklg
## BTstack Port for Windows Systems using the WinUSB Driver {#sec:windows-winusbPort}

The Windows-WinUSB port uses the native run loop and WinUSB API to access a USB Bluetooth dongle.

The port provides both a regular Makefile as well as a CMake build file. It uses native Win32 APIs for file access and does not require the Cygwin or mingw64 build/runtine. All examples can also be build with Visual Studio 2022 (e.g. Community Edition).

### Access to Bluetooth USB Dongle with Zadig {#sec:windows-winusbPort-access-to-bluetooth-usb-dongle-with-zadig}

To allow WinUSB to access an USB Bluetooth dongle, you need to install a special device driver to make it accessible to user space processes. 

It works like this:

-  Download [Zadig](http://zadig.akeo.ie)
-  Start Zadig
-  Select Options -> "List all devices"
-  Select USB Bluetooth dongle in the big pull down list
-  Select WinUSB in the right pull down list
-  Select "Replace Driver"

![Zadig showing CYW20704A2](zadig-cyw20704.png)

After the new driver was installed, your device is shown in the Device Manager with Device Provider 'libwdi'

![Device Manager showing CYW20704A2](device-manager-cyw20704.png)

### Visual Studio 2022 {#sec:windows-winusbPort-visual-studio-2022}

Visual Studio can directly open the provided `port/windows-winusb/CMakeLists.txt` and allows to compile and run all examples.
For this, the C++ CMake tools for Windows is required. They are part of the Desktop development with C++ workloads.

### mingw64  {#sec:windows-winusbPort-mingw64}

It can also be compiles with a regular Unix-style toolchain like [mingw-w64](https://www.mingw-w64.org).
mingw64-w64 is based on [MinGW](https://en.wikipedia.org/wiki/MinGW), which '...provides a complete Open Source programming tool set which is suitable for the development of native MS-Windows applications, and which do not depend on any 3rd-party C-Runtime DLLs.'

In the MSYS2 shell, you can install everything with pacman:

    $ pacman -S git
    $ pacman -S cmake
    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-toolchain
    $ pacman -S mingw-w64-x86_64-portaudio
    $ pacman -S python
    $ pacman -S winpty

### Compilation with CMake

With mingw64-w64 installed, just go to the port/windows-h4 directory and use CMake as usual

    $ cd port/windows-h4
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Note: When compiling with msys2-32 bit and/or the 32-bit toolchain, compilation fails
as `conio.h` seems to be mission. Please use msys2-64 bit with the 64-bit toolchain for now.

### Console Output {#sec:windows-winusbPort-console-output}

When running the examples in the MSYS2 shell, the console input (via btstack_stdin_support) doesn't work. It works in the older MSYS and also the regular CMD.exe environment. Another option is to install WinPTY and then start the example via WinPTY like this:

    $ winpty ./spp_and_le_counter.exe

## BTstack Port for Windows Systems with Intel Wireless 8260/8265 Controllers {#sec:windows-winusb-intelPort}

Same as port/windows-winusb, but customized for Intel Wireless 8260 and 8265 Controllers.
These controller require firmware upload and configuration to work. Firmware and config is downloaded from the Linux firmware repository.

The port provides both a regular Makefile as well as a CMake build file. It uses native Win32 APIs for file access and does not require the Cygwin or mingw64 build/runtine. All examples can also be build with Visual Studio 2022 (e.g. Community Edition).


### Access to Bluetooth USB Dongle with Zadig {#sec:windows-winusb-intelPort-access-to-bluetooth-usb-dongle-with-zadig}

To allow WinUSB to access an USB Bluetooth dongle, you need to install a special device driver to make it accessible to user space processes.

It works like this:

-  Download [Zadig](http://zadig.akeo.ie)
-  Start Zadig
-  Select Options -> "List all devices"
-  Select USB Bluetooth dongle in the big pull down list
-  Select WinUSB in the right pull down list
-  Select "Replace Driver"

![Zadig showing CYW20704A2](zadig-cyw20704.png)

After the new driver was installed, your device is shown in the Device Manager with Device Provider 'libwdi'

![Device Manager showing CYW20704A2](device-manager-cyw20704.png)

### Visual Studio 2022 {#sec:windows-winusb-intelPort-visual-studio-2022}

Visual Studio can directly open the provided `port/windows-winusb-intel/CMakeLists.txt` and allows to compile and run all examples.

### mingw64  {#sec:windows-winusb-intelPort-mingw64}

It can also be compiles with a regular Unix-style toolchain like [mingw-w64](https://www.mingw-w64.org).
mingw64-w64 is based on [MinGW](https://en.wikipedia.org/wiki/MinGW), which '...provides a complete Open Source programming tool set which is suitable for the development of native MS-Windows applications, and which do not depend on any 3rd-party C-Runtime DLLs.'

In the MSYS2 shell, you can install everything with pacman:

    $ pacman -S git
    $ pacman -S cmake
    $ pacman -S make
    $ pacman -S mingw-w64-x86_64-toolchain
    $ pacman -S mingw-w64-x86_64-portaudio
    $ pacman -S python
    $ pacman -S winpty

### Compilation with CMake

With mingw64-w64 installed, just go to the port/windows-h4 directory and use CMake as usual

    $ cd port/windows-h4
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Note: When compiling with msys2-32 bit and/or the 32-bit toolchain, compilation fails
as `conio.h` seems to be mission. Please use msys2-64 bit with the 64-bit toolchain for now.

### Console Output {#sec:windows-winusb-intelPort-console-output}

When running the examples in the MSYS2 shell, the console input (via btstack_stdin_support) doesn't work. It works in the older MSYS and also the regular CMD.exe environment. Another option is to install WinPTY and then start the example via WinPTY like this:

    $ winpty ./gatt_counter.exe

The packet log will be written to hci_dump.pklg
## BTstack Port for Zephyr RTOS {#sec:zephyrPort}

### Overview {#sec:zephyrPort-overview}

This port targets any platform supported by Zephyr that either contains a built-in Bluetooth Controller
or is connected to an external Controller via one of the supported Zephyr HCI Transports drivers (see `zephyr/drivers/bluetooth/hci`)

### Status {#sec:zephyrPort-status}

Tested with nRF52 DK (PCA10040), nRF52840 DK (PC10056) and nRF5340 DK (PCA10095) boards only. It uses the fixed static random BD ADDR stored in NRF_FICR/NRF_FICR_S, which will not compile on non nRF SoCs.

### Build Environment {#sec:zephyrPort-build-environment}
The first step needs to done once. Step two is needed every time to setup the environment.

### 1. Build Environment Preconditions

Follow the getting started [guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
until you are able to build an example.

Then update the `ZEPHYR_ROOT` variable in `env.sh` to point to your `zephyrproject`. Defaults to `~/zephyrproject`


### 2. Prepare the build environment

To setup your environment to build a BTstack example, run the provided setup in `env.sh`.

```sh
source env.sh
```

### Building and Running on nRF52840 {#sec:zephyrPort-building-and-running-on-nrf52840}

### 1. Build Example

You can build an example using:
```sh
west build -b nrf52840dk/nrf52840
```

`nrf52840dk/nrf52840` selected the Nordic nRF52840 DK. For the older nRF52 DK with nRF52832, you can specify nrf52dk/nrf52832.
To get a list of all supported Zephyr targets, run `west boards`

To change zephyr platform settings use:
```sh
west build -b nrf52840dk/nrf52840 -t menuconfig
```

To build a different example, e.g. the `gatt_streamer_server`, set the EXAMPLE environment variable:
```sh
EXAMPLE=gatt_streamer_server west build -b nrf52840dk/nrf52840
```

### 2. Flash Example

To flash a connected board:
```sh
west flash
```

### Building and Running on nRF5340 {#sec:zephyrPort-building-and-running-on-nrf5340}

The nrf5340 is a dual core SoC, where one core is used for Bluetooth HCI functionality and
the other for the high level application logic. So both cores need to be programmed separately.
Using the nRF5340-DK for example allows debug output on both cores to separate UART ports.
For the nRF5340 a network core firmware is required, which one depends on the use-case.
With 2. an options is given.

### 1. Building the application
build using:
```sh
west build -b nrf5340dk/nrf5340/cpuapp
```
with debug:
```sh
west build -b nrf5340dk/nrf5340/cpuapp -- -DOVERLAY_CONFIG=debug_overlay.conf
```

### 2. Using zephyr network core image
the `hci_rgmsg` application needs to be loaded first to the network core.
Configure network core by selecting the appropriate config file, for example `nrf5340_cpunet_iso-bt_ll_sw_split.conf`.
additionally it's required to increase the main stack size from
```sh
CONFIG_MAIN_STACK_SIZE=512
```
to
```sh
CONFIG_MAIN_STACK_SIZE=4096
```
then the network core image can be compiled and flashed
```sh
west build -b nrf5340dk/nrf5340/cpunet -- -DCONF_FILE=nrf5340_cpunet_iso-bt_ll_sw_split.conf
west flash
```
or with debugging
```sh
west build -b nrf5340dk/nrf5340/cpunet -- -DCONF_FILE=nrf5340_cpunet_iso-bt_ll_sw_split.conf -DOVERLAY_CONFIG=debug_overlay.conf
west flash
```

### Build and Running on STM32 Nucleo 144 board with Ezurio M.2 Adapter and M.2 Bluetooth Controller {#sec:zephyrPort-build-and-running-on-stm32-nucleo-144-board-with-ezurio-m-2-adapter-and-m-2-bluetooth-controller}

Ezurio's [Wi-Fi M.2 2230 to STM32 Nucleo Adapter Card](https://www.ezurio.com/product/wi-fi-m-2-2230-to-stm32-nucleo-adapter-card)
allows to connect any Bluetooth/Wifi M.2 Card to most STM32 Nucleo-144 boards. M.2 modules are available from multiple module 
vendors, e.g. Ezurio, u-blox, and Embedded Artists.

The adapter board takes care of 3.3V and 1.8V translation and routes SDIO for Wifi and UART for Bluetooth to the Nucleo board.
On most Nucleo-144 boards, the Bluetooth UART ends up on the same STM32, namely: PD3-PD6. 
See `port/zephyr/boards/shields/ezurio_m2_nucleo_144_adapter.overlay` for details.

We tested CYW55513 and CYW55573 M.2 modules with the following boards:

| Nucleo board  | Working |
|---------------|---------|
| nucleo_f439zi | x       | 
| nucleo_h753zi | x       | 
| nucleo_h563zi | x       | 
| nucleo_h755zi |         | 

The PatchRAM .hcd files can be placed in the project folder and specified by the `CONFIG_AIROC_CUSTOM_FIRMWARE_HCD_BLOB`
option in the corresponding `boards/$BOARD.conf` file.

### 1. Build Example

```sh
west build -b nucleo_f439zi --shield ezurio_m2_nucleo_144_adapter
west build -b nucleo_h755zi_q/stm32h755xx/m7 --shield ezurio_m2_nucleo_144_adapter
```

### TODO {#sec:zephyrPort-todo}

- Allow/document use of Zephyr HCI Drivers
