/******************************************************************************
  * @file    hw_sht2x.h
  * @author  Wireless link team
  * @version V0.0.1
  * @date    2020-09-25
  * @brief   sht2x temp/humi header file
******************************************************************************/
#ifndef HW_SHT2X_H_H_H
#define HW_SHT2X_H_H_H

#include "hw_misc.h"

#define SHT2X_PERIPH_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE
#define SHT2X_SCL_GPIO GPIOB
#define SHT2X_SDA_GPIO GPIOB
#define SHT2X_SCL_PIN GPIO_PIN_8
#define SHT2X_SDA_PIN GPIO_PIN_9

#define SHT2X_IIC_SCLK_LOW HAL_GPIO_WritePin(SHT2X_SCL_GPIO,SHT2X_SCL_PIN,GPIO_PIN_RESET)
#define SHT2X_IIC_SCLK_HIGH HAL_GPIO_WritePin(SHT2X_SCL_GPIO,SHT2X_SCL_PIN,GPIO_PIN_SET)
#define SHT2X_IIC_SDIN_LOW HAL_GPIO_WritePin(SHT2X_SDA_GPIO,SHT2X_SDA_PIN,GPIO_PIN_RESET)
#define SHT2X_IIC_SDIN_HIGH HAL_GPIO_WritePin(SHT2X_SDA_GPIO,SHT2X_SDA_PIN,GPIO_PIN_SET)
#define SHT2X_IIC_READ_SDA (((SHT2X_SDA_GPIO->IDR&SHT2X_SDA_PIN)!=0)?1:0)

#define SHT2X_HIGH_BIT_MASK 0x80
#define SHT2X_IIC_ADDR 0x80
#define SHT2X_IIC_READ 0x1
#define SHT2X_IIC_WRITE 0x0

typedef enum
{
    sht2x_cmd_trig_temp_hm = 0xe3, /* command trig. temp meas. hold master */
    sht2x_cmd_trig_humi_hm = 0xe5,/* command trig. humidity meas. hold master */
    sht2x_cmd_trig_temp_poll = 0xf3,/* command trig. temp meas. no hold master */
    sht2x_cmd_trig_humi_poll = 0xf5,/* command trig. humidity meas. no hold master */
    sht2x_cmd_write_user_reg = 0xe6, /* command writing user register */
    sht2x_cmd_read_user_reg = 0xe7,/* command reading user register */
    sht2x_cmd_soft_reset = 0xfe,/* command soft reset */
} sht2x_cmd_e;

uint8_t hw_sht2x_init(void);
uint8_t hw_sht2x_sw_reset(void);
uint8_t hw_sht2x_read_user_reg(void);
void hw_sht2x_write_user_reg(uint8_t reg);
double hw_sht2x_get_humi_hm(void);
double hw_sht2x_get_humi_poll(void);
double hw_sht2x_get_temp_hm(void);
double hw_sht2x_get_temp_poll(void);

#endif

