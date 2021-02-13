#include "stm32l476xx.h"

#include "ADC.h"
#include "LED.h"

#include <stdint.h>

void ADC_Wakeup(void) {
	int wait_time;
	
	// To start ADC operations, the following sequence should be applied
	// DEEPPWD = 0: ADC not in deep-power down
	// DEEPPWD = 1: ADC in deep-power-down (default reset state)
	if ((ADC1->CR & ADC_CR_DEEPPWD) == ADC_CR_DEEPPWD)
		ADC1->CR &= ~ADC_CR_DEEPPWD; // Exit deep power down mode if still in that state
	
	// Enable the ADC internal voltage regulator
	// Before performing any operation such as launching a calibration or enabling the ADC, the ADC
	// voltage regulator must first be enabled and the software must wait for the regulator start-up time.
	ADC1->CR |= ADC_CR_ADVREGEN;	
	
	// Wait for ADC voltage regulator start-up time
	// The software must wait for the startup time of the ADC voltage regulator (T_ADCVREG_STUP) 
	// before launching a calibration or enabling the ADC.
	// T_ADCVREG_STUP = 20 us
	wait_time = 20 * (80000000 / 1000000);
	while(wait_time != 0) {
		wait_time--;
	}   
}

void ADC_Common_Configuration()
{
	// enable I/O analog switch voltage booster
	SYSCFG->CFGR1 |= SYSCFG_CFGR1_BOOSTEN;
	
	// configure the ADC common control register
	ADC123_COMMON->CCR |= ADC_CCR_VREFEN; // enable vref_int
	ADC123_COMMON->CCR &= ~(ADC_CCR_PRESC | ADC_CCR_DUAL | ADC_CCR_CKMODE); // clock not divided, independent mode, reset clock mode
	ADC123_COMMON->CCR |= ADC_CCR_CKMODE_0; // set clock mode to synchronous HCLK/1
}

void ADC_Pin_Init(void)
{	
	// Enable GPIO Port A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// set PA1 to analog, no pull-up/no pull-down
	GPIOA->MODER &= ~GPIO_MODER_MODE1; // clear
	GPIOA->MODER |= GPIO_MODER_MODE1; // set mode to analog (11)
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1; // reset to no pull-up/pull-down (00)
	
	// connect PA1 to ADC input
	GPIOA->ASCR |= GPIO_ASCR_ASC1;
}

void ADC_Init(void)
{
	// Enable & Reset ADC Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN; // enable ADC clock
	RCC->AHB2RSTR |= RCC_AHB2RSTR_ADCRST; // reset the ADC
	RCC->AHB2RSTR &= ~RCC_AHB2RSTR_ADCRST; // clear reset bit
	
	// Other ADC Initialization
	ADC_Pin_Init();
	ADC_Common_Configuration();
	ADC_Wakeup();
	
  // ADC configuration
	ADC1->CR |= ADC_CR_ADDIS; // disable ADC
	ADC1->CFGR &= ~(ADC_CFGR_RES | ADC_CFGR_ALIGN); // 12 bit resolution (00), right alignment (0)
	ADC1->SQR1 &= ~ADC_SQR1_L; // set sequence length to 1
	ADC1->SQR1 &= ~ADC_SQR1_SQ1; // reset
	ADC1->SQR1 |= 0x06; // set first conversion channel to 6
	ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_6; // set channel 6 to single ended mode
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP6; // reset
	ADC1->SMPR1 |= (ADC_SMPR1_SMP6_0 | ADC_SMPR1_SMP6_1); // set channel 6 sample time to 24.5
	ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTEN); // single conversion mode, hardware trigger detection disabled
	
	ADC1->CR |= ADC_CR_ADEN; // enable ADC
	while((ADC1->ISR & ADC_ISR_ADRDY) == 0); // wait for the ready flag
}
