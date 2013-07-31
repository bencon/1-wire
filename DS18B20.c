#include "main.h"
#include "oneWire.h"
#include "DS18B20.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern DS18B20 status_B20;
extern oneWire info;
extern Bool T_EN;
extern Bool R_EN;
extern uint8_t temp; //dummy for clearing the SCI4 recieve full flag
extern uint8_t SCI1TOBESENT;//use this only when you have a single 1-wire chip on the bus


//stores the ROM data of the chip in status.romTable[0]
void aquireROM_B20(void){ 
    reset();
    writeByte(DS18B20_READROM);
    storeData(8, status_B20.romTable[0]);
    if (status_B20.deviceCount == 0){
        status_B20.deviceCount = 1;
    }
}

//stores the reading of the given rom address in the status struct
void aquireScratch_B20(uint8_t rom[], uint8_t deviceNum){
    reset();
    writeByte(DS18B20_MATCHROM);
    writeROM(rom);
    writeByte(DS18B20_READSCRATCH);
    storeData(9, status_B20.scratchpad[deviceNum]);
}

//data is what is to be written, len is how many bytes
//writeScratch(0xAB, 2) will put A in Th register and B into Tl register
//writeScratch(0xABC,3) puts C into config register
//Th, Tl, Config --> bytes 2, 3, 4 of ds18b20
void writeScratch_B20(uint32_t data, uint8_t len, uint8_t rom[]){
    int i;
    reset();
    writeByte(DS18B20_MATCHROM);
    writeROM(rom);
    writeByte(DS18B20_WRITESCRATCH);
    fillWrite(data, len);
    for (i=0;i<len;i++){
       writeByte(info.toWrite[i]);
    }
}

BOOL checkScratch_B20(uint8_t deviceIndex){  //checks to see if the scratchpad has valid data
   if ((status_B20.scratchpad[deviceIndex][0] != 0xFF) && (status_B20.scratchpad[deviceIndex][0] != 0x00)){
      return 1;
   } 
   else{
      return 0;
   }
}

/*
void writeFamily(uint8_t familyCode){
    reset();
    writeByte(MATCHROM);
    writeByte(familyCode);
}
*/

BOOL readPower(uint8_t rom[]){ //returns 1 if device is using parasite power
    reset();
    writeByte(DS18B20_MATCHROM);
    writeROM(rom);
    writeByte(DS18B20_READPOWER);
    storeData(0,NULL);
    if (info.storage[1] == 0x00){
        return 1;
    } 
    else{
        return 0;
    }
}




void convertTemp(uint8_t rom[], BOOL skip){//if skip ==1, issue a convert to all devices
     reset();
     if (!skip){
       writeByte(DS18B20_MATCHROM);
       writeROM(rom);
     }
     else{
       writeByte(DS18B20_SKIPROM);
     }
     writeByte(DS18B20_CONVERTT);
     info.mode = 1; 
}

double farenheitConversion(uint8_t device){
    /*
     uint16_t raw = (status.scratchpad[device][1]<<8) | status.scratchpad[device][0];
     uint8_t config;
     double baseFrac;
     config = (status.scratchpad[device][4] >> 5) & 0x03; //grabs the important configuration register bits
     if (config == 0x00){ //9 bit resolution
        raw = raw <<3;
     } 
     else if (config == 0x01){ //10 bit resolution
        raw = raw <<2;
     }
     else if (config == 0x02){ //11 bit resolution   
        raw = raw <<1;    
     }
     //otherwise default 12 bit resolution
     //celcius = (double)raw / 16.0
     return (double)(((double)raw /16.0) * 1.8) +32.0;  //for degrees farenheit
     */
     uint16_t raw = (status_B20.scratchpad[device][1]<<8) | status_B20.scratchpad[device][0];
     uint8_t config;
     double baseFrac;
     uint8_t whole;
     uint8_t frac;
     double celcius;
     config = (status_B20.scratchpad[device][4] >> 5) & 0x03; //grabs the important configuration register bits
     if (config == 0x00){ //9 bit resolution
        baseFrac = 0.5;
        whole =  raw >> 1;
        frac = raw & 0x01;
     } 
     else if (config == 0x01){ //10 bit resolution
        baseFrac = 0.25;
        whole =  raw >> 2;
        frac = raw & 0x03;
     }
     else if (config == 0x02){ //11 bit resolution   
        baseFrac = 0.125;
        whole =  raw >> 3;
        frac = raw & 0x07;
     } 
     else{  //otherwise default 12 bit resolution
        baseFrac = 0.0625;
        whole =  raw >> 4;
        frac = raw & 0x0E;
     }
     celcius = whole; //+ (frac * baseFrac);
     return (celcius*1.8)+32.0;  //for degrees farenheit
}


