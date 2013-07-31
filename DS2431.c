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

//TODO
void copyScratch_2431(uint8_t data, uint16_t address, uint8_t romNum, uint8_t len){
    int i;
    reset();
    writeByte(MATCHROM);
    writeROM(status_2431.romTable[romNum]);
    writeByte(DS2431_WRITESCRATCH);
    for (i=0;i<2;i++){ //write least significant then most significant byte of target address
       writeByte((address>>(8*i)) & 0xF);
    }
    for (i=0;i<8;i++){//just writes the same byte 8 times, this should be fixed
       writeByte(data);
    }
    readScratch_2431(romNum);
    //reset?
    writeByte(DS2431_COPYSCRATCH);
    for (i=0;i<3;i++){
        writeByte(status_2431.scratchpad[i]); 
    }
    storeData(len, status_2431.memory[romNum]);
    
}

//reads scratchpad and puts the data in the given scratchpad memory 
void readScratch_2431(uint8_t romNum){
    //reset();//reset necessary?
    writeByte(MATCHROM);
    writeROM(status_2431.romTable[romNum]);
    writeByte(DS2431_READSCRATCH);
    storeData(20, status_2431.scratchpad);
}

void readMemory_2431(uint8_t romNum, uint16_t address, uint8_t len){
    int i;
    reset();
    writeByte(MATCHROM);
    writeROM(status_2431.romTable[romNum]);
    writeByte(DS2431_READMEM);
    for (i=0;i<2;i++){ //write least significant then most significant byte of target address
       writeByte((address>>(8*i)) & 0xF);
    }
    for (i=0;i<8;i++){//just writes the same byte 8 times, this should be fixed
       storeData(len, status_2431.memory[romNum]);
    }
}

//add init funtions