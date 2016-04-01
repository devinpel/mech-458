#include "stepper.h"
#include "tim3.h"
#include <util/delay_basic.h>
#include <stdio.h>


void stepperinit(void)
{
	DDRA = 0b00111111;	//porta pin 7 (for stepper button) and 6 (for PWM button) to input remaining to output
}

void homestepper(void)
{
	uint8_t nextstep = 0; 
	while(HomeFlag == 0)
	{
		nextstep = curstep + 1;
		if (nextstep == 4)
		{
			nextstep = 0;
		}
		if (tim1tickflag == delaytim3)
		{
			testcwfunc(nextstep);
		}
	} 
	HomeFlag = 0;
	EIMSK &= ~(_BV(INT6));
}

uint8_t movestepper(char nextpart, char lastpart)
{
	char move = 0;
	char nextstep = 0;
	char delayconst = 21;
	char accel = 8;
	
// 	usartTX(nextpart+0x30);
// 	usartTXs("\t");
// 	usartTX(lastpart+0x30);
// 	usartTXs("\n\r");
	
	move = lastpart - nextpart;
	if (move == 2 || move == -2)	//ccw
	{
		if (step < 20)
		{
			delaytim3--;
			delaytim3--;
			
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 86)
		{
			//delaytim3 = delaytim3 + (delaytim3 / 10);
			delaytim3++;
			delaytim3++;
			if (delaytim3 > delayconst)
			{
				delaytim3 = delayconst;
			}
		}
		
		nextstep = curstep - 1;
		
		if (nextstep == -1)
		{
			nextstep = 3;
		}
		if (step <= 100)
		{
			//casefunc(nextstep);
			testcwfunc(nextstep);
			step++;
		}
		if (step >= 100)
		{
			step = 0;
			lastpart = nextpart;
		}
	}
	
	else if (move == 3 || move == -1)
	{
		if (step < 20)
		{
			//delaytim3 = delaytim3 - (delaytim3 / 10);
			delaytim3--;
			delaytim3--;
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 36)
		{
			//delaytim3 = delaytim3 + (delaytim3 / 10);
			delaytim3++;
			delaytim3++;
			if (delaytim3 > delayconst)
			{
				delaytim3 = delayconst;
			}
		}
		nextstep = curstep - 1;
		if (nextstep == -1)
		{
			nextstep = 3;
		}
		if (step <= 50)
		{
			//casefunc(nextstep);
			testcwfunc(nextstep);
			step++;
		}
		if (step >= 54)
		{
			step = 0;
			lastpart = nextpart;
		}
	}
		
	else if (move == 1 || move == -3)	//ccw
	{
		if (step < 20)
		{
			//delaytim3 = delaytim3 - (delaytim3 / 10);
			delaytim3--;
			delaytim3--;
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 36)
		{
			//delaytim3 = delaytim3 + (delaytim3 / 10);
			delaytim3++;
			delaytim3++;
			if (delaytim3 > delayconst)
			{
				delaytim3 = delayconst;
			}
		}
		nextstep = curstep + 1;
		if (nextstep == 4)
		{
			nextstep = 0;
		}
		if (step <= 50)
		{
			testcwfunc(nextstep);
			//testcwfunc(nextstep);
			step++;
		}	
		if (step >= 50)
		{
			step = 0;
			lastpart = nextpart;
		}
	}
	return(lastpart);
}

void casefunc(uint8_t nextstep)
{
	switch (nextstep)
	{
		case 0:
		PORTA = 0b00000011;		//step 1
		curstep = 0;
		tim1tickflag = 0;
		break;
		
		case 1:
		PORTA = 0b00011000;		//step 2
		curstep = 1;
		tim1tickflag = 0;
		break;
		
		case 2:
		PORTA = 0b00000101;		//step 3
		curstep = 2;
		tim1tickflag = 0;
		break;
		
		case 3:
		PORTA = 0b00101000;		//step 4
		curstep = 3;
		tim1tickflag = 0;
		break;
	}
}

void testccwfunc(uint8_t nextstep)
{
	switch (nextstep)
	{
		case 0:
		PORTA = 0b00101011;		//step 1
		curstep = 0;
		tim1tickflag = 0;
		break;
		
		case 1:
		PORTA = 0b00011011;		//step 2
		curstep = 1;
		tim1tickflag = 0;
		break;
		
		case 2:
		PORTA = 0b00011101;		//step 3
		curstep = 2;
		tim1tickflag = 0;
		break;
		
		case 3:
		PORTA = 0b00101101;		//step 4
		curstep = 3;
		tim1tickflag = 0;
		break;
	}
}
void testcwfunc(uint8_t nextstep)
{
	switch (nextstep)
	{
		case 0:
		PORTA = 0b00011011;		//step 1 motor one+
		curstep = 0;
		tim1tickflag = 0;
		break;
		
		case 1:
		PORTA = 0b00011101;		//step 2 motor two+
		curstep = 1;
		tim1tickflag = 0;
		break;
		
		case 2:
		PORTA = 0b00101101;		//step 3 motor one-
		curstep = 2;
		tim1tickflag = 0;
		break;
		
		case 3:
		PORTA = 0b00101011;		//step 4 motor two-
		curstep = 3;
		tim1tickflag = 0;
		break;
	}
}


