/******************************************************************************
  * @file    hw_oled.h
  * @author  Wireless link team
  * @version V0.0.1
  * @date    2020-9-25
  * @brief   oled header file
******************************************************************************/
#ifndef HW_OLED_H_H_H
#define HW_OLED_H_H_H

#include "hw_misc.h"

#define SSD1306_PERIPH_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define SSD1306_SCL_GPIO GPIOB
#define SSD1306_SDA_GPIO GPIOB
#define SSD1306_SCL_PIN GPIO_PIN_4
#define SSD1306_SDA_PIN GPIO_PIN_5

#define HIGH_BIT_MASK 0x80
#define SSD1306_IIC_CMD_MASK (0<<6)
#define SSD1306_IIC_DAT_MASK (1<<6)
#define SSD1306_SLAVE_ADDR 0x78
#define SSD1306_IIC_READ 0x1
#define SSD1306_IIC_WRITE 0x0

#define SSD1306_MAX_COL		128
#define SSD1306_MAX_RAW		64
#define SSD1306_MAX_PAGE	7

#define SSD1306_TIME_INTEVAL_2FRAME 0x7
#define SSD1306_TIME_INTEVAL_3FRAME 0x4
#define SSD1306_TIME_INTEVAL_4FRAME 0x5
#define SSD1306_TIME_INTEVAL_5FRAME 0x0
#define SSD1306_TIME_INTEVAL_25FRAME 0x6
#define SSD1306_TIME_INTEVAL_64FRAME 0x1
#define SSD1306_TIME_INTEVAL_128FRAME 0x2
#define SSD1306_TIME_INTEVAL_256FRAME 0x3
#define SSD1306_TIME_INTERVAL_MASK 0x07
#define SSD1306_LEFT_H_SCROLL 0x0
#define SSD1306_RIGHT_H_SCROLL 0x1

#define SSD1306_LEFT_V_SCROLL 0x2
#define SSD1306_RIGHT_V_SCROLL 0x1


#define OLED_DATA 	0
#define OLED_CMD  	1
#define FONT16X8_LENGTH	16
#define FONT16X8_WEIGHT	8

#define FONT8X6_LENGTH	8
#define FONT8X6_WEIGHT	6

#define IIC_SCLK_LOW HAL_GPIO_WritePin(SSD1306_SCL_GPIO, SSD1306_SCL_PIN, GPIO_PIN_RESET)
#define IIC_SCLK_HIGH HAL_GPIO_WritePin(SSD1306_SCL_GPIO, SSD1306_SCL_PIN, GPIO_PIN_SET)
#define IIC_SDIN_LOW HAL_GPIO_WritePin(SSD1306_SDA_GPIO, SSD1306_SDA_PIN, GPIO_PIN_RESET)
#define IIC_SDIN_HIGH HAL_GPIO_WritePin(SSD1306_SDA_GPIO, SSD1306_SDA_PIN, GPIO_PIN_SET)

