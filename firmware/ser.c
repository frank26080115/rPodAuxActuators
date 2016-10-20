#include "ser.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define UBRRHn UBRRH1
#define UBRRLn UBRRL1
#define USCRnB USCR1B
#define USCRnC USCR1C
#define UDRn   UDR1
#define USBSn  USBS1
#define RXENn  RXEN1
#define TXENn  TXEN1
#define RXCIEn RXCIE1
#define TXCIEn TXCIE1
#define xUSART_RX_vect
#define xUSART_TX_vect

#define SER_BUFF_SIZE 128
volatile uint8_t ser_rx_head;
volatile uint8_t ser_rx_tail;
volatile uint8_t ser_tx_head;
volatile uint8_t ser_tx_tail;
volatile uint8_t ser_rx_buff[SER_BUFF_SIZE];
volatile uint8_t ser_tx_buff[SER_BUFF_SIZE];
volatile char ser_tx_busy;

void ser_init()
{
	UBRRHn = UBRRH_VALUE;
	UBRRLn = UBRRL_VALUE;

	UCSRBn = 0x00; // perform a reset on UART

	ser_rx_head = 0;
	ser_rx_tail = 0;
	ser_tx_head = 0;
	ser_tx_tail = 0;
	ser_tx_busy = 0;

	UCSRnC |= _BV(USBSn); // 2 stop bits to help the clock
	UCSRnB = _BV(RXENn) | _BV(TXENn) | _BV(RXCIEn) | _BV(TXCIEn); // start UART
}

int16_t ser_read()
{
	uint8_t x;

	// check if any in buffer
	uint16_t r = SER_BUFF_SIZE;
	cli();
	r += ser_rx_head;
	r -= ser_rx_tail;
	r %= SER_BUFF_SIZE;
	if (r <= 0) {
		sei();
		return -1; // return underrun
	}

	x = ser_rx_buff[ser_rx_tail]; // read in from buffer

	// advance pointer
	ser_rx_tail += 1;
	ser_rx_tail %= SER_BUFF_SIZE;

	sei();

	return x;
}

void ser_tx(uint8_t x)
{
	uint16_t r;

	// wait until there is room
	do
	{
		r = SER_BUFF_SIZE;
		cli();
		r += ser_tx_tail;
		r -= ser_tx_head;
		sei();
		r %= SER_BUFF_SIZE;
	}
	while (r >= SER_BUFF_SIZE);

	cli();
	// place into outgoing buffer
	ser_tx_buff[ser_tx_tail] = x;
	ser_tx_tail += 1;
	ser_tx_tail %= SER_BUFF_SIZE;

	// if no ongoing is happening, we need to manually trigger a tx
	if (ser_tx_busy == 0)
	{
		UDRn = x;
		ser_tx_head = (ser_tx_head + 1) % SER_BUFF_SIZE;
		ser_tx_busy = 1;
	}
	sei();
}

ISR(xUSART_RX_vect)
{
	uint8_t x = UDRn; // read it in
	uint16_t n;
	// check the next spot in buffer
	n = ser_rx_head;
	n += 1;
	n %= SER_BUFF_SIZE;
	if (n != ser_rx_tail) // is there room in buffer
	{
		ser_rx_buff[ser_rx_head] = x; // save in buffer
		ser_rx_head = n; // advance pointer
	}
}

ISR(xUSART_TX_vect)
{
	if (ser_tx_head != ser_tx_tail) // have more in the buffer
	{
		UDRn = ser_tx_buff[ser_tx_head]; // send the next one
		// move the pointer
		ser_tx_head += 1;
		ser_tx_head %= SER_BUFF_SIZE;
	}
	else
	{
		// buffer empty, all done
		ser_tx_busy = 0;
	}
}