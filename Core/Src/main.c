/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    main.c
 * @brief   任务3：U8G2 移植到 STM32F103C8 + SSD1306 128x64 I2C
 *          轮播：U8G2 Demo 图形 / 学号姓名 / 长文本水平滚动 / 动态移动圆
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "u8g2.h"
#include "u8g2_port.h"
#include "oled.h"   /* HZK_ZHANGYINGQI 16x16 汉字字模(纵向列格式) */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);

/* USER CODE BEGIN PV */
static u8g2_t u8g2;

/* 轮播模式 */
enum { MODE_DEMO = 0, MODE_ID, MODE_SCROLL, MODE_ANIM, MODE_COUNT };
static uint8_t  mode = MODE_DEMO;
static uint16_t mode_frame = 0;

static int16_t  scroll_x = 128;
static uint16_t anim_phase = 0;

/* 16x16 纵向列字模 -> u8g2 横向行位图(2字节/行, MSB 在左) */
static uint8_t han_bmp[3][32];
static void conv_han16(const uint8_t *src, uint8_t *dst)
{
  int row, col;
  for (row = 0; row < 16; row++)
  {
    int bit = row & 7;
    int base = (row < 8) ? 0 : 16;
    uint8_t left = 0, right = 0;
    for (col = 0; col < 8; col++)
      if (src[base + col] & (1 << bit)) left  |= (uint8_t)(0x80 >> col);
    for (col = 8; col < 16; col++)
      if (src[base + col] & (1 << bit)) right |= (uint8_t)(0x80 >> (col - 8));
    dst[row * 2]     = left;
    dst[row * 2 + 1] = right;
  }
}

static const char SCROLL_TEXT[] =
  "Hello, Welcome to Chongqing Jiaotong University IoT Lab 205!  ";

/* 绘制当前模式内容（firstPage/nextPage 分页内每页都会调用） */
static void draw_frame(void)
{
  switch (mode)
  {
    case MODE_DEMO:
    {
      u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
      u8g2_DrawFrame(&u8g2, 0, 0, 128, 63);
      u8g2_DrawStr(&u8g2, 22, 10, "U8G2 DEMO");
      u8g2_DrawCircle(&u8g2, 34, 38, 16, U8G2_DRAW_ALL);
      u8g2_DrawDisc(&u8g2, 94, 38, 14, U8G2_DRAW_ALL);
      u8g2_DrawHLine(&u8g2, 8, 58, 112);
      u8g2_DrawLine(&u8g2, 8, 24, 120, 24);
      break;
    }
    case MODE_ID:
    {
      u8g2_SetFont(&u8g2, u8g2_font_9x18_tr);
      u8g2_DrawStr(&u8g2, 8, 12, "632407030528");
      u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
      u8g2_DrawStr(&u8g2, 8, 34, "Name:");
      u8g2_DrawXBMP(&u8g2, 44, 28, 16, 16, han_bmp[0]);
      u8g2_DrawXBMP(&u8g2, 66, 28, 16, 16, han_bmp[1]);
      u8g2_DrawXBMP(&u8g2, 88, 28, 16, 16, han_bmp[2]);
      u8g2_DrawHLine(&u8g2, 8, 54, 112);
      break;
    }
    case MODE_SCROLL:
    {
      u8g2_SetFont(&u8g2, u8g2_font_6x12_tr);
      u8g2_DrawStr(&u8g2, scroll_x, 28, SCROLL_TEXT);
      break;
    }
    case MODE_ANIM:
    {
      /* 填充圆水平往复移动，带拖尾小圆 */
      int16_t cx = 16 + (anim_phase % 200);
      if (cx > 112) cx = 112 - (cx - 112);
      u8g2_DrawStr(&u8g2, 30, 6, "MOVING BALL");
      u8g2_DrawDisc(&u8g2, cx, 38, 10, U8G2_DRAW_ALL);
      u8g2_DrawCircle(&u8g2, cx - 18, 38, 3, U8G2_DRAW_ALL);
      u8g2_DrawCircle(&u8g2, cx + 18, 38, 3, U8G2_DRAW_ALL);
      u8g2_DrawHLine(&u8g2, 4, 56, 120);
      break;
    }
  }
}

/* 每帧结束后推进动画/切换状态 */
static void step_state(void)
{
  mode_frame++;
  switch (mode)
  {
    case MODE_SCROLL:
      scroll_x -= 2;
      if (scroll_x < -(int16_t)u8g2_GetStrWidth(&u8g2, SCROLL_TEXT))
      {
        mode = MODE_ANIM; mode_frame = 0; scroll_x = 128;
      }
      break;
    case MODE_ANIM:
      anim_phase += 4;
      if (mode_frame >= 150) { mode = MODE_DEMO; mode_frame = 0; anim_phase = 0; }
      break;
    case MODE_DEMO:
      if (mode_frame >= 120) { mode = MODE_ID; mode_frame = 0; }
      break;
    case MODE_ID:
      if (mode_frame >= 150) { mode = MODE_SCROLL; mode_frame = 0; scroll_x = 128; }
      break;
  }
}
/* USER CODE END PV */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* 汉字字模转换为 u8g2 位图 */
  conv_han16(HZK_ZHANGYINGQI[0], han_bmp[0]);
  conv_han16(HZK_ZHANGYINGQI[1], han_bmp[1]);
  conv_han16(HZK_ZHANGYINGQI[2], han_bmp[2]);

  /* U8G2 初始化：SSD1306 128x64, 硬件I2C字节回调 + HAL延时回调 */
  u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0,
                                     u8x8_byte_hw_i2c,
                                     u8x8_gpio_and_delay_hal);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetPowerSave(&u8g2, 0);
  u8g2_ClearBuffer(&u8g2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* _f 为 1024 字节全屏缓冲，使用 ClearBuffer / SendBuffer，不用分页 FirstPage/NextPage */
    u8g2_ClearBuffer(&u8g2);
    draw_frame();
    u8g2_SendBuffer(&u8g2);

    step_state();
    HAL_Delay(40);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Option 1: HSE external crystal, 8MHz x9 = 72MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK)
  {
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) == HAL_OK)
    {
      return;
    }
  }

  /* Option 2: HSI internal clock (no crystal needed), 4MHz x12 = 48MHz */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file; (void)line;
  __disable_irq();
  while (1) {}
}
#endif
