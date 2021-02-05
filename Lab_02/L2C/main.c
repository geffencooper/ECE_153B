/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 2C
 */
 
#include "stm32l476xx.h"

#include "LCD.h"
#include "LED.h"
#include "RTC.h"
#include "SysClock.h"

// Helper macro to convert a value from 2 digit decimal format to BCD format
// __VALUE__ Byte to be converted
#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))
#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))


char strTime[12] = {0};
char strDate[12] = {0};

void RTC_Set_Alarm(void) 
{
	// disable both alarms
	RTC->CR &= ~RTC_CR_ALRAE;
	RTC->CR &= ~RTC_CR_ALRBE;
	
	// unlock so can write to RTC registers
	RTC_Disable_Write_Protection();
	
	// disable alarm interrupts
	RTC->CR &= ~RTC_CR_ALRAE;
	RTC->CR &= ~RTC_CR_ALRBE;
	RTC->CR &= ~RTC_CR_ALRAIE;
	RTC->CR &= ~RTC_CR_ALRBIE;
	
	// wait for alarm write flag to be ready
	while((RTC->ISR & RTC_ISR_ALRAWF) == 0);
	while((RTC->ISR & RTC_ISR_ALRBWF) == 0);
	
	// initialize alarm A to trigger on 30 seconds
	                // set the tens value                set the unit value
	RTC->ALRMAR |= ((__RTC_CONVERT_BIN2BCD(1) << 4) + (__RTC_CONVERT_BIN2BCD(0) << 0));
	
	// initialize alarm B to trigger every 1 second
	               // set the tens value                set the unit value
	RTC->ALRMBR |= ((__RTC_CONVERT_BIN2BCD(0) << 4) + (__RTC_CONVERT_BIN2BCD(1) << 0));

	// to trigger when seconds field is 30, reset mask 1 so seconds comparison is used
	RTC->ALRMAR &= ~RTC_ALRMAR_MSK1;
	RTC->ALRMAR |= RTC_ALRMAR_MSK2;
	RTC->ALRMAR |= RTC_ALRMAR_MSK3;
	RTC->ALRMAR |= RTC_ALRMAR_MSK4;

	// to trigger the alarm every second, set all to don't cares because the value doesn't matter
	RTC->ALRMBR |= RTC_ALRMAR_MSK1;
	RTC->ALRMBR |= RTC_ALRMAR_MSK2;
	RTC->ALRMBR |= RTC_ALRMAR_MSK3;
	RTC->ALRMBR |= RTC_ALRMAR_MSK4;

	// enable the alarms and their interrupts
	RTC->CR |= RTC_CR_ALRAE;
	RTC->CR |= RTC_CR_ALRBE;
	RTC->CR |= RTC_CR_ALRAIE;
	RTC->CR |= RTC_CR_ALRBIE;
	
	// lock the RTC registers
	RTC_Enable_Write_Protection();
}

void RTC_Alarm_Enable(void) 
{
	// configure interrupt to trigger on rising edge for exti line 18
	EXTI->RTSR1 |= EXTI_RTSR1_RT18;
	
	// set the interrupt and event mask
	EXTI->IMR1 |= EXTI_IMR1_IM18;
	EXTI->EMR1 |= EXTI_EMR1_EM18;
	
	// clear the pending interrupt
	EXTI->PR1 |= EXTI_PR1_PIF18;
	
	// enable the interrupt
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
	// set the priority of the interrupt
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);
}

void RTC_Alarm_IRQHandler(void) 
{
	// clear interrupt pending flags
	EXTI->PR1 |= EXTI_PR1_PIF18;
	
	// toggle red LED if alarm A triggered
	if(RTC->ISR & RTC_ISR_ALRAF)
	{
		// clear event flag
		RTC->ISR &= ~RTC_ISR_ALRAF;
		
		// toggle the red LED
		Red_LED_Toggle();
	}
	
	// toggle green LED if alarm B triggered
	if(RTC->ISR & RTC_ISR_ALRBF)
	{
		// clear the alarm B event flag
		RTC->ISR &= ~RTC_ISR_ALRBF;
		
		// toggle the green LED
		Green_LED_Toggle();
	}
}

int main(void) 
{	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	LED_Init();
	LCD_Initialization();
	
	RTC_Init();
	RTC_Alarm_Enable();
	RTC_Set_Alarm();
	
	while(1) 
	{
		Get_RTC_Calendar(strTime, strDate);
		LCD_DisplayString(strTime);
	}
}
