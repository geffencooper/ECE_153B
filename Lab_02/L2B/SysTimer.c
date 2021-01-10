/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 1B
 */

#include "SysTimer.h"

uint32_t volatile msTicks;

void SysTick_Init(void) {
	// SysTick Control & Status Register
	SysTick->CTRL = 0; // Disable SysTick IRQ and SysTick Counter
	
	// SysTick Reload Value Register
	SysTick->LOAD = 1000; // SysClk is MSI which is 8Mhz, CLKSOURCE is 0 so SysTick = 8MHz/8 = 1Mhz --> 1us*1000=1ms
	
	// SysTick Current Value Register
	SysTick->VAL = 0;
	
	// Configure and Enable SysTick interrupt in NVIC
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 1); // Set Priority to 1
	
	// Enables SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does not assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Select clock source
	// If CLKSOURCE = 0, the external clock is used. The frequency of SysTick clock is the frequency of the AHB clock divided by 8.
	// If CLKSOURCE = 1, the processor clock is used.
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;		
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
}

void SysTick_Handler(void) 
{
	++msTicks;
}

void delay(uint32_t T) 
{
	// store the current value of msTicks
	int currentTicks = (int)msTicks; 
	
	// time elapsed since entered function
	int elapsed = (int)msTicks - currentTicks;
	
	// when msTicks overflows and rolls over it is possible that
	// elapsed will become negative so need to account for this case
	if(elapsed < 0)
	{
		// account for this by adding the time before the rollover
		elapsed = (INT32_MAX - currentTicks) + msTicks;
	}
	
	// wait until T amount of msTicks has passed since entered function
	while(elapsed < T)
	{
		elapsed = (int)msTicks - currentTicks;
		
		if(elapsed < 0)
		{
			elapsed = (INT32_MAX - currentTicks) + msTicks;
		}
	}	
}