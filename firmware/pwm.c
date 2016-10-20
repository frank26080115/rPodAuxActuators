#include "pwm.h"
#include "systmr.h"
extern volatile systick_t systmr_ticks;

#include <avr/io.h>

#define PWM_CLOCK_DIVIDER 0x01

void pwm_init(uint8_t t)
{
	switch (t)
	{
		case 0:
			OCR0A = 0;
			OCR0B = 0;
			TCCR0A = _BV(COM0A1) | _BV(COM0B1);
			TCCR0B = PWM_CLOCK_DIVIDER;
			break;
		case 2:
			OCR2A = 0;
			OCR2B = 0;
			TCCR2A = _BV(COM2A1) | _BV(COM2B1);
			TCCR2B = PWM_CLOCK_DIVIDER;
			break;
		case 1:
			OCR1A = 0;
			OCR1B = 0;
			OCR1C = 0;
			TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1C1) | _BV(WGM10);
			TCCR1B = PWM_CLOCK_DIVIDER;
			break;
		case 3:
			OCR3A = 0;
			OCR3B = 0;
			OCR3C = 0;
			TCCR3A = _BV(COM3A1) | _BV(COM3B1) | _BV(COM3C1) | _BV(WGM30);
			TCCR3B = PWM_CLOCK_DIVIDER;
			break;
		default:
			break;
	}
}

void pwm_ovfEnable(uint8_t t)
{
	switch (t)
	{
		case 0:
			TIMSK0 |= _BV(TOIE0);
			TIMSK1 = 0; TIMSK2 = 0; TIMSK3 = 0;
			TCCR0B = (TCCR0B & 0xF8) | PWM_CLOCK_DIVIDER;
			break;
		case 1:
			TIMSK1 |= _BV(TOIE1);
			TIMSK0 = 0; TIMSK2 = 0; TIMSK3 = 0;
			TCCR1B = (TCCR1B & 0xF8) | PWM_CLOCK_DIVIDER;
			break;
		case 2:
			TIMSK2 |= _BV(TOIE2);
			TIMSK1 = 0; TIMSK0 = 0; TIMSK3 = 0;
			TCCR2B = (TCCR2B & 0xF8) | PWM_CLOCK_DIVIDER;
			break;
		case 3:
			TIMSK3 |= _BV(TOIE3);
			TIMSK1 = 0; TIMSK2 = 0; TIMSK0 = 0;
			TCCR3B = (TCCR3B & 0xF8) | PWM_CLOCK_DIVIDER;
			break;
	}
}

ISR(TIMER0_OVF_vect) {
	systmr_ticks++;
}
ISR_ALIAS(TIMER1_OVF_vect, TIMER0_OVF_vect);
ISR_ALIAS(TIMER2_OVF_vect, TIMER0_OVF_vect);
ISR_ALIAS(TIMER3_OVF_vect, TIMER0_OVF_vect);