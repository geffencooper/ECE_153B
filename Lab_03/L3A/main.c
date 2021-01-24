/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 3A
 */

#include "stm32l476xx.h"
 
void PWM_Init() 
{
	// Enable GPIO Port E Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	// Enable TIM1 Clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	// Configure PE8
	// set mode to alternative function
	GPIOE->MODER &= ~GPIO_MODER_MODE8; // reset
	GPIOE->MODER |= ~GPIO_MODER_MODE8_0; // set to 01 = alternative function
	
	// set output speed to very high
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8; // reset
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED8; // set to 11 = very high speed
	
	// set PUPD to no pull up no pull down
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR8; // reset, no pull up, pull down = 00
	
	// configure the alternative function for GPIO PE8 to TIM1_CH1N
	// TIM1_CH1N=AF1, pin8 is in the upper half of the registers
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL8;  // reset pin 8 selection
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_0; // set bit 0 to select AF1 --> 0001 
	
	// Configure PWM Output for TIM1 CH 1N
	// set the direction to count up
	TIM1->CR1 &= ~TIM_CR1_DIR; // dir = counts up, reset bit to 0
	
	// set the prescaler (sysclock is 4MHz, prescaler scales this to desired tick period)
	TIM1->PSC &= ~TIM_PSC_PSC; // reset the prescaler
	TIM1->PSC = 3; // make it 1MHz --> 4Mhz/(3+1)
	
	// set the autoreload register (counter max value, defines timer frequency --> sawtooth wave)
	TIM1->ARR &= ~TIM_ARR_ARR; // reset the auto reload register
	TIM1->ARR = 999; // counting period = (1+999) * 1/1MHz --> 1ms
	
	// use the timer in output compare mode (output compare 1)
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M; // clear the output compare bits
	TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // set to PWM mode 1 by setting bits 1,2 (0110)
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // enable output preload
	
	// set the output polarity for CH1N
	TIM1->CCER &= ~TIM_CCER_CC1NP; // 0 = output polarity is active high

	// enable the CH1N output
	TIM1->CCER |= TIM_CCER_CC1NE;
	
	// enable the main output
	TIM1->BDTR |= TIM_BDTR_MOE;
	
	// set the CCR, capture compare value to 1/2 the arr+1 for 50% duty cycle
	TIM1->CCR1 &= ~TIM_CCR1_CCR1; // reset
	TIM1->CCR1 = (TIM1->ARR+1)/2;
	
	// enable the counter for timer 1
	TIM1->CR1 |= TIM_CR1_CEN;
}

// pass in value 0-100
void set_duty_cycle(uint8_t dc)
{
	// DC = CCR/(ARR+1) --> CCR = DC*(ARR+1)
	TIM1->CCR1 &= ~TIM_CCR1_CCR1;  // reset CCR
	TIM1->CCR1 = ((TIM1->ARR+1)*dc)/100; // set CCR so that get desired DC
}

int main() 
{
	// Initialization - We will use the default 4 MHz clock
	PWM_Init();
	
	// Periodic Dimming
	int i;
	int j;
	int k;
	while(1) 
	{
		// glowing gradually
		j = 0;
		k = 0;
		while(j <= 100)
		{
			set_duty_cycle(j);
			for(i=0; i<75000; ++i); // Some Delay
			k++;
			j+=k;
		}
		
		// fading gradually
		while(j > 0)
		{
			set_duty_cycle(j);
			for(i=0; i<75000; ++i); // Some Delay
			j-=k;
			k--;
		}
	}
	
	return 0;
}
