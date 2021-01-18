/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 1B
 */

#include "stm32l476xx.h"
#include "LED.h"
#include "SysTimer.h"

void System_Clock_Init(void) 
{
	// Select MSI as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;    // Reset system clock switch bits of clock configuration register to 00
	RCC->CFGR |= RCC_CFGR_SW_MSI; // Set flag to select MSI as system clock
	
	// Set MSI clock range
	RCC->CR &= ~RCC_CR_MSIRANGE;  // Reset the MSIRANGE bits in the register
	RCC->CR |= RCC_CR_MSIRANGE_7; // Set bits with value that makes range 8 MHz
	
	// Use the MSI clock range that is defined in RCC_CR
	RCC->CR |= RCC_CR_MSIRGSEL;  // Set the MSIRGSEL bit so the RCC_CR bits are used to select MSI clock
	
	// Enable MSI oscillator
	RCC->CR |= RCC_CR_MSION;     // Set the MSION bit
	
	// Wait until MSI is ready
	while((RCC->CR & RCC_CR_MSIRDY) == 0);
}

int main() 
{
	// Initialization 
	System_Clock_Init();
	SysTick_Init();
	LED_Init();
	
	//Red_LED_On();
	while(1) 
	{
		// toggle the red LED
		Red_LED_Toggle();
		
		// wait 1000ms
		delay(1000);
	}
}
