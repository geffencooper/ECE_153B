#ifndef __STM32L476G_DISCOVERY_LED_H
#define __STM32L476G_DISCOVERY_LED_H

#include "stm32l476xx.h"

// call all LED init functions
void LED_init();

// initialize the red LED to normal output
void red_LED_init();

// initialize the green LED to alternative output
void green_LED_init();

// toggle the red LED when pass 2V
void red_LED_on();
void red_LED_off();

#endif
