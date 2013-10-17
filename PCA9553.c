#include "PCA9553.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "i2c.h"

extern i2cStruct LED_Driver;
extern i2cStruct *previous;
uint8_t LED_LSO; //current state of the LED selector

//use this to write a value to a single register of the LED DRIVER
//as defined in the header file
void LEDDriverWriteByte(uint8_t reg, uint8_t value){
	//while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
	previous = &LED_Driver;
	LED_Driver.buffer[0] = LED_DRIVER_ADDRESS;
	LED_Driver.buffer[1] = reg;
	LED_Driver.buffer[2] = value; //lsB of address
	LED_Driver.instructionCount = 3;
	LED_Driver.currentInstruction = 0;
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm
}

//use this function to completely write the LED Driver
void LEDDriverWriteFull(uint8_t value1,uint8_t value2,uint8_t value3,uint8_t value4,uint8_t value5){
  //while (i2c_fsm(LED_Driver,0)!=I2C_IDLE);   //wait for fsm to go idle
  previous = &LED_Driver;
  LED_Driver.buffer[0] = AUTO_INCREMENT | PSC0;  //this should be the same as 11h
  LED_Driver.buffer[1] = value1;
  LED_Driver.buffer[2] = value2;
  LED_Driver.buffer[3] = value3;
  LED_Driver.buffer[4] = value4;
  LED_Driver.buffer[5] = value5;
  LED_Driver.instructionCount = 6;
  LED_Driver.currentInstruction = 0;
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm
}

/*select led 1-4 (power-internet) and 0-3 for setting
setting 0- off
        1- on
        2- blink rate 1
        3- blink rate 2
*/ 

//channel can be either 1 or 2
//rate can be between .023 seconds to 5.82 seconds
void LEDSetBlink(uint8_t channel, double rate){
  uint8_t newValue;
  uint8_t reg;
  I2CWait(); 
  previous = &LED_Driver;
  newValue = (uint8_t)(rate*44 - 1);
  if (channel == 1){
    reg = 0x01;
  } 
  else {
    reg = 0x03;
  }
  LED_Driver.buffer[0] = 0xC4;
  LED_Driver.buffer[1] = reg;
  LED_Driver.buffer[2] = newValue;  
  LED_Driver.instructionCount = 3;
  LED_Driver.currentInstruction = 0;
  LED_Driver.ready = 0; 
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);        
}

//sets brightness of led channel
//select channel 0 or 1
//choose pwm between 0-256 (0 is 100%, 256 is 0%)
void LEDSetPWM(uint8_t channel, uint8_t pwm){
  uint8_t reg;
  I2CWait(); 
  previous = &LED_Driver;
  if (channel == 1){
    reg = 0x02;
  } 
  else {
    reg = 0x04;
  }
  LED_Driver.buffer[0] = 0xC4;
  LED_Driver.buffer[1] = reg;
  LED_Driver.buffer[2] = pwm;  
  LED_Driver.instructionCount = 3;
  LED_Driver.currentInstruction = 0;
  LED_Driver.ready = 0; 
  (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR); 
}
 
//chose led 1(power)-4(internet) , setting 0-3
/* setting 0 off
           1 on
           2 channel 1
           3 channel 2
*/      
void LEDSet(uint8_t lednum, uint8_t setting){
   uint8_t value=5;
   uint8_t shift=5;
   I2CWait(); 
   previous = &LED_Driver;
   switch (setting){
    case 0:
       value = 1;
       break;
    case 1:
       value = 0;
       break;
    case 2:
       value = 2;
       break;
    case 3:
       value = 3;
       break;
   }
   switch (lednum){
    case 1:
       shift = 0;
       break;
    case 2:
       shift = 2;
       break;
    case 3:
       shift = 4;
       break;
    case 4:
       shift = 6;
       break;
   }
   /*
   if ((value == 5)||(shift ==5)){
      break;
   }
   */
   switch (lednum){
    case 1:
       LED_LSO = (LED_LSO & 0xFC) | value;
       break;
    case 2:
       LED_LSO = (LED_LSO & 0xF3) | (value<<shift);
       break;
    case 3:
       LED_LSO = (LED_LSO & 0xCF) | (value<<shift);
       break;
    case 4:
       LED_LSO = (LED_LSO & 0x3F) | (value<<shift);
       break;
   }  
   LED_Driver.buffer[0] = 0xC4;
   LED_Driver.buffer[1] = 0x05;
   LED_Driver.buffer[2] = LED_LSO;
   LED_Driver.instructionCount = 3;
   LED_Driver.currentInstruction = 0;
   LED_Driver.ready = 0; 
   (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR); 
}
  

//checks if the light is on or off. Returns 1 if light is on or in either
//of the pwm states
BOOL checkON(uint8_t light){
   BOOL output = 0;
   switch(light){
    case 1:
        if ((LED_LSO & 0x03)!=0){
           output = 1;
        } 
        else{
           output = 0;
        }
        break;
    case 2:
        if ((LED_LSO & 0x0C)!=0){
           output = 1;
        }
        else{
           output = 0;
        }     
        break;
    case 3:
        if ((LED_LSO & 0x30)!=0){
           output = 1;
        }
        else{
           output = 0;
        }        
        break;  
    case 4:
        if ((LED_LSO & 0xC0)!=0){
           output = 1;
        }
        else{
           output = 0;
        }
        break;
   }
   return output;                
}

//currently just swaps the state of LED to on/off. Need to fix this to 
//be able to save state of LED if in one of the PWM states
void toggleLED(uint8_t light){

   LEDSet(light, !checkON(light) );

}


void testPCA(){
   I2CWait(); 
   previous = &LED_Driver;
   LED_Driver.buffer[0] = 0xC4;
   LED_Driver.buffer[1] = 0x11;
   /*
   LED_Driver.buffer[2] = 0x2b; //test as in programming example
   LED_Driver.buffer[3] = 0x80;
   LED_Driver.buffer[4] = 0x0A;
   LED_Driver.buffer[5] = 0xC0;
   LED_Driver.buffer[6] = 0xE4;
   */
   LED_Driver.buffer[2] = 0x2b; // PWM0 blink period 1 sec
   LED_Driver.buffer[3] = 0x80; // PWM0 50% duty cycles
   LED_Driver.buffer[4] = 0x0A; // blink period 11/44s
   LED_Driver.buffer[5] = 0x80; // PWM1 50% duty
   LED_Driver.buffer[6] = 0b11011000; //internet at PWM1, error off, charging at pwm0, power on
   LED_LSO = 0b11011000;
   //LED_Driver.buffer[6] = 0b10101010;
   LED_Driver.instructionCount = 7;
   LED_Driver.currentInstruction = 0;
   LED_Driver.ready = 0; 
   (void)i2c_fsm(&LED_Driver,I2C_EEPROM_WR);   //set the fsm 
}