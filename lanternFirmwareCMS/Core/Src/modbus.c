#include "modbus.h"
#include "stm32f0xx_hal.h"
#include <string.h>

/* USART1 + DMA handles (from usart.c) */
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

/* App hooks */
void App_SetMode(uint16_t mode);
void App_SetBrightness(uint16_t br);

/* Default slave address */
#ifndef MB_ADDR_DEFAULT
#define MB_ADDR_DEFAULT 0xA2
#endif

/* Registers */
static uint16_t reg_input[5] = {0};   /* [0]=status, [1..4]=currents */
static uint16_t reg_holding[2] = {0}; /* [0]=mode,   [1]=brightness */

/* Buffers */
#define MB_RX_BUFSZ 256
static uint8_t rxbuf[MB_RX_BUFSZ];
static uint8_t txbuf[3 + 2 * 125 + 2];

static uint8_t mb_addr = MB_ADDR_DEFAULT;

/* CRC16/MODBUS */
static uint16_t crc16(const uint8_t *data, uint16_t len)
{
  uint16_t crc = 0xFFFF;
  for (uint16_t i = 0; i < len; i++)
  {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++)
    {
      uint16_t lsb = crc & 1u;
      crc >>= 1;
      if (lsb)
        crc ^= 0xA001;
    }
  }
  return crc;
}

/* RX control (DMA + IDLE) */
static void start_rx(void)
{
  HAL_UART_Receive_DMA(&huart1, rxbuf, MB_RX_BUFSZ);
  __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

/* Called from USART1_IRQHandler on IDLE */
void Modbus_UART_IdleISR(void)
{
  uint16_t n = (uint16_t)(MB_RX_BUFSZ - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx));
  HAL_UART_DMAStop(&huart1);

  if (n >= 4U)
  {
    uint8_t addr = rxbuf[0];
    uint8_t fcode = rxbuf[1];
    uint16_t rxcrc = (uint16_t)rxbuf[n - 2] | ((uint16_t)rxbuf[n - 1] << 8);
    uint16_t calc = crc16(rxbuf, n - 2);

    if (addr == mb_addr && rxcrc == calc)
    {
      /* ---------------- READ HOLDING/INPUT REGISTERS ---------------- */
      if ((fcode == 0x03U || fcode == 0x04U) && n >= 8U)
      {
        uint16_t start = ((uint16_t)rxbuf[2] << 8) | rxbuf[3];
        uint16_t qty = ((uint16_t)rxbuf[4] << 8) | rxbuf[5];

        const uint16_t *src = (fcode == 0x03U) ? reg_holding : reg_input;
        uint16_t maxregs = (fcode == 0x03U) ? 2U : 5U;

        if (qty >= 1U && (start + qty) <= maxregs)
        {
          uint16_t idx = 0;
          txbuf[idx++] = mb_addr;
          txbuf[idx++] = fcode;
          txbuf[idx++] = (uint8_t)(qty * 2U);

          for (uint16_t i = 0; i < qty; i++)
          {
            uint16_t v = src[start + i];
            txbuf[idx++] = (uint8_t)(v >> 8);
            txbuf[idx++] = (uint8_t)(v & 0xFF);
          }

          uint16_t c = crc16(txbuf, idx);
          txbuf[idx++] = (uint8_t)(c & 0xFF);
          txbuf[idx++] = (uint8_t)(c >> 8);
          HAL_UART_Transmit_DMA(&huart1, txbuf, idx);
        }
        else
        {
          uint8_t ex[5] = {mb_addr, (uint8_t)(fcode | 0x80U), 0x02U, 0U, 0U};
          uint16_t c = crc16(ex, 3);
          ex[3] = (uint8_t)(c & 0xFF);
          ex[4] = (uint8_t)(c >> 8);
          HAL_UART_Transmit_DMA(&huart1, ex, 5);
        }
      }
      /* ---------------- WRITE SINGLE REGISTER ---------------- */
      else if (fcode == 0x06U && n >= 8U)
      {
        uint16_t reg = ((uint16_t)rxbuf[2] << 8) | rxbuf[3];
        uint16_t val = ((uint16_t)rxbuf[4] << 8) | rxbuf[5];
        uint8_t ex = 0x00U;

        switch (reg)
        {
        case 0x0000: /* MODE: valid on 2-flasher = {0,2,4,5} */
        {
          uint8_t valid = (val == 0U) || (val == 2U) || (val == 4U) || (val == 5U);
          if (valid)
          {
            reg_holding[0] = val;
            App_SetMode(val);
            reg_input[0] = val; /* mirror live status for 0x04 reads */
          }
          else
          {
            ex = 0x03U; /* Illegal Data Value */
          }
        }
        break;

        case 0x0001: /* BRIGHTNESS: 0..255 */
          if (val <= 255U)
          {
            reg_holding[1] = val;
            App_SetBrightness(val);
          }
          else
          {
            ex = 0x03U; /* Illegal Data Value */
          }
          break;

        default:
          ex = 0x02U; /* Illegal Data Address */
          break;
        }

        if (ex == 0x00U)
        {
          /* normal echo */
          uint16_t c = crc16(rxbuf, 6);
          txbuf[0] = mb_addr;
          txbuf[1] = 0x06U;
          txbuf[2] = rxbuf[2];
          txbuf[3] = rxbuf[3];
          txbuf[4] = rxbuf[4];
          txbuf[5] = rxbuf[5];
          txbuf[6] = (uint8_t)(c & 0xFF);
          txbuf[7] = (uint8_t)(c >> 8);
          HAL_UART_Transmit_DMA(&huart1, txbuf, 8);
        }
        else
        {
          /* exception reply */
          uint8_t e[5] = {mb_addr, (uint8_t)(0x06U | 0x80U), ex, 0U, 0U};
          uint16_t c = crc16(e, 3);
          e[3] = (uint8_t)(c & 0xFF);
          e[4] = (uint8_t)(c >> 8);
          HAL_UART_Transmit_DMA(&huart1, e, 5);
        }
      }

      /* ---------------- ILLEGAL FUNCTION ---------------- */
      else
      {
        uint8_t ex[5] = {mb_addr, (uint8_t)(fcode | 0x80U), 0x01U, 0U, 0U};
        uint16_t c = crc16(ex, 3);
        ex[3] = (uint8_t)(c & 0xFF);
        ex[4] = (uint8_t)(c >> 8);
        HAL_UART_Transmit_DMA(&huart1, ex, 5);
      }
    }
  }

  start_rx(); /* re-arm */
}

/* Public init */
void Modbus_Init(void)
{
  mb_addr = MB_ADDR_DEFAULT;

  memset(reg_input, 0, sizeof(reg_input));
  memset(reg_holding, 0, sizeof(reg_holding));

  start_rx();
}

/* Helpers called by app/sensors */
void Modbus_SetStatus(uint16_t st) { reg_input[0] = st; }
void Modbus_SetBrightnessReg(uint16_t b) { reg_holding[1] = (b > 255U) ? 255U : b; }
void Modbus_SetCurrentRegs(uint16_t mA0, uint16_t mA1)
{
  reg_input[1] = mA0;
  reg_input[2] = mA1;
  reg_input[3] = 0;
  reg_input[4] = 0;
}
