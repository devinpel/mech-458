/*
 * data.c
 *
 * Created: 3/10/2016 1:20:34 PM
 *  Author: Tristan
 *	aluminum 0
 *	black 1
 *  steel 2
 *	White 3
 *
 */ 

#include "data.h"
#include "usart.h"

// This function will load part data into the circular array
// Using the modulus in the array index allows for the values
// in the array to be over written as more and more parts 
// come through.
void insert_data (struct data *input, uint8_t val)
{
	input->queue[input->head % 16] = val;
// 	usartTX(val+0x30);
// 	usartTXs("\n\r");
	input->head = input->head + 1;
}

// This function removes parts from the array after they are
// dropped in the bin. If the tail pointer is ever larger then the
// head pointer then something has gone wrong somewhere.
//
// The modulus is used again in the array index to loop the array.
uint8_t pop_data (struct data *input)
{
	uint8_t temp;
	
	if(input->tail >= input->head)
	{
		usartTXs("ERROR tail has over run head\n\r");
		usartTXs("Resetting Tail\n\r");
		input->tail = input->head;
	}
	input->datapulled = 1;
	temp = input->queue[input->tail % 16];
	input->tail = input->tail + 1;
	
	return temp;
}

// This function can display a 10bit ADC result value in the data struct
// on the serial console.
// 
// This is primarily for debugging purposes.
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

//This function displays an 8bit value on the serial port
void display_data_value (uint8_t val)
{
	usartTX((val /10) % 10 + 0x30);
	usartTX((val) % 10 + 0x30);
	usartTXs("\n\r");
}

// This function clears the data que and resets all counters to 0
// To be called on initialization.
void clearQueue (struct data *input)
{
	uint8_t i;
	
	input->head = 0;
	input->tail = 0;
	input->black = 0;
	input->white = 0;
	input->aluminum = 0;
	input->steel = 0;
	input->unknown = 0;
	input->datapulled = 0;
	
	for (i=0; i<16; i++)
	{
		input->queue[i] = 0x00;
	}
}

// This function can display any 10bit ADC result on the serial console.
//
// This is primarily for debugging purposes.
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

// Calibration mode. 
// This mode streams ADC result values out the serial port as a part
// moves passed the reflective sensor.
//
// To exit this mode restart the uC
void calibration (void)
{

	char ones, tens, hundereds, thousands;
	
	usartTXs("Entering calibration mode\n\r");
	usartTXs("-------------------------\n\r");	
	
	calibrationFlag = 0;
	
	EIMSK &= ~(_BV(INT0));
	
	while (1)
	{

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
				usartTXs("\n\r");
				ADC_result_flag = 0;
				ADCSRA |= ADCStart;
			}
			if (ReflectiveFlag == 0)
			{
				usartTXs("\n\r");
			}
		}
	}
}

// Assign a numarical value to a part based on it's minimum ADC value.
//
// Pass this function the data struct to store the value and the ADC 
// sensor value. 
uint16_t sort_data (struct data *input, uint16_t storeADC)
{
	uint8_t ones, tens, hundereds, thousands;
	
	//Black
	if (storeADC >= 860 && storeADC <= 1000)
	{
		insert_data(input, 1);
		displayVal(storeADC);
		storeADC = 1023;
		input->black++;
	}
	//White
	else if (storeADC >= 750 && storeADC <= 859)
	{
		insert_data(input, 3);
		displayVal(storeADC);
		storeADC = 1023;
		input->white++;
	}
	//Aluminum
	else if (storeADC >= 0 && storeADC <= 46)
	{
		insert_data(input, 0);
		displayVal(storeADC);
		storeADC = 1023;
		input->aluminum++;
	}
// 	//steel
	else if (storeADC >= 47 && storeADC <= 299)
	{
		insert_data(input, 2);
		displayVal(storeADC);
		storeADC = 1023;
		input->steel++;
	}
	//Unknown
	else if (storeADC >= 300 && storeADC <= 749)
	{
		insert_data(input, 5);
		displayVal(storeADC);
		storeADC = 1023;
		input->unknown++;
	}

	return storeADC;
}

// Once the pause button is hit, sum up all the know parts in the bin
// and on the belt and display it over the serial port.
void display_paused_data (struct data *input)
{
	uint8_t aluminum = 0, black = 0, white = 0, steel = 0, unknown = 0;
	uint8_t temp;

	if(input->head >= (input->tail) && input->datapulled == 1)
	{	
		for (temp = (input->tail % 16) - 1; temp < (input->head % 16); temp++)
		{
			if (input->queue[temp] == 0)
			{
				aluminum++;
			}
			else if (input->queue[temp] == 1)
			{
				black ++;
			}
			else if (input->queue[temp] == 2)
			{
				steel ++;
			}
			else if (input->queue[temp] == 3)
			{
				white ++;
			}
			else if (input->queue[temp] == 5)
			{
				unknown ++;
			}			
		}
	}
	usartTXs("Parts on the belt\n\r");
	usartTXs("Aluminum\t");
	display_data_value(aluminum);
	usartTXs("Black\t\t");
	display_data_value(black);
	usartTXs("Steel\t\t");
	display_data_value(steel);
	usartTXs("White\t\t");
	display_data_value(white);
	usartTXs("Unknown\t\t");
	display_data_value(unknown);
// 	usartTXs("Unsorted\t\t");
// 	display_data_value(count - input->head);
// 	usartTXs("count is\t\t");
// 	display_data_value(count);
// 	usartTXs("head is\t\t");
// 	display_data_value(input->head);
	
	
	usartTXs("\n\rParts in the Bin\n\r");
	usartTXs("Aluminum\t");
	display_data_value(input->aluminum - aluminum);
	usartTXs("Black\t\t");
	display_data_value(input->black - black);
	usartTXs("Steel\t\t");
	display_data_value(input->steel - steel);
	usartTXs("White\t\t");
	display_data_value(input->white - white);
	usartTXs("Unknown\t\t");
	display_data_value(input->unknown - unknown);
	
}