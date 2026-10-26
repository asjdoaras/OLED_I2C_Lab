/**
 ******************************************************************************
 * @file    u8g2_port.c
 * @brief   U8G2 HAL 移植层：硬件 I2C(hi2c1) 字节回调 + GPIO/延时回调
 *          适配 STM32F103C8 + SSD1306 128x64 @ I2C1(PB6=SCL, PB7=SDA)
 ******************************************************************************
 */
#include "u8g2.h"
#include "i2c.h"
#include "main.h"

/* 自动探测到的 OLED 从机地址：0=未探测，0x78 失败自动回退 0x7A */
static uint8_t i2c_addr = 0;

static uint8_t port_transmit(uint8_t *data, uint16_t len)
{
  if (i2c_addr == 0)
  {
    if (HAL_I2C_Master_Transmit(&hi2c1, 0x78, data, len, 100) == HAL_OK)
      i2c_addr = 0x78;
    else if (HAL_I2C_Master_Transmit(&hi2c1, 0x7A, data, len, 100) == HAL_OK)
      i2c_addr = 0x7A;
    else
      return 0;
  }
  else
  {
    if (HAL_I2C_Master_Transmit(&hi2c1, i2c_addr, data, len, 100) != HAL_OK)
      return 0;
  }
  return 1;
}

/*---------------- 硬件 I2C 字节回调（SEND 立即独立发送，标准 HAL 写法） ----------------*/
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t *data;
  switch (msg)
  {
    case U8X8_MSG_BYTE_INIT:
      /* hi2c1 已在 MX_I2C1_Init 完成初始化，这里空操作 */
      break;

    case U8X8_MSG_BYTE_START_TRANSFER:
      break;

    case U8X8_MSG_BYTE_SEND:
      data = (uint8_t *)arg_ptr;
      port_transmit(data, arg_int);
      break;

    case U8X8_MSG_BYTE_END_TRANSFER:
      break;

    case U8X8_MSG_BYTE_SET_DC:
      break;

    default:
      return 0;
  }
  return 1;
}

/*---------------- GPIO 与延时回调 ----------------*/
uint8_t u8x8_gpio_and_delay_hal(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      break;

    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);
      break;

    case U8X8_MSG_DELAY_10MICRO:
    {
      /* 粗略忙等 10us @72MHz */
      volatile uint32_t n = 72;
      while (n--) { __NOP(); }
      break;
    }

    case U8X8_MSG_DELAY_NANO:
    case U8X8_MSG_DELAY_I2C:
    case U8X8_MSG_DELAY_100NANO:
      break;

    default:
      return 0;
  }
  return 1;
}
