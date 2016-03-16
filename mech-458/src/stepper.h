/*
 * stepper.h
 *
 * Created: 2016-03-07 2:32:18 PM
 *  Author: mech458
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

#include <asf.h>

void stepperinit(void);
void stepthestepper(U8 step);
void stepthestepperneg(U8 step);
void findstep(void);
void movestepper(void);

extern volatile unsigned char lastpart;
extern volatile unsigned char nextpart;

#endif /* STEPPER_H_ */