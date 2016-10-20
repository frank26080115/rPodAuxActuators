#include "ser.h"
#include "comm.h"

int16_t comm_hal_read()
{
	return ser_read();
}

void comm_hal_tx(uint8_t x)
{
	ser_tx(x);
}