#include "ADC.h"
#include "LCD.h"
#include "LED.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"

#include <stdio.h>

// converts a number to a string, zero pads the remaining spots in buffer
void num_to_string(char* buffer, int size, int num)
{
	// keep track of the sign
	uint8_t is_negative = 0;
	if(num < 0)
	{
		num*=-1; // make it positive so can mod correctly
		is_negative = 1;
	}
	
	// iterate backwards from last index
	int i = size-1;
	
	// convert int to string
	while(num !=0)
	{
		buffer[i] = (num%10 + 48); // get digit, convert to ASCII
		num /= 10;
		i--;
		
		// number too long for buffer
		if(i < 0)
		{
			break;
		}
	}
	
	// pad remaining spots in buffer with zeros
	for(; i >= 0; i--)
	{
		buffer[i] = 48;
	}
	
	// add a negative sign if necessary
	if(is_negative)
	{
		buffer[0] = '-';
	}
}

int main(void) 
{
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz
    ADC_Init();
    // [TODO] Initialize LEDs and PWM

    LCD_Initialization();
    LCD_Clear();

		int adc_data = 0;
		char msg[6];
    while (1) 
		{
        // Trigger ADC and get result
				ADC1->CR |= ADC_CR_ADSTART;
				while((ADC1->ISR & ADC_ISR_EOC) == 0); // wait for conversion to be complete
				adc_data = ADC1->DR;
				num_to_string(msg, sizeof(msg), adc_data);
				LCD_DisplayString((uint8_t*)msg);
        // [TODO] LED behavior based on ADC result
    }
}
