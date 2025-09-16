#include "pwm.h"
#include "tim.h"

extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

/* Map logical brightness (0..255) to CCR with INVERTED polarity:
   0   -> OFF  (CCR ~ ARR, output high all period, active-low driver = off)
   255 -> FULL (CCR = 0,   output low all period, active-low driver = on)
*/
static inline uint32_t map_brightness_to_ccr(TIM_HandleTypeDef *htim, uint8_t v)
{
  uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim);
  /* scale (255 - v) into [0..arr+1), then clamp to arr */
  uint32_t num = (uint32_t)(255u - v) * (arr + 1u);
  uint32_t ccr = num / 255u;
  if (ccr > arr) ccr = arr;
  return ccr;
}

static inline void set_ccr_both(uint8_t left, uint8_t right)
{
  __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, map_brightness_to_ccr(&htim16, left));
  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, map_brightness_to_ccr(&htim17, right));
}

void PWM_Init(void)
{
  /* Ensure outputs are OFF before enabling PWM to avoid a power-up flash */
  set_ccr_both(0, 0);  /* logical OFF */
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
}

void PWM_SetBoth(uint8_t left, uint8_t right)
{
  set_ccr_both(left, right);
}
