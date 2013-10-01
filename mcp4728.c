#include "mcp4728.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"

extern i2cStruct DAC;
extern i2cStruct *previous;


//page 40 of manual for example and how to set instruction
void DACSequentialWrite(uint8_t startChannel, uint16_t instruction){
  /*
  uint8_t index;
  uint8_t count = 4;
  while (i2c_fsm(DAC,0) != I2C_IDLE);   //wait for fsm to go idle
     DAC->buffer[0] = DAC_WRITE;
     DAC->buffer[1] = (SEQUENTIAL_DAC_EEPROM_WRITE <<3)| (startChannel << 1) | 0;
     DAC->buffer[2] = instruction >> 8;
     DAC->buffer[3] = (uint8_t)(0x00FF & instruction);
     for (index = startChannel;index <3;index++){
         DAC->buffer[count] = instruction >> 8;
         DAC->buffer[count+1] = (uint8_t)(0x00FF & instruction);
         count+=2;
     }
     //instruction count, current instruction, etc.
     */ 
}

void testDAC(){
    previous = &DAC;
    DAC.buffer[0] = DAC_WRITE;
    DAC.buffer[1] = 0x50;
    DAC.buffer[2] = 0x9F;
    DAC.buffer[3] = 0xFF;
    DAC.buffer[4] = 0x97;
    DAC.buffer[5] = 0xFF;
    DAC.buffer[6] = 0x93;
    DAC.buffer[7] = 0xFF;
    DAC.buffer[8] = 0x91;
    DAC.buffer[9] = 0xFF;
    DAC.instructionCount = 10;
    DAC.currentInstruction = 0;
    (void)i2c_fsm(&DAC,I2C_EEPROM_WR);   //set the fsm
}
    
    
    
     
 
        