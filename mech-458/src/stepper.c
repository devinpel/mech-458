#include "stepper.h"


/* initialize porta for use by the stepper */
void stepperinit(void)
{
	DDRA = 0b00111111;	//porta pin 7 and 6 unused, remaining set to output
}

/* rotate the stepper to the home position */
void homestepper(void)
{
	uint8_t nextstep = 0; 
	while(HomeFlag == 0)	//rotate until the hall effect sensor is hit
	{
		nextstep = curstep + 1;	// set next step
		if (nextstep == 4)
		{
			nextstep = 0;
		}
		if (tim1tickflag == delaytim3)	//if the wait time between step is hit 
		{
			nextstepmove(nextstep);	//increment stepper motor to next step
		}
	} 
	HomeFlag = 0;	//reset interrupt flag
	EIMSK &= ~(_BV(INT6));	//turn off hall effect sensor interrupt
}

/* function call to move stepper to next step and to next part location */
uint8_t movestepper(struct data *input, char nextpart, char lastpart)
{
	//set initial variable values
	char move = 0;	
	char nextstep = 0;
	char delayconst = 20;
	char accel = 6;
	
	move = lastpart - nextpart;	//find quickest path to next part location
	
	if(nextpart == 5)	// if unknown part kick it off belt to current bin
	{
		PORTE = 0x02;	// start motor
		//reset interupt flags
		EndofBeltFlag = 0;
		tim3tickflag = 0;
		tim1tickflag = 0;
		input->datapulled = 0;
	}
	else if (move == 2 || move == -2)	//rotate counter clockwise 180 degrees
	{
		if (step < 20)	//accelerate
		{
			delaytim3--;
			delaytim3--;
			
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 89)	//decelerate
		{
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
		if (step <= 100)	//rotate to next step	
		{
			nextstepmove(nextstep);
			step++;
		}
		if (step >= 100)	//enter when rotation complete
		{
			step = 0;
			lastpart = nextpart;
		}
	}
	
	else if (move == 3 || move == -1)	//rotate clockwise 90 degrees
	{
		if (step < 20)	//accelerate
		{
			delaytim3--;
			delaytim3--;
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 39)	//decelerate
		{
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
		if (step <= 50)	//rotate to next step	
		{
			nextstepmove(nextstep);
			step++;
		}
		if (step >= 50)	//enter when rotation complete
		{
			step = 0;
			lastpart = nextpart;
		}
	}
		
	else if (move == 1 || move == -3)	//rotate counter clockwise 90 degrees
	{
		if (step < 20)	//accelerate
		{
			delaytim3--;
			delaytim3--;
			if (delaytim3 < accel)
			{
				delaytim3 = accel;
			}
		}
		if (step > 39)	//decelerate
		{
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
		if (step <= 50)	//rotate to next step
		{
			nextstepmove(nextstep);
			step++;
		}	
		if (step >= 50)	//enter when rotation complete
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
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 1:
		PORTA = 0b00011000;		//step 2
		curstep = 1;
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 2:
		PORTA = 0b00000101;		//step 3
		curstep = 2;
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 3:
		PORTA = 0b00101000;		//step 4
		curstep = 3;
		tim1tickflag = 0;	//reset timer flag
		break;
	}
}

void nextstepmove(uint8_t nextstep)
{
	switch (nextstep)
	{
		case 0:
		PORTA = 0b00011011;		//step 1 motor one+
		curstep = 0;
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 1:
		PORTA = 0b00011101;		//step 2 motor two+
		curstep = 1;
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 2:
		PORTA = 0b00101101;		//step 3 motor one-
		curstep = 2;
		tim1tickflag = 0;	//reset timer flag
		break;
		
		case 3:
		PORTA = 0b00101011;		//step 4 motor two-
		curstep = 3;
		tim1tickflag = 0;	//reset timer flag
		break;
	}
}


