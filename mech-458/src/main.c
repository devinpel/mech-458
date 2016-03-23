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
volatile unsigned char step;
volatile unsigned char delaytim3;



int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	board_init();
	
	uint16_t storeADC = 1023;
//	char i = 0;
	struct data *input, in;

	
	char lastpart;
	char nextpart;
		
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
	
	step = 0;
	lastpart = 1;
	nextpart = 1;
	delaytim3 = 28;
	
	sei();
	
	DDRD = 0b11110000; /* Sets 7-4 pins on Port D to output */
	PORTD |= 0b10100000; /* initialize port to high – turn on LEDs */
	DDRC = 0b11111111; /* Sets all pins on Port C to output */	
	
	ADCSRA |= ADCStart;
	
	input->head = 0;
	input->tail = 0;
	input->size = 0;
	input->datapulled = 0;
	clearQueue(input);
	
	usartTXs("Serial port initialized\n\r");
	usartTXs("Entering calibration mode\n\r");
	usartTXs("-------------------------\n\r");	
	homestepper();
	pwmcw();
	
	EndofBeltFlag = 0;
			
	while(1)
	{
		
		if ((input->datapulled == 0) && (input->head != input->tail))
		{
			nextpart = pop_data(input);
			//usartTXs("here");
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
		}
/*		
		
		if (tim3tickflag == delaytim3)
		{	
			if (lastpart != nextpart)
			{
				lastpart = movestepper(nextpart, lastpart);
			}
		
			else if((lastpart == nextpart) && (input->head > input->tail))
			{
				if ((EndofBeltFlag == 1))
				{
				nextpart = pop_data(input);
				usartTXs("oops");
				
				EndofBeltFlag = 0;
				pwmcw();
				}
			}
			//setup next and last part
						
		}
		*/
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
		//if (storeADC > 860 && storeADC < 900)
		if (storeADC >= 915 && storeADC <= 1000 && ReflectiveFlag == 0)
		{
			insert_data(input, 1);
			displayVal(storeADC);
			storeADC = 1023;
			PORTD = 0b00010000;
		}
		//White
		else if (storeADC >= 720 && storeADC <= 914 && ReflectiveFlag == 0)
		{
			insert_data(input, 3);
			displayVal(storeADC);
			storeADC = 1023;
			PORTD = 0b00100000;
		}
		//Aluminum
		else if (storeADC <= 300 && ReflectiveFlag == 0)
		{
			insert_data(input, 0);
			displayVal(storeADC);
			storeADC = 1023;
			PORTD = 0b01000000;
		}
		//Steal
		else if (storeADC >= 301 && storeADC <= 719 && ReflectiveFlag == 0)
		{
			insert_data(input, 2);
			displayVal(storeADC);
			storeADC = 1023;
			PORTD = 0b10000000;
		}
// 		else
// 		{
// 			usartTXs("Undetermined part\n\r");
// 				
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
// 		}
		//increment the count to keep track of how many pieces have passed
		input->size ++;
	}//end while
}//end main
