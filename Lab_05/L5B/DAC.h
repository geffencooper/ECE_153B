#ifndef __STM32L476G_DISCOVERY_DAC_H
#define __STM32L476G_DISCOVERY_DAC_H

#include <stdint.h>

void DAC_Init(void);

void DAC_Write_Value(uint32_t value);

#endif /* __STM32L476G_DISCOVERY_DAC_H */
