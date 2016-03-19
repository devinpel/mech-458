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


volatile unsigned char tim3tickflag;
volatile unsigned char curstep;
volatile unsigned char steppermove;
volatile unsigned char steps;
volatile unsigned char lastpart;
volatile unsigned char nextpart;
volatile unsigned char delaytim3;



int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	
	uint16_t storeADC = 1023;
//	char i = 0;
	struct data *input, in;
//	uint8_t ones, tens, hundereds, thousands;
		
	input = &in;
	
	cli();
	adcinit();
	pwminit(); 
	stepperinit();
	timer3init();
	usartInit(0x000C);	//Set baud to 9600, too much error at higher bauds for 1MHz clock
	interruptInit();
	
	//Initialize interrupt flags
	ReflectiveFlag = 0;
	EndofBeltFlag = 0;
	ADC_result_flag = 0;
	tim3tickflag = 0;
	curstep = 0;
	steppermove = 0;
	steps = 0;
	lastpart = 0;
	nextpart = 2;
	delaytim3 = 36;
	
	sei();
	
	DDRD = 0b11110000; /* Sets 7-4 pins on Port D to output */
	PORTD |= 0b10100000; /* initialize port to high – turn on LEDs */
	DDRC = 0b11111111; /* Sets all pins on Port C to output */	
	
	ADCSRA |= ADCStart;
	
	input->head = 0;
	input->tail = 0;
	input->size = 0;
	clearQueue(input);
	
	usartTXs("Serial port initialized\n\r");
	usartTXs("Entering calibration mode\n\r");
	usartTXs("-------------------------\n\r");	
	
	pwmcw();
	
	EndofBeltFlag = 0;
			
	while(1)
	{
		if (ReflectiveFlag == 1)
		{	
			ReflectiveFlag = 0;
			while (ReflectiveFlag == 0)
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
// 			ones = (storeADC % 10);
// 			tens = ((storeADC / 10) % 10);
// 			hundereds = ((storeADC / 100) % 10);
// 			thousands = ((storeADC / 1000) % 10);
// 			usartTXs("Stored ADC\n\r");
// 			usartTX(thousands + 0x30);
// 			usartTX(hundereds + 0x30);
// 			usartTX(tens + 0x30);
// 			usartTX(ones + 0x30);
// 			usartTX('\n');
// 			usartTX('\r');	
 			ReflectiveFlag = 0;
//			i++;
			//Black
			//if (storeADC > 860 && storeADC < 900)
			if (storeADC > 860)
			{
				insert_data(input, 1);
			}
			//White
			else if (storeADC > 800 && storeADC < 850)
			{
				insert_data(input, 3);
			}
			//Aluminum
			else if (storeADC < 150)
			{
				insert_data(input, 0);
			}
			//Steal
			else if (storeADC > 150 && storeADC < 300)
			{
				insert_data(input, 2);
			}
			//increment the count to keep track of how many pieces have passed
			input->size ++;
			storeADC = 1023;
 		}//end if
// 		if(EndofBeltFlag == 1)
// 		{
// 			pwmbrake();
// 			EndofBeltFlag = 0;
// 		}
// 		if (i > 15)
// 		{
// 			i = 0;
// 			display_data(input);
// 			usartTXs("Pop data 1\n\r");
// 			usartTX (pop_data(input)+0x30);
// 			usartTXs("\n\rPop data 2\n\r");
// 			usartTX (pop_data(input)+0x30);
// 		}//end if
	}//end while
}//end main
