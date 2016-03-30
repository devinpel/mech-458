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
volatile unsigned char steps;
volatile unsigned char step;
volatile unsigned char delaytim3;
volatile unsigned char steppermove;

//volatile unsigned char lastpart;
//volatile unsigned char nextpart;


int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	uint8_t ones, tens, hundereds, thousands;
	uint16_t storeADC = 1023;
	struct data *input, in;

	char lastpart;
	char nextpart;
	char stored;
		
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
	stored = 0;
	
	step = 0;
	lastpart = 1;
	nextpart = 1;
	delaytim3 = 22;
	
	sei();
	
	DDRD = 0b11110000; /* Sets 7-4 pins on Port D to output */
	PORTD |= 0b10100000; /* initialize port to high – turn on LEDs */
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
		
		//case
		if ((input->datapulled == 0) && (input->head != input->tail))
		{
			nextpart = pop_data(input);
		}
		
		else if (lastpart != nextpart)
		{
			if (tim1tickflag == delaytim3 && tim3tickflag >= 2)
			{
				lastpart = movestepper(nextpart, lastpart);
			}
		}
		else if (EndofBeltFlag == 1 && lastpart == nextpart)
		{
			input->datapulled = 0;
			pwmcw();
			EndofBeltFlag = 0;
			tim3tickflag = 0;
			//while (tim3tickflag <= 2);
			tim1tickflag = 0;
		}
		
		//case
		if (ReflectiveFlag == 1)
		{	
			if(ADC_result_flag == 1)
			{
				//Look for the min value from ADC as a part moves past the sensor
				if (ADC_result < storeADC)
				{
					storeADC = ADC_result;
					
				}
				//Once the min value has been found, sort it and store into array.
// 				else
// 				{
// 					
// 				}
				ADC_result_flag = 0;
				ADCSRA |= ADCStart;				
			}
		}
		if (storeADC < 1023 && ReflectiveFlag == 0)
		{
			storeADC = sort_data (input, storeADC);
		}

	//increment the count to keep track of how many pieces have passed 		
		
		if (PauseFlag == 1)
		{
			display_paused_data(input);
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
				tim3tickflag = 0;
			}
			
			if (tim3tickflag == 50) //got to ten seconds
			{
				PORTD |= 0xF0;
				pwmbrake();
				RampDownFlag = 0;
				usartTXs("\n\rParts in the Bin\n\r");
				usartTXs("Aluminum\t");
				display_data_value(input->aluminum);
				usartTXs("Black\t\t");
				display_data_value(input->black);
				usartTXs("Steel\t\t");
				display_data_value(input->steel);
				usartTXs("White\t\t");
				display_data_value(input->white);
			}
		}
	}//end while
}//end main
