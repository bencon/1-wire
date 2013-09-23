#ifndef _PCA9553_H
#define _PCA9553_H

#include "main.h"


//dont forget to | with WR/RD
#define LED_DRIVER_ADDRESS 0xC4 //if this doesn't work, try C6

#define AUTO_INCREMENT 0x10 //need to or with B2-B0
//B2 - B0 bits
#define PSC0 0b001
#define PWM0 0b010
#define PSC1 0b011
#define PWM1 0b100
#define LS0  0b101

void LEDDriverWriteByte(uint8_t reg, uint8_t value);
void LEDDriverWriteFull(uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4,uint8_t value5); 


#endif