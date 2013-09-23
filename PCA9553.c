#include "PCA9553.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"

extern i2cStruct *LED_Driver;

//use this to write a value to a single register of the LED DRIVER
//as defined in the header file
void LEDDriverWriteByte(uint8_t reg, uint8_t value){
	while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
	LED_Driver->buffer[0] = LED_DRIVER_ADDRESS;
	LED_Driver->buffer[1] = reg;
	LED_Driver->buffer[2] = value; //lsB of address
	LED_Driver->instructionCount = 3;
	LED_Driver->currentInstruction = 0;
  (void)i2c_fsm(LED_Driver,I2C_EEPROM_WR);   //set the fsm
}

//use this function to completely write the LED Driver
void LEDDriverWriteFull(uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4,uint8_t value5){
  while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
  LED_Driver->buffer[0] = AUTO_INCREMENT | PSC0;  //this should be the same as 11h
  LED_Driver->buffer[1] = value1;
  LED_Driver->buffer[2] = value2;
  LED_Driver->buffer[3] = value3;
  LED_Driver->buffer[4] = value4;
  LED_Driver->buffer[5] = value5;
  LED_Driver->instructionCount = 6;
  LED_Driver->currentInstruction = 0;
  (void)i2c_fsm(LED_Driver,I2C_EEPROM_WR);   //set the fsm
}