
#ifndef TIM3_H_
#define TIM3_H_

#include <asf.h>

//Register defines
#define CTC			0b00001000	//Set timer to count compare mode
#define pre1		0b00000001	//Clock prescaler = 1
#define pre8		0b00000010	//Clock prescaler = 8
#define pre64		0b00000011	//Clock prescaler = 64
#define pre256		0b00000100	//Clock prescaler = 256
#define pre1024		0b00000101	//Clock prescaler = 1024
#define CMatchIntEn	0b00000010	//Interrupt enable for timer 1 count match
#define CMatchIntFl	0b00000010 	//Interrupt flag for timer 1 count match

//External global variables
//volatile extern char ticks;
extern volatile unsigned char tim3tickflag;
extern volatile unsigned char curstep;
extern volatile unsigned char steppermove;
extern volatile unsigned char steps;
extern volatile unsigned char lastpart;
extern volatile unsigned char nextpart;
extern volatile unsigned char delaytim3;
extern volatile unsigned char tim1tickflag;


//Function prototypes
void timer3init (void);
void timer1init (void);


#endif /* TIM3_H_ */