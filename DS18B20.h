#ifndef _DS18B20_H
#define _DS18B20_H

//#############  DS18B20 Commands  ###################

#define DS18B20_RESET 0xF0
#define DS18B20_READROM 0x33
#define DS18B20_MATCHROM 0x55
#define DS18B20_SEARCHROM 0xF0
#define DS18B20_SKIPROM 0xCC
#define DS18B20_CONVERTT 0x44
#define DS18B20_WRITESCRATCH 0x4E
#define DS18B20_READSCRATCH 0xBE 
#define DS18B20_COPYSCRATCH 0x48
#define DS18B20_RECALL 0xB8
#define DS18B20_READPOWER 0xB4

//void reset(void);
void aquireScratch_B20(uint8_t rom[], uint8_t deviceNum);
void writeScratch_B20(uint32_t data, uint8_t len, uint8_t rom[]);
void aquireROM_B20(void);
BOOL readPower_B20(uint8_t rom[]);
//void searchROM(uint8_t rom_num, uint8_t discrepancy_count);
//void pushStack(uint8_t bit_num, uint8_t rom_num, uint8_t temp);
//void popStack();
void convertTemp(uint8_t rom[], BOOL skip);
BOOL checkScratch_B20(uint8_t deviceIndex);
double farenheitConversion(uint8_t device);

typedef struct{
  uint8_t romTable[16][8];//to hold at max devices
  uint8_t deviceCount; //a count of how many 1-wire deivices are on the bus
  uint8_t scratchpad[8][9];
  uint8_t stage;
} DS18B20;

#endif