/************************************************************* 
 * SSD1306 OLED commands are divided into 7 categories:
 * Basic commands
 * Scroll command
 * Address setting command
 * Hardware configuration command
 * Timing and driver scheme configuration commands
 * Advanced graphics command
 * Charge pump command
**************************************************************/
typedef enum
{
	/* 1. Basic commands */
	/* Set the contrast, range 0~0xff, the higher the value, the higher the contrast */
	oled_cmd_contrast = 0x81,
	/* Where the contents of GDDRAM are output */
	oled_cmd_entire_display_on_with_ram = 0xa4,	
	/* Ignore GDDRAM content to light up the full screen */
	oled_cmd_entire_display_on_without_ram = 0xa5,	
	/* Normal display, that is, pixel 1 represents light */
	oled_cmd_normal_display = 0xa6,		
	/* Invert the display so that the pixel 0 represents the light */
	oled_cmd_inverse_display = 0xa7,		
	/* Turn off oled display */
	oled_cmd_display_off = 0xae,	
	/* Turn on oled display */
	oled_cmd_display_on = 0xaf,							

	/* 2. Scroll command */
	/* Scroll horizontally to the right */
	oled_cmd_right_hor_scroll = 0x26,	
	/* Scroll left horizontally */
	oled_cmd_left_horl_scroll = 0x27,	
	/* Scroll vertically and shift to the right */
	oled_cmd_ver_right_hor_scroll = 0x29,	
	/* Scroll vertically and shift to the left */
	oled_cmd_ver_left_horl_scroll = 0x2a,
	/* Stop rolling */
	oled_cmd_stop_scroll = 0x2e,
	/* Start rolling */
	oled_cmd_start_scroll = 0x2f,	
	/* Sets the vertical scroll area range */
	oled_cmd_set_ver_scroll_area = 0xa3,					

	/* 3. Address setting command */
	/* Set the low starting address of the column in page address mode, range 0x0~0x1f */
	oled_cmd_lower_col_start_addr = 0x0,	
	/* Set the high starting address of the column in page address mode, range 0x10~0x1f */
	oled_cmd_higher_col_start_addr = 0x10,
	/* Set memory address mode */
	oled_cmd_set_mem_addr_mode = 0x20,	
	/* Set column address */
	oled_cmd_set_col_addr = 0x21,	
	/* Set page address */
	oled_cmd_set_page_addr = 0x22,
	/* Page0 of the page address pattern */
	oled_cmd_set_page0_start = 0xb0,
	/* Page1 of the page address pattern */
	oled_cmd_set_page1_start = 0xb1,
	/* Page2 of the page address pattern */
	oled_cmd_set_page2_start = 0xb2,	
	/* Page3 of the page address pattern */
	oled_cmd_set_page3_start = 0xb3,
	/* Page4 of the page address pattern */
	oled_cmd_set_page4_start = 0xb4,
	/* Page5 of the page address pattern */
	oled_cmd_set_page5_start = 0xb5,
	/* Page6 of the page address pattern */
	oled_cmd_set_page6_start = 0xb6,
	/* Page7 of the page address pattern */
	oled_cmd_set_page7_start = 0xb7,

	/* 4. Hardware configuration command */
	/* Set the starting line of the screen to 0x40~0x7f, corresponding to lines 0~63, respectively */
	oled_cmd_set_start_line_lowset = 0x40,
	/* Remap, mapping column address 0 to SEG0 */
	oled_cmd_set_remap0 = 0xa0,
	/* Remap, mapping column address 127 to SEG0 */
	oled_cmd_set_remap1 = 0xa1,
	/* Set reuse rate */
	oled_cmd_set_multiplex_ratio = 0xa8,
	/* Scan from COM0 to COM[n-1] */
	oled_cmd_set_com_normal_scan = 0xc0,
	/* Scan from COM[n-1] to COM0 */
	oled_cmd_set_com_remap_scan = 0xc8,	
	/* Sets the offset to display */
	oled_cmd_set_display_offset = 0xd3,
	/* Sets the region color to close */
	oled_cmd_set_area_color_mode_off = 0xd8,
	/* Set the column pin hardware configuration */
	oled_cmd_set_compin_hw_conf = 0xda,

	/* 5. Timing and driver scheme configuration commands */
	/* Set the display clock divider value/oscillation frequency */
	oled_cmd_set_clk_div_osc_freq = 0xd5,
	/* Set the pre-charge cycle */
	oled_cmd_set_precharge_period = 0xd9,
	/* Set VCOMH backpressure value */
	oled_cmd_set_vcomh_deselect_level = 0xdb,
	/* Empty instruction */
	oled_cmd_nop = 0xe3,

	/* 6. Advanced graphics command */
	/* Settings fade out */
	oled_cmd_set_colour_fading = 0x23,
	/* Set amp */
	oled_cmd_set_amp = 0xd6,
	
	/* 7. Charge pump command */
	/* Charge setting pump */
	oled_cmd_set_charge_pump = 0x8d,
	
}oled_cmd_e;

uint8_t hw_oled_init(void);
uint8_t hw_oled_clear(void);
uint8_t hw_oled_display_on(void);
uint8_t hw_oled_display_off(void);
uint8_t hw_oled_set_contrast(uint8_t value);
uint8_t hw_oled_entry_display_with_ram(void);
uint8_t hw_oled_entry_display_without_ram(void);
uint8_t hw_oled_set_normal_display(void);
uint8_t hw_oled_set_inverse_display(void);
uint8_t hw_oled_set_horizontal_scroll(uint8_t direct,uint8_t start_page,uint8_t end_page,uint8_t interval);
uint8_t hw_oled_set_vertical_scroll(uint8_t direct,uint8_t start_page,uint8_t end_page,uint8_t interval,uint8_t offset);
uint8_t hw_oled_scroll_active(void);
uint8_t hw_oled_scroll_deactive(void);
uint8_t hw_oled_show_char(uint8_t col,uint8_t page,uint8_t show_char,uint8_t char_lenght);
uint8_t hw_oled_show_num(uint8_t col,uint8_t page,uint32_t num,uint8_t num_len,uint8_t char_lenght);
uint8_t hw_oled_show_string(uint8_t col,uint8_t page,uint8_t *string,uint8_t char_lenght);
uint8_t hw_oled_show_chinese(uint8_t col,uint8_t page,uint8_t chinese_index);
uint8_t hw_oled_draw_bmp(uint8_t col_start,uint8_t page_start,uint8_t col_end,uint8_t page_end,uint8_t *bmp);


#endif

