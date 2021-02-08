#include "stm32l476xx.h"

#include "L3GD20.h"
#include "SPI.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "UART.h"
#include "string.h"

#include <stdio.h>

typedef struct 
{
	float x; 
	float y; 
	float z; 
} L3GD20_Data_t;

// used to dereference data from buffer
typedef struct 
{
	int16_t x_raw; 
	int16_t y_raw; 
	int16_t z_raw; 
} Gyro_Raw_Data_t;

int main(void){
	System_Clock_Init();   // System Clock = 80 MHz
	SysTick_Init();
	
	// Initialize Gyroscope
	GYRO_Init();  

	// Initialize USART2
	UART2_Init();
	UART2_GPIO_Init();
	USART_Init(USART2);

	uint8_t read_buffer[6] = {0};
	L3GD20_Data_t gyro_data = {0,0,0};
	while(1) 
	{
		// read the status register to check if data is available
		GYRO_IO_Read(read_buffer, L3GD20_STATUS_REG_ADDR, 1);
		
		// check ZXY axes new data is available (whole set)
		if(read_buffer[0] & L3GD0_SR_ZYXDA)
		{
			// read the data for the x-axis, y-axis, z-axis sequentially (address auto increments on read)
			GYRO_IO_Read(read_buffer, L3GD20_OUT_X_L_ADDR, 6);
			
			// the raw data for each axis is in 16 bit 2's complement
			// each digit of raw data is 0.070 degrees/sec, multiply the raw data by 0.07 to get dps
			gyro_data.x = (((Gyro_Raw_Data_t*)read_buffer)->x_raw)*(0.07F);
			gyro_data.y = (((Gyro_Raw_Data_t*)read_buffer)->y_raw)*(0.07F);
			gyro_data.z = (((Gyro_Raw_Data_t*)read_buffer)->z_raw)*(0.07F);
		}
		
		printf("X: %f\tY: %f\tZ: %f\n", gyro_data.x, gyro_data.y, gyro_data.z);
		delay(500); // Small delay between receiving measurements
	}
}
