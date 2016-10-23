/*
handles communication to and from the FCU
*/

#include "comm.h"
#include "systmr.h"
#include "fletcher.h"

#define COMM_BUFF_SIZE 64
uint8_t comm_buffer[COMM_BUFF_SIZE];
uint8_t comm_buff_idx = 0;

void comm_task()
{
	static enum {
		STATE_WAITING,
		STATE_READING,
	}
	state = STATE_WAITING;
	static systmr_t tmr = 0;

	int16_t c;
	c = comm_hal_read();
	if (c < 0) // nothing in buffer
	{
		if ((systmr_nowMillis() - tmr) > 100) { // timeout
			state = STATE_WAITING;
			comm_buff_idx = 0;
		}

		return;
	}

	if (state == STATE_WAITING)
	{
		if (c == COMM_MAGIC) // got sync word
		{
			comm_buff_idx = 0; // so start for the first
			comm_buffer[comm_buff_idx++] = c;
			state = STATE_READING; // we have the sync word
			tmr = systmr_nowMillis();
			return;
		}
	}
	else
	{
		tmr = systmr_nowMillis();
		comm_buffer[comm_buff_idx++] = c; // save
		if (comm_buff_idx >= sizeof(commpkt_t)) // we got enough for a full packet`
		{
			comm_handle();
			state = STATE_WAITING; // reset state machine
			comm_buff_idx = 0;
		}
	}
}

void comm_handle()
{
	int i;
	uint32_t checksum_calculated;
	static uint8_t prev_seq = 0xAA;
	commpkt_t* pkt = (commpkt_t*)comm_buffer;
	if (pkt->seq == prev_seq) {
		// outdated packet
		// TODO: report error
		return;
	}
	checksum_calculated = fletcher16(comm_buffer, sizeof(commpkt_t) - sizeof(uint16_t));
	if (checksum_calculated != pkt->checksum) {
		// bad packet
		// TODO: report error
		return;
	}
	app_handleComm(pkt);
	prev_seq = pkt->seq;
}

void comm_send(uint8_t opcode, uint8_t* data, uint8_t len)
{
	int i;
	#if defined (COMM_REATTEMPTS) && COMM_REATTEMPTS > 1
	int j;
	#endif
	static uint8_t seq = 0x05;
	static uint8_t outbuff[COMM_BUFF_SIZE];
	commpkt_t* pkt = (commpkt_t*)outbuff;
	pkt->magic = COMM_MAGIC;
	pkt->seq = seq++;
	pkt->opcode = opcode;
	for (i = 0; i < len && i < COMM_PKT_DATA_SIZE; i++)
	{
		pkt->data[i] = data[i];
	}
	pkt->checksum = fletcher16(outbuff, sizeof(commpkt_t) - sizeof(uint16_t));

	// reattempts are harmless if the bandwidth is available, due to the way the sequence number works
	#if defined (COMM_REATTEMPTS) && COMM_REATTEMPTS > 1
	for (j = 0; j < COMM_REATTEMPTS; j++)
	#endif
	{
		for (i = 0; i < COMM_BUFF_SIZE; i++)
		{
			comm_hal_tx(outbuff[i]);
		}
	}
}