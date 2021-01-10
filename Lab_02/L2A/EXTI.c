/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 1B
 */

#include "EXTI.h"
#include "LED.h"

void EXTI_Init(void) 
{
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

	
	// Configure SYSCFG EXTI
	/*
			We need to configure the external interrupt configuration register (EXTICR)
			so that the button GPIO pins are linked to the EXTI lines.
			
			GPIO pins 0-15 are multiplexed to the first 16 EXTI lines (0-15) 
			for each port A-G(H only for 0,1). Ex: PA0-PH0 are multiplexed to EXTI0.
			
			There are 4 EXTICRs for pins 0-3, 4-7, 8-11, 12-15  Ex: EXTICR[0] is for pin 0-3
			
			Recall: Center=PA0, Left=PA1, Right=PA2
			
			1. Reset bit locations for button GPIOs
			2. Set bit locations for button GPIOs
			
			For clarity, each individual GPIO pin was configured as shown below.
			(Step 2 is actually redundant because the bits are being configured to 000)
	*/
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;   // 1. Reset bits 2:0   of EXTICR1 (for PA0)
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1;   //    Reset bits 6:4   of EXTICR1 (for PA1)
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;   //		Reset bits 10:8  of EXTICR1 (for PA2)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // 2. Set bits 2:0   of EXTICR1 to 000 (PA0 --> EXTI0)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA; //    Set bits 6:4   of EXTICR1 to 000 (PA1 --> EXTI1)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA; //		Set bits 10:8  of EXTICR1 to 000 (PA2 --> EXTI2)
	
	
	// Configure EXTI Trigger
	/*
			We need to configure falling trigger selection register 1 (EXTI_FTSR1)
			for the corresponding EXTI lines we initialized.
			
			EXTI_FTSR1 bits 16:0 correspond to EXTI lines 0-16
			
			1. Set bit locations for button GPIOs
			
			For clarity, each individual GPIO pin was configured as shown below.
	*/
	EXTI->FTSR1 |= EXTI_FTSR1_FT0; // Set bit 0 to enable FT for EXTI0 (PA0)
	EXTI->FTSR1 |= EXTI_FTSR1_FT1; // Set bit 1 to enable FT for EXTI1 (PA1)
	EXTI->FTSR1 |= EXTI_FTSR1_FT2; // Set bit 2 to enable FT for EXTI2 (PA2)
	
	
	// Enable EXTI
	/*
			We need to enable EXTI for the corresponding lines we initialized
			by configuring the interrupt mask register.
			
			EXTI_IMR1 bits 31:0 correspond to EXTI lines 0-31
			
			1. Set bit locations for button GPIOs
			
			For clarity, each individual GPIO pin was configured as shown below.
	*/
	EXTI->IMR1 |= EXTI_IMR1_IM0; // Set bit 0 to enable EXTI0 (PA0)
	EXTI->IMR1 |= EXTI_IMR1_IM1; // Set bit 1 to enable EXTI1 (PA1)
	EXTI->IMR1 |= EXTI_IMR1_IM2; // Set bit 2 to enable EXTI2 (PA2)
	
	
	// Configure and Enable in NVIC
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_SetPriority(EXTI1_IRQn, 0);
	NVIC_SetPriority(EXTI2_IRQn, 0);
}

// [TODO] Write Interrupt Handlers (look in startup_stm32l476xx.s to find the 
// interrupt handler names that you should use)

// center button pressed
void EXTI0_IRQHandler(void)
{
	// clear the pending register by writing a 1 to it
	EXTI->PR1 |= EXTI_PR1_PIF0;
	
	// toggle red and green LEDs
	Green_LED_Toggle();
	Red_LED_Toggle();
}

// left button pressed
void EXTI1_IRQHandler(void)
{
	// clear the pending register by writing a 1 to it
	EXTI->PR1 |= EXTI_PR1_PIF1;
	
	// toggle green LED
	Green_LED_Toggle();
}

// right button pressed
void EXTI2_IRQHandler(void)
{
	// clear the pending register by writing a 1 to it
	EXTI->PR1 |= EXTI_PR1_PIF2;
	
	// toggle red LED;
	Red_LED_Toggle();
}

void ExampleInterruptHandler(void) 
{
	// Clear interrupt pending bit
	
	// Define behavior that occurs when interrupt occurs
}