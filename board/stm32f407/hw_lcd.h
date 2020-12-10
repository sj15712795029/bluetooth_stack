/******************************************************************************
  * @file    hw_lcd.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-10-02
  * @brief   lcd header file
******************************************************************************/

#ifndef HW_LCD_H_H_H
#define HW_LCD_H_H_H

#include "hw_misc.h"


#define BANK_LCD_D         ((uint32_t)0x6C000080) /* DATA ADDRESS */
#define BANK_LCD_C         ((uint32_t)0x6C000000) /* COMMAND ADDRESS */

#define LCD_BK_HIGH	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define LCD_BK_LOW	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define LCD_RST_HIGH	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET)
#define LCD_RST_LOW	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET)

#define LCD_ID_R61509V 0xB509

#define LCD_H_WIDTH 400
#define LCD_H_HEIGHT 240
#define LCD_V_WIDTH 240
#define LCD_V_HEIGHT 400

typedef enum
{
    DIR_HORIZONTAL, /* 横向的 */
    DIR_VERTICAL, /* 纵向的 */
} lcd_dir_e;

typedef struct
{
    uint16_t width;			/* LCD 宽度 */
    uint16_t height;			/* LCD 高度 */
    uint16_t id;				/* LCD ID */
    lcd_dir_e  dir;		/* LCD dir */
    uint16_t	wramcmd;	/* 开始写gram指令 */
    uint16_t  setxcmd;		/* 设置x坐标指令 */
    uint16_t  setycmd;		/* 设置y坐标指令 */
} lcd_info_t;


#define COLOR_WHITE			0xFFFF
#define COLOR_BLACK			0x0000
#define COLOR_BLUE			0x001F
#define COLOR_BRED			0XF81F
#define COLOR_GRED			0XFFE0
#define COLOR_GBLUE			0X07FF
#define COLOR_RED           	 	0xF800
#define COLOR_MAGENTA		0xF81F
#define COLOR_GREEN			0x07E0
#define COLOR_CYAN			0x7FFF
#define COLOR_YELLOW		0xFFE0

/* Register list */
#define REG_DEVICE_CODE                  0x000
#define REG_DRIVER_OUTPUT                0x001
#define REG_LCD_DR_WAVE_CTRL             0x002
#define REG_ENTRY_MODE                   0x003
#define REG_OUTL_SHARP_CTRL              0x006
#define REG_DISP_CTRL1                   0x007
#define REG_DISP_CTRL2                   0x008
#define REG_DISP_CTRL3                   0x009
#define REG_LPCTRL                       0x00B
#define REG_EXT_DISP_CTRL1               0x00C
#define REG_EXT_DISP_CTRL2               0x00F
#define REG_PAN_INTF_CTRL1               0x010
#define REG_PAN_INTF_CTRL2               0x011
#define REG_PAN_INTF_CTRL3               0x012
#define REG_PAN_INTF_CTRL4               0x020
#define REG_PAN_INTF_CTRL5               0x021
#define REG_PAN_INTF_CTRL6               0x022
#define REG_FRM_MRKR_CTRL                0x090

#define REG_PWR_CTRL1                    0x100
#define REG_PWR_CTRL2                    0x101
#define REG_PWR_CTRL3                    0x102
#define REG_PWR_CTRL4                    0x103
#define REG_PWR_CTRL5                    0x107
#define REG_PWR_CTRL6                    0x110
#define REG_PWR_CTRL7                    0x112

#define REG_RAM_HADDR_SET                0x200
#define REG_RAM_VADDR_SET                0x201
#define REG_RW_GRAM                      0x202
#define REG_RAM_HADDR_START              0x210
#define REG_RAM_HADDR_END                0x211
#define REG_RAM_VADDR_START              0x212
#define REG_RAM_VADDR_END                0x213
#define REG_RW_NVM                       0x280
#define REG_VCOM_HVOLTAGE1               0x281
#define REG_VCOM_HVOLTAGE2               0x282

