/******************************************************************************
  * @file    hw_button.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2021-12-28
  * @brief   lcd header file
******************************************************************************/
#ifndef HW_LCD_H_H_H
#define HW_LCD_H_H_H

#include "hw_misc.h"

#define LCD_RS_Pin GPIO_PIN_3
#define LCD_RS_GPIO_Port GPIOE
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOE
#define LCD_BL_Pin GPIO_PIN_0
#define LCD_BL_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_1
#define LCD_RESET_GPIO_Port GPIOE


#define LCD_RST_H	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET)
#define LCD_RST_L		HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET)

#define LCD_DC_H		HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET)
#define LCD_DC_L		HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)

#define LCD_BLK_H  	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)
#define LCD_BLK_L  	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)

#define LCD_CS_H  	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#define LCD_CS_L  	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)


typedef enum
{
	lcd_cmd_nop = 0x00,
	lcd_cmd_soft_reset = 0x01,
	lcd_cmd_sleep_out = 0x11,
	lcd_cmd_normal_display_on = 0x13,
	lcd_cmd_display_on = 0x29,/* Display ON */
	lcd_cmd_col_addr_set = 0x2a,/* Column Address Set */
	lcd_cmd_page_addr_set = 0x2b,/* Page Address Set */
	lcd_cmd_mem_write = 0x2c,/* Memory Write */
	lcd_cmd_mem_access_ctl = 0x36, /* Memory Access Control ???????????? */
	lcd_cmd_if_pixel_format = 0x3a,/* Interface Pixel Format */
	lcd_cmd_inversion_display = 0xb4,/* Display Inversion Control */
	lcd_cmd_display_func_ctl = 0xb6, /* Display Function Control */
	lcd_cmd_pwr_ctl_2 = 0xc1, /* Power Control 2 */
	lcd_cmd_vcom_ctl = 0xc5,/* VCOM Control */
	lcd_cmd_positive_gamma_ctl = 0xe0,/* Positive Gamma Control */
	lcd_cmd_negative_gamma_ctl = 0xe1,/* Negative Gamma Correction */
}lcd_cmd_e;

#define USE_HORIZONTAL 2 

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 320
#define LCD_H 480
#else
#define LCD_W 480
#define LCD_H 320
#endif


/* Color */
#define WHITE	0xFCFCFC
#define BLACK	0X000000
#define RED		0xFC0000
#define GREEN	0x00FC00
#define BLUE	0x0000FC
#define BROWN	0xDB9370
#define YELLOW	0xFFFF00

uint8_t bsp_lcd_init(void);
uint8_t bsp_lcd_fill_color(uint32_t xsta,uint32_t ysta,uint32_t xend,uint32_t yend,uint32_t color);


#endif

