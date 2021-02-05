#include "stm32l476xx.h"

#include "L3GD20.h"
#include "SPI.h"
#include "SysClock.h"
#include "SysTimer.h"
#include "UART.h"

#include <stdio.h>

typedef struct {
	float x; 
	float y; 
	float z; 
} L3GD20_Data_t;

int main(void){
	System_Clock_Init();   // System Clock = 80 MHz
	SysTick_Init();
	
	// Initialize Gyroscope
	GYRO_Init();  

	// Initialize USART2
	UART2_Init();
	UART2_GPIO_Init();
	USART_Init(USART2);

	while(1) {
		// [TODO]

		delay(500); // Small delay between receiving measurements
	}
}
