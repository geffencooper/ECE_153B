#include "LED.h"

void LED_init()
{
	green_LED_init();
	red_LED_init();
}

// initialize the red LED to normal output
void red_LED_init()
{
	// Enable GPIO Port B Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
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
}

// initialize the green LED to alternative output
void green_LED_init()
{
	// Enable GPIO Port E Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	// Configure PE8
	// set mode to alternative function
	GPIOE->MODER &= ~GPIO_MODER_MODE8; // reset
	GPIOE->MODER |= GPIO_MODER_MODE8_1; // set to 10 = alternative function
	
	// set output speed to very high
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8; // reset
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED8; // set to 11 = very high speed
	
	// set PUPD to no pull up no pull down
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR8; // reset, no pull up, pull down = 00
	
	// configure the alternative function for GPIO PE8 to TIM1_CH1N
	// TIM1_CH1N=AF1, pin8 is in the upper half of the registers
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL8;  // reset pin 8 selection
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_0; // set bit 0 to select AF1 --> 0001 
}

// toggle the red LED when pass 2V
void red_LED_on()
{
	// set the red LED
	GPIOB->ODR |= GPIO_ODR_OD2;
}

void red_LED_off()
{
	// reset the red LED
	GPIOB->ODR &= ~GPIO_ODR_OD2;
}