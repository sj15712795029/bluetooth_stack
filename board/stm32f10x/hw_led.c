/******************************************************************************
  * @file    hw_led.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   led operate source file
******************************************************************************/

#include "hw_led.h"


/******************************************************************************
 * func name   : hw_led_init
 * para        : NULL
 * return      : led init result
 * description : LED init,LED0->PE14 LED1->PE13
******************************************************************************/
uint8_t hw_led_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_PERIPH_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO, &GPIO_InitStructure);

    return HW_ERR_OK;
}
