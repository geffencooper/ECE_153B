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
	uint16_t x_raw; 
	uint16_t y_raw; 
	uint16_t z_raw; 
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
			// read the data for the x-axis high and low registers
			GYRO_IO_Read(read_buffer+1, L3GD20_OUT_X_H_ADDR, 1);
			GYRO_IO_Read(read_buffer, L3GD20_OUT_X_L_ADDR, 1);
			
			// read the data for the y-axis high and low registers
			GYRO_IO_Read(read_buffer+3, L3GD20_OUT_Y_H_ADDR, 1);
			GYRO_IO_Read(read_buffer+2, L3GD20_OUT_Y_L_ADDR, 1);
			
			// read the data for the z-axis high and low registers
			GYRO_IO_Read(read_buffer+5, L3GD20_OUT_Z_H_ADDR, 1);
			GYRO_IO_Read(read_buffer+4, L3GD20_OUT_Z_L_ADDR, 1);
			
			// the raw data for each axis is in 16 bit 2's complement
			// each digit of raw data is 0.070 degrees/sec, multiply the raw data by 0.07 to get dps
			gyro_data.x = (((Gyro_Raw_Data_t*)read_buffer)->x_raw)*0.07;
			gyro_data.y = (((Gyro_Raw_Data_t*)read_buffer)->y_raw)*0.07;
			gyro_data.z = (((Gyro_Raw_Data_t*)read_buffer)->z_raw)*0.07;
		}
		
		printf("X: %f\tY: %f\tZ: %f\n", gyro_data.x, gyro_data.y, gyro_data.z);
		delay(500); // Small delay between receiving measurements
	}
}
