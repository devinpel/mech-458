/*
 * usart.h
 *
 * Created: 3/2/2016 10:29:04 AM
 *  Author: Tristan
 */ 


#ifndef USART_H_
#define USART_H_

#include <asf.h>
#include <stdint.h>

#define RxEn		0b00010000
#define TxEn		0b00001000
#define Mode8Bit	0b00000110
#define DoubSpeed	0b00000010
#define TxBuffRdy	0b00100000

void usartInit (uint16_t baud);
void usartTX (unsigned char data);
void usartTXs (const char *string);


#endif /* USART_H_ */