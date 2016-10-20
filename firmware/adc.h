#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

#define ADC_ENABLE_ASYNC
#define ADC_ENABLE_FILTERING

void adc_init();
uint16_t adc_read_10(uint8_t);
uint8_t adc_read_8(uint8_t);

#ifndef ADC_ENABLE_ASYNC
void adc_read_start(uint8_t);
int16_t adc_read_end_10();
int16_t adc_read_end_8();
#endif

#endif