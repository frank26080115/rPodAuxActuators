#include "adc.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef HAS_IDLE_TASKS
extern void idle_tasks(void);
#else
#define idle_tasks()
#endif

#define ADC_CHANNEL_CNT 8
#define ADMUX_DEFAULT		( _BV(REFS0) )
#define ADCSRA_DEFAULT		( _BV(ADEN) | 0x07 )

#ifdef ADC_ENABLE_ASYNC
volatile char adc_hasNew[ADC_CHANNEL_CNT];
volatile uint16_t adc_result[ADC_CHANNEL_CNT];
volatile static uint8_t adc_curChanIdx = 0;
#endif

#ifdef ADC_ENABLE_FILTERING
#define ADC_DEFAULT_FILTER_CONST 0.9
volatile double adc_filteredResult[ADC_CHANNEL_CNT];
#endif

void adc_init()
{
	// advanced start only for processors that supports it
	#if defined(ADCSRC)
	ADCSRC = 0xDF; // slowest start and slowest track-and-hold
	#endif
	ADCSRA = _BV(ADEN) | 0x07; // enable with clk/128
	#if defined(AVDDOK) && defined(REFOK)
	while ((ADCSRB & (_BV(AVDDOK) | _BV(REFOK))) == 0); // wait for analog supply and ref to ready
	#elif defined(AVDDOK) && !defined(REFOK)
	while ((ADCSRB & (_BV(AVDDOK))) == 0);
	#elif !defined(AVDDOK) && defined(REFOK)
	while ((ADCSRB & (_BV(REFOK))) == 0);
	#endif

	for (uint8_t i = 0; i < ADC_CHANNEL_CNT; i++)
	{
		#ifdef ADC_ENABLE_ASYNC
		adc_hasNew[i] = 0;
		adc_result[i] = 0;
		#endif
		#ifdef ADC_ENABLE_FILTERING
		adc_filteredResult[i] = -1.0d;
		#endif
	}

	#ifdef ADC_ENABLE_ASYNC
	ADMUX  = ADMUX_DEFAULT | adc_curChanIdx;
	ADCSRA = ADCSRA_DEFAULT | _BV(ADSC) | _BV(ADIE);
	#endif
}

uint16_t adc_read_10(uint8_t chan)
{
	#if !defined(ADC_ENABLE_ASYNC)
	ADMUX  = ADMUX_DEFAULT | chan;
	ADCSRA = ADCSRA_DEFAULT | _BV(ADSC);
	while(bit_is_clear(ADCSRA, ADIF)) { idle_tasks(); }
	ADCSRA = (ADCSRA | _BV(ADIF)) & ~_BV(ADSC);
	uint16_t r = ADC;
	return r;
	#elif defined(ADC_ENABLE_FILTERING)
	while (adc_hasNew[chan] == 0) { idle_tasks(); }
	uint16_t r = lround(adc_filteredResult[chan]);
	adc_hasNew[chan] = 0;
	return r;
	#else
	while (adc_hasNew[chan] == 0) { idle_tasks(); }
	uint16_t r = adc_result[chan];
	adc_hasNew[chan] = 0;
	return r;
	#endif
}

uint8_t adc_read_8(uint8_t chan)
{
	return (uint8_t)(adc_read_10(chan) >> 2);
}

#ifndef ADC_ENABLE_ASYNC
void adc_read_start(uint8_t chan)
{
	ADMUX  = ADMUX_DEFAULT | chan;
	ADCSRA = ADCSRA_DEFAULT | _BV(ADSC);
}

int16_t adc_read_end_10()
{
	if (bit_is_clear(ADCSRA, ADIF))
	{
		return -1;
	}
	else
	{
		int16_t r = ADC;
		return r;
	}
}

int16_t adc_read_end_8()
{
	if (bit_is_clear(ADCSRA, ADIF))
	{
		return -1;
	}
	else
	{
		int16_t r = ADC;
		return r >> 8;
	}
}
#endif

#ifdef ADC_ENABLE_ASYNC
ISR(ADC_vect)
{
	uint16_t res = ADC;
	uint8_t curChan = adc_curChanIdx;
	adc_result[curChan] = res;
	#ifdef ADC_ENABLE_FILTERING
	if (adc_filteredResult[curChan] < 0.0d) {
		adc_filteredResult[curChan] = res;
	}
	else {
		adc_filteredResult[curChan] = (adc_filteredResult[curChan] * ADC_DEFAULT_FILTER_CONST) + (res * (1.0d - ADC_DEFAULT_FILTER_CONST));
	}
	#endif
	adc_hasNew[curChan] = 1;
	curChan = (curChan + 1) % ADC_CHANNEL_CNT;
	adc_curChanIdx = curChan;

	ADMUX  = ADMUX_DEFAULT | curChan;
	ADCSRA = ADCSRA_DEFAULT | _BV(ADSC) | _BV(ADIE);
}
#endif