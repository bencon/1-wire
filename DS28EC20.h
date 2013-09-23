#ifndef _DS28EC20_H
#define _DS28EC20_H

//Family Code = 0x43

//#############  DS2431 Commands  ###################

//All of these defines are the same as the DS2431 except for 
//extendedreadmem

#define DS28EC20_WRITESCRATCH 0x0F
#define DS28EC20_READSCRATCH 0xAA
#define DS28EC20_COPYSCRATCH 0x55
#define DS28EC20_EXTENDEDREADMEM 0xF0
#define DS28EC20_READMEM 0xF0
#define DS28EC20_READROM 0x33
#define DS28EC20_MATCHROM 0x55
#define DS28EC20_SEARCHROM 0xF0
#define DS28EC20_SKIPROM 0xCC
#define DS28EC20_RESUME 0xA5
#define DS28EC20_OVERDRIVESKIP 0x3C
#define DS28EC20_OVERDRIVEMATCH 0x69


void readScratch_28EC20(uint8_t romNum);
void copyScratch_28EC20(uint8_t data[8], uint16_t address, uint8_t romNum, uint8_t len);
void readMemory_28EC20(uint8_t romNum, uint16_t address, uint8_t len);
void setProtection_28EC20(uint8_t romNum, uint8_t page0, uint8_t page1, uint8_t page2, uint8_t page3);

typedef struct{
  uint8_t romTable[16][8];//to hold at max devices
  uint8_t deviceCount; //a count of how many 1-wire deivices are on the bus
  uint8_t scratchpad[40];
  uint8_t memory[2][2560];
  uint8_t stage;
} DS28EC20;

#endif




