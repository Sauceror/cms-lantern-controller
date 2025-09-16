/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f0xx_it.h
 * @brief   Interrupt handler prototypes.
 ******************************************************************************
 */
/* USER CODE END Header */
#ifndef __STM32F0xx_IT_H
#define __STM32F0xx_IT_H

#ifdef __cplusplus
extern "C"
{
#endif

  /* Cortex-M0 */
  void NMI_Handler(void);
  void HardFault_Handler(void);
  void SVC_Handler(void);
  void PendSV_Handler(void);
  void SysTick_Handler(void);

  /* DMA/ADC/UART/TIM/I2C used in this project */
  void DMA1_Channel1_IRQHandler(void);
  void DMA1_Channel2_3_IRQHandler(void); /* no-op (I2C1 DMA not used) */
  void DMA1_Channel4_5_IRQHandler(void);
  void ADC1_IRQHandler(void);
  void TIM16_IRQHandler(void);
  void TIM17_IRQHandler(void);
  void I2C1_IRQHandler(void);
  void USART1_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0xx_IT_H */
