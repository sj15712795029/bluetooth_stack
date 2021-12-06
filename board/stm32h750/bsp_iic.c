/******************************************************************************
  * @file    bsp_iic.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-12-22
  * @brief   bsp iic protocol source file
******************************************************************************/


#include "bsp_iic.h"

void bsp_iic_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pins : EEPROM_SCL_Pin EEPROM_SDA_Pin */
    GPIO_InitStruct.Pin = IIC_24C02_SCL_PIN|IIC_24C02_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    IIC_24C02_IIC_SDA(1);
    IIC_24C02_IIC_SCL(1);
}

void bsp_iic_start(void)
{
    IIC_24C02_SDA_OUT();
    IIC_24C02_IIC_SDA(1);
    IIC_24C02_IIC_SCL(1);
    hw_loop_wait(4);
    IIC_24C02_IIC_SDA(0);
    hw_loop_wait(4);
    IIC_24C02_IIC_SCL(0);
}

void bsp_iic_stop(void)
{
    IIC_24C02_SDA_OUT();
    IIC_24C02_IIC_SCL(0);
    IIC_24C02_IIC_SDA(0);
    hw_loop_wait(4);
    IIC_24C02_IIC_SCL(1);
    IIC_24C02_IIC_SDA(1);
    hw_loop_wait(4);
}

uint8_t bsp_iic_wait_ack(void)
{
    uint8_t ucErrTime=0;
    IIC_24C02_SDA_IN();
    IIC_24C02_IIC_SDA(1);
    hw_loop_wait(1);
    IIC_24C02_IIC_SCL(1);
    hw_loop_wait(1);
    while(IIC_24C02_READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            bsp_iic_stop();
            return 1;
        }
    }
    IIC_24C02_IIC_SCL(0);
    return 0;
}

void bsp_iic_ack(void)
{
    IIC_24C02_IIC_SCL(0);
    IIC_24C02_SDA_OUT();
    IIC_24C02_IIC_SDA(0);
    hw_loop_wait(2);
    IIC_24C02_IIC_SCL(1);
    hw_loop_wait(2);
    IIC_24C02_IIC_SCL(0);
}

void bsp_iic_nack(void)
{
    IIC_24C02_IIC_SCL(0);
    IIC_24C02_SDA_OUT();
    IIC_24C02_IIC_SDA(1);
    hw_loop_wait(2);
    IIC_24C02_IIC_SCL(1);
    hw_loop_wait(2);
    IIC_24C02_IIC_SCL(0);
}

void bsp_iic_send_byte(uint8_t txd)
{
    uint8_t t;
    IIC_24C02_SDA_OUT();
    IIC_24C02_IIC_SCL(0);
    for(t=0; t<8; t++)
    {
        IIC_24C02_IIC_SDA((txd&0x80)>>7);
        txd<<=1;
        hw_loop_wait(2);
        IIC_24C02_IIC_SCL(1);
        hw_loop_wait(2);
        IIC_24C02_IIC_SCL(0);
        hw_loop_wait(2);
    }
}

uint8_t bsp_iic_read_byte(uint8_t ack)
{
    unsigned char i,receive=0;
    IIC_24C02_SDA_IN();
    for(i=0; i<8; i++ )
    {
        IIC_24C02_IIC_SCL(0);
        hw_loop_wait(2);
        IIC_24C02_IIC_SCL(1);
        receive<<=1;
        if(IIC_24C02_READ_SDA)receive++;
        hw_loop_wait(1);
    }
    if (!ack)
        bsp_iic_nack();
    else
        bsp_iic_ack();
    return receive;
}


