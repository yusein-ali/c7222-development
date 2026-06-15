#include "ble.hpp"

#include <cassert>

#include "platform.hpp"

namespace c7222 {

#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
namespace {
constexpr std::uint32_t kBtstackRunLoopTaskStackDepthWords = 2048;
constexpr std::uint32_t kBtstackRunLoopTaskPriorityOffset = 2;
} // namespace
#endif

Ble* Ble::instance_ = nullptr;

Ble* Ble::GetInstance(bool enable_hci_logging, BtstackPort* btstack_port) {
	if(instance_ == nullptr) {
		instance_ = new Ble(btstack_port);
	} else if(btstack_port != nullptr) {
		if(instance_->btstack_port_ == nullptr) {
			instance_->btstack_port_ = btstack_port;
		}
		assert(instance_->btstack_port_ == btstack_port &&
			   "Ble singleton was already created with a different BtstackPort");
	}
	assert(instance_ != nullptr && "Failed to allocate Ble singleton instance");
	// make sure that platform is initialized before enabling HCI logging, as it may be required for logging to work properly.
	auto* platform = Platform::GetInstance();
	assert(platform != nullptr && "Platform singleton instance is null in Ble::GetInstance");
	const bool platform_initialized = platform->EnsureArchInitialized();
	assert(platform_initialized && "Failed to initialize platform in Ble::GetInstance");
	(void)platform_initialized;
	// Enable HCI logging if requested (requires platform initialization).
	if(enable_hci_logging) {
		instance_->EnableHCILoggingToStdout();
	}
	return instance_;
}

void Ble::EnsureBtstackPortInitialized() {
	assert(btstack_port_ != nullptr && "Ble requires a non-null BtstackPort");
	assert(btstack_port_->Validate() && "BtstackPort validation failed");
	if(!btstack_port_->IsInitialized()) {
#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
		if(btstack_port_->UsesFreeRtosRunLoop()) {
			StartBtstackRunLoopTask();
			return;
		}
#endif
		const bool initialized = btstack_port_->Init();
		assert(initialized && "BtstackPort initialization failed");
		(void)initialized;

		if(btstack_port_->GetTables().Validate()) {
			const bool applied = btstack_port_->Apply();
			assert(applied && "BtstackPort settings could not be applied to BTstack");
			(void)applied;
		}
	}
}

#if defined(C7222_BLE_HAS_BTSTACK_FREERTOS_RUN_LOOP)
void Ble::StartBtstackRunLoopTask() {
	assert(btstack_port_ != nullptr && "Ble requires a non-null BtstackPort");
	if(btstack_run_loop_task_.IsValid()) {
		while(!btstack_run_loop_ready_.load() && !btstack_run_loop_failed_.load()) {
			FreeRtosTask::Delay(FreeRtosTask::MsToTicks(1));
		}
		assert(!btstack_run_loop_failed_.load() && "BTstack FreeRTOS run-loop task failed");
		return;
	}

	btstack_run_loop_ready_.store(false);
	btstack_run_loop_failed_.store(false);
	const bool created = btstack_run_loop_task_.Initialize(
		"BTstack",
		kBtstackRunLoopTaskStackDepthWords,
		FreeRtosTask::IdlePriority() + kBtstackRunLoopTaskPriorityOffset,
		[this](void*) { BtstackRunLoopTaskBody(); },
		nullptr);
	assert(created && "Failed to create BTstack FreeRTOS run-loop task");
	if(!created) {
		btstack_run_loop_failed_.store(true);
		return;
	}

	while(!btstack_run_loop_ready_.load() && !btstack_run_loop_failed_.load()) {
		FreeRtosTask::Delay(FreeRtosTask::MsToTicks(1));
	}
	assert(!btstack_run_loop_failed_.load() && "BTstack FreeRTOS run-loop task failed");
}

void Ble::BtstackRunLoopTaskBody() {
	bool ok = btstack_port_ != nullptr && btstack_port_->Validate();
	if(ok && !btstack_port_->IsInitialized()) {
		ok = btstack_port_->Init();
	}
	if(ok && btstack_port_->GetTables().Validate()) {
		ok = btstack_port_->Apply();
	}

	if(!ok) {
		btstack_run_loop_failed_.store(true);
		return;
	}

	btstack_run_loop_ready_.store(true);
	btstack_port_->ExecuteRunLoop();
}
#endif

void Ble::SetDeviceName(const std::string& name) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(
		AdvertisementData(AdvertisementDataType::kCompleteLocalName, name.c_str(), name.size()));
	bool ok = builder.Build();
	assert(ok && "AdvertisementDataBuilder contains invalid data after setting device name.");
	device_name_ = name;
}

void Ble::SetAdvertisementFlags(uint8_t flags) {
	if(gap_ == nullptr) {
		return;
	}
	auto& builder = gap_->GetAdvertisementDataBuilder();
	builder.ReplaceOrAdd(AdvertisementData(AdvertisementDataType::kFlags, &flags, sizeof(flags)));
	bool ok = builder.Build();
	assert(ok && "AdvertisementDataBuilder contains invalid data after setting flags.");
	advertisement_flags_ = flags;
}

AttributeServer* Ble::EnableAttributeServer(const void* context) {
	if(attribute_server_ != nullptr) {
		return attribute_server_;
	}
	if(attribute_server_ == nullptr) {
		EnsureBtstackPortInitialized();
		attribute_server_ = AttributeServer::GetInstance();
		attribute_server_->Init(context);

		const bool requires_encryption = attribute_server_->HasServicesRequiringEncryption();
		const bool requires_authentication =
			attribute_server_->HasServicesRequiringAuthentication();
		const bool requires_authorization =
			attribute_server_->HasServicesRequiringAuthorization();

		if(requires_encryption || requires_authentication || requires_authorization) {
			assert(security_manager_ != nullptr &&
				   "SecurityManager is required: AttributeServer contains secured characteristics. "
				   "Call Ble::EnableSecurityManager() before enabling the attribute server.");
		}

		if(security_manager_ != nullptr &&
		   (requires_encryption || requires_authentication || requires_authorization)) {
			assert(security_manager_->IsConfigured() &&
				   "SecurityManager must be configured before enabling a secured AttributeServer.");
			const auto handler_count = security_manager_->GetEventHandlerCount();
			assert(handler_count > 0 && "SecurityManager must have at least one event handler registered.");
			assert(security_manager_->ValidateConfiguration(requires_authentication,
															requires_authorization,
															requires_encryption) &&
				   "SecurityManager configuration mismatch: requirements cannot be satisfied "
				   "with current SecurityManager settings.");
		}
	}
	return attribute_server_;
}

SecurityManager* Ble::EnableSecurityManager(const SecurityManager::SecurityParameters& params) {
	if(security_manager_ == nullptr) {
		security_manager_ = SecurityManager::GetInstance();
		security_manager_->Configure(params);
		if(turned_on_) {
			EnsureSmEventHandlerRegistered();
		}
	}
	if(attribute_server_ != nullptr) {
		const bool requires_encryption = attribute_server_->HasServicesRequiringEncryption();
		const bool requires_authentication =
			attribute_server_->HasServicesRequiringAuthentication();
		const bool requires_authorization =
			attribute_server_->HasServicesRequiringAuthorization();
		assert(security_manager_->ValidateConfiguration(requires_authentication,
														requires_authorization,
														requires_encryption) &&
			   "SecurityManager configuration mismatch: requirements cannot be satisfied "
			   "with current SecurityManager settings.");
	}
	return security_manager_;
}

}  // namespace c7222
