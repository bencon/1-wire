#include "main.h"
#include "oneWire.h"
#include "DS18B20.h"
#include "DS2431.h"
#include "DS28EC20.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

extern oneWire info;
extern DS2431 status_2431;
extern DS18B20 status_B20;
extern Bool T_EN;
extern Bool R_EN;
extern uint8_t temp; //dummy for clearing the SCI4 recieve full flag
extern uint8_t SCI1TOBESENT;

searchStack stack[16];
uint8_t stackPtr;

void setBaud(int value){
    uint32_t baud_value;
    uint32_t baud_rate;
    baud_rate = 230400*value;
    baud_value = (uint32_t)(BUSCLK / (16*baud_rate));
    SCI4BDH = (uint8_t)(baud_value >> 8);
    SCI4BDL = (uint8_t)baud_value;
}

void initSCI(void){
    uint32_t baud_value;
    uint32_t baud_rate;
    baud_rate = 230400; //hopefully for 9600 baud rate (reset and presense pulse rate)
    //baud_rate = 115200;  //for 4800 baud rate 
    // Calculate Baud Module Divisor value depending on bus speed
    baud_value = (uint32_t)(BUSCLK / (16*baud_rate));
    SCI4BDH = (uint8_t)(baud_value >> 8);
    SCI4BDL = (uint8_t)baud_value;
    SCI4CR2 |= 0x0C; //enables transmitter and receiver
    SCI4CR2 |= 0x80; //enables transmitter interrupt
    SCI4CR2 |= 0x20; //enables receiver full interrupt (interrupt on SCI1SR_RDRF=1)
    
    SCI1BDH = (uint8_t)(baud_value >> 8);
    SCI1BDL = (uint8_t)baud_value;
    SCI1CR2 |= 0x0C; //enables transmitter and receiver
    SCI1CR2 |= 0x80; //enables transmitter interrupt
    SCI1CR2 |= 0x20; //enables receiver full interrupt (interrupt on SCI1SR_RDRF=1)
    

}
void init1Wire(void){
    uint8_t i;
    uint8_t j;
    info.stage = 0;
    info.data = 0x00;
    info.receive = 0x00;
    info.mode = 0;
    info.cycles = 0;
    info.read = 0;
    info.deviceCount = 0;  
    for (i=0;i<16;i++){
      /*
      for (j=0;j<8;j++){
        if (i<8){
          v.romTable[j][i] = 0x00;
        }
      }
      for (j=0;j<8;j++){ 
        if (i<9){
          info.scratchpad[j][i] = 0x00;
        }
      }
      */
      info.storage[i] = 0x00;
      info.toWrite[i] = 0x00;
    }
}


void en_T(void){  //enables transmit interrupts
    T_EN = 1;
    SCI4CR2_TIE = 1;    
}

void dis_T(void){ //disables transmit interrupts
    SCI4CR2_TIE = 0;
    T_EN = 0;	
}

void en_R(void){  //enables receive interrupts
    R_EN = 1;
    SCI4CR2_RIE = 1;
    (void)SCI4SR1; 
    (void)SCI4DRL;
}

void dis_R(void){  //disables receive interrupts
    SCI4CR2_RIE = 0;
    R_EN = 0; 
}

void up(void){ //pin 29 J103
  PTJ_PTJ0 = 1;
}

void down(void){
  PTJ_PTJ0 = 0;
}

void writeData(){
    if (info.cycles < 8){   
      if (info.data & 0x01){
         SCI4DRL = 0xFF; //write a 1
      } 
    	else{
         SCI4DRL = 0x00; //write a 0 
      }
      info.data = info.data >> 1;
    }  
    info.cycles +=1;
    en_T();
    if (info.cycles == info.stop){
      dis_T();
      dis_R();
      info.mode = 0;
      info.cycles = 0;  
    }
    

}
void readOneBit(){
     dis_T();
     temp = SCI4DRL; //dummy to clear receive int flag
     en_R();
     SCI4DRL = 0xFF; //initiate a read         
}
    

void SCI4_RXRoutine(void){
     dis_T();
     dis_R();
     if (SCI4DRL == 0xFF){
         info.receive |= (1<<(info.cycles%8));
     }  
     if ((((info.cycles+1) % 8) == 0) && (info.cycles > 0)){
        info.storage[(info.cycles/8)] = info.receive;  
        info.receive = 0x00;
     }
     info.cycles = info.cycles +1;
     if (info.cycles < info.stop){
        en_T();
     }
     else{
        info.done = 1;
     }
}

void SCI4_TXRoutine(void){
     dis_T();
     dis_R();
     if(info.mode == 1){ //write
         writeData();
     } 
     else if (info.mode == 2){ //read
         readOneBit(); 
     }
}

void writeByte(uint8_t command){   //Transfers a byte over TX
     dis_R();
     info.mode = 1;
     info.stop = 8;
     info.cycles = 0;
     info.receive = 0x00;
     info.data = command;
     en_T();
}

void writeBit(BOOL value){ //to be used when an entire byte is too much
     dis_R();
     info.mode = 1;
     info.stop = 1;
     info.cycles = 0;
     info.receive = 0x00;
     info.data = value;
     en_T();
}

//Issues a read command for 'len' bytes and stores them in 'location'
void storeData(uint8_t len, uint8_t *location){
     int i;
     info.receive = 0x00;
     info.cycles = 0;
     info.mode = 2; //for a read
     info.stop = 8*len;
     info.done = 0;
     en_T();
     while (!info.done){
     }
     if (len>0){
       for (i=0;i<len;i++){
            location[i] = info.storage[i];
       }
     }
}

