#include "tim3.h"

void timer3init (void)
{
	TCCR3B |= CTC;			//Turn on count to compare mode
	TCCR3B |= pre1;			//Set timer prescaler to f/256
	TIMSK3 |= CMatchIntEn;	//Enable interrupt for the count match function in timer 1
	OCR3A = 0x03E8;
	TIFR3  |= CMatchIntFl; //Clear count match function interrupt flag timer 1
}


ISR(TIMER3_COMPA_vect)
{
	tim3tickflag++;
	
	if(nextpart == lastpart && tim3tickflag == 5)
	{
		delaytim3 = 36;
		return;
	}
	
	if((((lastpart - nextpart) == 2) || ((lastpart - nextpart) == -2)) && tim3tickflag == delaytim3 && steps <= 100 && steppermove >= 1)		//CW 100 steps
	{
		steps++;
		if(steps <= 24){	
			delaytim3 --; 
			}
		
		if(steps <= 100 && steps >= 76){
			delaytim3 ++; 	}
		
		if (steps == 100)
		{
			delaytim3 = 36;
			steps = 0;
			lastpart = nextpart;
			nextpart = 3;
		}
		switch (curstep)
		{
			
			case 0:
			PORTA = 0b00000101;		//step 3
			curstep = 3;
			tim3tickflag = 0;
			break;
			
			case 3:
			PORTA = 0b00011000;		//step 2
			curstep = 2;
			tim3tickflag = 0;
			break;
			
			case 2:
			PORTA = 0b00000011;		//step 1
			curstep = 1;
			tim3tickflag = 0;
			break;
			
			case 1:
			PORTA = 0b00101000;		//step 4
			curstep = 0;
			tim3tickflag = 3;
			break;
		}
	}
	
	if((((lastpart - nextpart) == 3) || ((lastpart - nextpart) == -1)) && tim3tickflag == delaytim3 && steps <= 50 && steppermove >= 1)		//CW 50 steps
	{
		steps++;
		if(steps <= 24){
		delaytim3 --;}
		
		if(steps <= 50 && steps >= 26){
		delaytim3 ++; }
		
		if (steps == 50)
		{
			delaytim3 = 36;
			steps = 0;
			lastpart = nextpart;
			nextpart = 2;
		}
		switch (curstep)
		{
			
			case 0:
			PORTA = 0b00000101;		//step 3
			curstep = 3;
			tim3tickflag = 0;
			break;
			
			case 3:
			PORTA = 0b00011000;		//step 2
			curstep = 2;
			tim3tickflag = 0;
			break;
			
			case 2:
			PORTA = 0b00000011;		//step 1
			curstep = 1;
			tim3tickflag = 0;
			break;
			
			case 1:
			PORTA = 0b00101000;		//step 4
			curstep = 0;
			tim3tickflag = 3;
			break;
		}
	}
	
	if((((lastpart - nextpart) == 1) || ((lastpart - nextpart) == -3)) && tim3tickflag == delaytim3 && steps <= 50 && steppermove >= 1)		//CCW 50 steps
	{
		steps++;
		if(steps <= 24){
		delaytim3 --;}
		
		if(steps <= 50 && steps >= 26){
		delaytim3 ++; }
		
		if (steps ==  50)
		{
			delaytim3 = 36;
			steps = 0;
			lastpart = nextpart;
			nextpart = 2;
		}
		switch (curstep)
		{
		case 0:
			PORTA = 0b00000011;		//step 1
			curstep = 1;
			tim3tickflag = 0;
			break;
		
		case 1:
			PORTA = 0b00011000;		//step 2
			curstep = 2;
			tim3tickflag = 0;
			break;
		
		case 2:
			PORTA = 0b00000101;		//step 3
			curstep = 3;
			tim3tickflag = 0;
			break;
		
		case 3:
			PORTA = 0b00101000;		//step 4
			curstep = 0;
			tim3tickflag = 0;
			break;
		}
	}
	
	
	
	/*	
	if( tim3tickflag == 14 && steppermove == 1)	//Counter Clock Wise
	{
		steps++;
		switch (curstep)
		{
			
			case 0:
				PORTA = 0b00000011;		//step 1
				curstep = 1;
				tim3tickflag = 0;
				break;
				
			case 1:
				PORTA = 0b00011000;		//step 2
				curstep = 2;
				tim3tickflag = 0;
				break;
				
			case 2:
				PORTA = 0b00000101;		//step 3
				curstep = 3;
				tim3tickflag = 0;
				break;
				
			case 3:
				PORTA = 0b00101000;		//step 4
				curstep = 0;
				tim3tickflag = 0;
				break;	
		}
	}
	
	if( tim3tickflag == 14 && steppermove >= 2) //Clock Wise
	{
		steps++;
		switch (curstep)
		{
			
			case 0:
			PORTA = 0b00000101;		//step 3
			curstep = 3;
			tim3tickflag = 0;
			break;
			
			case 3:
			PORTA = 0b00011000;		//step 2
			curstep = 2;
			tim3tickflag = 0;
			break;
			
			case 2:
			PORTA = 0b00000011;		//step 1
			curstep = 1;
			tim3tickflag = 0;
			break;
			
			case 1:
			PORTA = 0b00101000;		//step 4
			curstep = 0;
			tim3tickflag = 3;
			break;				
		}
	}*/
	
	if(tim3tickflag == 36 && steppermove == 0)	// Home
	{
		switch (curstep)
		{
			
			case 0:
			PORTA = 0b00000011;		//step 1
			curstep = 1;
			tim3tickflag = 0;
			//homestep ++;
			break;
			
			case 1:
			PORTA = 0b00011000;		//step 2
			curstep = 2;
			tim3tickflag = 0;
			//homestep ++;
			break;
			
			case 2:
			PORTA = 0b00000101;		//step 3
			curstep = 3;
			tim3tickflag = 0;
			//homestep ++;
			break;
			
			case 3:
			PORTA = 0b00101000;		//step 4
			curstep = 0;
			tim3tickflag = 0;
			//homestep ++;
			break;
			
		}
	}
}

