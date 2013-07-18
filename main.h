/*******************************************************
* Typedefs (int is 16 bits)
********************************************************/

typedef unsigned char     uint8_t;  		/*unsigned 8 bit definition */
typedef unsigned short    uint16_t; 		/*unsigned 16 bit definition*/
typedef unsigned long int uint32_t; 		/*unsigned 32 bit definition*/
typedef unsigned long long int uint64_t;/*unsigned 64 bit definition*/
typedef signed char       int8_t;   		/*signed 8 bit definition */
typedef short             int16_t;  		/*signed 16 bit definition*/
typedef long int          int32_t;  		/*signed 32 bit definition*/
typedef unsigned char BOOL;				/*unsigned 8 bit bool definition */


/*******************************************************
* Function Definitions
********************************************************/

void DelayMs(uint16_t);
void DelayUs(uint16_t);


/*******************************************************
* Clock/Delays
********************************************************/
// Bus speed defined in initBusSpeed() function
#define BUSCLK 48000000UL
#define DELAY_1MS (48000/13) // Inner loop of DelayMs function is 13 cycles
                             // 48000 cycles for a 1ms delay on 48MHz clock
#define DELAY_1US 2          // About 48/3 FIXME




#define OUTPUT 1
#define INPUT  0