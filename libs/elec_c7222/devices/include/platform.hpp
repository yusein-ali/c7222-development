/**
 * @file platform.hpp
 * @brief Platform encapsulation for board-level devices.
 *
 * The Platform singleton centralizes architecture initialization and provides
 * convenient access to board-level devices (on-board LED, temperature sensor,
 * and PicoWBoard IO), plus Pico SDK timing helpers (sleep and tight-loop
 * utilities). This keeps application code small and consistent across
 * platforms.
 */
#ifndef ELEC_C7222_PLATFORM_HPP
#define ELEC_C7222_PLATFORM_HPP

#include <cstdint>
#include <memory>
#include <chrono>

#include "c7222_pico_w_board.hpp"
#include "onboard_led.hpp"
#include "onchip_temperature_sensor.hpp"

#include "led.hpp"
#include "button.hpp"
#include "pwm.hpp"



namespace c7222 {

/**
 * @class Platform
 * @brief Singleton access to platform-specific devices and initialization.
 *
 * Purpose:
 * - Centralizes **platform initialization** (Pico SDK / CYW43 setup) so
 *   application code does not need to call platform-specific init routines.
 * - Provides **convenience accessors** for common board devices (LEDs and
 *   buttons) while still allowing direct use of the dedicated device classes
 *   (`OnBoardLED`, `OnChipTemperatureSensor`, `PicoWBoard`, `Led`, `Button`).
 * - Exposes **timing helpers** for main-loop management (sleep and
 *   tight-loop utilities) in a consistent, platform-owned API.
 *
 * Dependencies:
 * - `OnBoardLED`: accessed via `GetOnBoardLed()`; must be explicitly
 *   initialized by the user.
 * - `OnChipTemperatureSensor`: accessed via `GetOnChipTemperatureSensor()`;
 *   must be explicitly initialized by the user.
 * - `PicoWBoard`: accessed via `GetPicoWBoard()` and the convenience LED/button
 *   helpers; provides higher-level board IO access.
 * - Pico timing helpers: exposed as static functions (see "Timing helpers").
 *
 * Pico platform initialization:
 * - `Initialize()` calls `EnsureArchInitialized()`, which initializes the
 *   Pico SDK architecture layer (including CYW43 on Pico W).
 * - Device classes remain explicit-init so callers stay in control of when
 *   hardware is configured.
 *
 * Proper usage:
 * - Always call `Platform::Initialize()` once early.
 * - Then explicitly initialize devices you intend to use (LED and temperature
 *   sensor).
 * - For board LEDs/buttons, use the convenience helpers or access `PicoWBoard`
 *   directly.
 *
 * Typical usage:
 * @code{.cpp}
 * auto* platform = c7222::Platform::GetInstance();
 * platform->Initialize();  // required before using any hardware-backed APIs
 * @endcode
 *
 * On-board LED (explicit init required):
 * @code{.cpp}
 * auto* platform = c7222::Platform::GetInstance();
 * platform->Initialize();
 * auto* led = platform->GetOnBoardLed();
 * led->Initialize();
 * led->On();
 * @endcode
 *
 * Temperature sensor (explicit init required):
 * @code{.cpp}
 * auto* platform = c7222::Platform::GetInstance();
 * platform->Initialize();
 * auto* sensor = platform->GetOnChipTemperatureSensor();
 * sensor->Initialize();
 * float temp_c = sensor->GetCelsius();
 * @endcode
 *
 * PicoWBoard LEDs and buttons:
 * @code{.cpp}
 * auto* platform = c7222::Platform::GetInstance();
 * platform->Initialize();
 *
 * // LEDs
 * platform->LedOn(c7222::PicoWBoard::LedId::LED1_RED);
 * platform->ToggleLed(c7222::PicoWBoard::LedId::LED1_GREEN);
 *
 * // PWM LED dimming (active-low board LEDs)
 * auto pwm = platform->CreateLedPwm(c7222::PicoWBoard::LedId::LED1_GREEN, 128);
 * // Use pwm while it owns the pin; call pwm->Enable(false) to release GPIO.
 *
 * // Buttons
 * bool pressed = platform->IsButtonPressed(c7222::PicoWBoard::ButtonId::BUTTON_B1);
 * platform->EnableButtonIrq(c7222::PicoWBoard::ButtonId::BUTTON_B1,
 *                           c7222::GpioInputEvent::FallingEdge,
 *                           [](uint32_t){ / * handle press * / });
 * @endcode
 *
 * Timing helpers:
 * @code{.cpp}
 * // Sleep for 250 ms between loop iterations.
 * while (true) {
 *   // ... do work ...
 *   c7222::Platform::SleepMs(250);
 * }
 * @endcode
 */
class Platform: public NonCopyableNonMovable {
  public:
	/**
	 * @brief Get the singleton instance.
	 */
	static Platform* GetInstance();

	/**
	 * @brief Initialize platform-specific hardware.
	 *
	 * This performs architecture-level initialization (e.g., CYW43 on Pico W).
	 * It does **not** automatically initialize the on-board LED or temperature
	 * sensor; those must be initialized explicitly by the user.
	 * @return true when initialization succeeds.
	 */
	bool Initialize();

	/**
	 * @brief Ensure the platform architecture is initialized.
	 * @return true when architecture init succeeds.
	 */
	bool EnsureArchInitialized();

