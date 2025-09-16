#include "app.h"
#include "pwm.h"
#include "modbus.h"
#include "main.h"   /* HAL_GetTick */

/* 0..255 duty */
static uint8_t  g_brightness = 128;  /* default mid for immediate visible output */
static uint16_t g_mode       = 5;    /* default ALL ON */
static uint8_t  phase        = 0;
static uint32_t last_toggle  = 0;

/* Apply current mode/brightness and mirror live status (input reg 0) */
static void apply_outputs(void)
{
  /* Hard off if mode==0 or brightness==0 (avoid faint glow on hardware) */
  if (g_mode == 0U || g_brightness == 0U)
  {
    PWM_SetBoth(0, 0);
    Modbus_SetStatus(0U);
    return;
  }

  switch (g_mode)
  {
  default:
  case 0: /* ALL OFF */
    PWM_SetBoth(0, 0);
    break;

  case 1: /* UP/DOWN flashing — for 2 heads treat same as wig-wag (alternate) */
  case 2: /* SIDE/SIDE (alternate) */
  case 3: /* WIG/WAG (alternate) */
    PWM_SetBoth(phase ? g_brightness : 0,
                phase ? 0 : g_brightness);
    break;

  case 4: /* ALL FLASH (together) */
    PWM_SetBoth(phase ? g_brightness : 0,
                phase ? g_brightness : 0);
    break;

  case 5: /* ALL ON */
    PWM_SetBoth(g_brightness, g_brightness);
    break;
  }

  /* Mirror requested/active mode to Input Reg[0] so 0x04 reads show status */
  Modbus_SetStatus(g_mode);
}

void App_Init(void)
{
  PWM_Init();

  /* publish defaults so 0x03/0x04 reads show something sensible */
  Modbus_SetBrightnessReg(g_brightness);  /* holding reg[1] */
  apply_outputs();                        /* sets input reg[0] + PWMs */
}

void App_Task(void)
{
  const uint32_t now = HAL_GetTick();

  /* 500 ms flashing cadence for flashing modes (1/2/3/4) */
  if ((now - last_toggle) >= 500U)
  {
    last_toggle = now;

    if (g_mode == 1U || g_mode == 2U || g_mode == 3U || g_mode == 4U)
    {
      phase ^= 1U;
      apply_outputs();  /* re-apply for flash step */
    }
  }
}

/* Called by Modbus when holding reg 0 (mode) is written */
void App_SetMode(uint16_t mode)
{
  /* Accept spec’s full range 0..5; illegal -> OFF */
  g_mode = (mode <= 5U) ? mode : 0U;

  /* Reset flash phase/timer so behavior is deterministic after change */
  phase = 0;
  last_toggle = HAL_GetTick();

  apply_outputs();  /* immediate effect + live status mirror */
}

/* Called by Modbus when holding reg 1 (brightness) is written (0..255) */
void App_SetBrightness(uint16_t br)
{
  if (br > 255U) br = 255U;
  g_brightness = (uint8_t)br;

  /* keep holding reg[1] in sync for 0x03 reads */
  Modbus_SetBrightnessReg(g_brightness);

  apply_outputs();  /* reflect new brightness right away */
}
