/******************************************************************************
  * @file    hw_button.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-09-25
  * @brief   button source file
******************************************************************************/

#include "hw_button.h"

/******************************************************************************
 * func name   : hw_button_init
 * para        : NULL
 * return      : button init result
 * description : button init,KEY->PC4
******************************************************************************/
uint8_t hw_button_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOC_CLK_ENABLE();
    /*Configure GPIO pin : USR_BUTTON_Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    return HW_ERR_OK;
}

