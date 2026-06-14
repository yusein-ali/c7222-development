// Raspberry Pi Pico platform implementation.
#include "platform.hpp"

#include <cstdint>
#include <chrono>

#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/cyw43_arch.h"
#include "ble/le_device_db_tlv.h"
#include "btstack_tlv.h"
#include "btstack_tlv_none.h"

#include "c7222_pico_w_board.hpp"

#define C7222_HAS_FREERTOS 0
#if defined(CYW43_WL_GPIO_LED_PIN) && __has_include("FreeRTOS.h")
#undef C7222_HAS_FREERTOS
#define C7222_HAS_FREERTOS 1
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#endif

namespace {
	#if defined(CYW43_WL_GPIO_LED_PIN)
	#if C7222_HAS_FREERTOS
static bool cyw43_init_timer_started = false;
static bool timer_run = false;
static TimerHandle_t cyw43_init_timer = nullptr;

static void cyw43_arch_init_timer_callback(TimerHandle_t timer) {
	(void)timer;
	auto* platform = c7222::Platform::GetInstance();
	timer_run = true;
	(void)platform->EnsureArchInitialized();
	if(cyw43_init_timer){
		(void) xTimerDelete(cyw43_init_timer, 0);
		cyw43_init_timer = nullptr;
	}
}
void DisableBtstackPersistenceStorage() {
	static bool persistence_disabled = false;
	if(persistence_disabled) {
		return;
	}

	const btstack_tlv_t* tlv_none = btstack_tlv_none_init_instance();
	btstack_tlv_set_instance(tlv_none, nullptr);
	le_device_db_tlv_configure(tlv_none, nullptr);
	persistence_disabled = true;
}
#endif
#endif

}

namespace c7222 {


bool Platform::EnsureArchInitialized() {
#if defined(CYW43_WL_GPIO_LED_PIN)
	if (arch_initialized_) {
		return true;
	}
	#if !defined(C7222_ENABLE_BLE)
		timer_run = true;
	#endif
	#if C7222_HAS_FREERTOS
	if(timer_run) {
		if(cyw43_arch_init() != 0) {
			timer_run = false;
			arch_initialized_ = false;
			return false;
		}
		DisableBtstackPersistenceStorage();
		arch_initialized_ = true;
		return true;
	} else {
		stdio_init_all();
		if(!cyw43_init_timer_started) {
			if(!cyw43_init_timer) {
				cyw43_init_timer = xTimerCreate("cyw43_init",
												pdMS_TO_TICKS(1),
												pdFALSE,
												nullptr,
												cyw43_arch_init_timer_callback);
				assert(cyw43_init_timer != nullptr && "Failed to create CYW43 init timer");
			}

			if(cyw43_init_timer == nullptr) {
				arch_initialized_ = false;
				return false;
			}
			(void) xTimerStart(cyw43_init_timer, 0);
			cyw43_init_timer_started = true;
			return true;
		} else if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
			return true;
		} else {
			uint32_t wait_ms = 0;
			while(arch_initialized_ == false) {
				vTaskDelay(pdMS_TO_TICKS(10));
				wait_ms += 10;
				if(wait_ms > 5000) {
					assert(false && "CYW43 architecture initialization timed out");
					return false;
				}
			}
			return arch_initialized_;
		}
	}
	#else // no freertos, safe to init directly
	stdio_init_all();
	if(cyw43_arch_init() != 0) {
		arch_initialized_ = false;
		return false;
	}
		arch_initialized_ = true;
	return true;
	#endif
#else // no cyw43, just mark as initialized
	stdio_init_all();
	arch_initialized_ = true;
	return true;
#endif
}

bool Platform::Initialize() {
	if (initialized_) {
		return true;
	}
	if (!EnsureArchInitialized()) {
		initialized_ = false;
		return false;
	}
	initialized_ = true;
	return initialized_;
}

void Platform::SleepMs(uint32_t ms) {
	sleep_ms(ms);
}

void Platform::SleepUs(uint64_t us) {
	sleep_us(us);
}

void Platform::SleepUntil(std::chrono::steady_clock::time_point target) {
	const auto now = std::chrono::steady_clock::now();
	if (target <= now) {
		return;
	}
	const auto delta = target - now;
	const auto us = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
	sleep_until(delayed_by_us(get_absolute_time(), static_cast<uint64_t>(us)));
}

void Platform::TightLoopContents() {
	tight_loop_contents();
}




} // namespace c7222
