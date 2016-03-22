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

void stepperinit(void);
void casefunc(uint8_t nextstep);
uint8_t movestepper(uint8_t nextpart, uint8_t lastpart);
void homestepper(void);

extern volatile unsigned char step;
extern volatile unsigned char HomeFlag;
extern volatile unsigned char delaytim3;

extern volatile unsigned char EndofBeltFlag;



#endif /* STEPPER_H_ */