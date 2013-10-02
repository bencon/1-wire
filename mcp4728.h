#ifndef _MCP4728_H
#define _MCP4728_H

//MCP4728 Write Commands (bits c2-c0[, w1-w0])
#define DAC_WRITE 0xC0
#define DAC_READ 0xC1
#define SELECT_VREF_BIT 0b100
#define SELECT_POWER_DOWN_BITS 0b101
#define SELECT_GAIN 0b110
#define SEQUENTIAL_DAC_EEPROM_WRITE 0b01010
#define SINGLE_DAC_EEPROM_WRITE 0b01011

#include "main.h"

void DACSingleChannelWrite(uint8_t channel, double voltage);
void DACSequentialWrite(double C1, double C2, double C3, double C4);
void testDAC(void);

//Notes-- In order to use the general call read address command, the LDAC pin
//	needs a logic transition from "High" to "Low". This is how the bus
//	knows which device to address
//This should be OK since we are only using one DAC

/*
VREF Select-> 0 = External VREF, 1 = Internal VREF (2.048V)
RDY/BSY pin is a pin that shows the status of programming activity
Can update analog outputs if UDAC or LDAC pin is low
DAC1/DAC0 are used to selected channels A-D
PD1/PD0 
  00- Normal Mode
  01- VOut is loaded with 1kohm resister to ground
  10-                     100kohm 
  11-                     500kohm
Gx - Gain Selection Bit-> 0=gain of 1 , 1=gain of 2
UDac-> 0= Upload. Output (Vout is updated), 1= do not upload

Vout:
vref = internal:   vout = (vref * Dn * Gx)/4096   <--- vref = 2.048
vref = vdd         vout = (vdd * Dn) / 4096
*/



#endif