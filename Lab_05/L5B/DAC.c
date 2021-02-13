#include "stm32l476xx.h"

static void DAC_Pin_Init(void) {
    // [TODO]
}

void DAC_Write_Value(uint32_t value) {
    // [TODO] Write DAC1Ch2 right-aligned 12-bit value
}

void DAC_Init(void) {
    DAC_Pin_Init();

    // [TODO] Enable DAC clock

    // [TODO] Enable software trigger mode

    // [TODO] Disable trigger

    // [TODO] DAC1 connected in normal mode to external pin only with buffer enabled

    // [TODO] Enable DAC channel 2

    DAC_Write_Value(0);
}
