#pragma once
#include <stdint.h>

void App_Init(void);
void App_Task(void);

/* Called by Modbus write handler */
void App_SetMode(uint16_t mode);       /* valid: 0,2,4,5 for 2-flasher */
void App_SetBrightness(uint16_t br);   /* 0..255; will clamp */
