/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   Prototypes for usart.c
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Handles used across modules */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef  hdma_usart1_rx;
extern DMA_HandleTypeDef  hdma_usart1_tx;

/* Init */
void MX_USART1_UART_Init(void);

/* No RX/IDLE APIs here: Modbus module owns RX DMA + IDLE */

#ifdef __cplusplus
}
#endif
#endif /* __USART_H__ */
