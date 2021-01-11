#include "poll_buttons.h"
#include "stm32l476xx.h"
#include "stdint.h"

// button states
#define PRESSED 1
#define RELEASED 0


void center()
{
	static uint8_t button_state = RELEASED;
	
	// read PA0, check if pressed
	if(GPIOA->IDR & GPIO_IDR_ID0) // mask = 0x01
	{
		// only toggle LEDs if button is not already being pressed
		if(button_state == RELEASED)
		{
			// toggle the green LED
			GPIOE->ODR ^= GPIO_ODR_OD8;
			
			// toggle the red LED
			GPIOB->ODR ^= GPIO_ODR_OD2;
			
			button_state = PRESSED;
		}
	}
	else
	{
		button_state = RELEASED;
	}
}


void left()
{
	static uint8_t button_state = RELEASED;
	
	// read PA1, check if pressed
	if(GPIOA->IDR & GPIO_IDR_ID1) // mask = 0x02
	{
		// only toggle LED if button is not already being pressed
		if(button_state == RELEASED)
		{
			// toggle the green LED
			GPIOE->ODR ^= GPIO_ODR_OD8;
			
			button_state = PRESSED;
		}
	}
	else
	{
		button_state = RELEASED;
	}
}


void right()
{
	static uint8_t button_state = RELEASED;
	
	// read PA2, check if pressed
	if(GPIOA->IDR & GPIO_IDR_ID2) // mask = 0x04
	{
		// only toggle LED if button is not already being pressed
		if(button_state == RELEASED)
		{
			// toggle the red LED
			GPIOB->ODR ^= GPIO_ODR_OD2;
			
			button_state = PRESSED;
		}
	}
	else
	{
		button_state = RELEASED;
	}
}


void up()
{
	static uint8_t button_state = RELEASED;
	
	// read PA3, check if pressed
	if(GPIOA->IDR & GPIO_IDR_ID3) // mask = 0x08
	{
		// only set LEDs if button is not already being pressed
		if(button_state == RELEASED)
		{
			// set the green LED
			GPIOE->ODR |= GPIO_ODR_OD8;
			
			// set the red LED
			GPIOB->ODR |= GPIO_ODR_OD2;
			
			button_state = PRESSED;
		}
	}
	else
	{
		button_state = RELEASED;
	}
}


void down()
{
	static uint8_t button_state = RELEASED;
	
	// read PA5, check if presed
	if(GPIOA->IDR & GPIO_IDR_ID5) // mask = 0x20
	{
		// only reset LEDs if button is not already being pressed
		if(button_state == RELEASED)
		{
			// reset the green LED
			GPIOE->ODR &= ~GPIO_ODR_OD8;
			
			// reset the red LED
			GPIOB->ODR &= ~GPIO_ODR_OD2;
			
			button_state = PRESSED;
		}
	}
	else
	{
		button_state = RELEASED;
	}
}
