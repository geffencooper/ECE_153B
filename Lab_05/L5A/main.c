#include "ADC.h"
#include "LCD.h"
#include "LED.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"

#include <stdio.h>

int main(void) {
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();
    // [TODO] Initialize LEDs and PWM

    LCD_Initialization();
    LCD_Clear();

    while (1) {
        // [TODO] Trigger ADC and get result

        // [TODO] LED behavior based on ADC result
    }
}
