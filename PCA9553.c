#include "PCA9553.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"

extern i2cStruct LED_Driver;
extern i2cStruct *previous;

//use this to write a value to a single register of the LED DRIVER
//as defined in the header file
void LEDDriverWriteByte(uint8_t reg, uint8_t value){
	//while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
	previous = &LED_Driver;
	LED_Driver.buffer[0] = LED_DRIVER_ADDRESS;
	LED_Driver.buffer[1] = reg;
	LED_Driver.buffer[2] = value; //lsB of address
	LED_Driver.instructionCount = 3;
	LED_Driver.currentInstruction = 0;
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm
}

//use this function to completely write the LED Driver
void LEDDriverWriteFull(uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4,uint8_t value5){
  //while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
  previous = &LED_Driver;
  LED_Driver.buffer[0] = AUTO_INCREMENT | PSC0;  //this should be the same as 11h
  LED_Driver.buffer[1] = value1;
  LED_Driver.buffer[2] = value2;
  LED_Driver.buffer[3] = value3;
  LED_Driver.buffer[4] = value4;
  LED_Driver.buffer[5] = value5;
  LED_Driver.instructionCount = 6;
  LED_Driver.currentInstruction = 0;
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm
}

void testPCA(){
   previous = &LED_Driver;
   LED_Driver.buffer[0] = 0xC4;
   LED_Driver.buffer[1] = 0x11;
   /*
   LED_Driver.buffer[2] = 0x2b; //test as in programming example
   LED_Driver.buffer[3] = 0x80;
   LED_Driver.buffer[4] = 0x0A;
   LED_Driver.buffer[5] = 0xC0;
   LED_Driver.buffer[6] = 0xE4;
   */
   LED_Driver.buffer[2] = 0x2b; // PWM0 blink period 1 sec
   LED_Driver.buffer[3] = 0x80; // PWM0 50% duty cycles
   LED_Driver.buffer[4] = 0x0A; // blink period 11/44s
   LED_Driver.buffer[5] = 0x80; // PWM1 50% duty
   LED_Driver.buffer[6] = 0b11011000; //internet at PWM1, error off, charging at pwm0, power on
   //LED_Driver.buffer[6] = 0b10101010;
   LED_Driver.instructionCount = 7;
   LED_Driver.currentInstruction = 0; 
   (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm 
}