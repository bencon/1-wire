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
void LEDSet(uint8_t lednum, uint8_t setting);
void LEDSetBlink(uint8_t channel, double rate);
void LEDSetPWM(uint8_t channel, uint8_t pwm);
BOOL checkON(uint8_t light);
void toggleLED(uint8_t light);

#define LED_OFF  0
#define LED_ON   1
#define LED_PWM1 2
#define LED_PWM2 3
                     
#define LED_POWER     1
#define LED_CHARGING   2
#define LED_ERROR     3
#define LED_INTERNET  4

/*
PWM duty cycles = (256 - value)/256
so for a 50% duty cycle, set value to 0x80 (128 decimal)

Blink Period = (value +1)/44
so a blink period of .25s, set value to 0x0A

LS0 - LED selector registor
00 - On, 01 0ff, 10, blink at PWM0 rate, 11 blink at PWM1 rate
bits 7:6- led3, 5:4- led2 3:2- led1, 1:0- led0

selecting a register to program (without auto-increment) - 0x00 | B2B1B0
B2B1B0
0 0 0  Input (read only)
0 0 1  PSC0
0 1 0  PWM0
0 1 1  PSC1
1 0 0  PWM1
1 0 1  LS0
*/

#endif