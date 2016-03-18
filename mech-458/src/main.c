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

volatile unsigned char ADC_result;
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
			ReflectiveFlag = 0;
			}
		if(EndofBeltFlag == 1)
		{
			pwmbrake();
			EndofBeltFlag = 0;
		}
	}
}
