// Simulated environment stub implementation.
#include "gpio.hpp"
#include <cassert>

namespace c7222 {
extern "C" {
bool c7222_grader_register_gpio_in(uint32_t pin, GpioIn* gpio_in);
void c7222_grader_unregister_gpio_in(uint32_t pin);
void c7222_grader_apply_gpio_in_config(uint32_t pin, const GpioIn::Config* config);
bool c7222_grader_read_gpio_in(uint32_t pin, GpioPullMode pull);

bool c7222_grader_register_gpio_out(uint32_t pin, GpioOut* gpio_out);
void c7222_grader_unregister_gpio_out(uint32_t pin);
void c7222_grader_apply_gpio_out_config(uint32_t pin, const GpioOut::Config* config);
void c7222_grader_write_gpio_out(uint32_t pin, bool level);
void c7222_grader_toggle_gpio_out(uint32_t pin);
}

GpioIn::GpioIn(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	const bool registered = c7222_grader_register_gpio_in(GetPin(), this);
	assert(registered && "GPIO pin already in use by another GpioIn instance");
	ApplyConfig();
}

GpioIn::~GpioIn() {
	c7222_grader_unregister_gpio_in(GetPin());
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
	return c7222_grader_read_gpio_in(GetPin(), config_.pull);
}

void GpioIn::ApplyConfig() {
	c7222_grader_apply_gpio_in_config(GetPin(), &config_);
}

GpioOut::GpioOut(uint32_t pin, const Config& config) : pin_(pin), config_(config) {
	assert(config_.Validate() && "Invalid GPIO configuration");
	const bool registered = c7222_grader_register_gpio_out(GetPin(), this);
	assert(registered && "GPIO pin already in use by another GpioOut instance");
	ApplyConfig();
}

GpioOut::~GpioOut() {
	c7222_grader_unregister_gpio_out(GetPin());
}

void GpioOut::Configure(const Config& config) {
	assert(config.Validate() && "Invalid GPIO configuration");
	config_ = config;
	ApplyConfig();
}

void GpioOut::Write(bool value) {
	c7222_grader_write_gpio_out(GetPin(), value);
}

void GpioOut::Toggle() {
	c7222_grader_toggle_gpio_out(GetPin());
}

void GpioOut::ApplyConfig() {
	c7222_grader_apply_gpio_out_config(GetPin(), &config_);
}

} // namespace c7222
