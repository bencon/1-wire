#include <hidef.h>           /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "oneWire.h"
#include "DS18B20.h"
#include "DS2431.h"
#include "DS28EC20.h"
#include "application.h"
#include "i2c.h"
#include "PCA9553.h"
#include "mcp4728.h"


#pragma CODE_SEG __NEAR_SEG NON_BANKED

void interrupt _Startup(void);
void interrupt Unimplemented_ISR(void);

#pragma CODE_SEG DEFAULT

uint8_t SCI1TOBESENT;
uint8_t SCICOUNT;
uint8_t temp; //dummy for clearing the SCI4 recieve full flag
Bool T_EN;
Bool R_EN;                      
DS18B20 status_B20;
DS2431 status_2431;
DS28EC20 status_28EC20;
oneWire info;

//i2c variables
i2cStruct **previous;
i2cStruct *DAC;
i2cStruct *LED_Driver;

/*     ///main for 1-wire
void main(void) {
  DDRJ_DDRJ7 = OUTPUT;
  DDRJ_DDRJ6 = OUTPUT;
  DDRJ_DDRJ0 = OUTPUT;
  PTJ_PTJ6 = 0;
  PTJ_PTJ0 = 0;
  initSCI();
  init1Wire();
  SCI1CR2_TIE = 0;
  dis_T();
  dis_R();
	EnableInterrupts;
  for(;;) {
    _FEED_COP(); // feeds the dog 
    process();  
    DelayMs(1);
  }
 }
 */

//main for i2c  
void main(void){
   i2cInit();
   initI2cStruct(DAC);
   initI2cStruct(LED_Driver);
   DelayMs(1);
   //EnableInterrupts;
   for(;;) {
     _FEED_COP(); // feeds the dog 
     i2cProcess();  
     DelayMs(1);
   } 
   
}
  
  


void DelayMs(uint16_t ms){
	volatile uint16_t i;  // Needs volatile so compiler does not optimize
	while (ms > 0){
		i = DELAY_1MS;
		_FEED_COP();  //important
		while (i > 0) {
			i = i - 1;
		}
		ms = ms - 1;
	}
}

void DelayUs(uint16_t us){
	volatile uint16_t i;  // Needs volatile so compiler does not optimize
	while (us > 0){
		i = DELAY_1US;
    _FEED_COP();
		while (i > 0) {
			i = i - 1;
		}
		us = us - 1;
	}
}



#pragma CODE_SEG __NEAR_SEG NON_BANKED


//INTERRUPTS
    
void interrupt Unimplemented_ISR(void)
{
  asm BGND; //software breakpoint
}

void interrupt SCI4_ISR(void){  
  if(SCI4SR1_RDRF){           //##############  RECEIVE  ###############
      if(SCI4CR2_RIE && R_EN){   
        SCI4_RXRoutine();
      }
  }
  if(SCI4SR1_TDRE){           //##############  TRANSMIT  ###############
      if (SCI4CR2_TIE && T_EN){
        SCI4_TXRoutine();
      }
  }
  //down();   
}
void interrupt SCI1_ISR(void){                       

  uint8_t rcv;
  
  if(SCI1SR1_RDRF){
      rcv = SCI1DRL; //sequence to clear the receieve data register full flag
  } 

  if((SCI1SR1_TDRE) && SCI1CR2_TIE){
      SCI1DRL = SCI1TOBESENT;  
      SCI1CR2_TIE = 0;	// disable transmit interrupts	
  } 
}


///////////////************////I2c intterupt
void I2C_ISR(void){    //put in interrupts.c and fix name
	if (IIC0_IBSR_TCF){	//a transfer was completed
		i2c_fsm(*previous, 0);	//call I2C state machine
	}	
	if (IIC0_IBSR_IBAL){ //master lost arbitration
		IIC0_IBSR_IBAL = 1;
		(void)i2c_fsm(*previous, I2C_IDLE); //set the I2C machine to idle state
	}
	IIC0_IBSR_IBIF = 1; //clear the interrupt flag
}

typedef void (*near tIsrFunc)(void);

