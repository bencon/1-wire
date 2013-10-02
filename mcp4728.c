#include "mcp4728.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"

extern i2cStruct DAC;
extern i2cStruct *previous;

//specify channel 1-4 and the voltage, eg. 2.5 that you want to write
void DACSingleChannelWrite(uint8_t channel, double voltage){
    uint16_t a;
    previous = &DAC;
    a = 1000*voltage;
    DAC.buffer[0] = DAC_WRITE;
    DAC.buffer[1] = (0x58 | ((channel-1)<<1));
    DAC.buffer[2] = (uint8_t)(0x90 | (a>>8));
    DAC.buffer[3] = (uint8_t)(0xFF & a);   
}

//page 40 of manual for example and how to set instruction
//the parameters are the desired voltages for channels 1-4 in Volts
void DACSequentialWrite(double C1, double C2, double C3, double C4){
    uint16_t a;
    uint16_t b;
    uint16_t c;
    uint16_t d;
    previous = &DAC;
    a = 1000*C1;
    b = 1000*C2;
    c = 1000*C3;
    d = 1000*C4;
    if (a>0xFFF){
      a = 0xFFF;
    }
    if (b>0xFFF){
      b = 0xFFF;
    }
    if (c>0xFFF){
      c = 0xFFF;
    }
    if (d>0xFFF){
      d = 0xFFF;
    }
    DAC.buffer[0] = DAC_WRITE;
    DAC.buffer[1] = 0x50;
    DAC.buffer[2] = (uint8_t)(0x90 | (a>>8));
    DAC.buffer[3] = (uint8_t)(0xFF & a);
    DAC.buffer[4] = (uint8_t)(0x90 | (b>>8));
    DAC.buffer[5] = (uint8_t)(0xFF & b);
    DAC.buffer[6] = (uint8_t)(0x90 | (c>>8));
    DAC.buffer[7] = (uint8_t)(0xFF & c);
    DAC.buffer[8] = (uint8_t)(0x90 | (d>>8));
    DAC.buffer[9] = (uint8_t)(0xFF & d);
    DAC.instructionCount = 10;
    DAC.currentInstruction = 0;    
    (void)i2c_fsm(&DAC,I2C_EEPROM_WR);   //set the fsm}
}

//see page 40 in RM
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
    
    
    
     
 
        