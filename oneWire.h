#ifndef _ONE_WIRE_H
#define _ONE_WIRE_H

//#############  DS18B20 Commands  ###################

#define RESET 0xF0
#define READROM 0x33
#define MATCHROM 0x55
#define SEARCHROM 0xF0
#define SKIPROM 0xCC


void initSCI(void);
void setBaud(int value);
void init1Wire(void);
void writeByte(uint8_t command);
void writeBit(BOOL value);
void en_T(void);
void dis_T(void);
void en_R(void);
void dis_R(void);
void up(void);
void down(void);
void SCI4_RXRoutine(void);
void SCI4_TXRoutine(void);
void writeData(void);
void readOneBit(void);
void storeData(uint8_t len, uint8_t *location);
void getBits(uint8_t len);
void fillWrite(uint32_t data, uint8_t len);
void resetStack(void);
void pushStack(uint8_t bit_num, uint8_t rom_num, uint8_t incomplete);
void popStack(void);
void searchROM(uint8_t location[16][8] , uint8_t rom_num, uint8_t discrepancy_count, uint8_t family, uint8_t * deviceCount);
void readROM(uint8_t *location, uint8_t *deviceCount);

void reset();
void writeROM(uint8_t rom[]);

typedef struct{
  uint8_t stage; //0- aquire ROM, 1- aquire scratchpad,
  uint8_t mode; //0- doing nothing, 1- write, 2-read
  uint16_t cycles; //basic index
  uint16_t stop; //index of when to stop current instruction, i.e cycles == stop
  uint8_t data; //byte to be transmitted
  uint8_t receive;
  uint8_t deviceCount; //a count of how many 1-wire deivices are on the bus
  uint8_t storage[16]; //read data goes here before a transfer to its appropriate location
  uint8_t toWrite[16]; //data to write
  uint8_t read;
  uint8_t done;
} oneWire;

typedef struct{  //structure used when doing SearchROM
  uint8_t rom_num;
  uint8_t bit_num;
  uint8_t incomplete;
} searchStack;

#endif