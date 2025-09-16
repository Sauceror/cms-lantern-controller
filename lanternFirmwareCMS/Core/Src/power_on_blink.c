// power_on_blink.c
#include "stm32f0xx_hal.h"
#include <stdint.h>
#include "pwm.h" // PWM_Init, PWM_SetBoth (TIM16/TIM17)
#include "app.h" // App_SetMode, App_SetBrightness

/* Clamp helper */
static inline uint8_t clamp_u8(uint16_t v) { return (v > 255U) ? 255U : (uint8_t)v; }

/**
 * Blink both lanterns at power-on using the raw PWM layer (TIM16/TIM17),
 * then restore the requested final mode/brightness via the app API.
 *
 * @param times            number of blinks (e.g., 3)
 * @param on_ms            on-time per blink in milliseconds (e.g., 180)
 * @param off_ms           off-time per blink in milliseconds (e.g., 180)
 * @param final_mode       0=ALL OFF, 2=SIDE/SIDE, 4=ALL FLASH, 5=ALL ON
 * @param final_brightness 0..255 (logical brightness)
 */
void PowerOn_Blink(uint8_t times,
                   uint16_t on_ms,
                   uint16_t off_ms,
                   uint16_t final_mode,
                   uint16_t final_brightness)
{
    /* 1) Bring up PWM on TIM16/TIM17 */
    PWM_Init();

    /* 2) Start from a known OFF state */
    PWM_SetBoth(0, 0); // logical OFF on both sides
    HAL_Delay(60);

    /* 3) Make the blink obvious (full brightness), but bypass app state machine */
    for (uint8_t i = 0; i < times; i++)
    {
        PWM_SetBoth(255, 255); // both ON (max)
        HAL_Delay(on_ms);

        PWM_SetBoth(0, 0); // both OFF
        HAL_Delay(off_ms);
    }

    /* 4) Restore requested post-boot state via app API */
    uint8_t fb = clamp_u8(final_brightness);
    App_SetBrightness(fb);

    /* Only valid modes should be commanded; if something else is passed, fall back to OFF */
    switch (final_mode)
    {
    case 0:
    case 2:
    case 4:
    case 5:
        App_SetMode(final_mode);
        break;
    default:
        App_SetMode(0); // safe fallback
        break;
    }
}

/* Default: 3 blinks, 180 ms on/off, end in ALL OFF @ 128 */
void PowerOn_Blink_Default(void)
{
    PowerOn_Blink(/*times*/ 3, /*on_ms*/ 180, /*off_ms*/ 180,
                  /*final_mode*/ 0, /*final_brightness*/ 128);
}
