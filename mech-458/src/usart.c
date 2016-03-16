/*
 * usart.c
 *
 * Created: 3/2/2016 10:28:50 AM
 *  Author: Tristan
 */ 

#include "usart.h"

void usartInit (uint16_t baud)
{
	//Set baud rate
	UBRR1H = (uint8_t)(baud >> 8);
	UBRR1L = (uint8_t)(baud);
	
	UCSR1A |= DoubSpeed;
	
	UCSR1B |= TxEn;			//Enable TX mode for USART1
	//UCSR1B |= RxEn;		//Enable RX mode for USART1
	UCSR1C |= Mode8Bit;		//Set Usart to 8 bit data mode
}

void usartTX (unsigned char data)
{
	while (!(UCSR1A & TxBuffRdy));	//Wait for the TX buffer to be ready for data
	
	UDR1 = data;
}

void usartTXs (const char *string)
{
	uint8_t c = 0;
	
	while (string [c] != 0)
	{
		usartTX(string[c]);
		c++;
	}
}