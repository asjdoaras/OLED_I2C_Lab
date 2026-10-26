#ifndef __OLED_H
#define __OLED_H

#include "main.h"

/* Scroll descriptor: type=0 -> code=ASCII; type=1 -> han=16x16 glyph pointer */
typedef struct
{
  uint8_t type;
  uint8_t code;
  const uint8_t *han;
} OLED_Glyph;

void OLED_Write_Cmd(uint8_t cmd);
void OLED_Write_Data(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(uint8_t x, uint8_t page);
void OLED_ShowChar(uint8_t x, uint8_t page, uint8_t ch);
void OLED_ShowString(uint8_t x, uint8_t page, const char *str);
void OLED_ShowNum(uint8_t x, uint8_t page, uint32_t num, uint8_t len);
void OLED_ShowChinese(uint8_t x, uint8_t page, const uint8_t *ch);

/* framebuffer layer */
void OLED_FB_Clear(void);
void OLED_FB_ShowString(uint8_t x, uint8_t page, const char *str);
void OLED_FB_DrawChar(uint8_t x, uint8_t page, uint8_t ch);
void OLED_FB_DrawChinese(uint8_t x, uint8_t page, const uint8_t *ch);
void OLED_Flip(void);
uint16_t OLED_ScrollRender(const OLED_Glyph *seq, uint16_t n, uint8_t page, uint16_t offset);

extern const uint8_t HZK_ZHANGYINGQI[3][32];
extern const uint8_t HZK_EXTRA[18][32];

#endif
