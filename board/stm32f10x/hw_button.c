/******************************************************************************
  * @file    hw_button.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-12-04
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
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO,ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    /* KEY->PC4 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return HW_ERR_OK;
}


