#include "stm32l476xx.h"

static void DAC_Pin_Init(void) 
{
  // Enable GPIO Port A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// set PA5 to analog, no pull-up/no pull-down
	GPIOA->MODER &= ~GPIO_MODER_MODE5; // clear
	GPIOA->MODER |= GPIO_MODER_MODE5; // set mode to analog (11)
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5; // reset to no pull-up/pull-down (00)
}

void DAC_Write_Value(uint32_t value) 
{
    // Write DAC1Ch2 right-aligned 12-bit value
		DAC->DHR12R2 = (value & 0x0FFF); 
}

void DAC_Init(void)
{
    DAC_Pin_Init();

    // Enable DAC clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
	
	  // disable DAC channel 2 while configuring settings
	  DAC->CR  &= ~DAC_CR_EN2;
	
    // Enable software trigger mode
		DAC->CR &= ~DAC_CR_TSEL2;
	  DAC->CR |= DAC_CR_TSEL2;
	
    // Disable trigger
		DAC->CR &= ~DAC_CR_TEN2;
	
    // DAC1 connected in normal mode to external pin only with buffer enabled
		DAC->MCR &= ~DAC_MCR_MODE2;
	
    // Enable DAC channel 2
		DAC->CR  |= DAC_CR_EN2;
	
    DAC_Write_Value(0);
}
