/*
 * stepper.h
 *
 * Created: 2016-03-07 2:32:18 PM
 *  Author: mech458
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

#include <asf.h>
#include "usart.h"
#include "tim3.h"
#include "data.h"
#include <util/delay_basic.h>
#include <stdio.h>

void stepperinit(void);
void casefunc(uint8_t nextstep);
uint8_t movestepper(struct data *input, char nextpart, char lastpart);
void homestepper(void);
void nextstepmove(uint8_t nextstep);

extern volatile unsigned char step;
extern volatile unsigned char HomeFlag;
extern volatile unsigned char delaytim3;

extern volatile unsigned char EndofBeltFlag;



#endif /* STEPPER_H_ */