/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 2C
 */
 
#include "LED.h"

void LED_Init(void)
{
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
}

void Red_LED_Off(void) 
{
	// reset the red LED
	GPIOB->ODR &= ~GPIO_ODR_OD2;
}

void Red_LED_On(void) 
{
	// set the red LED
	GPIOB->ODR |= GPIO_ODR_OD2;
}

void Red_LED_Toggle(void)
{
	// toggle the red LED
	GPIOB->ODR ^= GPIO_ODR_OD2;
}

void Green_LED_Off(void) 
{
	// reset the green LED
	GPIOE->ODR &= ~GPIO_ODR_OD8;
}

void Green_LED_On(void) 
{
	// set the green LED
	GPIOE->ODR |= GPIO_ODR_OD8;
}

void Green_LED_Toggle(void) 
{
	// toggle the green LED
	GPIOE->ODR ^= GPIO_ODR_OD8;
}
