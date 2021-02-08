#include "stm32l476xx.h"

#include "I2C.h"
#include "LCD.h"
#include "SysClock.h"
	
// converts a number to a string, zero pads the remaining spots in buffer
void num_to_string(char* buffer, int size, int num)
{
	// keep track of the sign
	uint8_t is_negative = 0;
	if(num < 0)
	{
		num*=-1; // make it positive so can mod correctly
		is_negative = 1;
	}
	
	// iterate backwards from last index
	int i = size-1;
	
	// convert int to string
	while(num !=0)
	{
		buffer[i] = (num%10 + 48); // get digit, convert to ASCII
		num /= 10;
		i--;
		
		// number too long for buffer
		if(i < 0)
		{
			break;
		}
	}
	
	// pad remaining spots in buffer with zeros
	for(; i >= 0; i--)
	{
		buffer[i] = 48;
	}
	
	// add a negative sign if necessary
	if(is_negative)
	{
		buffer[0] = '-';
	}
}
	
int main(void) 
{
	System_Clock_Init(); // System Clock = 80 MHz
	
	LCD_Initialization();
	LCD_Clear();
	
	// Initialize I2C
	I2C_GPIO_Init();
	I2C_Initialization();
	
	int i;
	char message[6];
	uint8_t SlaveAddress = 0;
	uint8_t Data_Receive = 0;
	uint8_t Data_Send = 0;
	int8_t temp = 0;
	
	// TC74A0 5.0VAT --> address = 1001000
	// Note the "<< 1" must be present because bit 0 is treated as a don't care in 7-bit addressing mode
	SlaveAddress = 0x48 << 1;
	
	// set mode to normal by writing to configuration register
	Data_Send = 1; // config register address 0x01
	I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	Data_Send = 0; // normal mode
	I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
	
	while(1) 
	{	
		// First, send a command to the sensor for reading the temperature
		Data_Send = 0; // temp register address
		I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
		
		// Next, get the measurement
		I2C_ReceiveData(I2C1, SlaveAddress, &Data_Receive, 1);
		
		// test negative temp --> -5 degrees
		// Data_Receive = 0xFB;
		
		// convert data to signed
		temp = Data_Receive;
		
		// convert the temp to a string
		num_to_string(message, sizeof(message), temp);
		
		// Print Temperature to LCD
		LCD_DisplayString((uint8_t *)message);
		
		// Some delay
		for(i = 0; i < 50000; ++i); 
	}
}
