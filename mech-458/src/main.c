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



int main (void)
{
	// Insert system clock initialization code here (sysclk_init()).

	//Variable definitions
	board_init();
	uint8_t ones, tens, hundereds, thousands;
	uint16_t storeADC = 1023;
	struct data *input, in;

	char lastpart;
	char nextpart;
	char stored;
	
	//Data struct creation	
	input = &in;
	
	//Initialization
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
	count = 0;
	
	step = 0;
	lastpart = 1;
	nextpart = 1;
	delaytim3 = 22;
	
	sei();
	
	DDRD = 0b11110000; /* Sets 7-4 pins on Port D to output */
	PORTD |= 0b10100000; /* initialize port to high – turn on LEDs */
	DDRC = 0b11111111; /* Sets all pins on Port C to output */	
	
	//Start first ADC conversion
	ADCSRA |= ADCStart;

	//set all values in the data struct to zero.
	clearQueue(input);
	
	usartTXs("Serial port initialized\n\r");
	usartTXs("-------------------------\n\r");	
	
	homestepper();	//Move stepper to home position
	pwmcw();		//Turn on belt
	
	EndofBeltFlag = 0;
	
	//Enter calibration mode if butten is pressed on startup
	if (calibrationFlag == 1)
	{
		calibration();
	}
			
	while(1)
	{
		//Determine if there is a new part in the que. If there is get it.
		if ((input->datapulled == 0) && (input->head != input->tail))
		{
			nextpart = pop_data(input);
		}
		//Move stepper based on the next part to go into the bin
		else if (lastpart != nextpart && EndofBeltFlag == 1)
		{
			if (tim1tickflag == delaytim3 && tim3tickflag >= 2)
			{
				lastpart = movestepper(input, nextpart, lastpart);
			}
		}
		//Wait for bit to be in the right spot before moving part to the bin
		else if (EndofBeltFlag == 1 && lastpart == nextpart)
		{
			input->datapulled = 0;
			PORTE = 0x02;
			EndofBeltFlag = 0;
			tim3tickflag = 0;
			tim1tickflag = 0;
		}
		
		//case
		//Check to see if there's a part infront of the reflective sensor. This
		//is controlled by an interrupt
		if (ReflectiveFlag == 1)
		{	
			//ADC conversion complete
			if(ADC_result_flag == 1)
			{
				//Look for the min value from ADC as a part moves past the sensor
				if (ADC_result < storeADC)
				{
					storeADC = ADC_result;
					
				}
				ADC_result_flag = 0;
				ADCSRA |= ADCStart;		//Start next conversion		
			}
		}
		//Once min value is found, use that value to identify the part and have the 
		//part stored in the array.
		if (storeADC < 1023 && ReflectiveFlag == 0)
		{
			storeADC = sort_data (input, storeADC);
		}	
		//If the user pushes the pause button, stop the belt and display all known parts and
		//there location
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
		//Rampdown, check for any new parts on the belt, sort all remaining parts.
		//Everything a new part is seen, timer to shut down is reset.
		if (RampDownFlag == 1)
		{			
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
				while(1);
			}
		}
	}//end while
}//end main
