/**
 * \file
 *
 * timer1 is scheduler 
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "adc.h"
#include "usart.h"
#include "interrupt.h"
#include "pwm.h"
#include <string.h>
#include "tim3.h"
#include "stepper.h"
#include "data.h"

volatile uint16_t ADC_result;
volatile unsigned char ADC_result_flag;
volatile unsigned char EndofBeltFlag;
volatile unsigned char count;
volatile unsigned char ReflectiveFlag;
volatile unsigned char PauseFlag;
volatile unsigned char RampDownFlag;
volatile unsigned char HomeFlag;
volatile unsigned char calibrationFlag;


volatile unsigned char tim3tickflag;
volatile unsigned char tim1tickflag;
volatile unsigned char curstep;
volatile unsigned char step;
volatile unsigned char delaytim3;



int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	
	uint16_t storeADC = 1023;
	struct data *input, in;
	uint8_t ones, tens, hundereds, thousands, sorted;

	char lastpart;
	char nextpart;
		
	input = &in;
	
	cli();
	adcinit();
	pwminit(); 
	stepperinit();
	timer1init();
	timer3init();
	usartInit(0x000C);	//Set baud to 9600, too much error at higher bauds for 1MHz clock
	interruptInit();
	
	//Initialize interrupt flags
	ReflectiveFlag = 0;
	EndofBeltFlag = 0;
	ADC_result_flag = 0;
	RampDownFlag = 0;
	calibrationFlag = 0;
	tim1tickflag = 0;
	tim3tickflag = 0;
	curstep = 0;
	
	step = 0;
	lastpart = 1;
	nextpart = 1;
	delaytim3 = 28;
	
	sei();
	
	DDRD = 0b11110000; /* Sets 7-4 pins on Port D to output */
	PORTD |= 0b10100000; /* initialize port to high � turn on LEDs */
	DDRC = 0b11111111; /* Sets all pins on Port C to output */	
	
	ADCSRA |= ADCStart;

	clearQueue(input);
	
	usartTXs("Serial port initialized\n\r");
	usartTXs("-------------------------\n\r");	
	
	homestepper();
	pwmcw();
	
	EndofBeltFlag = 0;
	
	if (calibrationFlag == 1)
	{
		calibration();
	}
	
			
	while(1)
	{
		if ((input->datapulled == 0) && (input->head != input->tail))
		{
			nextpart = pop_data(input);
		}
		
		else if (lastpart != nextpart)
		{
			if (tim3tickflag == delaytim3)
			{
				lastpart = movestepper(nextpart, lastpart);
			}
		}
		else if (EndofBeltFlag == 1 && lastpart == nextpart)
		{
			input->datapulled = 0;
			pwmcw();
			EndofBeltFlag = 0;
			tim1tickflag = 0;
			while (tim1tickflag <= 1)
			{
			}
		}
		if (ReflectiveFlag == 1)
		{	
			if(ADC_result_flag == 1)
			{
				if (ADC_result < storeADC)
				{
					storeADC = ADC_result;
				}
				ADC_result_flag = 0;
				ADCSRA |= ADCStart;				
			}
		}		
	
		//Black
		if (storeADC >= 836 && storeADC <= 1000 && ReflectiveFlag == 0)
		{
			insert_data(input, 1);
			displayVal(storeADC);
			storeADC = 1023;
			input->black++;
			sorted = 1;
		}
		//White
		else if (storeADC >= 775 && storeADC <= 835 && ReflectiveFlag == 0)
		{
			insert_data(input, 3);
			displayVal(storeADC);
			storeADC = 1023;
			input->white++;
			sorted = 1;
		}
		//Aluminum
		else if (storeADC >= 50 && storeADC <= 100 && ReflectiveFlag == 0)
		{
			insert_data(input, 0);
			displayVal(storeADC);
			storeADC = 1023;
			input->aluminum++;
			sorted = 1;
		}
		//Steal
		else if (storeADC >= 400 && storeADC <= 650 && ReflectiveFlag == 0)
		{
			insert_data(input, 2);
			displayVal(storeADC);
			storeADC = 1023;
			input->steal++;
			sorted = 1;
		}
		
		else if (storeADC < 1023 && ReflectiveFlag == 1)
		{
			usartTXs("Undetermined part\n\r");
			
			ones = (storeADC % 10);
			tens = ((storeADC / 10) % 10);
			hundereds = ((storeADC / 100) % 10);
			thousands = ((storeADC / 1000) % 10);

			usartTX(thousands + 0x30);
			usartTX(hundereds + 0x30);
			usartTX(tens + 0x30);
			usartTX(ones + 0x30);
			usartTX('\n');
			usartTX('\r');
			
			storeADC = 1023;
			input->unknown++;
		}
		sorted = 0;
		//increment the count to keep track of how many pieces have passed 		
		
		if (PauseFlag == 1)
		{
			usartTXs("Aluminum\t");
			usartTX((input->aluminum /10) % 10 + 0x30);
			usartTX((input->aluminum) % 10 + 0x30);
			usartTXs("\n\r");
			usartTXs("Black\t");
			usartTX((input->black /10) % 10 + 0x30);
			usartTX((input->black) % 10 + 0x30);
			usartTXs("\n\r");
			usartTXs("Steel\t");
			usartTX((input->steal /10) % 10 + 0x30);
			usartTX((input->steal) % 10 + 0x30);
			usartTXs("\n\r");
			usartTXs("White\t");
			usartTX((input->white /10) % 10 + 0x30);
			usartTX((input->white) % 10 + 0x30);
			usartTXs("\n\r");
			usartTXs("Unknown\t");
			usartTX((input->unknown /10) % 10 + 0x30);
			usartTX((input->unknown) % 10 + 0x30);
			usartTXs("\n\r");
			
			/*
			usartTXs("Total\t");
			usartTX((input->aluminum /10) % 10 + 0x30);
			usartTX((input->aluminum) % 10 + 0x30);
			usartTXs("\n\r");
			*/
			
			PauseFlag = 0;
			while (PauseFlag == 0)
			{
				pwmbrake();
			}
			pwmcw();
			PauseFlag = 0;
		}
		
		if (RampDownFlag == 1)
		{
			//start 10 second timer
			//tim1tickflag = 0;
			
			if (ReflectiveFlag == 1)
			{
				//restart 10 second timer
				tim1tickflag = 0;
			}
			
			if (tim1tickflag == 20) //got to ten seconds
			{
				PORTD |= 0xF0;
				pwmbrake();
			}
		}
	}//end while
}//end main
