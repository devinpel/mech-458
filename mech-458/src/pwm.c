#include "pwm.h"
#include <util/delay_basic.h>
#include <stdio.h>

void pwminit (void)
{
	/*setting up fast mode PWM using timer 0 */
	//Port B direction
	
	DDRB = 0b10000000; // set PB7 to output for PWM out; all remaining to input
	DDRE = 0x0F;
	
	//7-6 compare match output A set to clear OC0A on compare match and set on OC0A on top, 5-4 compare match output B set to off, 3-2 reserved, 1-0 WGM1:0 set for OCRA to update at top
	TCCR0A = 0b10000011;
	
	//7-6 set to zero to allow for PWM, 5-4 reserved, 3 waveform generation WGM2, 2-0 set clock to be 8-bit from prescaler
	TCCR0B = 0b00000010;
	
	//OCR0A set to top for 8-bit timer
	OCR0A = 0x1F;
		
	//7-3 reserved, 2 for time B or some shit, 1 set to one to enable timer/counter0 interupts, incorrect -> 0 pretty sure this should be set to zero???? (should eb set as one to enable flag)
	//TIMSK0 = 0b00000010;	
}

void pwmchange (U8 newpwm)
{
	OCR0A = newpwm;
}

void pwmcw (void)
{
	PORTE = 0x12;
}

void pwmccw(void)
{
	PORTE = 0x21;
}

void pwmbrake(void)
{
	PORTE = 0x40;
}

void pwmshutdown(void)
{
	PORTE = 0x8F;	
}
