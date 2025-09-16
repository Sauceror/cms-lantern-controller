#pragma once
#include "main.h"

/* Default Modbus address */
#ifndef MB_ADDR_DEFAULT
#define MB_ADDR_DEFAULT 0xA2 /* 162 */
#endif

/* Init + ISR entry point (called from USART1_IRQHandler on IDLE) */
void Modbus_Init(void);
void Modbus_UART_IdleISR(void);

/* App/sensors setters -> implemented in modbus.c */
void Modbus_SetStatus(uint16_t st);                     /* Input reg[0] */
void Modbus_SetBrightnessReg(uint16_t b);               /* Holding reg[1] */
void Modbus_SetCurrentRegs(uint16_t mA0, uint16_t mA1); /* Input reg[1..4] */
