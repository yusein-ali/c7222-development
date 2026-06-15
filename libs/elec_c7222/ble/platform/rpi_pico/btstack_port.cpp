#include "btstack_port.hpp"

#include <cassert>
#include <cstdint>

#include "pico/time.h"

#if defined(C7222_BLE_HAS_BTSTACK)
extern "C" {
#include "ble/le_device_db_tlv.h"
#include "btstack_run_loop.h"
#include "btstack_tlv.h"
#include "hci.h"

const btstack_run_loop_t* btstack_run_loop_freertos_get_instance(void);
}
#endif

#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
extern "C" uint32_t hal_time_ms(void) {
	return static_cast<uint32_t>(to_ms_since_boot(get_absolute_time()));
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
	if(!GetTables().Validate()) {
		return true;
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

bool BtstackPort::Validate() const {
	// Pico W/CYW43 BTstack setup is owned by the Pico SDK CYW43 BTstack port.
	// Empty C7222 BTstack tables are therefore valid for this platform.
	return true;
}

bool BtstackPort::UsesFreeRtosRunLoop() const {
#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
	return GetRunLoop() == btstack_run_loop_freertos_get_instance();
#else
	return false;
#endif
}

void BtstackPort::ExecuteRunLoop() {
#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
	btstack_run_loop_execute();
#endif
}

} // namespace c7222
