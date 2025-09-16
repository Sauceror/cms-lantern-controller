#include "sensors.h"
#include "adc.h"
#include "modbus.h"

/* 2-sample DMA buffer: IN8 (PB0), IN9 (PB1) */
static uint16_t adc_dma[2] = {0, 0};

/* Calibration/scaling: adjust to your shunt & amp */
#define VREF_mV 3300u
#define ADC_MAX 4095u
#define SHUNT_mOHM 100u /* example 0.1Ω -> 100 mΩ */
#define GAIN_NUM 10u    /* amplifier gain numerator */
#define GAIN_DEN 1u     /* amplifier gain denominator */

static inline uint16_t adc_to_mA(uint16_t adc)
{
  /* V = adc/4095 * 3.3V; I = V / (R * gain); return mA */
  uint32_t VmV = (uint32_t)adc * VREF_mV / ADC_MAX;
  uint32_t mA = (VmV * GAIN_DEN) / (SHUNT_mOHM * GAIN_NUM);
  if (mA > 0xFFFFu)
    mA = 0xFFFFu;
  return (uint16_t)mA;
}

void Sensors_Init(void)
{
  /* Optional: ADC calibration (F0 has simple calibration) */
#ifdef ADC_CR_ADCAL
  HAL_ADCEx_Calibration_Start(&hadc);
#endif
  /* Start ADC in DMA mode: 2 channels circular */
  HAL_ADC_Start_DMA(&hadc, (uint32_t *)adc_dma, 2);
}

void Sensors_Task(void)
{
  /* Convert to mA and publish to Modbus input regs */
  uint16_t i0 = adc_to_mA(adc_dma[0]);
  uint16_t i1 = adc_to_mA(adc_dma[1]);
  Modbus_SetCurrentRegs(i0, i1);
}
