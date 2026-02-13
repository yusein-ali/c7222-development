// Platform implementation (shared utilities).
#include "platform.hpp"
#include <cassert>

namespace c7222 {

Platform* Platform::instance_{nullptr};

Platform* Platform::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new Platform();
		assert(instance_ != nullptr && "Failed to allocate Platform singleton instance");
	}
	return instance_;
}

Platform::Platform(): initialized_(false), arch_initialized_(false) {

}

std::unique_ptr<PwmOut> Platform::CreateLedPwm(PicoWBoard::LedId id, uint8_t dim) {
	const uint32_t pin = static_cast<uint32_t>(static_cast<uint8_t>(id));
	const float duty = static_cast<float>(dim) / 255.0f;
	PwmOut::Config cfg(pin);
	cfg.period_us = 1000.0f;
	cfg.duty_cycle = duty;
	cfg.active_low = true;
	cfg.enabled = true;
	return std::make_unique<PwmOut>(pin, cfg);
}

} // namespace c7222
