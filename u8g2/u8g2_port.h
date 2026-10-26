#ifndef __U8G2_PORT_H
#define __U8G2_PORT_H

#include "u8g2.h"

/* 硬件 I2C 字节回调（hi2c1） */
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
/* GPIO 与 HAL 延时回调 */
uint8_t u8x8_gpio_and_delay_hal(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif
