// Pico W board helper implementation (shared).
#include "c7222_pico_w_board.hpp"

#include <cassert>

#include "platform.hpp"

namespace c7222 {
namespace {

constexpr PicoWBoard::LedId kLedIds[] = {
	PicoWBoard::LedId::LED1_RED,
	PicoWBoard::LedId::LED1_GREEN,
	PicoWBoard::LedId::LED2_RED,
	PicoWBoard::LedId::LED2_GREEN,
	PicoWBoard::LedId::LED3_RED,
	PicoWBoard::LedId::LED3_GREEN
};

constexpr PicoWBoard::ButtonId kButtonIds[] = {
	PicoWBoard::ButtonId::BUTTON_B1,
	PicoWBoard::ButtonId::BUTTON_B2,
	PicoWBoard::ButtonId::BUTTON_B3,
	PicoWBoard::ButtonId::BUTTON_B4
};

int LedIndex(PicoWBoard::LedId id) {
	for(size_t i = 0; i < PicoWBoard::kLedCount; ++i) {
		if(kLedIds[i] == id) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

int ButtonIndex(PicoWBoard::ButtonId id) {
	for(size_t i = 0; i < PicoWBoard::kButtonCount; ++i) {
		if(kButtonIds[i] == id) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

} // namespace
std::unique_ptr<PicoWBoard> PicoWBoard::instance_ = nullptr;

PicoWBoard::PicoWBoard()
	: initialized_(true),
	  leds_{{
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED1_RED)), false, GpioDriveStrength::mA4, true),
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED1_GREEN)), false, GpioDriveStrength::mA4, true),
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED2_RED)), false, GpioDriveStrength::mA4, true),
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED2_GREEN)), false, GpioDriveStrength::mA4, true),
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED3_RED)), false, GpioDriveStrength::mA4, true),
		  Led(static_cast<uint32_t>(static_cast<uint8_t>(LedId::LED3_GREEN)), false, GpioDriveStrength::mA4, true)
	  }},
	  buttons_{{
		  Button(static_cast<uint32_t>(static_cast<uint8_t>(ButtonId::BUTTON_B1)), GpioPullMode::PullUp),
		  Button(static_cast<uint32_t>(static_cast<uint8_t>(ButtonId::BUTTON_B2)), GpioPullMode::PullUp),
		  Button(static_cast<uint32_t>(static_cast<uint8_t>(ButtonId::BUTTON_B3)), GpioPullMode::PullUp),
		  Button(static_cast<uint32_t>(static_cast<uint8_t>(ButtonId::BUTTON_B4)), GpioPullMode::PullUp)
	  }} {
	assert(Platform::GetInstance()->IsInitialized() && "Platform must be initialized before PicoWBoard");
}

PicoWBoard* PicoWBoard::GetInstance() {
	if(!instance_) {
		instance_ = std::unique_ptr<PicoWBoard>(new PicoWBoard());
	}
	return instance_.get();
}

Led& PicoWBoard::GetLed(LedId id) {
	assert(initialized_ && "PicoWBoard not initialized");
	const int idx = LedIndex(id);
	assert(idx >= 0 && static_cast<size_t>(idx) < leds_.size() && "Invalid LED id");
	return leds_[static_cast<size_t>(idx)];
}

Button& PicoWBoard::GetButton(ButtonId id) {
	assert(initialized_ && "PicoWBoard not initialized");
	const int idx = ButtonIndex(id);
	assert(idx >= 0 && static_cast<size_t>(idx) < buttons_.size() && "Invalid button id");
	return buttons_[static_cast<size_t>(idx)];
}


} // namespace c7222
