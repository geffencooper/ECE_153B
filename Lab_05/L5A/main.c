#include "ADC.h"
#include "LCD.h"
#include "LED.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"

#include <stdio.h>

// converts a float to a string with 3 fraction and 2 integer digits "01.234V"
void voltage_to_string(char* buffer, int size, float num)
{
	// get the integer digits and fraction digits
	int integer_dig = (int)num;
	int fraction_dig = (num - (float)integer_dig)*1000;
	
	// starting index of integer part
	int i = 1;
	
	// convert integer part to string
	while(integer_dig != 0)
	{
		buffer[i] = (integer_dig%10 + 48); // get digit, convert to ASCII
		integer_dig /= 10;
		i--;
		
		// number too long for buffer
		if(i < 0)
		{
			break;
		}
	}
	
	// pad remaining integer spots in buffer with zeros
	for(; i >= 0; i--)
	{
		buffer[i] = 48;
	}
	
	// convert float to digits
	buffer[2] = '.'; // set the decimal point
	
	// starting index of fraction part
	i = size-2;
	
	// convert fraction part to string
	while(fraction_dig != 0)
	{
		buffer[i] = (fraction_dig%10 + 48); // get digit, convert to ASCII
		fraction_dig /= 10;
		i--;
		
		// number too long for buffer
		if(i < 0)
		{
			break;
		}
	}
	// pad remaining integer spots in buffer with zeros
	for(; i > 2; i--)
	{
		buffer[i] = 48;
	}
	buffer[size-1] = 'V';
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
		float voltage = 0;
		char measurement[7];
	
    while (1) 
		{
        // Trigger ADC, wait till complete
				ADC1->CR |= ADC_CR_ADSTART;
				while((ADC1->ISR & ADC_ISR_EOC) == 0); // wait for conversion to be complete
				
			  // get ADC result, convert to voltage
				adc_data = ADC1->DR;
			
				voltage = 3.3*(((float)adc_data)/4096);
				float test = voltage;
			
				voltage_to_string(measurement, sizeof(measurement), voltage);
				LCD_DisplayString((uint8_t*)measurement);
        // [TODO] LED behavior based on ADC result
    }
}
