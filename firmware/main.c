#include "main.h"

#include "comm.h"
#include "adc.h"
#include "pwm.h"
#include "systmr.h"

systmr_t comm_tmr = 0;

int main()
{
	systmr_t loop_tmr = 0;
	systmr_t now = 0;
	// TODO: setup code

	while (1)
	{
		if (((now = systmr_nowMillis()) - loop_tmr) >= 10)
		{
			loop_tmr = now;

			// TODO: code stuff here that happens every 10 ms
		}

		if ((now - comm_tmr) > 500) { // communication watchdog expired
			// TODO: go into safe mode
		}
	}

	while (1);
	return 0;
}

void app_handleComm(commpkt_t* pkt)
{
	comm_tmr = systmr_nowMillis();
}