	/**
	 * @name Timing helpers
	 *
	 * These static wrappers provide access to common Pico SDK timing utilities
	 * for main-loop control and low-power waiting. Callers may use these without
	 * a Platform instance.
	 */
	///@{
	/**
	 * @brief Sleep for a number of milliseconds.
	 * @param ms Milliseconds to sleep.
	 */
	static void SleepMs(uint32_t ms);

	/**
	 * @brief Sleep for a number of microseconds.
	 * @param us Microseconds to sleep.
	 */
	static void SleepUs(uint64_t us);

	/**
	 * @brief Sleep until a steady-clock time point.
	 * @param target Target time point.
	 */
	static void SleepUntil(std::chrono::steady_clock::time_point target);

	/**
	 * @brief Body for tight polling loops (no-op hook).
	 */
	static void TightLoopContents();
	///@}

	/**
	 * @brief Access the on-board LED.
	 *
	 * Note: Call `Initialize()` first. The LED's own `Initialize()` must be
	 * called explicitly by the user; Platform does not auto-initialize it.
	 *
	 * Example:
	 * @code{.cpp}
	 * auto* platform = c7222::Platform::GetInstance();
	 * platform->Initialize();
	 * auto* led = platform->GetOnBoardLed();
	 * led->Initialize();
	 * led->On();
	 * @endcode
	 */
	/**
	 * @brief Access the on-board LED singleton.
	 */
	OnBoardLED* GetOnBoardLed(){
		return OnBoardLED::GetInstance();
	}

	/**
	 * @brief Access the on-chip temperature sensor.
	 *
	 * Note: Call `Initialize()` first. The sensor's own `Initialize()` must be
	 * called explicitly by the user; Platform does not auto-initialize it.
	 *
	 * Example:
	 * @code{.cpp}
	 * auto* platform = c7222::Platform::GetInstance();
	 * platform->Initialize();
	 * auto* sensor = platform->GetOnChipTemperatureSensor();
	 * sensor->Initialize();
	 * float temp_c = sensor->GetCelsius();
	 * @endcode
	 */
	/**
	 * @brief Access the on-chip temperature sensor singleton.
	 */
	OnChipTemperatureSensor* GetOnChipTemperatureSensor(){
		return OnChipTemperatureSensor::GetInstance();
	}

	/**
	 * @brief Access the PicoWBoard singleton.
	 */
	PicoWBoard* GetPicoWBoard() {
		return PicoWBoard::GetInstance();
	}

	/**
	 * @brief Get a board LED by logical ID.
	 */
	Led& GetLed(PicoWBoard::LedId id) {
		return GetPicoWBoard()->GetLed(id);
	}

	/**
	 * @brief Get a board button by logical ID.
	 */
	Button& GetButton(PicoWBoard::ButtonId id) {
		return GetPicoWBoard()->GetButton(id);
	}

	/**
	 * @brief Create a PWM output for a board LED pin.
	 *
	 * @param id Board LED identifier.
	 * @param dim Brightness 0-255 (0 = off, 255 = fully on).
	 * @return Unique pointer to a PWM output configured for the LED pin.
	 *
	 * Notes:
	 * - Board LEDs are active-low, so the PWM output is configured as active-low.
	 * - Do not drive the same pin with `Led` and `PwmOut` at the same time.
	 *   If you previously used `Led`, stop using it and allow PWM to take over.
	 */
	std::unique_ptr<PwmOut> CreateLedPwm(PicoWBoard::LedId id, uint8_t dim);

	/**
	 * @brief Enable IRQs for a board button.
	 */
	void EnableButtonIrq(PicoWBoard::ButtonId id,
						 GpioInputEvent events,
						 std::function<void(uint32_t)> handler) {
		GetPicoWBoard()->GetButton(id).EnableIrq(events, std::move(handler));
	}

	/**
	 * @brief Disable IRQs for a board button.
	 */
	void DisableButtonIrq(PicoWBoard::ButtonId id) {
		GetPicoWBoard()->GetButton(id).DisableIrq();
	}

	/**
	 * @brief Read the current state of a board button (true if pressed).
	 */
	bool IsButtonPressed(PicoWBoard::ButtonId id) {
		return GetPicoWBoard()->GetButton(id).IsPressed();
	}

	/**
	 * @brief Toggle a board LED.
	 */
	void ToggleLed(PicoWBoard::LedId id) {
		GetPicoWBoard()->GetLed(id).Toggle();
	}

	/**
	 * @brief Turn a board LED on.
	 */
	void LedOn(PicoWBoard::LedId id) {
		GetPicoWBoard()->GetLed(id).On();
	}

	/**
	 * @brief Turn a board LED off.
	 */
	void LedOff(PicoWBoard::LedId id) {
		GetPicoWBoard()->GetLed(id).Off();
	}

	/**
	 * @brief Return true if platform initialization has completed.
	 */
	bool IsInitialized() const {
		return initialized_;
	}

  private:
	/**
	 * @brief Private constructor for singleton usage.
	 */
	Platform();
	~Platform() = default;
	/// Singleton instance (lazy-initialized).
	static Platform* instance_;

	/// True after successful `Initialize()`.
	bool initialized_{false};
	/// True after architecture initialization (e.g., CYW43).
	bool arch_initialized_{false};
	/// True after board IO has been initialized (if used).
	bool board_io_initialized_{false};

};

} // namespace c7222

#endif // ELEC_C7222_PLATFORM_HPP
