#include "main.h"
#include "oneWire.h"
#include "DS2431.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern oneWire info;
extern DS2431 status_2431;
extern Bool T_EN;
extern Bool R_EN;
extern uint8_t temp; //dummy for clearing the SCI4 recieve full flag
extern uint8_t SCI1TOBESENT;//use this only when you have a single 1-wire chip on the bus

//len determines the number of rows you want to copy the same 8 bytes to
//since there are 16 writable rows, 16 should be the greatest len
void copyScratch_2431(uint8_t data[8], uint16_t address, uint8_t romNum, uint8_t len){
    uint8_t debug;
    int i;
    int j;
    for (j=0;j<len;j++){ 
      reset();
      writeByte(MATCHROM);
      writeROM(status_2431.romTable[romNum]);
      writeByte(DS2431_WRITESCRATCH);
      for (i=0;i<2;i++){ //write least significant then most significant byte of target address
         debug = (address>>(8*i)) & 0xFF;
         writeByte((address>>(8*i)) & 0xFF);
      }
      for (i=0;i<8;i++){
         writeByte(data[i]);
      }
      readScratch_2431(romNum);
      reset();//reset?
      writeByte(MATCHROM); //
      writeROM(status_2431.romTable[romNum]); //
      writeByte(DS2431_COPYSCRATCH);
      for (i=0;i<3;i++){
          writeByte(status_2431.scratchpad[i]); 
      }
      address = address + 8;
      DelayUs(800);
    }
    //DelayMs(300);
    //readMemory_2431(romNum, address, len); //may not want this here
    
}

//This is intended to set all memory to FF so that EPROM mode will work. Should probably either fix or remove this function
void setOnes_2431(uint8_t romNum, BOOL readMem){
    uint8_t ind;
    uint16_t add = 0x0000;
    uint8_t ones[] = {{0xEF},{0xEF},{0xEF},{0xEF},{0xFF},{0xFF},{0xFF},{0xFF}};
    for (ind=0;ind<16;ind++){
        add = ind*8;
        copyScratch_2431(ones,add,romNum,1);  
    }
    if (readMem){
        readMemory_2431(romNum, 0x0000, 128);   //optional
    }
}

//reads scratchpad and puts the data in the given scratchpad memory 
void readScratch_2431(uint8_t romNum){
    reset();//reset necessary?
    writeByte(MATCHROM);
    writeROM(status_2431.romTable[romNum]);
    writeByte(DS2431_READSCRATCH);
    storeData(20, status_2431.scratchpad);
}

void readMemory_2431(uint8_t romNum, uint16_t address, uint8_t len){
    uint8_t debug;
    int i;
    reset();
    writeByte(MATCHROM);
    writeROM(status_2431.romTable[romNum]);
    writeByte(DS2431_READMEM);
    for (i=0;i<2;i++){ //write least significant then most significant byte of target address
       debug = (address>>(8*i)) & 0xFF;
       writeByte((address>>(8*i)) & 0xFF);
    }
    up();
    storeData(len, &status_2431.memory[romNum][address]);
    down();
}

//Use this function to set protections on pages 0-3 of memory
//55h for Write Protect, AAh for Read, 00h for no protections 
void setProtection_2431(uint8_t romNum, uint8_t page0, uint8_t page1, uint8_t page2, uint8_t page3){
    uint8_t data[8];
    data[0] = page0;
    data[1] = page1;
    data[2] = page2;
    data[3] = page3;
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;
    copyScratch_2431(data, 0x0080, romNum, 1); 
}

