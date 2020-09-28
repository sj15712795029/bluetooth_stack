/******************************************************************************
  * @file    hw_sht2x.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-09-25
  * @brief   sht2x temp/humi source file
******************************************************************************/
#include "hw_sht2x.h"

static GPIO_InitTypeDef GPIO_InitStruct = {0};


static void sht2x_protocol_iic_sda_output(void);
static void sht2x_protocol_iic_sda_input(void);
static void sht2x_protocol_iic_start(void);
static uint8_t sht2x_protocol_iic_wait_ack(void);
static void sht2x_protocol_iic_stop(void);
static void sht2x_protocol_iic_ack(void);
static void sht2x_protocol_iic_noack(void);
static void sht2x_protocol_iic_write_byte(uint8_t byte);
static uint8_t sht2x_protocol_iic_read_byte(uint8_t ack);

uint8_t hw_sht2x_init(void)
{
	

	SHT2X_PERIPH_CLK_ENABLE();
	
    GPIO_InitStruct.Pin =SHT2X_SCL_PIN|SHT2X_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHT2X_SCL_GPIO, &GPIO_InitStruct);
    SHT2X_IIC_SCLK_HIGH;
    SHT2X_IIC_SDIN_HIGH;

    hw_sht2x_sw_reset();

    return HW_ERR_OK;
}

uint8_t hw_sht2x_sw_reset(void)
{
    sht2x_protocol_iic_start();
    sht2x_protocol_iic_write_byte(SHT2X_IIC_ADDR | SHT2X_IIC_WRITE);
    sht2x_protocol_iic_wait_ack();
    sht2x_protocol_iic_write_byte(sht2x_cmd_soft_reset);
    sht2x_protocol_iic_wait_ack();
    sht2x_protocol_iic_stop();

    return HW_ERR_OK;

}

double hw_sht2x_get_humi_poll()
{
    double humi;
    uint8_t  ack,tmp1, tmp2;
    uint16_t srh;
    uint8_t i = 0;


    sht2x_protocol_iic_start();
    sht2x_protocol_iic_write_byte(SHT2X_IIC_ADDR | SHT2X_IIC_WRITE);
    sht2x_protocol_iic_wait_ack();
    sht2x_protocol_iic_write_byte(sht2x_cmd_trig_humi_poll);
    sht2x_protocol_iic_wait_ack();

    do
    {
        hw_delay_ms(100);
        sht2x_protocol_iic_start();
        sht2x_protocol_iic_write_byte(SHT2X_IIC_ADDR | SHT2X_IIC_READ);
        i++;
        ack = sht2x_protocol_iic_wait_ack();
        if(i == 100)
            break;
    }while(ack != 0);


    tmp1 = sht2x_protocol_iic_read_byte(1);
    tmp2 = sht2x_protocol_iic_read_byte(1);
    sht2x_protocol_iic_read_byte(0);
    sht2x_protocol_iic_stop();

    srh = (tmp1 << 8) | (tmp2 << 0);
    srh &= ~0x0003;
    humi = ((double)srh * 0.00190735) - 6;

    return humi;
}


double hw_sht2x_get_temp_poll()
{
    double temp;
    uint8_t ack,tmp1, tmp2;
    uint16_t st;
    uint8_t i = 0;

    sht2x_protocol_iic_start();
    sht2x_protocol_iic_write_byte(SHT2X_IIC_ADDR | SHT2X_IIC_WRITE);
    sht2x_protocol_iic_wait_ack();
    sht2x_protocol_iic_write_byte(sht2x_cmd_trig_temp_poll);
    sht2x_protocol_iic_wait_ack();

    do
    {
        hw_delay_ms(100);
        sht2x_protocol_iic_start();
        sht2x_protocol_iic_write_byte(SHT2X_IIC_ADDR | SHT2X_IIC_READ);
        i++;
        ack = sht2x_protocol_iic_wait_ack();
        if(i == 100)
            break;
    }while(ack != 0);


    tmp1 = sht2x_protocol_iic_read_byte(1);
    tmp2 = sht2x_protocol_iic_read_byte(1);
    sht2x_protocol_iic_read_byte(0);
    sht2x_protocol_iic_stop();

    st = (tmp1 << 8) | (tmp2 << 0);
    st &= ~0x0003;
    temp = ((double)st * 0.00268127) - 46.85;

    return (temp);
}





