/*
 * data.c
 *
 * Created: 3/10/2016 1:20:34 PM
 *  Author: Tristan
 *	aluminum 0
 *	black 1
 *  steal 2
 *	White 3
 *
 */ 

#include "data.h"
#include "usart.h"

void insert_data (struct data *input, uint8_t val)
{
	input->queue[input->head % 16] = val;
// 	usartTX(val+0x30);
// 	usartTXs("\n\r");
	input->head = input->head + 1;
}

uint8_t pop_data (struct data *input)
{
	uint8_t temp;
	
	if(input->tail >= input->head)
	{
		usartTXs("ERROR tail has over run head");
	}
	input->datapulled = 1;
	temp = input->queue[input->tail % 16];
	input->tail = input->tail + 1;
//	usartTXs("tail = ");
//	usartTX(input->tail + 0x30);
//	usartTXs("\n\r");
	return temp;
}

void display_data (struct data *input)
{
	uint8_t i,ones, tens, hundereds, thousands;
	uint16_t temp;
	
	usartTX('\n');
	usartTX('\r');
	
	for (i=0; i<16; i++)
	{
		temp = input->queue[i];
		
		ones = (temp % 10);
		tens = ((temp / 10) % 10);
		hundereds = ((temp / 100) % 10);
		thousands = ((temp / 1000) % 10);

		usartTX(thousands + 0x30);
		usartTX(hundereds + 0x30);
		usartTX(tens + 0x30);
		usartTX(ones + 0x30);
		usartTX('\n');
		usartTX('\r');
	}
}

void clearQueue (struct data *input)
{
	uint8_t i;
	
	input->head = 0;
	input->tail = 0;
	input->black = 0;
	input->white = 0;
	input->aluminum = 0;
	input->steal = 0;
	input->datapulled = 0;
	
	for (i=0; i<16; i++)
	{
		input->queue[i] = 0x00;
	}
}

void displayVal (uint16_t storeADC)
{
	uint8_t ones, tens, hundereds, thousands;	

	ones = (storeADC % 10);
	tens = ((storeADC / 10) % 10);
	hundereds = ((storeADC / 100) % 10);
	thousands = ((storeADC / 1000) % 10);
	usartTXs("Stored ADC\n\r");
	usartTX(thousands + 0x30);
	usartTX(hundereds + 0x30);
	usartTX(tens + 0x30);
	usartTX(ones + 0x30);
	usartTX('\n');
	usartTX('\r');
}

void calibration (void)
{

	char ones, tens, hundereds, thousands;
	
	usartTXs("Entering calibration mode\n\r");
	usartTXs("-------------------------\n\r");	
	
	calibrationFlag = 0;
	
	while (1)
	{
		if (calibrationFlag == 1)
		{
			pwmcw();
			calibrationFlag = 0;
		}
		if (ReflectiveFlag == 1)
		{
			while(ADC_result_flag == 1)
			{
				ones = (ADC_result % 10);
				tens = ((ADC_result / 10) % 10);
				hundereds = ((ADC_result / 100) % 10);
				thousands = ((ADC_result / 1000) % 10);
				usartTX(thousands + 0x30);
				usartTX(hundereds + 0x30);
				usartTX(tens + 0x30);
				usartTX(ones + 0x30);
				usartTX('\n');
				usartTX('\r');	
				ADC_result_flag = 0;
				ADCSRA |= ADCStart;
			}
		}
	
		if(EndofBeltFlag == 1);
		{
			pwmbrake();	
		}
	}
}