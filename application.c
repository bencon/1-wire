#include "main.h"
#include "DS18B20.h"
#include "DS2431.h"
#include "DS28EC20.h"
#include "oneWire.h"
#include "application.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"
#include "mcp4728.h"
#include "PCA9553.h"

extern DS18B20 status_B20;
extern DS2431 status_2431;
extern DS28EC20 status_28EC20;
extern oneWire info;
extern i2cStruct DAC;
extern i2cStruct LED_Driver; 
BOOL powerCheck;
uint8_t index;
BOOL first= 0;
uint8_t ad;
double temperature;
uint8_t sample[8] = {{0x01},{0x02},{0x03},{0x04},{0x05},{0x06},{0x07},{0x08}};
uint8_t sample2[8] = {{0x01},{0x01},{0x01},{0x01},{0x01},{0x01},{0x01},{0x01}};
//uint8_t sample3[8] = {{0x01},{0x02},{0x03},{0x04},{0x05},{0x06},{0x07},{0x08}};

//////////////I2c Variables

uint8_t currentState;
void i2cProcess(void){

   if (first == 0){
      index = 1;
      first = 1;
   }
   if (index==1){
      DACSequentialWrite(.5,.5,.5,.5);
      DelayMs(1);
      testPCA();
      //DelayMs(1);
      DACSingleChannelWrite(2,2);
      DelayMs(1);
      DACSingleChannelWrite(1,2);
      index = 2;
      //DelayMs(1);
      LEDSetBlink(0,.01);
      //DelayMs(1);
      LEDSetBlink(1,.01);
      //DelayMs(1);
      LEDSetPWM(0,1);
      //DelayMs(1);
      LEDSetPWM(1,200);
      //DelayUs(25);
      LEDSet(LED_POWER,LED_PWM2);
      //DelayUs(12);
      LEDSet(LED_CHARGING,LED_ON);
      //DelayUs(5);
      LEDSet(LED_ERROR,LED_ON);
      //DelayUs(3);
      LEDSet(LED_INTERNET,LED_ON);

   }
   if (index ==2){
      //testPCA();   
      index =3;
   }
   
   /*
   switch (index){
    case 3:
      index = 4;
      LEDSet(1,1);
      DelayMs(2);
      break;
    case 4:
      index = 5;
      LEDSet(2,1);
      DelayMs(2);
      break;   
    case 5:
      index = 6;
      LEDSet(3,1);
      DelayMs(2);
      break;
    case 6:
      index = 7;
      LEDSet(4,1);
      DelayMs(10);
      break;
    case 7:
      index = 8;
      LEDSet(1,0);
      DelayMs(2);
      break;
    case 8:
      index = 9;
      LEDSet(2,0);
      DelayMs(2);
      break;
    case 9:
      index = 10;
      LEDSet(3,0);
      DelayMs(2);
      break;
    case 10:
      index = 3;
      LEDSet(4,0);
      DelayMs(2);
      break;
   }
   */         
   
}
                 
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


  
