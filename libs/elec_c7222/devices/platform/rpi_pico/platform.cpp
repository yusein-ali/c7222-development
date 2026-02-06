// Raspberry Pi Pico platform implementation.
#include "platform.hpp"

#include <cstdint>

#include "pico/cyw43_arch.h"

#include "c7222_pico_w_board.h"


namespace c7222 {


bool Platform::EnsureArchInitialized() {
#if defined(CYW43_WL_GPIO_LED_PIN)
	if (arch_initialized_) {
		return true;
	}
	if (cyw43_arch_init() != 0) {
		arch_initialized_ = false;
		return false;
	}
	arch_initialized_ = true;
	return true;
#else
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
	const bool led_ok = OnBoardLED::GetInstance()->Initialize();
	const bool temp_ok = OnChipTemperatureSensor::GetInstance()->Initialize();
	initialized_ = led_ok && temp_ok;
	return initialized_;
}


bool Platform::InitializeBoardIo() {
	if(board_io_initialized_) {
		return true;
	}
	c7222_pico_w_board_init_gpio();
	board_io_initialized_ = true;
	return true;
}


Led Platform::CreateLed(BoardLed led, bool initial_on, GpioPin::DriveStrength drive) const {
	const uint32_t pin = static_cast<uint32_t>(static_cast<uint8_t>(led));
	return Led(pin, initial_on, drive, true);
}


Button Platform::CreateButton(BoardButton button, GpioPin::PullMode pull) const {
	const uint32_t pin = static_cast<uint32_t>(static_cast<uint8_t>(button));
	return Button(pin, pull);
}

} // namespace c7222
