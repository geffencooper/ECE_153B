#include "EXTI.h"

#include "DAC.h"

void EXTI_Init(void) {
    // [TODO] Configure EXTI for joystick UP and CENTER
}

#define DAC_MIN 0
#define DAC_MAX 4095
#define DAC_INCREMENT 256

static uint32_t dac_value = 0;

// [TODO] Interrupt handlers
