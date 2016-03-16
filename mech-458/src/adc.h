/*
 * adc.h
 *
 * Created: 2/29/2016 2:21:00 PM
 *  Author: Tristan
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <asf.h>
	
#define ADCEnable		0b10000000
#define ADCIntEn		0b00001000
#define ADCIntFl		0b00010000
#define ADCExRef		0b01000000
#define ADCLeftAdjust	0b00100000
#define ADCStart		0b01000000

extern volatile unsigned char ADC_result;
extern volatile unsigned char ADC_result_flag;		

void adcinit (void);


#endif /* ADC_H_ */