#ifndef _I2C_H
#define _I2C_H

#include "main.h"

#define MCP4728_CODE 1100 //ms 4 bits of the MCP4728 address
//next three bits A2,A1, and A0 are user defined. Default is 000

#define RD 1
#define WR 0

//I2C General Call Commands
#define GENERAL_ADDRESS 0x00
#define GENERAL_RESET 0x06 //contents of EEPROM are loaded into each DAC input and output registers immediately
#define GENERAL_WAKE-UP 0x09 //resets the power-down bits PD1, PD0
#define GENERAL_SOFTWARE_UPDATE 0x08 //updates all DAC analog outputs at the same time
#define GENERAL_READ_ADDRESS 0x0C //outputs its address bits stored in EERPROM and register, but needs the LDAC pin to transition from high to low

typedef enum{
	I2C_IDLE,
	I2C_START,I2C_REPEATED_START,
	I2C_WRITE_DATA,
	I2C_WRITE_BYTE,
	I2C_DUMMY_READ, I2C_READ_BYTE,
	I2C_STOP,
	I2C_EEPROM_WR, I2C_EEPROM_RD //may delete/replace
} i2c_states;  

typedef struct{
  uint8_t buffer[15]; //buffer of instructions to be executed (10 is arbitrary)
  uint8_t instructionCount; //number of instructions in the buffer so the fsm knows when to stop
  uint8_t currentInstruction; //instruction to be executed
  enum i2c_states state;
  char current_command;
  char ready;  
} i2cStruct;    

void i2cInit(void);
i2c_states i2c_fsm(i2cStruct *device, char new_state);
//char eeprom_byteread(uint8_t address);
//void eeprom_byte_write(uint8_t address, char data);
void initI2cStruct(i2cStruct *device);
void I2CWait(void);




#endif

