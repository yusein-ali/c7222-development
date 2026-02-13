// Simulated environment stub implementation.
#include "gpio.hpp"
#include <cassert>
#include <map>

namespace c7222 {
namespace {
static std::map<uint32_t, GpioIn*> gpio_in_to_object_map{};
static std::map<uint32_t, GpioOut*> gpio_out_to_object_map{};
} // namespace

GpioIn::GpioIn(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	auto it = gpio_in_to_object_map.find(GetPin());
	assert(it == gpio_in_to_object_map.end() && "GPIO pin already in use by another GpioIn instance");
	gpio_in_to_object_map[GetPin()] = this;
	ApplyConfig();
}

GpioIn::~GpioIn() {
	gpio_in_to_object_map.erase(GetPin());
}

void GpioIn::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}

void GpioIn::EnableIrq(GpioInputEvent events, std::function<void(uint32_t)> handler) {
	assert(events != GpioInputEvent::None && "At least one event must be specified to enable IRQ");
	assert(handler != nullptr && "Handler must be provided to enable IRQ");
	config_.input_events = events;
	config_.irq_handler = std::move(handler);
}

void GpioIn::DisableIrq() {
	config_.input_events = GpioInputEvent::None;
	config_.irq_handler = nullptr;
}

bool GpioIn::Read() const {
	return false;
}

void GpioIn::ApplyConfig() {}

GpioOut::GpioOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	auto it = gpio_out_to_object_map.find(GetPin());
	assert(it == gpio_out_to_object_map.end() && "GPIO pin already in use by another GpioOut instance");
	gpio_out_to_object_map[GetPin()] = this;
	ApplyConfig();
}

GpioOut::~GpioOut() {
	gpio_out_to_object_map.erase(GetPin());
}

void GpioOut::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}

void GpioOut::Write(bool) {}

void GpioOut::Toggle() {}

void GpioOut::ApplyConfig() {}

} // namespace c7222
