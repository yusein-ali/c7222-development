// Raspberry Pi Pico implementation of Button (GPIO IRQ dispatch).
#include "button.hpp"

#include "hardware/gpio.h"

#include <array>
#include <cassert>

namespace c7222 {

namespace {

// Route GPIO interrupts to Button instances (indexed by GPIO number).
static std::array<Button*, NUM_BANK0_GPIOS> g_button_by_gpio{};

// Pico SDK callback (single global callback) that forwards to Button.
static void pico_gpio_irq_callback(uint gpio, uint32_t events) {
	Button::IrqTrampoline(static_cast<uint32_t>(gpio), events);
}

} // namespace

Button::Config Button::MakeConfig(uint32_t pin, PullMode pull) {
	Config cfg(static_cast<int32_t>(pin));
	cfg.direction = Direction::Input;
	cfg.pull = pull;
	cfg.output_type = OutputType::PushPull;
	cfg.drive = DriveStrength::mA4;
	cfg.initial_state = false;
	return cfg;
}

Button::Button(uint32_t pin, PullMode pull)
	: GpioPin(pin, MakeConfig(pin, pull)) {}

void Button::RegisterHandler(Handler handler, void* user_data, bool enable) {
	_handler = handler;
	_user_data = user_data;

	if(GetPin() < g_button_by_gpio.size()) {
		g_button_by_gpio[GetPin()] = this;
	}

	if(_handler == nullptr) {
		EnableIrq(false);
		return;
	}

	EnableIrq(enable);
}

void Button::UnregisterHandler() {
	EnableIrq(false);
	_handler = nullptr;
	_user_data = nullptr;
	if(GetPin() < g_button_by_gpio.size()) {
		g_button_by_gpio[GetPin()] = nullptr;
	}
}

void Button::EnableIrq(bool enable) {
	// Only falling edge is relevant for the C7222 board buttons.
	gpio_set_irq_enabled_with_callback(GetPin(),
	                                  GPIO_IRQ_EDGE_FALL,
	                                  enable,
	                                  pico_gpio_irq_callback);
}

bool Button::IsPressed() const {
	// Active-low by default when using pull-up.
	return !Read();
}

void Button::InvokeHandlerFromIrq() {
	if(_handler != nullptr) {
		_handler(_user_data);
	}
}

void Button::IrqTrampoline(uint32_t gpio, uint32_t events) {
	if(gpio >= g_button_by_gpio.size()) {
		return;
	}

	Button* btn = g_button_by_gpio[gpio];
	if(btn == nullptr) {
		return;
	}

	if((events & GPIO_IRQ_EDGE_FALL) != 0u) {
		btn->InvokeHandlerFromIrq();
	}
}

} // namespace c7222
