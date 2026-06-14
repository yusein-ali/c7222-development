#include "btstack_port.hpp"

#include <cassert>

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
	if(!Validate()) {
		assert(false && "BtstackPort::Apply requires at least one configured BTstack table");
		return false;
	}

	const auto& tables = GetTables();

	if(tables.hci_transport_config != nullptr && tables.hci_transport == nullptr) {
		assert(false && "BtstackPort::Apply requires hci_transport when hci_transport_config is set");
		return false;
	}

	if(tables.chipset != nullptr && tables.hci_transport == nullptr) {
		assert(false && "BtstackPort::Apply requires hci_transport before chipset can be applied");
		return false;
	}

	if(tables.tlv_context != nullptr && tables.tlv == nullptr) {
		assert(false && "BtstackPort::Apply requires tlv when tlv_context is set");
		return false;
	}

#if !defined(C7222_BLE_HAS_BTSTACK)
	assert(false && "BtstackPort::Apply requires a BTstack-capable platform");
	return false;
#else
	if(tables.run_loop != nullptr) {
		btstack_run_loop_init(static_cast<const btstack_run_loop_t*>(tables.run_loop));
	}

	if(tables.tlv != nullptr) {
		auto* tlv = static_cast<const btstack_tlv_t*>(tables.tlv);
		btstack_tlv_set_instance(tlv, tables.tlv_context);
		le_device_db_tlv_configure(tlv, tables.tlv_context);
	}

	if(tables.hci_transport != nullptr) {
		hci_init(static_cast<const hci_transport_t*>(tables.hci_transport),
				 tables.hci_transport_config);
	}

	if(tables.chipset != nullptr) {
		hci_set_chipset(static_cast<const btstack_chipset_t*>(tables.chipset));
	}

	return true;
#endif
}

} // namespace c7222
