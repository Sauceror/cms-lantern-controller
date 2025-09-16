/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   ADC1 config: 2 channels (PB0=IN8, PB1=IN9) with DMA
  ******************************************************************************
  */
/* USER CODE END Header */

#include "adc.h"

/* USER CODE BEGIN 0 */
ADC_HandleTypeDef  hadc;
DMA_HandleTypeDef  hdma_adc;
/* USER CODE END 0 */

void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* ADC clocking and instance */
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait      = DISABLE;
  hadc.Init.LowPowerAutoPowerOff  = DISABLE;
  hadc.Init.ContinuousConvMode    = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc) != HAL_OK) { Error_Handler(); }

  /* GPIO PB0/PB1 as analog */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin  = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Channel IN8 (PB0) */
  sConfig.Channel      = ADC_CHANNEL_8;
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }

  /* Channel IN9 (PB1) */
  sConfig.Channel = ADC_CHANNEL_9;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) { Error_Handler(); }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
  if (adcHandle->Instance == ADC1)
  {
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA: ADC1 -> memory (circular), halfword alignment for 12-bit ADC */
    hdma_adc.Instance                 = DMA1_Channel1;
    hdma_adc.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode                = DMA_CIRCULAR;
    hdma_adc.Init.Priority            = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK) { Error_Handler(); }
    __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc);

    HAL_NVIC_SetPriority(ADC1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(ADC1_IRQn);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
  if (adcHandle->Instance == ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0 | GPIO_PIN_1);
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
    HAL_NVIC_DisableIRQ(ADC1_IRQn);
  }
}
