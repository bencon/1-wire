#include "i2c.h" 
#include "PCA9553.h"
#include "mcp4728.h"
#include "main.h"
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

                         
//extern i2cStruct *LED_Driver;
//extern i2cStruct *DAC;
extern i2cStruct *previous;
 

void i2cInit(void){
   //*** busclock defined as BUSCLK 48000000UL
   //IBAD = Only to be used if the IIC bus is addressed as a slave
   IIC0_IBCR = 0xF0; //bit 7-IIC bus enabled,bit 6- Interrupts enabled, bit 5- configured for master mode, bit 4- Transmit mode
   IIC0_IBFD = 0x08; //random test byte 
}

//I2C finite State Machine

enum i2c_states i2c_fsm(i2cStruct *device, char new_state){
  //previous = &device;
	if (new_state == I2C_EEPROM_RD){
		device->current_command = RD; //probably need to do a #define
		device->state = I2C_START;
	}	
	if (new_state == I2C_EEPROM_WR){
		device->current_command = WR;
		device->state = I2C_START; //go to I2C_START state
	}
	if (!IIC0_IBSR_TCF) return (new_state); //If transfer not complete
	IIC0_IBSR_TCF = 1;  //clear the transfer complete flag
	
	switch (device->state){
		case I2C_IDLE:
			break;
		case I2C_START:
			IIC0_IBCR = 0xF0;
			IIC0_IBDR = device->buffer[0];
			if(device->instructionCount > 0){
			    device->state = I2C_WRITE_DATA;
			    device->currentInstruction = 1;
			} 
			else {
			    device->state = I2C_STOP;
			}
			break;
		case I2C_WRITE_DATA:
		  
			IIC0_IBDR = device->buffer[device->currentInstruction];
			device->currentInstruction++;
			if (device->currentInstruction == device->instructionCount){
			    device->state = I2C_STOP;
			}
			//otherwise stay in the same state
			break;
			
		/*	  //need something like this for repeated start
		case I2C_WRITE_LOWER_ADDRESS:
			IBDR = device->buffer[2];
			if (current_command == WR){
				i2c_state = I2C_WRITE_BYTE;
			}
			else{
				i2c_state = I2C_REPEATED_START; //in the case for a read command
			}
			break;
		*/
		case I2C_WRITE_BYTE:
			IIC0_IBDR = device->buffer[3];
			device->state = I2C_STOP;
			break;
		case I2C_REPEATED_START:
			IIC0_IBCR_RSTA = 1; //repeated start
			IIC0_IBDR = device->buffer[0] | RD; //send the address field (R/W = RD)
			device->state = I2C_DUMMY_READ;
			break;
		case I2C_DUMMY_READ:
			IIC0_IBCR_TX_RX = 0;
			device->buffer[3] = IIC0_IBDR; // dummy read
			device->state = I2C_STOP;
			break;
		case I2C_READ_BYTE:
			IIC0_IBCR_TXAK = 1; //send NACK on next read
			device->buffer[3] = IIC0_IBDR;
			device->state = I2C_STOP;
			break;
		case I2C_STOP:
			IIC0_IBCR_MS_SL = 0;  //send a stop(go to slave mode)
			device->state = I2C_IDLE;
			device->ready = 1;
			break;
	}
}

//buffer of fixed size with slave address and any other necessary data exchange, buffer len lets the state machine know
//when to stop transfering data. deviceState should unique for each device (led driver, DAC, etc.. ) to allow multiple 
//fsms at once
//char I2C_byteRead(uint8_t buffer[], uint8_t buffer_len, char deviceState){

/*
char eeprom_byteread(uint8_t address){
	while (i2c_fsm(0)!=I2C_IDLE);  // wait for fsm to go idle
	device->buffer[0] = EEPROM_ID | EEPROM_ADR;
	device->buffer[1] = address >> 8; //msB of address
	device->buffer[2] = address; //lsB of address
	device->ready = 0;
	i2c_fsm(I2C_EEPROM_RD);  //set the state machine
	while (!device->ready); // wait until command is complete
	return (device->buffer[3]);  //return the data read
}

void eeprom_byte_write(unsigned int address, char data ){
	while (i2c_fsm(0)!=I2C_IDLE);   //wait for fsm to go idle
	device->buffer[0] = EEPROM_ID | EEPROM_ADR;
	device->buffer[1] = address >> 8; //msB of address
	device->buffer[2] = address; //lsB of address
	device->buffer[3] = data;
	i2c_fsm(I2C_EEPROM_WR);   //set the fsm
}
*/


void i2cSpeedTest(i2cStruct *device){
  previous = device;
	//while (i2c_fsm(device,0)!=I2C_IDLE);  // wait for fsm to go idle
	device->buffer[0] = 0x55;
	device->buffer[1] = 0x55; //msB of address
	device->buffer[2] = 0x55; //lsB of address
	device->buffer[3] = 0x55; //lsB of address
	device->buffer[4] = 0x55; //lsB of address
	device->buffer[5] = 0x55; //lsB of address
	device->buffer[6] = 0x00; //lsB of address
	device->buffer[7] = 0x55; //lsB of address
	device->buffer[8] = 0x55; //lsB of address
	device->ready = 0;
	device->instructionCount = 8;
	i2c_fsm(device,I2C_EEPROM_WR);  //set the state machine
	while (!device->ready); // wait until command is complete
}

void i2cNoInterruptTest(i2cStruct *device){
  previous = device;
	(device->buffer)[0] = 0x55;
	(device->buffer)[1] = 0x56; 
	(device->buffer)[2] = 0x57;
	(device->buffer)[3] = 0x00;
	(device->buffer)[4] = 0x58;
	(device->buffer)[5] = 0x59;
	(device->buffer)[6] = 0x60;
	(device->buffer)[7] = 0x61;
	(device->buffer)[8] = 0x62;
	device->currentInstruction = 0;
	device->instructionCount = 8; 
	device->ready = 0;
	i2c_fsm(device,I2C_EEPROM_WR);  //set the state machine
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);	
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
	(void)i2c_fsm(device,0);
	while(!IIC0_IBSR_TCF);
}


void initI2cStruct(i2cStruct *device){
  int i;
  for (i=0;i<15;i++){
    device->buffer[i] = 0;
  }
  device->instructionCount = 0;
  device->currentInstruction = 0;
  device->state = I2C_IDLE;
  device->current_command = 0;  
}