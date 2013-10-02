#ifndef _PCA9553_H
#define _PCA9553_H

#include "main.h"


//dont forget to | with WR/RD
#define LED_DRIVER_ADDRESS 0xC4 //if this doesn't work, try C6

#define AUTO_INCREMENT 0x10 //need to or with B2-B0
//B2 - B0 bits
#define PSC0 0x01
#define PWM0 0b010
#define PSC1 0b011
#define PWM1 0b100
#define LS0  0b101

void LEDDriverWriteByte(uint8_t reg, uint8_t value);
void LEDDriverWriteFull(uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4,uint8_t value5); 
void testPCA(void);

/*
PWM duty cycles = (256 - value)/256
so for a 50% duty cycle, set value to 0x80 (128 decimal)

Blink Period = (value +1)/44
so a blink period of .25s, set value to 0x0A

LS0 - LED selector registor
00 - On, 01 0ff, 10, blink at PWM0 rate, 11 blink at PWM1 rate
bits 7:6- led3, 5:4- led2 3:2- led1, 1:0- led0
*/

#endif