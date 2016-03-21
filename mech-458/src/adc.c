/*
 * adc.c
 *
 * Created: 2/29/2016 2:20:45 PM
 *  Author: Tristan
 */ 
#include "adc.h"

void adcinit (void)
{	
	ADCSRA |= ADCEnable;		//Enable ADC module 
	ADCSRA |= ADCIntEn;			//Enable ADC Interrupt
	//ADMUX  |= ADCLeftAdjust;	//Make ADC result left adjust
	ADMUX  |= ADCExRef;			//Turn on external ref for ADC	
}

ISR(ADC_vect)
{
	ADC_result = ADCL | (ADCH << 8);
	ADC_result_flag ^= 1;
	PORTC = ADC_result;
}