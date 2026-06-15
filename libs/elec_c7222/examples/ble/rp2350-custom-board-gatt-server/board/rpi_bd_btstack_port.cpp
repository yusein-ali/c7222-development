/**
 * @file rpi_bd_btstack_port.cpp
 * @brief RpiBdBtstackPort implementation — BTstack table selection and lifecycle.
 */
#include "rpi_bd_btstack_port.hpp"

#include "rpi_bd_hub_mcu.h"

extern "C" {
#include "btstack_memory.h"
#include "btstack_tlv_none.h"
#include "btstack_uart.h"
#include "btstack_uart_block.h"
#include "hci_transport.h"
#include "hci_transport_h4.h"
}

namespace {

constexpr const char* kBleUartDeviceName = "rpi-bd-ble-uart";

// Fields: type, baud_rate, init_baud_rate, flowcontrol, device_name, parity.
// flowcontrol=1 enables CTS/RTS; init and runtime baud are both 921600 (no
// baud-rate switching required by this controller).
hci_transport_config_uart_t ble_uart_config = {
	HCI_TRANSPORT_CONFIG_UART,
	RPI_BD_BLE_UART_BAUD,
	RPI_BD_BLE_UART_BAUD,
	1,
	kBleUartDeviceName,
	BTSTACK_UART_PARITY_OFF,
};

c7222::BtstackPortTables CreateRpiBdBtstackPortTables() {
	const btstack_uart_block_t* uart    = btstack_uart_block_embedded_instance();
	const hci_transport_t*      transport = hci_transport_h4_instance(uart);
	// No bonding persistence: keys are lost on reset. Use a real TLV backend
	// (e.g. btstack_tlv_flash_bank) if reconnection without re-pairing is needed.
	const btstack_tlv_t*        tlv     = btstack_tlv_none_init_instance();

	return c7222::BtstackPortTables::CreateWithFreeRtosRunLoop(
		transport,
		&ble_uart_config,
		nullptr,  // no vendor chipset driver; controller speaks standard H4
		tlv,
		nullptr);
}

} // namespace

RpiBdBtstackPort::RpiBdBtstackPort()
	: c7222::BtstackPort(CreateRpiBdBtstackPortTables()) {
}

bool RpiBdBtstackPort::Init() {
	if(IsInitialized()) {
		return true;
	}

	// Must be called before Apply() writes tables into BTstack, as BTstack
	// accesses its memory pools immediately during HCI initialization.
	btstack_memory_init();
	SetInitialized(true);
	return true;
}

void RpiBdBtstackPort::Deinit() {
	if(!IsInitialized()) {
		return;
	}

	btstack_memory_deinit();
	SetInitialized(false);
}
