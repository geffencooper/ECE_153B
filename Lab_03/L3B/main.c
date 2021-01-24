/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 3A
 */
 
#include <stdio.h> 
 
#include "stm32l476xx.h"
#include "lcd.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;

// PB6
void Input_Capture_Setup() 
{
	// Enable GPIO Port B Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// Configure PB6
	// set mode to alternative function
	GPIOB->MODER &= ~GPIO_MODER_MODE6; // reset
	GPIOB->MODER |= ~GPIO_MODER_MODE6_0; // set to 01 = alternative function

	// configure the alternative function for GPIO PB6 to TIM4_CH1
	// TIM4_CH1=AF2, pin6 is in the lower half of the registers
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;  // reset pin 8 selection
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1; // set bit 1 to select AF2 --> 0010 

	// set PUPD to no pull up no pull down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6; // reset, no pull up, pull down = 00

	// Enable TIM4 Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	// set the prescaler (sysclock is 16MHz, prescaler scales this to desired tick period)
	TIM4->PSC &= ~TIM_PSC_PSC; // reset the prescaler
	TIM4->PSC = 15; // make it 16Mhz/(15+1) = 1MHz
	
	// enable auto-reload preload
	TIM4->CR1 |= TIM_CR1_ARPE;
	
	// set the autoreload register to max value
	TIM4->ARR = TIM_ARR_ARR;
	
	// configure capture compare mode register to map input capture 1 to timer input 1
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S; // reset
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; // set bit one so IC1 --> TI1
	
	// capture rising and falling edges, set CC1NP and CC1P
	TIM4->CCER |= (TIM_CCER_CC1NP | TIM_CCER_CC1P);
	
	// enable capturing
	TIM4->CCER |= TIM_CCER_CC1E;
	
	// enable interrupt and DMA requests and update interrupt enable
	TIM4->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UDE | TIM_DIER_UIE);

	// enable update generation in event generation register
	TIM4->EGR |= TIM_EGR_UG;
	
	// clear update interrupt flag
	TIM4->SR &= ~TIM_SR_UIF;
	
	// set the direction to count up
	TIM4->CR1 &= ~TIM_CR1_DIR; // dir = counts up, reset bit to 0

	// enable the counter for timer 4
	TIM4->CR1 |= TIM_CR1_CEN;
	
	// Configure and Enable in NVIC
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, 2);
}

void TIM4_IRQHandler(void) 
{
	// check if event was overflow
	if(TIM4->SR & TIM_SR_UIF)
	{
		// clear update interrupt flag
		TIM4->SR &= ~TIM_SR_UIF;
		
		// if overflow happened after first capture, store pre-rollover time
		if(currentValue)
		{
			overflowCount = 0xFFFF - currentValue;
		}
	}
		
	// check if input capture flag triggered
	if(TIM4->SR & TIM_SR_CC1IF)
	{
		// check if second consecutive interrupt (current already set)
		if(currentValue)
		{
			// move the first value, store the second value
			lastValue = currentValue;
			currentValue = TIM4->CCR1;
			
			// if an overflow happened between triggers, add on pre-rollover time to current time
			if(overflowCount)
			{
				timeInterval = overflowCount + currentValue;
			}
			// otherwise, just subtract the value
			else
			{
				timeInterval = currentValue - lastValue;
			}
			
			// reset the values
			currentValue = 0;
			lastValue = 0;
			overflowCount = 0;
		}
		// first trigger
		else
		{
			currentValue = TIM4->CCR1;
		}
	}
}

// PE11
void Trigger_Setup() 
{
	// Enable GPIO Port E Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	// Configure PE11
	// set mode to alternative function
	GPIOE->MODER &= ~GPIO_MODER_MODE11; // reset
	GPIOE->MODER |= ~GPIO_MODER_MODE11_0; // set to 01 = alternative function

	// configure the alternative function for GPIO PE11 to TIM1_CH2
	// TIM1_CH2=AF1, pin11 is in the upper half of the registers
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL11;  // reset pin 8 selection
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL11_0; // set bit 0 to select AF1 --> 0001 

	// set PUPD to no pull up no pull down
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR11; // reset, no pull up, pull down = 00

	// set the output to push pull
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT11; // bit = 0 --> push pull
	
	// set output speed to very high
	GPIOE->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED11; // reset
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED11; // set to 11 = very high speed

	// Enable TIM1 Clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	// set the prescaler (sysclock is 16MHz, prescaler scales this to desired tick period)
	TIM1->PSC &= ~TIM_PSC_PSC; // reset the prescaler
	TIM1->PSC = 15; // make it 16Mhz/(15+1) = 1MHz
	
	// enable auto-reload preload
	TIM1->CR1 |= TIM_CR1_ARPE;
	
	// set the autoreload register to max value
	TIM1->ARR = TIM_ARR_ARR;
	
	// high pulse of 10us (PWM mode 1 so high when count < CCR)
	// PW = (CCR-1)*(clock period)
	// CCR = PW/(clock period) + 1 = 10us/(1/10MHz) + 1 = 101
	TIM1->CCR1 &= ~TIM_CCR1_CCR1; // reset CCR
	TIM1->CCR1 = 101;
	
	// use the timer in output compare mode (output compare 1)
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M; // clear the output compare bits
	TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // set to PWM mode 1 by setting bits 1,2 (0110)
  TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // enable output preload

	// enable the output
	TIM1->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	// set main output enable, set off-state selection for run mode
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->BDTR |= TIM_BDTR_OSSR;
	
	// enable update generation in event generation register
	TIM1->EGR |= TIM_EGR_UG;
	
	// enable update interrupt
	TIM1->DIER |= TIM_DIER_UIE;
	
	// clear update interrupt flag
	TIM1->SR &= ~TIM_SR_UIF;
	
	// set the direction to count up
	TIM1->CR1 &= ~TIM_CR1_DIR; // dir = counts up, reset bit to 0

	// enable the counter for timer 1
	TIM1->CR1 |= TIM_CR1_CEN;
}

int main(void) 
{	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
  
	// Input Capture Setup
	Input_Capture_Setup();
	
	// Trigger Setup
	Trigger_Setup();

	// Setup LCD
	LCD_Initialization();
	LCD_Clear();
	
	char message[6];
	while(1) 
  {
		// timer clock is 1 MHz --> 1us fidelity
		// d = PW(us)/58 cm
		
		// if the time interval is 38ms then object is out of range
		if(timeInterval/1000 == 38)
		{
			for(int i = 0; i < 6; i++)
			{
				message[i] = '-';
			}
		}
		// otherwise, find the distance, convert to a string
		else
		{
			int cm = timeInterval / 58;
			int i = 0;
			// convert int to string
			while(cm !=0)
			{
				message[i] = (cm%10 + 48);
				cm /= 10;
				i++;
			}
			// reverse the string because it will copy in backwards
			for(int j = 0; j < i/2; j++)
			{
					char tmp = message[j];
					message[j] = message[i-j-1];
					message[i-j-1] = tmp;
			}
			// set the null terminator of the string
			message[i] = '\0';
		}
		
		LCD_DisplayString((uint8_t *) message);
	}
}
