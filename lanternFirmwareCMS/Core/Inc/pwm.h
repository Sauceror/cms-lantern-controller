#pragma once
#include <stdint.h>

void PWM_Init(void);
/* Duty 0..255 on both channels */
void PWM_SetBoth(uint8_t left, uint8_t right);
