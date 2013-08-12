#include "main.h"
#include "DS18B20.h"
#include "DS2431.h"
#include "DS28EC20.h"
#include "oneWire.h"
#include "application.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern DS18B20 status_B20;
extern DS2431 status_2431;
extern DS28EC20 status_28EC20;
extern oneWire info; 
BOOL powerCheck;
uint8_t index;
BOOL first= 0;
uint8_t ad;
double temperature;
uint8_t sample[8] = {{0x01},{0x02},{0x03},{0x04},{0x05},{0x06},{0x07},{0x08}};
uint8_t sample2[8] = {{0x01},{0x01},{0x01},{0x01},{0x01},{0x01},{0x01},{0x01}};
//uint8_t sample3[8] = {{0x01},{0x02},{0x03},{0x04},{0x05},{0x06},{0x07},{0x08}};

void process(void){
  
  if (status_2431.stage == 0){
     searchROM(status_B20.romTable,0,0,0x28, &status_B20.deviceCount);
     DelayUs(100);
     searchROM(status_2431.romTable,0,0,0x2D, &status_2431.deviceCount);
     DelayUs(100);
     searchROM(status_28EC20.romTable,0,0,0x43, &status_28EC20.deviceCount);
     DelayUs(100);
     status_2431.stage = 1;
     //readROM(status_28EC20.romTable[0], &status_28EC20.deviceCount);
  } 
  else if (status_2431.stage ==1){ 
     writeScratch_B20(0xAABBFF,3, status_B20.romTable[0]);
     writeScratch_B20(0xCCDDFF,3, status_B20.romTable[1]);
     //writeScratch_B20(0x1122FF,3, status_B20.romTable[2]);
     setProtection_2431(1,0x00,0x00,0x00,0x00);
     //setOnes_2431(0,0);
     DelayUs(100);
     convertTemp(NULL,1);
     copyScratch_2431(sample, 0x0000, 0, 16);
     copyScratch_2431(sample, 0x0000, 1, 16);
     copyScratch_28EC20(sample, 0x000,0,80); 
     DelayUs(100);
     for (ad = 0;ad<144;ad+=8){
        up();
        readMemory_2431(0,ad, 8);
        readMemory_2431(1,ad, 8);
        readMemory_28EC20(0,ad,8);
        down();
        DelayUs(100);
     }
     status_2431.stage = 2;
  } 
  else if (status_2431.stage ==2){ 
      for (index=0;index<3;index++){
          aquireScratch_B20(status_B20.romTable[index], index);
      }
      //readMemory_2431(1, 0x0000, 20);
      stop();
  }
  
  else if (status_2431.stage == 10){  //do nothing, wait for a continue    
  }
  
}

void stop(void){
  status_B20.stage = 10;
  status_2431.stage = 10;
}

void init(void){
  init1Wire(); //sets stage to 0 and clears status variables
}


  
