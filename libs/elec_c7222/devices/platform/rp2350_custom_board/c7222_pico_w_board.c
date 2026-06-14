#include "c7222_pico_w_board.h"

#include <assert.h>

#include "hardware/gpio.h"

#if defined(CYW43_WL_GPIO_LED_PIN)
#include "pico/cyw43_arch.h"
#endif

static bool c7222_board_initialized = false;
static bool pico_w_onboard_led_initialized = false;

void c7222_pico_w_board_init_gpio(void) {
	gpio_init((uint) C7222_PICO_W_LED3_GREEN);
	gpio_set_dir((uint) C7222_PICO_W_LED3_GREEN, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED3_GREEN, 1);
	gpio_init((uint) C7222_PICO_W_LED3_RED);
	gpio_set_dir((uint) C7222_PICO_W_LED3_RED, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED3_RED, 1);
	gpio_init((uint) C7222_PICO_W_LED2_GREEN);
	gpio_set_dir((uint) C7222_PICO_W_LED2_GREEN, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED2_GREEN, 1);
	gpio_init((uint) C7222_PICO_W_LED2_RED);
	gpio_set_dir((uint) C7222_PICO_W_LED2_RED, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED2_RED, 1);
	gpio_init((uint) C7222_PICO_W_LED1_GREEN);
	gpio_set_dir((uint) C7222_PICO_W_LED1_GREEN, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED1_GREEN, 1);
	gpio_init((uint) C7222_PICO_W_LED1_RED);
	gpio_set_dir((uint) C7222_PICO_W_LED1_RED, GPIO_OUT);
	gpio_put((uint) C7222_PICO_W_LED1_RED, 1);

	gpio_init((uint) C7222_PICO_W_BUTTON_B1);
	gpio_set_dir((uint) C7222_PICO_W_BUTTON_B1, GPIO_IN);
	gpio_pull_up((uint) C7222_PICO_W_BUTTON_B1);
	gpio_init((uint) C7222_PICO_W_BUTTON_B2);
	gpio_set_dir((uint) C7222_PICO_W_BUTTON_B2, GPIO_IN);
	gpio_pull_up((uint) C7222_PICO_W_BUTTON_B2);
	gpio_init((uint) C7222_PICO_W_BUTTON_B3);
	gpio_set_dir((uint) C7222_PICO_W_BUTTON_B3, GPIO_IN);
	gpio_pull_up((uint) C7222_PICO_W_BUTTON_B3);
	gpio_init((uint) C7222_PICO_W_BUTTON_B4);
	gpio_set_dir((uint) C7222_PICO_W_BUTTON_B4, GPIO_IN);
	gpio_pull_up((uint) C7222_PICO_W_BUTTON_B4);

	c7222_board_initialized = true;
}

void c7222_pico_w_board_init_led(enum c7222_led_type led) {
	(void)led;
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_init_led: call c7222_pico_w_board_init_gpio() first");
}

void c7222_pico_w_board_button_init(enum c7222_button_type button,
									void (*handler)(uint32_t, uint32_t),
									uint32_t events,
									bool enabled) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_button_init: call c7222_pico_w_board_init_gpio() first");
	gpio_init((uint) button);
	gpio_set_dir((uint) button, GPIO_IN);
	gpio_pull_up((uint) button);

	if(handler != NULL) {
		gpio_set_irq_enabled_with_callback((uint) button,
										   events,
										   enabled,
										   (gpio_irq_callback_t) handler);
	}
	if(!enabled) {
		gpio_set_irq_enabled((uint) button, events, false);
	}
}

bool c7222_pico_w_board_button_read(enum c7222_button_type button) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_button_read: call c7222_pico_w_board_init_gpio() first");
	return gpio_get((uint) button);
}

bool c7222_pico_w_board_led_read(enum c7222_led_type led) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_led_read: call c7222_pico_w_board_init_gpio() first");
	return !gpio_get((uint) led);
}

void c7222_pico_w_board_led_on(enum c7222_led_type led) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_led_on: call c7222_pico_w_board_init_gpio() first");
	gpio_put((uint) led, 0);
}

void c7222_pico_w_board_led_off(enum c7222_led_type led) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_led_off: call c7222_pico_w_board_init_gpio() first");
	gpio_put((uint) led, 1);
}

void c7222_pico_w_board_led_toggle(enum c7222_led_type led) {
	assert(c7222_board_initialized &&
		   "c7222_pico_w_board_led_toggle: call c7222_pico_w_board_init_gpio() first");
	if(c7222_pico_w_board_led_read(led)) {
		c7222_pico_w_board_led_off(led);
	} else {
		c7222_pico_w_board_led_on(led);
	}
}

bool c7222_pico_w_onboard_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
	pico_w_onboard_led_initialized = true;
	return true;
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
	pico_w_onboard_led_initialized = true;
	return true;
#else
	return false;
#endif
}

void c7222_pico_w_onboard_led_set(bool on) {
	assert(pico_w_onboard_led_initialized &&
		   "c7222_pico_w_onboard_led_set: call c7222_pico_w_onboard_led_init() first");
#if defined(PICO_DEFAULT_LED_PIN)
	gpio_put(PICO_DEFAULT_LED_PIN, on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
#else
	(void) on;
#endif
}

void c7222_pico_w_onboard_led_on(void) {
	assert(pico_w_onboard_led_initialized &&
		   "c7222_pico_w_onboard_led_on: call c7222_pico_w_onboard_led_init() first");
	c7222_pico_w_onboard_led_set(true);
}

void c7222_pico_w_onboard_led_off(void) {
	assert(pico_w_onboard_led_initialized &&
		   "c7222_pico_w_onboard_led_off: call c7222_pico_w_onboard_led_init() first");
	c7222_pico_w_onboard_led_set(false);
}

void c7222_pico_w_onboard_led_toggle(void) {
	assert(pico_w_onboard_led_initialized &&
		   "c7222_pico_w_onboard_led_toggle: call c7222_pico_w_onboard_led_init() first");
	bool current = c7222_pico_w_onboard_led_read();
	c7222_pico_w_onboard_led_set(!current);
}

bool c7222_pico_w_onboard_led_read(void) {
	assert(pico_w_onboard_led_initialized &&
		   "c7222_pico_w_onboard_led_read: call c7222_pico_w_onboard_led_init() first");
#if defined(PICO_DEFAULT_LED_PIN)
	return gpio_get(PICO_DEFAULT_LED_PIN);
#elif defined(CYW43_WL_GPIO_LED_PIN)
	// No direct read API; track state externally if needed.
	return false;
#else
	return false;
#endif
}
