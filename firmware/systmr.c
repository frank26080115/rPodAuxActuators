#include "systmr.h"

volatile systick_t systmr_ticks = 0;

void systmr_init()
{
}

systmr_t systmr_nowMillis()
{
	systick_t x;
	cli();
	x = systmr_ticks;
	sei();
	// mathematics setup for 16 MHz with no clock divider on the timers
	x *= 2;
	x /= 125;
	return (systmr_t)x;
}

systmr_t systmr_nowMicros()
{
	systick_t x;
	cli();
	x = systmr_ticks;
	sei();
	// mathematics setup for 16 MHz with no clock divider on the timers
	x *= 16;
	return (systmr_t)x;
}