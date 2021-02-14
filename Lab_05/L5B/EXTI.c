#include "EXTI.h"

#include "DAC.h"

void EXTI_Init(void) 
{
	// Enable GPIO Port A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
  // Configure EXTI for joystick UP and CENTER
	// Initialize Joystick
	/*
			We need to configure the GPIOs of the joystick  (PA0, PA3):
			1. Mode = Input       --> configure port A mode register
			2. PU/PD = Pull-Down  --> configure port A pull-up/pull-down register
	*/
	GPIOA->MODER &= ~GPIO_MODER_MODE0; // center button output
	GPIOA->MODER &= ~GPIO_MODER_MODE3; // left button output
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;   
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD3; 
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_1;
	GPIOA->PUPDR |= GPIO_PUPDR_PUPD3_1;  
	
	// Configure SYSCFG EXTI
	/*
			We need to configure the external interrupt configuration register (EXTICR)
			so that the button GPIO pins are linked to the EXTI lines.
			
			GPIO pins 0-15 are multiplexed to the first 16 EXTI lines (0-15) 
			for each port A-G(H only for 0,1). Ex: PA0-PH0 are multiplexed to EXTI0.
			
			There are 4 EXTICRs for pins 0-3, 4-7, 8-11, 12-15  Ex: EXTICR[0] is for pin 0-3
			
			Recall: Center=PA0, UP=PA3
			
			1. Reset bit locations for button GPIOs
			2. Set bit locations for button GPIOs
	*/
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0; 
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3; 
	
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA; // connect EXTI for PA0 --> CENTER
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI3_PA; // connect EXTI for PA3 --> UP
	
	// Configure EXTI Trigger
	/*
			We need to configure falling trigger selection register 1 (EXTI_FTSR1)
			for the corresponding EXTI lines we initialized.
			
			EXTI_FTSR1 bits 16:0 correspond to EXTI lines 0-16
			
			1. Set bit locations for button GPIOs
	*/
	EXTI->FTSR1 |= EXTI_FTSR1_FT0; // Set bit 0 to enable FT for EXTI0 (PA0)
	EXTI->FTSR1 |= EXTI_FTSR1_FT3; // Set bit 1 to enable FT for EXTI1 (PA3)
	
	
	// Enable EXTI
	/*
			We need to enable EXTI for the corresponding lines we initialized
			by configuring the interrupt mask register.
			
			EXTI_IMR1 bits 31:0 correspond to EXTI lines 0-31
			
			1. Set bit locations for button GPIOs
	*/
	EXTI->IMR1 |= EXTI_IMR1_IM0; // Set bit 0 to enable EXTI0 (PA0)
	EXTI->IMR1 |= EXTI_IMR1_IM3; // Set bit 3 to enable EXTI1 (PA3)
	
	// Configure and Enable in NVIC
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_SetPriority(EXTI3_IRQn, 0);
}

#define DAC_MIN 0
#define DAC_MAX 4095
#define DAC_INCREMENT 256

static uint32_t dac_value = 0;

// Interrupt handlers

// center button pressed
void EXTI0_IRQHandler(void)
{
	// clear the pending register by writing a 1 to it
	EXTI->PR1 |= EXTI_PR1_PIF0;
	
	// increment the DAC value
	if((DAC_MAX - dac_value) <= DAC_INCREMENT)
	{
		dac_value = DAC_MAX;
	}
	else
	{
		dac_value += DAC_INCREMENT;
	}
	DAC_Write_Value(dac_value);
}

// up button pressed
void EXTI3_IRQHandler(void)
{
	// clear the pending register by writing a 1 to it
	EXTI->PR1 |= EXTI_PR1_PIF3;
	
	// decrement the DAC value
	if((dac_value - DAC_MIN) <= DAC_INCREMENT)
	{
		dac_value = DAC_MIN;
	}
	else
	{
		dac_value -= DAC_INCREMENT;
	}
	DAC_Write_Value(dac_value);
}
