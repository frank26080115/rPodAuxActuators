#ifndef _COMM_H_
#define _COMM_H_

#include <stdint.h>

#define COMM_MAGIC_1 0x5A
#define COMM_MAGIC_2 0x22 // these two must never be the same

#define COMM_PKT_DATA_SIZE 32
typedef struct
{
	uint8_t magic_1;
	uint8_t magic_2;
	uint8_t seq; // must increment or otherwise be different from previous, this messes with the checksum so that we can reject a bad DMA
	uint8_t opcode;
	uint8_t data[COMM_PKT_DATA_SIZE];
	uint16_t checksum; // fletcher16
}
commpkt_t;

// must be implemented elsewhere
extern int16_t comm_hal_read(void);
extern void comm_hal_tx(uint8_t);

// public functions
void comm_task(void);
void comm_send(uint8_t opcode, uint8_t* data, uint8_t len);

// application layer callback
extern void app_handleComm(commpkt_t*);

#endif