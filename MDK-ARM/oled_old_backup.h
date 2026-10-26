#ifndef __OLED_H
#define __OLED_H
#include "main.h"

void OLED_Write_Cmd(uint8_t cmd);
void OLED_Write_Data(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(uint8_t x, uint8_t page);
void OLED_ShowChinese(uint8_t x,uint8_t page,const uint8_t *ch);
void OLED_Show8x16Num(uint8_t x,uint8_t page,uint8_t num);

#endif
