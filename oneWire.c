#include "main.h"
#include "oneWire.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

//#include <math.h>


extern oneWire status;
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
    status.stage = 0;
    status.data = 0x00;
    status.receive = 0x00;
    status.mode = 0;
    status.cycles = 0;
    status.read = 0;
    status.deviceCount = 0;  
    for (i=0;i<16;i++){
      for (j=0;j<8;j++){
        if (i<8){
          status.romTable[j][i] = 0x00;
        }
      }
      for (j=0;j<8;j++){ 
        if (i<9){
          status.scratchpad[j][i] = 0x00;
        }
      }
      status.storage[i] = 0x00;
      status.toWrite[i] = 0x00;
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

void up(void){
  PTJ_PTJ0 = 1;
}

void down(void){
  PTJ_PTJ0 = 0;
}

void writeData(){
    if (status.cycles < 8){   
      if (status.data & 0x01){
         SCI4DRL = 0xFF; //write a 1
      } 
    	else{
         SCI4DRL = 0x00; //write a 0 
      }
      status.data = status.data >> 1;
    }  
    status.cycles +=1;
    en_T();
    if (status.cycles == status.stop){
      dis_T();
      dis_R();
      status.mode = 0;
      status.cycles = 0;  
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
         status.receive |= (1<<(status.cycles%8));
     }  
     if ((((status.cycles+1) % 8) == 0) && (status.cycles > 0)){
        status.storage[(status.cycles/8)] = status.receive;  
        status.receive = 0x00;
     }
     status.cycles = status.cycles +1;
     if (status.cycles < status.stop){
        en_T();
     }
     else{
        status.done = 1;
     }
}

void SCI4_TXRoutine(void){
     dis_T();
     dis_R();
     if(status.mode == 1){ //write
         writeData();
     } 
     else if (status.mode == 2){ //read
         readOneBit(); 
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

void writeByte(uint8_t command){   //Transfers a byte over TX
     dis_R();
     status.mode = 1;
     status.stop = 8;
     status.cycles = 0;
     status.receive = 0x00;
     status.data = command;
     en_T();
}

void writeBit(BOOL value){ //to be used when an entire byte is too much
     dis_R();
     status.mode = 1;
     status.stop = 1;
     status.cycles = 0;
     status.receive = 0x00;
     status.data = value;
     en_T();
}

//Issues a read command for 'len' bytes and stores them in 'location'
void storeData(uint8_t len, uint8_t *location){
     int i;
     status.receive = 0x00;
     status.cycles = 0;
     status.mode = 2; //for a read
     status.stop = 8*len;
     status.done = 0;
     en_T();
     while (!status.done){
     }
     if (len>0){
       for (i=0;i<len;i++){
            location[i] = status.storage[i];
       }
     }
}

//puts len reads in the status.receive variable
//use this to aquire less than a byte of data from RX
//first bit goes in status.recieve[7], next is [6]
void getBits(uint8_t len){
     status.receive = 0x00;
     status.cycles = 0;
     status.mode = 2; //for a read
     status.stop = len;
     status.done = 0;
     en_T();
     while (!status.done){
     }
}

//use this only when you have a single 1-wire chip on the bus
//stores the ROM data of the chip in status.romTable[0]
void aquireROM(void){ 
    reset();
    writeByte(READROM);
    storeData(8, status.romTable[0]);
    if (status.deviceCount == 0){
        status.deviceCount = 1;
    }
}

//stores the reading of the given rom address in the status struct
void aquireScratch(uint8_t rom[], uint8_t deviceNum){
    reset();
    writeByte(MATCHROM);
    writeROM(rom);
    writeByte(READSCRATCH);
    storeData(9, status.scratchpad[deviceNum]);
}

//data is what is to be written, len is how many bytes
//writeScratch(0xAB, 2) will put A in Th register and B into Tl register
//writeScratch(0xABC,3) puts C into config register
//Th, Tl, Config --> bytes 2, 3, 4 of ds18b20
void writeScratch(uint32_t data, uint8_t len, uint8_t rom[]){
    int i;
    reset();
    writeByte(MATCHROM);
    writeROM(rom);
    writeByte(WRITESCRATCH);
    fillWrite(data, len);
    for (i=0;i<len;i++){
       writeByte(status.toWrite[i]);
    }
}

BOOL checkScratch(uint8_t deviceIndex){  //checks to see if the scratchpad has valid data
   if ((status.scratchpad[deviceIndex][0] != 0xFF) && (status.scratchpad[deviceIndex][0] != 0x00)){
      return 1;
   } 
   else{
      return 0;
   }
}

//sends the given rom address over the TX line in order to address the given device
void writeROM(uint8_t rom[]){
    int i;
    for (i=0;i<8;i++){
       writeByte(rom[i]);
    }
}

//sets the status.toWrite variable appropriately
void fillWrite(uint32_t data, uint8_t len){
    uint8_t i;
    uint8_t test;
    for (i=0; i<len; i++){
       test = ((data >> (uint8_t)(8*(len-(i+1)))) & 0xFF);
       SCI1TOBESENT = test;
       SCI1CR2_TIE = 1;	// enable transmit interrupts
       status.toWrite[i] = ((data >> (uint8_t)(8*(len-(i+1)))) & 0xFF);
    }
}

BOOL readPower(uint8_t rom[]){ //returns 1 if device is using parasite power
    reset();
    writeByte(MATCHROM);
    writeROM(rom);
    writeByte(READPOWER);
    storeData(0,NULL);
    if (status.storage[1] == 0x00){
        return 1;
    } 
    else{
        return 0;
    }
}

//aquires the ROM addresses of all 1-wire devices on the bus and stores them in status.romTable
//id bit num, discrpancy stack, rom count
//stack[x][0] -> bit number of given discrepency
//stack[x][1] -> rom number where discrepency was found
//stack[x][2] -> partial byte stored at discrepency fork
void searchROM(uint8_t rom_num, uint8_t discrepancy_count){
#pragma MESSAGE DISABLE C12056
#pragma MESSAGE DISABLE C1855 
     uint8_t bit_num; //count from 0 to 63
     uint8_t temp;    //storage for byte before shifting it to the rom table                     
     BOOL j;
     BOOL k;
     BOOL debug=0;
     reset();
     if (rom_num == 0){
         stackPtr = 0; 
     }
     writeByte(SEARCHROM);

     bit_num =0;
     if (rom_num > 0){
         popStack();
         for (bit_num=0;bit_num<(stack[stackPtr]).bit_num;bit_num++){
            getBits(2);
            if (((stack[stackPtr]).bit_num-bit_num)<=((stack[stackPtr]).bit_num%8)){
              debug = ((stack[stackPtr].temp >> (bit_num%8)) & 0x01);
              writeBit((stack[stackPtr].temp >> (bit_num%8)) & 0x01);
            } 
            else{
              debug = ((status.romTable[stack[stackPtr].rom_num][bit_num/8]) >> (bit_num%8)) & 0x01;  
              writeBit(((status.romTable[stack[stackPtr].rom_num][bit_num/8]) >> (bit_num%8)) & 0x01);
            }
            if (((bit_num+1)%8 == 0) && (bit_num>0)){
              status.romTable[rom_num][bit_num/8] =  status.romTable[stack[stackPtr].rom_num][bit_num/8];
            }
         }
         temp = stack[stackPtr].temp; 
         discrepancy_count--;
         temp = (temp |= (1 << (bit_num%8)));
         getBits(2);
         writeBit(1);
         bit_num++;            
     }
     while (bit_num < 64){
         getBits(2);
         j = (status.receive & 0x01);
         k = ((status.receive >> 1) & 0x01);
         if ((j == k) && j){ //error state, restart search
            //searchROM(0,0);
            return;
         } 
         else if (j == k){ //discrepancy-> push to stack
            pushStack(bit_num, rom_num, temp);
            writeBit(0);
            discrepancy_count++;
         } else if (j>k){ //only 1's
            writeBit(1);
            temp = (temp |= (1 << (bit_num%8)));
         }
         else {  //only 0's
            writeBit(0);
         }
         if ((bit_num+1)%8 ==0){
            status.romTable[rom_num][bit_num/8] = temp;
            temp = 0x00;
         }
         bit_num++;
     }
     if (discrepancy_count > 0){
        searchROM(rom_num+1, discrepancy_count);
     }
     status.deviceCount++;
}

void pushStack(uint8_t bit_num, uint8_t rom_num, uint8_t temp){
     stack[stackPtr].bit_num = bit_num;
     stack[stackPtr].rom_num = rom_num;
     stack[stackPtr].temp = temp;
     stackPtr++;
}

void popStack(){
     stackPtr--; 
}

void convertTemp(uint8_t rom[], BOOL skip){//if skip ==1, issue a convert to all devices
     reset();
     if (!skip){
       writeByte(MATCHROM);
       writeROM(rom);
     }
     else{
       writeByte(SKIPROM);
     }
     writeByte(CONVERTT);
     status.mode = 1; 
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
     uint16_t raw = (status.scratchpad[device][1]<<8) | status.scratchpad[device][0];
     uint8_t config;
     double baseFrac;
     uint8_t whole;
     uint8_t frac;
     double celcius;
     config = (status.scratchpad[device][4] >> 5) & 0x03; //grabs the important configuration register bits
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
