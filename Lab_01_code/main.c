/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 1A
 */

#include "stm32l476xx.h"
#include "poll_buttons.h"
 
void Init() 
{
	// Enable HSI (16 MHz)
	RCC->CR |= RCC_CR_HSION; // set HSI clock enable bit of clock control register
	while((RCC->CR & RCC_CR_HSIRDY) == 0); // wait for HSI clock ready flag to be set
	
	// Select HSI as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW; // reset system clock switch bits of clock configuration register to 00
	RCC->CFGR |= RCC_CFGR_SW_HSI; // set flag to 01 to select HSI as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) == 0); // wait for system clock switch status to be ready
	
	// Enable GPIO Clocks
	/* 
			We need to enable the peripheral clock for GPIO Ports A (joystick), B (Red LED), E(Green LED)
			This is done by setting the corresponding bits of the peripheral clock enable register RCC_AHB2ENR
	*/
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // enable clock for GPIO port A, mask = 0x00000001, bit 0
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // enable clock for GPIO port B, mask = 0x00000002, bit 1
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN; // enable clock for GPIO port E, mask = 0x00000010, bit 4
	
	
	// Initialize Red LED
	/*
			We need to configure the GPIO of the Red LED (PB2):
			1. Mode = Output                     --> configure port B mode register
			2. Output Type = Push-Pull           --> configure port B output type register
			3. PU/PD = No Pull-Up, No Pull-Down  --> configure port B pull-up/pull-down register
	*/
	GPIOB->MODER &= ~GPIO_MODER_MODE2;  // 1. Reset bits 5:4 (pin 2) of port B MODER to 00,    mask = 0x00000030 <--> 0b110000
	GPIOB->MODER |= GPIO_MODER_MODE2_0; //    Then set bits 5:4 (pin 2) of port B MODER to 01, mask = 0x00000010 <--> 0b010000
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT2;  // 2. Set bit 2 (pin 2) of port B OTYPER to 0,         mask = 0x00000004 <--> 0b000100
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD2;  // 3. Set bits 5:4 (pin 2) of port B PUPDR to 00,      mask = 0x00000030 <--> 0b110000
	
	// reset the red LED so off on start
	GPIOB->ODR &= ~GPIO_ODR_OD2;
	
	
	// Initialize Green LED
	/*
			We need to configure the GPIO of the Green LED (PE8):
			1. Mode = Output                     --> configure port E mode register
			2. Output Type = Push-Pull           --> configure port E output type register
			3. PU/PD = No Pull-Up, No Pull-Down  --> configure port E pull-up/pull-down register
	*/
	GPIOE->MODER &= ~GPIO_MODER_MODE8;  // 1. Reset bits 17:16 (pin 8) of port E MODER to 00,    mask = 0x00030000 <--> 0b110000000000000000
	GPIOE->MODER |= GPIO_MODER_MODE8_0; //    Then set bits 17:16 (pin 8) of port E MODER to 01, mask = 0x00010000 <--> 0b010000000000000000
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT8;  // 2. Set bit 8 (pin 8) of port E OTYPER to 0,           mask = 0x00000100 <--> 0b000000000100000000
	GPIOE->PUPDR  &= ~GPIO_PUPDR_PUPD8; // 3. Set bits 17:16 (pin 8) of port B PUPDR to 00,      mask = 0x00000030 <--> 0b110000000000000000
	
	// reset the green LED so off on start
	GPIOE->ODR &= ~GPIO_ODR_OD8;
	
	
	// Initialize Joystick
	/*
			We need to configure the GPIOs of the joystick  (PA0, PA1, PA2, PA3, PA5):
			1. Mode = Input       --> configure port A mode register
			2. PU/PD = Pull-Down  --> configure port A pull-up/pull-down register
			(for clarity, each individual GPIO pin was configured as shown below)
	*/
	GPIOA->MODER &= ~GPIO_MODER_MODE0;   // 1. Reset bits 1:0 (pin 0) of port A MODER to 00,   mask = 0x00000003 <--> 0b000000000011 
	GPIOA->MODER &= ~GPIO_MODER_MODE1;   //    Reset bits 3:2 (pin 1) of port A MODER to 00,   mask = 0x0000000C <--> 0b000000001100
	GPIOA->MODER &= ~GPIO_MODER_MODE2;   //    Reset bits 5:4 (pin 2) of port A MODER to 00,   mask = 0x00000030 <--> 0b000000110000
	GPIOA->MODER &= ~GPIO_MODER_MODE3;   //    Reset bits 7:6 (pin 3) of port A MODER to 00,   mask = 0x000000C0 <--> 0b000011000000
	GPIOA->MODER &= ~GPIO_MODER_MODE5;   //    Reset bits 11:10 (pin 5) of port A MODER to 00, mask = 0x00000C00 <--> 0b110000000000
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;   // 2. Reset bits 1:0 (pin 0) of port A PUPDR to 00,   mask = 0x00000003 <--> 0b000000000011
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1;   //    Reset bits 3:2 (pin 1) of port A PUPDR to 00,   mask = 0x0000000C <--> 0b000000001100
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD2;   //    Reset bits 5:4 (pin 2) of port A PUPDR to 00,   mask = 0x00000030 <--> 0b000000110000
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3;   //    Reset bits 7:6 (pin 3) of port A PUPDR to 00,   mask = 0x000000C0 <--> 0b000011000000
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;   //    Reset bits 11:10 (pin 5) of port A PUPDR to 00, mask = 0x00000C00 <--> 0b110000000000
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_1;  //    Set bits 1:0 (pin 0) of port A PUPDR to 10,     mask = 0x00000002 <--> 0b000000000010
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD1_1;  //    Set bits 3:2 (pin 1) of port A PUPDR to 10,     mask = 0x00000008 <--> 0b000000001000
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_1;  //    Set bits 5:4 (pin 2) of port A PUPDR to 10,     mask = 0x00000020 <--> 0b000000100000
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_1;  //    Set bits 7:6 (pin 3) of port A PUPDR to 10,     mask = 0x00000080 <--> 0b000010000000
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_1;  //    Set bits 11:10 (pin 5) of port A PUPDR to 10,   mask = 0x00000800 <--> 0b100000000000
}
 
int main() 
{
	// Initialization
	Init();
	
	// Polling to Check for Button Presses
	while(1)
	{
		center();
		left();
		right();
		up();
		down();
	}
	
	return 0;
}
