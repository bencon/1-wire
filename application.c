#include "main.h"
#include "DS18B20.h"
#include "DS2431.h"
#include "oneWire.h"
#include "application.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern DS18B20 status_B20;
extern DS2431 status_2431;
extern oneWire info; 
BOOL powerCheck;
uint8_t index;
BOOL first= 0;
double temperature;

void process(void){
  
  if (status_2431.stage == 0){
     searchROM(status_B20.romTable,0,0,0x28, &status_B20.deviceCount);
     DelayUs(100);
     searchROM(status_2431.romTable,0,0,0x2D, &status_2431.deviceCount);
     DelayUs(100);
     status_2431.stage = 1;
  } 
  else if (status_2431.stage ==1){  
     writeScratch_B20(0xAABBFF,3, status_B20.romTable[0]);
     writeScratch_B20(0xCCDDFF,3, status_B20.romTable[1]);
     writeScratch_B20(0x1122FF,3, status_B20.romTable[2]);
     copyScratch_2431(0xAB,0x0000, 0, 60);
     convertTemp(NULL,1);
     DelayMs(100);
     status_2431.stage = 2;
  } 
  else if (status_2431.stage ==2){ 
      for (index=0;index<3;index++){
          aquireScratch_B20(status_B20.romTable[index], index);
      }
      readMemory_2431(0, 0x0000, 100);
      stop();
  }
  
  else if (status_2431.stage == 10){  //do nothing, wait for a continue    
  }
  
  /*
  if (status.stage == 0){
      
      //aquireROM();  //only for use with single one-wire device
      
      searchROM(0,0);
      writeScratch(0xAABBFF,3, status.romTable[0]);
      writeScratch(0xCCDDFF,3, status.romTable[1]);
      writeScratch(0x1122FF,3, status.romTable[2]);
      if (!first){  
        convertTemp(NULL,1);
        first = 1;
      }
      up();
      DelayMs(100);
      down();
      DelayMs(1);
      up();
      status.stage = 2;
    
  }
  else if (status.stage == 1){
      for (index=0;index<status.deviceCount;index++){
          aquireScratch(status.romTable[index], index);
      }
      //if (checkScratch(0)){
          status.stage =3;
      //}
  } 
  else if (status.stage == 2){
       //writeScratch(0xAABBFF,3, status.romTable[0]);
      // writeScratch(0xCCDDFF,3, status.romTable[1]);
       //writeScratch(0x1122FF,3,status.romTable[2]);
       status.stage = 1;
  } 
  else if (status.stage == 3){ ///check power stage
        powerCheck= readPower(status.romTable[0]);
        temperature = farenheitConversion(0);
        stop();
  }
  else if (status.stage == 10){  //do nothing, wait for a continue    
  }
  */
}

void stop(void){
  status_B20.stage = 10;
  status_2431.stage = 10;
}

void init(void){
  init1Wire(); //sets stage to 0 and clears status variables
}


  
