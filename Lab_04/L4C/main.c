#include "stm32l476xx.h"

#include "I2C.h"
#include "LCD.h"
#include "SysClock.h"
	
int main(void) {
	System_Clock_Init(); // System Clock = 80 MHz
	
	LCD_Initialization();
	LCD_Clear();
	
	// Initialize I2C
	I2C_GPIO_Init();
	I2C_Initialization();
	
	int i;
	char message[6];
	uint8_t SlaveAddress;
	uint8_t Data_Receive;
	uint8_t Data_Send;
	while(1) {	
		// Determine Slave Address
		//
		// Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
		SlaveAddress = 0b1111111 << 1; // STUB - Fill in correct address 
		
		// [TODO] - Get Temperature
		// 
		// First, send a command to the sensor for reading the temperature
		// Next, get the measurement
		
		// [TODO] - Print Temperature to LCD
		
		// Some delay
		for(i = 0; i < 50000; ++i); 
	}
}
