#ifndef _SER_H_
#define _SER_H_

#include <stdint.h>

void ser_init(void);
int16_t ser_read(void);
void ser_tx(uint8_t);

#endif