static void sht2x_protocol_iic_write_byte(uint8_t byte)
{
    uint8_t t;
    sht2x_protocol_iic_sda_output();
    SHT2X_IIC_SCLK_LOW;
    for(t=0; t<8; t++)
    {
        if(((byte&0x80)>>7) == 1)
            SHT2X_IIC_SDIN_HIGH;
        else
            SHT2X_IIC_SDIN_LOW;

        byte<<=1;
        hw_delay_us(2);
        SHT2X_IIC_SCLK_HIGH;
        hw_delay_us(2);
        SHT2X_IIC_SCLK_LOW;
        hw_delay_us(2);
    }

}

static uint8_t sht2x_protocol_iic_read_byte(uint8_t ack)
{
    uint8_t i,receive=0;
    sht2x_protocol_iic_sda_input();
    for(i=0; i<8; i++ )
    {
        SHT2X_IIC_SCLK_LOW;
        hw_delay_us(2);
        SHT2X_IIC_SCLK_HIGH;
        receive<<=1;
        if(SHT2X_IIC_READ_SDA)receive++;
        hw_delay_us(1);
    }
    if (!ack)
        sht2x_protocol_iic_noack();
    else
        sht2x_protocol_iic_ack();
    return receive;
}

static void sht2x_protocol_iic_sda_output()
{
    GPIO_InitStruct.Pin = SHT2X_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(SHT2X_SDA_GPIO, &GPIO_InitStruct);
}

static void sht2x_protocol_iic_sda_input()
{
    GPIO_InitStruct.Pin = SHT2X_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SHT2X_SDA_GPIO, &GPIO_InitStruct);
}

static void sht2x_protocol_iic_start()
{
    sht2x_protocol_iic_sda_output();

    SHT2X_IIC_SCLK_HIGH;
    SHT2X_IIC_SDIN_HIGH;
    hw_delay_us(4);
    SHT2X_IIC_SDIN_LOW;
    hw_delay_us(4);
    SHT2X_IIC_SCLK_LOW;
}


static uint8_t sht2x_protocol_iic_wait_ack()
{
    uint8_t err_count = 0;
    sht2x_protocol_iic_sda_input();

    SHT2X_IIC_SDIN_HIGH;
    hw_delay_us(1);
    SHT2X_IIC_SCLK_HIGH;
    hw_delay_us(1);

    while(SHT2X_IIC_READ_SDA)
    {
        err_count++;
        if(err_count > 250)
        {
            sht2x_protocol_iic_stop();
            return 1;
        }
    }
    SHT2X_IIC_SCLK_LOW;

    return 0;
}

static void sht2x_protocol_iic_ack()
{
    SHT2X_IIC_SCLK_LOW;
    sht2x_protocol_iic_sda_output();
    SHT2X_IIC_SDIN_LOW;
    hw_delay_us(2);
    SHT2X_IIC_SCLK_HIGH;
    hw_delay_us(2);
    SHT2X_IIC_SCLK_LOW;
}

static void sht2x_protocol_iic_noack()
{
    SHT2X_IIC_SCLK_LOW;
    sht2x_protocol_iic_sda_output();
    SHT2X_IIC_SDIN_HIGH;
    hw_delay_us(2);
    SHT2X_IIC_SCLK_HIGH;
    hw_delay_us(2);
    SHT2X_IIC_SCLK_LOW;
}

static void sht2x_protocol_iic_stop()
{
    sht2x_protocol_iic_sda_output();

    SHT2X_IIC_SCLK_LOW;
    SHT2X_IIC_SDIN_LOW;

    hw_delay_us(4);
    SHT2X_IIC_SCLK_HIGH;
    SHT2X_IIC_SDIN_HIGH;
    hw_delay_us(4);
}
