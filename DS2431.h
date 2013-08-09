#ifndef _DS2431_H
#define _DS2431_H


//#############  DS2431 Commands  ###################

#define DS2431_WRITESCRATCH 0x0F
#define DS2431_READSCRATCH 0xAA
#define DS2431_COPYSCRATCH 0x55
#define DS2431_READMEM 0xF0
#define DS2431_READROM 0x33
#define DS2431_MATCHROM 0x55
#define DS2431_SEARCHROM 0xF0
#define DS2431_SKIPROM 0xCC
#define DS2431_RESUME 0xA5
#define DS2431_OVERDRIVESKIP 0x3C
#define DS2431_OVERDRIVEMATCH 0x69

void readScratch_2431(uint8_t romNum);
void copyScratch_2431(uint8_t data[8], uint16_t address, uint8_t romNum, uint8_t len);
void readMemory_2431(uint8_t romNum, uint16_t address, uint8_t len);
void setProtection_2431(uint8_t romNum, uint8_t page0, uint8_t page1, uint8_t page2, uint8_t page3);
//BOOL checkScratch(uint8_t deviceIndex);
void setOnes_2431(uint8_t romNum, BOOL readMem);

typedef struct{
  uint8_t romTable[16][8];//to hold at max devices
  uint8_t deviceCount; //a count of how many 1-wire deivices are on the bus
  uint8_t scratchpad[14];
  uint8_t memory[8][144];
  uint8_t stage;
} DS2431;

#endif

/*
Protection Settings

55h -> write protect
AAh -> EPROM mode

Three address registers, TA1, TA1, E/S
TA1 and TA2 must be loaded with the target
address to which the data is written or from
which the data is read. E/S is a read-only
transfer status register, used to verify 
data integrity with write commands


*/



