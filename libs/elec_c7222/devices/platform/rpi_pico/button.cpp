// Raspberry Pi Pico implementation of Button (GPIO IRQ dispatch).
#include "button.hpp"

#include "hardware/gpio.h"

#include <cassert>
#include <list>
#include <map>

namespace c7222 {

namespace {

// Route GPIO interrupts to Button instances (keyed by GPIO number).
static std::map<uint32_t, std::list<Button*>> g_buttons_by_gpio{};

static bool any_handler_for_pin(uint32_t pin) {
	auto it = g_buttons_by_gpio.find(pin);
	if(it == g_buttons_by_gpio.end()) {
		return false;
	}
	for(Button* btn : it->second) {
		if(btn != nullptr && btn->HasHandler()) {
			return true;
		}
	}
	return false;
}

// Pico SDK callback (single global callback) that forwards to Button instances.
static void pico_gpio_irq_callback(uint gpio, uint32_t events) {
	auto it = g_buttons_by_gpio.find(static_cast<uint32_t>(gpio));
	if(it == g_buttons_by_gpio.end()) {
		return;
	}

	for(Button* btn : it->second) {
		if(btn != nullptr) {
			btn->CallIrqHandler(events);
		}
	}
}

} // namespace

void Button::UnregisterHandler() {
	PlatformUnregister(this);
	EnableIrq(false);
	handler_ = nullptr;
}

void Button::EnableIrq(bool enable) {
	// Only falling edge is relevant for the C7222 board buttons.
	if(enable) {
		// Enable only if any button on this pin has a handler.
		if(!any_handler_for_pin(GetPin())) {
			return;
		}
	} else {
		// Do not disable IRQ if other buttons on the same pin still have handlers.
		if(any_handler_for_pin(GetPin())) {
			return;
		}
	}
	gpio_set_irq_enabled_with_callback(GetPin(),
	                                  GPIO_IRQ_EDGE_FALL,
	                                  enable,
	                                  pico_gpio_irq_callback);
}

void Button::PlatformRegister(Button* button) {
	if(button == nullptr) {
		return;
	}
	g_buttons_by_gpio[button->GetPin()].push_back(button);
}

void Button::PlatformUnregister(Button* button) {
	if(button == nullptr) {
		return;
	}
	auto it = g_buttons_by_gpio.find(button->GetPin());
	if(it == g_buttons_by_gpio.end()) {
		return;
	}

	auto& list = it->second;
	list.remove(button);
	if(list.empty()) {
		g_buttons_by_gpio.erase(it);
	}
}

} // namespace c7222
