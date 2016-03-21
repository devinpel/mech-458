#include "stepper.h"
#include "tim3.h"
#include <util/delay_basic.h>
#include <stdio.h>


void stepperinit(void)
{
	DDRA = 0b00111111;	//porta pin 7 (for stepper button) and 6 (for PWM button) to input remaining to output
	PORTD = 0b11110000;
}
/*
void movestepper(void)
{
	
	if(lastpart == 0)	//aluminium
	{
// 		switch(nextpart)
// 		{
// 			case 0:		//no move
// 				lastpart = 0;
// 				break;
// 				
// 			case 1:		// move to black
// 				lastpart = nextpart;
// 				//set variables to move
// 				break;
// 			
// 			case 2:		// move to white
// 				lastpart = nextpart;
// 				//set variables to move
// 				break;
// 				
// 			case 3:		// move to steel
// 				lastpart = nextpart;
// 				//set variables to move
// 				break;
		}
	}
}*/