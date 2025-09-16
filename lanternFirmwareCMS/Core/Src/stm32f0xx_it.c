/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 * Copyright (c) 2025 ST.
 * All rights reserved.
 ******************************************************************************
 */
/* USER CODE END Header */

#include "main.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN Includes */
#include "usart.h"
#include "modbus.h"
/* USER CODE END Includes */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc;
extern ADC_HandleTypeDef hadc;

extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
void NMI_Handler(void)              { while (1) { } }
void HardFault_Handler(void)        { while (1) { } }
void SVC_Handler(void)              { }
void PendSV_Handler(void)           { }
void SysTick_Handler(void)          { HAL_IncTick(); }

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

void DMA1_Channel1_IRQHandler(void)             /* ADC DMA */
{
  HAL_DMA_IRQHandler(&hdma_adc);
}

/* Not using I2C1 DMA: leave empty to avoid undefined references */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* No-op */
}

void DMA1_Channel4_5_IRQHandler(void)          /* USART1 TX/RX DMA */
{
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void ADC1_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&hadc);
}

void TIM16_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim16);
}

void TIM17_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim17);
}

void I2C1_IRQHandler(void) /* Keep if project uses I2C1 without DMA */
{
  /* Stub; fill if you actually use I2C1 */
}

/* USART1 global interrupt: RX IDLE delimit frames for Modbus */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);

  if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    Modbus_UART_IdleISR();           /* hand complete frame to Modbus */
  }
}
