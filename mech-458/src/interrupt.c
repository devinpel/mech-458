/*
 * interrupt.c
 *
 * Created: 3/2/2016 1:34:03 PM
 *  Author: Tristan
 */ 

#include "interrupt.h"

void interruptInit (void)
{
	EICRA |= _BV(ISC01);				//Set ext int0 to falling edge, EX Interrupt
	EICRA |= _BV(ISC11);				//Set ext int1 to falling edge, OI Interrupt 
	EICRA |= _BV(ISC20);				//Set ext int2 to either edge, OR Interrupt
	EICRB |= _BV(ISC41) | _BV(ISC40);	//Set ext int4 to rising edge, Pause button
	EICRB |= _BV(ISC51) | _BV(ISC50);	//Set ext int5 to rising edge, Ramp down button
	EICRB |= _BV(ISC61);				//Set ext int4 to falling edge, Home position
	
	EIMSK |= 0b11000111;				//Enable ext int1-int6
	//EIMSK |= 0b00100000;
}

//End of belt sensor
ISR(INT0_vect)
{
	EndofBeltFlag = 1;
	pwmbrake();
}

//Start of belt sensor
ISR(INT1_vect)
{
	count++;
}

//Reflective sensor activation
ISR(INT2_vect)
{
	ReflectiveFlag ^= 1;
}

//Pause Button
ISR(INT4_vect)
{
	PauseFlag = 1;
}

//Ramp down button
ISR(INT5_vect)
{
	RampDownFlag = 1;
	tim1tickflag = 0;
}

//Stepper home
ISR(INT6_vect)
{
	HomeFlag = 1;
}

ISR(INT7_vect)
{
	calibrationFlag = 1;
}

ISR(BADISR_vect)
{
	PORTD |= 0xF0;
}