#define REG_GAMMA_CTRL1                  0x300
#define REG_GAMMA_CTRL2                  0x301
#define REG_GAMMA_CTRL3                  0x302
#define REG_GAMMA_CTRL4                  0x303
#define REG_GAMMA_CTRL5                  0x304
#define REG_GAMMA_CTRL6                  0x305
#define REG_GAMMA_CTRL7                  0x306
#define REG_GAMMA_CTRL8                  0x307
#define REG_GAMMA_CTRL9                  0x308
#define REG_GAMMA_CTRL10                 0x309
#define REG_GAMMA_CTRL11                 0x30A
#define REG_GAMMA_CTRL12                 0x30B
#define REG_GAMMA_CTRL13                 0x30C
#define REG_GAMMA_CTRL14                 0x30D

#define REG_BIMG_NR_LINE                 0x400
#define REG_BIMG_DISP_CTRL               0x401
#define REG_BIMG_VSCROLL_CTRL            0x404

#define REG_PARTIMG1_POS                 0x500
#define REG_PARTIMG1_RAM_START           0x501
#define REG_PARTIMG1_RAM_END             0x502
#define REG_PARTIMG2_POS                 0x503
#define REG_PARTIMG2_RAM_START           0x504
#define REG_PARTIMG2_RAM_END             0x505

#define REG_SOFT_RESET                   0x600
#define REG_ENDIAN_CTRL                  0x606
#define REG_NVM_ACCESS_CTRL              0x6F0

/* Bits */
#define DRIVER_OUTPUT_SS_BIT             (1 << 8)
#define DRIVER_OUTPUT_SM_BIT             (1 << 10)

#define DRIVER_WAVE_CTL_BC_BIT		(1<<8)

#define ENTRY_MODE_TRI                   (1 << 15)
#define ENTRY_MODE_DFM                   (1 << 14)
#define ENTRY_MODE_BGR                   (1 << 12)
#define ENTRY_MODE_HWM                   (1 << 9)
#define ENTRY_MODE_ORG                   (1 << 7)
#define ENTRY_MODE_VID                   (1 << 5)
#define ENTRY_MODE_HID                   (1 << 4)
#define ENTRY_MODE_AM                    (1 << 3)
#define ENTRY_MODE_EPF(n)                (n & 3)

#define OUTL_SHARP_CTRL_EGMODE           (1 << 15)
#define OUTL_SHARP_CTRL_AVST(n)          ((n & 7) << 7)
#define OUTL_SHARP_CTRL_ADST(n)          ((n & 7) << 4)
#define OUTL_SHARP_CTRL_DTHU(n)          ((n & 3) << 2)
#define OUTL_SHARP_CTRL_DTHL(n)          (n & 3)

#define DISP_CTRL1_PTDE(n)               ((n & 4) << 12)
#define DISP_CTRL1_BASEE                 (1 << 8)
#define DISP_CTRL1_VON                   (1 << 6)
#define DISP_CTRL1_GON                   (1 << 5)
#define DISP_CTRL1_DTE                   (1 << 4)
#define DISP_CTRL1_D(n)                  (n & 3)

#define EXT_DISP_CTRL1_ENC(n)            ((n & 7) << 12)
#define EXT_DISP_CTRL1_RM(n)             ((n & 1) << 8)
#define EXT_DISP_CTRL1_DM(n)             ((n & 3) << 4)
#define EXT_DISP_CTRL1_RIM(n)            (n & 3)

#define PWR_CTRL1_SAP(n)                 ((n & 3) << 13)
#define PWR_CTRL1_SAPE                   (1 << 12)
#define PWR_CTRL1_BT(n)                  ((n & 7) << 8)
#define PWR_CTRL1_APE                    (1 << 7)
#define PWR_CTRL1_AP(n)                  ((n & 7) << 4)
#define PWR_CTRL1_DSTB                   (1 << 2)
#define PWR_CTRL1_SLP                    (1 << 1)

#define SOFT_RESET(n)                    (n << 0)

void hw_lcd_init(void);
uint16_t hw_lcd_get_id(void);
void hw_lcd_display_on(void);
void hw_lcd_display_off(void);
void hw_lcd_clear(uint16_t clear_color);
void hw_lcd_fill(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,uint16_t color);
void hw_lcd_draw_line(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color);

#endif



