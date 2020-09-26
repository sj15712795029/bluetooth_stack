/******************************************************************************
  * @file    hw_led.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-25
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
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    LED1_PERIPH_CLK_ENABLE();
    LED2_PERIPH_CLK_ENABLE();

    /*Configure GPIO pins : LED1_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED1_PIN|LED2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStruct);

    LED1_OFF;
    LED2_OFF;
    return HW_ERR_OK;
}

