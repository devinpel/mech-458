/*
 * interrupt.h
 *
 * Created: 3/2/2016 1:34:46 PM
 *  Author: Tristan
 */ 


#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <asf.h>
#include "tim3.h"
#include "pwm.h"

void interruptInit (void);


extern volatile unsigned char ADC_result_flag;
extern volatile unsigned char EndofBeltFlag;
extern volatile unsigned char count;
extern volatile unsigned char ReflectiveFlag;
extern volatile unsigned char PauseFlag;
extern volatile unsigned char RampDownFlag;
extern volatile unsigned char HomeFlag;


#endif /* INTERRUPT_H_ */