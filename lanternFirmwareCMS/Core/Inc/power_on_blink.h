// poweron_blink.h
#pragma once
#include <stdint.h>

void PowerOn_Blink(uint8_t times,
                   uint16_t on_ms,
                   uint16_t off_ms,
                   uint16_t final_mode,
                   uint16_t final_brightness);

void PowerOn_Blink_Default(void);
