/*
 * data.h
 *
 * Created: 3/10/2016 1:21:10 PM
 *  Author: Tristan
 */ 


#ifndef DATA_H_
#define DATA_H_

#include <asf.h>
#include <string.h>
#include "usart.h"
#include "pwm.h"
#include "adc.h"

struct data
{
	uint8_t head;
	uint8_t tail;
	uint8_t black;
	uint8_t white;
	uint8_t aluminum;
	uint8_t steel;
	uint8_t unknown;
	uint8_t queue [16];
	uint8_t datapulled;
};

extern volatile unsigned char calibrationFlag;
extern volatile unsigned char count;
extern volatile unsigned char ReflectiveFlag;

void insert_data (struct data *input, uint8_t val);
void display_data (struct data *input);
void display_data_value (uint8_t val);
void clearQueue (struct data *input);
uint8_t pop_data (struct data *input);
void displayVal (uint16_t storeADC);
void calibration (void);
uint16_t sort_data (struct data *input, uint16_t storeADC);
void display_paused_data (struct data *input);

#endif /* DATA_H_ */