const near tIsrFunc VectorTable[] @0xFF10 =
{
  Unimplemented_ISR,    // Vector base + 0x10   Spurious interrupt
  Unimplemented_ISR,    // Vector base + 0x12   System Call Interrupt (SYS)
  Unimplemented_ISR,    // Vector base + 0x14   MPU Access Error
  Unimplemented_ISR,    // Vector base + 0x16   XGATE software error interrupt
  Unimplemented_ISR,    // Vector base + 0x18   Reserved
  Unimplemented_ISR,    // Vector base + 0x1A   Reserved
  Unimplemented_ISR,    // Vector base + 0x1C   Reserved
  Unimplemented_ISR,    // Vector base + 0x1E   Reserved
  Unimplemented_ISR,    // Vector base + 0x20   Reserved
  Unimplemented_ISR,    // Vector base + 0x22   Reserved
  Unimplemented_ISR,    // Vector base + 0x24   Reserved
  Unimplemented_ISR,    // Vector base + 0x26   Reserved
  Unimplemented_ISR,    // Vector base + 0x28   Reserved
  Unimplemented_ISR,    // Vector base + 0x2A   Reserved
  Unimplemented_ISR,    // Vector base + 0x2C   Reserved
  Unimplemented_ISR,    // Vector base + 0x2E   Reserved
  Unimplemented_ISR,    // Vector base + 0x30   Reserved
  Unimplemented_ISR,    // Vector base + 0x32   Reserved
  Unimplemented_ISR,    // Vector base + 0x34   Reserved
  Unimplemented_ISR,    // Vector base + 0x36   Reserved
  Unimplemented_ISR,    // Vector base + 0x38   Reserved
  Unimplemented_ISR,    // Vector base + 0x3A   Reserved
  Unimplemented_ISR,    // Vector base + 0x3C   ATD1 Compare Interrupt
  Unimplemented_ISR,    // Vector base + 0x3E   ATD0 Compare Interrupt
  Unimplemented_ISR,    // Vector base + 0x40   TIM Pulse accumulator input edge
  Unimplemented_ISR,    // Vector base + 0x42   TIM Pulse accumulator A overflow
  Unimplemented_ISR,    // Vector base + 0x44   TIM timer overflow
  Unimplemented_ISR,    // Vector base + 0x46   TIM timer channel 7
  Unimplemented_ISR,    // Vector base + 0x48   TIM timer channel 6
  Unimplemented_ISR,    // Vector base + 0x4A   TIM timer channel 5
  Unimplemented_ISR,    // Vector base + 0x4C   TIM timer channel 4
  Unimplemented_ISR,    // Vector base + 0x4E   TIM timer channel 3
  Unimplemented_ISR,    // Vector base + 0x50   TIM timer channel 2
  Unimplemented_ISR,    // Vector base + 0x52   TIM timer channel 1
  Unimplemented_ISR,    // Vector base + 0x54   TIM timer channel 0
  Unimplemented_ISR,    // Vector base + 0x56   SCI7
  Unimplemented_ISR,    // Vector base + 0x58   Periodic interrupt timer channel 7
  Unimplemented_ISR,    // Vector base + 0x5A   Periodic interrupt timer channel 6
  Unimplemented_ISR,    // Vector base + 0x5C   Periodic interrupt timer channel 5
  Unimplemented_ISR,    // Vector base + 0x5E   Periodic interrupt timer channel 4
  Unimplemented_ISR,    // Vector base + 0x60   Reserved
  Unimplemented_ISR,    // Vector base + 0x62   Reserved
  Unimplemented_ISR,    // Vector base + 0x64   XGATE software trigger 7
  Unimplemented_ISR,    // Vector base + 0x66   XGATE software trigger 6
  Unimplemented_ISR,    // Vector base + 0x68   XGATE software trigger 5
  Unimplemented_ISR,    // Vector base + 0x6A   XGATE software trigger 4
  Unimplemented_ISR,    // Vector base + 0x6C   XGATE software trigger 3
  Unimplemented_ISR,    // Vector base + 0x6E   XGATE software trigger 2
  Unimplemented_ISR,    // Vector base + 0x70   XGATE software trigger 1
  Unimplemented_ISR,    // Vector base + 0x72   XGATE software trigger 0
  Unimplemented_ISR,    // Vector base + 0x74   Periodic interrupt timer channel 3
  Unimplemented_ISR,    // Vector base + 0x76   Periodic interrupt timer channel 2
  Unimplemented_ISR,    // Vector base + 0x78   Periodic interrupt timer channel 1
  Unimplemented_ISR,    // Vector base + 0x7A   Periodic interrupt timer channel 0
  Unimplemented_ISR,    // Vector base + 0x7C   High Temperature Interrupt (HTI)
  Unimplemented_ISR,    // Vector base + 0x7E   Autonomous periodical interrupt (API)
  Unimplemented_ISR,    // Vector base + 0x80   Low-voltage interrupt (LVI)
  Unimplemented_ISR,    // Vector base + 0x82   IIC1 Bus
  Unimplemented_ISR,    // Vector base + 0x84   SCI5
  SCI4_ISR,             // Vector base + 0x86   SCI4
  Unimplemented_ISR,    // Vector base + 0x88   SCI3
  Unimplemented_ISR,    // Vector base + 0x8A   SCI2
  Unimplemented_ISR,    // Vector base + 0x8C   PWM emergency shutdown
  Unimplemented_ISR,    // Vector base + 0x8E   Port P Interrupt
  Unimplemented_ISR,    // Vector base + 0x90   CAN4 transmit
  Unimplemented_ISR,    // Vector base + 0x92   CAN4 receive
  Unimplemented_ISR,    // Vector base + 0x94   CAN4 errors
  Unimplemented_ISR,    // Vector base + 0x96   CAN4 wake-up
  Unimplemented_ISR,    // Vector base + 0x98   CAN3 transmit
  Unimplemented_ISR,    // Vector base + 0x9A   CAN3 receive
  Unimplemented_ISR,    // Vector base + 0x9C   CAN3 errors
  Unimplemented_ISR,    // Vector base + 0x9E   CAN3 wake-up
  Unimplemented_ISR,    // Vector base + 0xA0   CAN2 transmit
  Unimplemented_ISR,    // Vector base + 0xA2   CAN2 receive
  Unimplemented_ISR,    // Vector base + 0xA4   CAN2 errors
  Unimplemented_ISR,    // Vector base + 0xA6   CAN2 wake-up
  Unimplemented_ISR,    // Vector base + 0xA8   CAN1 transmit
  Unimplemented_ISR,    // Vector base + 0xAA   CAN1 receive
  Unimplemented_ISR,    // Vector base + 0xAC   CAN1 errors
  Unimplemented_ISR,    // Vector base + 0xAE   CAN1 wake-up
  Unimplemented_ISR,    // Vector base + 0xB0   CAN0 transmit
  Unimplemented_ISR,    // Vector base + 0xB2   CAN0 receive
  Unimplemented_ISR,    // Vector base + 0xB4   CAN0 errors
  Unimplemented_ISR,    // Vector base + 0xB6   CAN0 wake-up
  Unimplemented_ISR,    // Vector base + 0xB8   FLASH
  Unimplemented_ISR,    // Vector base + 0xBA   FLASH Fault Detect
  Unimplemented_ISR,    // Vector base + 0xBC   SPI2
  Unimplemented_ISR,    // Vector base + 0xBE   SPI1
  I2C_ISR,              // Vector base + 0xC0   IIC0 bus
  Unimplemented_ISR,    // Vector base + 0xC2   SCI6
  Unimplemented_ISR,    // Vector base + 0xC4   CRG self-clock mode
  Unimplemented_ISR,    // Vector base + 0xC6   CRG PLL lock
  Unimplemented_ISR,    // Vector base + 0xC8   Pulse accumulator B overflow
  Unimplemented_ISR,    // Vector base + 0xCA   Modulus down counter underflow
  Unimplemented_ISR,    // Vector base + 0xCC   Port H
  Unimplemented_ISR,    // Vector base + 0xCE   Port J
  Unimplemented_ISR,    // Vector base + 0xD0   ATD1
  Unimplemented_ISR,    // Vector base + 0xD2   ATD0
  SCI1_ISR,             // Vector base + 0xD4   SCI1
  Unimplemented_ISR,    // Vector base + 0xD6   SCI0     
  Unimplemented_ISR,    // Vector base + 0xD8   SPI0     
  Unimplemented_ISR,    // Vector base + 0xDA   TIM Pulse accumulator input edge      
  Unimplemented_ISR,    // Vector base + 0xDC   TIM Pulse accumulator A overflow      
  Unimplemented_ISR,    // Vector base + 0xDE   TIM timer overflow      
  Unimplemented_ISR,    // Vector base + 0xE0   TIM timer channel 7      
  Unimplemented_ISR,    // Vector base + 0xE2   TIM timer channel 6      
  Unimplemented_ISR,    // Vector base + 0xE4   TIM timer channel 5      
  Unimplemented_ISR,    // Vector base + 0xE6   TIM timer channel 4      
  Unimplemented_ISR,    // Vector base + 0xE8   TIM timer channel 3      
  Unimplemented_ISR,    // Vector base + 0xEA   TIM timer channel 2      
  Unimplemented_ISR,    // Vector base + 0xEC   TIM timer channel 1      
  Unimplemented_ISR,    // Vector base + 0xEE   TIM timer channel 0      
  Unimplemented_ISR,    // Vector base + 0xF0   Real time interrupt
  Unimplemented_ISR,    // Vector base + 0xF2   IRQ      
  Unimplemented_ISR,    // Vector base + 0xF4   XIRQ      
  Unimplemented_ISR,    // Vector base + 0xF6   SWI      
  Unimplemented_ISR,    // Vector base + 0xF8   Unimplemented instruction trap 
  _Startup,             // 0xFA COP Watchdog Reset 
  _Startup,             // 0xFC Clock monitor reset
  _Startup              // 0xFE Illegal address/External PIN/Low-Voltage Reset/Pwr-On Reset
};

#pragma CODE_SEG DEFAULT      