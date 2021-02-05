#include "stm32l476xx.h"

#include "LED.h"
#include "SysClock.h"
#include "UART.h"

#include <string.h>
#include <stdio.h>

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void) 
{
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	
	// Initialize UART -- change the argument depending on the part you are working on
	Init_USARTx(2);
	
	char rxByte;
	
	// prompt the user to enter a command
	printf("\n---------------------------\n");
	printf("Enter a command:\tRed LED state: %i\n1. Turn on the red LED  (Y/y)\r\n2. Turn off the red LED  (N/n)\r\n", Red_LED_Read());
	while(1) 
	{
		scanf("%c", &rxByte);
		if(rxByte == 'y' || rxByte == 'Y')
		{
			Red_LED_On();
			printf("The red LED has been turned ON\n");
		}
		else if(rxByte == 'n' || rxByte == 'N')
		{
			Red_LED_Off();
			printf("The red LED has been turned OFF\n");
		}
		else
		{
			printf("The command entered is invalid. Try again\n");
		}
		printf("\n---------------------------\n");
		printf("Enter a command:\tRed LED state: %i\n1. Turn on the red LED  (Y/y)\r\n2. Turn off the red LED  (N/n)\r\n", Red_LED_Read());
	}
}
