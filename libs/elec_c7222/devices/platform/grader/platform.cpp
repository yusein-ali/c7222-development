// Grader platform implementation (no hardware init).
#include "platform.hpp"

#include <cstdint>

namespace c7222 {

bool Platform::EnsureArchInitialized() {
	arch_initialized_ = true;
	return true;
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
	if (board_io_initialized_) {
		return true;
	}
	board_io_initialized_ = true;
	return true;
}

Led Platform::CreateLed(BoardLed led, bool initial_on, GpioPin::DriveStrength drive) const {
	const uint32_t pin = static_cast<uint32_t>(static_cast<uint8_t>(led));
	// Keep semantics consistent with the real board (board LEDs are active-low).
	return Led(pin, initial_on, drive, true);
}

Button Platform::CreateButton(BoardButton button, GpioPin::PullMode pull) const {
	const uint32_t pin = static_cast<uint32_t>(static_cast<uint8_t>(button));
	return Button(pin, pull);
}

} // namespace c7222
