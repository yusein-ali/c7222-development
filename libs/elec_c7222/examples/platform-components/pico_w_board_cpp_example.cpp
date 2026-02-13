// Example: Using the PicoWBoard C++ API.
#include "pico/stdlib.h"

#include "c7222_pico_w_board.hpp"

int main() {
	stdio_init_all();

	c7222::PicoWBoard board;

	auto& led_red = board.GetLed(c7222::PicoWBoard::LedId::LED1_RED);
	auto& led_green = board.GetLed(c7222::PicoWBoard::LedId::LED1_GREEN);

	for(;;) {
		led_red.On();
		led_green.Off();
		sleep_ms(250);

		led_red.Off();
		led_green.On();
		sleep_ms(250);
	}
}
