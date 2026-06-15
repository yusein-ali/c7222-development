#include "btstack_port.hpp"

#if defined(C7222_BLE_HAS_BTSTACK)
extern "C" {
#include "ble/le_device_db_tlv.h"
#include "btstack_run_loop.h"
#include "btstack_tlv.h"
#include "hci.h"

const btstack_run_loop_t* btstack_run_loop_freertos_get_instance(void);
}
#endif

namespace c7222 {

BtstackPortTables BtstackPortTables::CreateWithFreeRtosRunLoop() {
	BtstackPortTables tables{};
#if defined(C7222_BLE_HAS_BTSTACK)
	tables.run_loop = btstack_run_loop_freertos_get_instance();
#endif
	return tables;
}

BtstackPortTables BtstackPortTables::CreateWithFreeRtosRunLoop(
	const void* hci_transport,
	const void* hci_transport_config,
	const void* chipset,
	const void* tlv,
	void* tlv_context) {
	BtstackPortTables tables = CreateWithFreeRtosRunLoop();
	tables.hci_transport = hci_transport;
	tables.hci_transport_config = hci_transport_config;
	tables.chipset = chipset;
	tables.tlv = tlv;
	tables.tlv_context = tlv_context;
	return tables;
}

bool BtstackPort::Apply() {
	return true;
}

bool BtstackPort::Validate() const {
	return true;
}

bool BtstackPort::UsesFreeRtosRunLoop() const {
	return false;
}

void BtstackPort::ExecuteRunLoop() {
}

} // namespace c7222
