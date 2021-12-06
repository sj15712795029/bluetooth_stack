/******************************************************************************
  * @file    bsp_iic.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-12-22
  * @brief   bsp iic protocol header file
******************************************************************************/


#ifndef BSP_IIC_H_H_H
#define BSP_IIC_H_H_H
#include "hw_misc.h"


#define IIC_24C02_SCL_GPIO GPIOB
#define IIC_24C02_SCL_PIN GPIO_PIN_6
#define IIC_24C02_SDA_GPIO GPIOB
#define IIC_24C02_SDA_PIN GPIO_PIN_7
#define IIC_24C02_SDA_OFFSET 8

#define IIC_24C02_SDA_IN()  {IIC_24C02_SDA_GPIO->MODER&=~(3<<(IIC_24C02_SDA_OFFSET*2));IIC_24C02_SDA_GPIO->MODER|=0<<IIC_24C02_SDA_OFFSET*2;}
#define IIC_24C02_SDA_OUT() {IIC_24C02_SDA_GPIO->MODER&=~(3<<(IIC_24C02_SDA_OFFSET*2));IIC_24C02_SDA_GPIO->MODER|=1<<IIC_24C02_SDA_OFFSET*2;}

#define IIC_24C02_IIC_SCL(n)  (n?HAL_GPIO_WritePin(IIC_24C02_SCL_GPIO,IIC_24C02_SCL_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(IIC_24C02_SCL_GPIO,IIC_24C02_SCL_PIN,GPIO_PIN_RESET))
#define IIC_24C02_IIC_SDA(n)  (n?HAL_GPIO_WritePin(IIC_24C02_SDA_GPIO,IIC_24C02_SDA_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(IIC_24C02_SDA_GPIO,IIC_24C02_SDA_PIN,GPIO_PIN_RESET))
#define IIC_24C02_READ_SDA    HAL_GPIO_ReadPin(IIC_24C02_SDA_GPIO,IIC_24C02_SDA_PIN)

void bsp_iic_init(void);		 
void bsp_iic_start(void);
void bsp_iic_stop(void);
void bsp_iic_send_byte(uint8_t txd);
uint8_t bsp_iic_read_byte(uint8_t ack);
uint8_t bsp_iic_wait_ack(void);
void bsp_iic_ack(void);
void bsp_iic_nack(void);	

#endif