//puts len reads in the info.receive variable
//use this to aquire less than a byte of data from RX
//first bit goes in info.recieve[7], next is [6]
void getBits(uint8_t len){
     info.receive = 0x00;
     info.cycles = 0;
     info.mode = 2; //for a read
     info.stop = len;
     info.done = 0;
     en_T();
     while (!info.done){
     }
}

//sets the info.toWrite variable appropriately
void fillWrite(uint32_t data, uint8_t len){
    uint8_t i;
    uint8_t test;
    for (i=0; i<len; i++){
       test = ((data >> (uint8_t)(8*(len-(i+1)))) & 0xFF);
       SCI1TOBESENT = test;
       SCI1CR2_TIE = 1;	// enable transmit interrupts
       info.toWrite[i] = ((data >> (uint8_t)(8*(len-(i+1)))) & 0xFF);
    }
}


// ############# Code below may not belong here ############

void writeROM(uint8_t rom[]){
    int i;
    for (i=0;i<8;i++){
       writeByte(rom[i]);
    }
}

void reset(){ //the necessary reset sequence to address 1-wire devices
     setBaud(1);
     temp = SCI4DRL; //dummy to clear receive int flag
     temp = SCI4SR1; //clears other flags
     SCI4DRL = 0xF0; //reset pulse
     DelayUs(50);
     setBaud(12);
     DelayUs(10);
}

void readROM(uint8_t location[8], uint8_t *deviceCount){ 
    reset();
    writeByte(READROM);
    storeData(8, location);
    if (status_B20.deviceCount == 0){
        status_B20.deviceCount = 1;
    }
    *deviceCount = 1;
}

//aquires the ROM addresses of all 1-wire devices on the bus and stores them in status.romTable
//id bit num, discrpancy stack, rom count
//stack[x][0] -> bit number of given discrepency
//stack[x][1] -> rom number where discrepency was found
//stack[x][2] -> partial byte stored at discrepency fork
void searchROM(uint8_t location[16][8] , uint8_t rom_num, uint8_t discrepancy_count, uint8_t family, uint8_t * deviceCount){
#pragma MESSAGE DISABLE C12056
#pragma MESSAGE DISABLE C1855 
     uint8_t bit_num; //count from 0 to 63
     uint8_t incomplete=0;    //storage for byte before shifting it to the rom table                     
     BOOL j;
     BOOL k;
     BOOL debug=0;
     reset();
     if (rom_num == 0){
         stackPtr = 0; 
     }
     writeByte(SEARCHROM);
     for (bit_num=0;bit_num<8;bit_num++){
         getBits(2);
         writeBit((family>>bit_num)&0x01);  
     }
     location[rom_num][0] = family;
     if (rom_num > 0){
         popStack();
         for (bit_num=8;bit_num<(stack[stackPtr]).bit_num;bit_num++){
            getBits(2);
            if (((stack[stackPtr]).bit_num-bit_num)<=((stack[stackPtr]).bit_num%8)){
              debug = ((stack[stackPtr].incomplete >> (bit_num%8)) & 0x01);
              writeBit((stack[stackPtr].incomplete >> (bit_num%8)) & 0x01);
            } 
            else{
              debug = (location[rom_num][bit_num/8] >> (bit_num%8) & 0x01);  
              writeBit(location[rom_num][bit_num/8] >> (bit_num%8) & 0x01);
            }
            if (((bit_num+1)%8 == 0) && (bit_num>0)){
              location[rom_num][bit_num/8] =  location[stack[stackPtr].rom_num][bit_num/8];
              
            }
         }
         incomplete = stack[stackPtr].incomplete; 
         discrepancy_count--;
         incomplete = (incomplete |= (1 << (bit_num%8)));
         getBits(2);
         writeBit(1);
         bit_num++;            
     }
     while (bit_num < 64){
         getBits(2);
         j = (info.receive & 0x01);
         k = ((info.receive >> 1) & 0x01);
         if ((j == k) && j){ //error state, restart search
            //searchROM(0,0);
            return;
         } 
         else if (j == k){ //discrepancy-> push to stack
            pushStack(bit_num, rom_num, incomplete);
            writeBit(0);
            discrepancy_count++;
         } else if (j>k){ //only 1's
            writeBit(1);
            incomplete = (incomplete |= (1 << (bit_num%8)));
         }
         else {  //only 0's
            writeBit(0);
         }
         if ((bit_num+1)%8 ==0){
            location[rom_num][bit_num/8] = incomplete;
            //status_B20.romTable[rom_num][bit_num/8] = temp;
            incomplete = 0x00;
         }
         bit_num++;
     }
     info.deviceCount++;
     if (discrepancy_count > 0){
        searchROM(location, rom_num+1, discrepancy_count, family, deviceCount);
        
     } 
     else{
        resetStack();
		    *deviceCount = info.deviceCount;
		    info.deviceCount = 0;
     }
     
}


void pushStack(uint8_t bit_num, uint8_t rom_num, uint8_t incomplete){
     stack[stackPtr].bit_num = bit_num;
     stack[stackPtr].rom_num = rom_num;
     stack[stackPtr].incomplete = incomplete;
     stackPtr++;
}

void popStack(){
     stackPtr--; 
}

void resetStack(){
     int i;
     for (i = 0;i<16;i++){
         stack[i].bit_num = 0;
         stack[i].rom_num = 0;
         stack[i].incomplete = 0;
     }
}









