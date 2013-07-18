#include "main.h"
#include "oneWire.h"
#include "application.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern oneWire status;
BOOL powerCheck;
uint8_t index;

void process(void){
  if (status.stage == 0){ 
      //aquireROM();  //only for use with single one-wire device
      searchROM(0,0);
      status.stage = 2;
  }
  else if (status.stage == 1){
      for (index=0;index<status.deviceCount;index++){
          aquireScratch(status.romTable[index], index);
      }
      if (checkScratch(0)){
          status.stage =3;
      }
  } 
  else if (status.stage == 2){
       writeScratch(0xAABB,2, status.romTable[0]);
       writeScratch(0xCCDDEE,3, status.romTable[1]);
       writeScratch(0x112233,3,status.romTable[2]);
       status.stage = 1;
  } 
  else if (status.stage == 3){ ///check power stage
        powerCheck= readPower(status.romTable[0]);
        stop();
  }
  else if (status.stage == 10){  //do nothing, wait for a continue    
  }
}

void stop(void){
  status.stage = 10;
}

void init(void){
  init1Wire(); //sets stage to 0 and clears status variables
}


  
