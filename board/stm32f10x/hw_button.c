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
 * description : button init,KEY1->PB1 KEY2->PB0 KEY3->PC5 KEY4->PC4
******************************************************************************/
uint8_t hw_button_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

    /* KEY1->PB1 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line=EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KEY1_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KEY1_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* KEY2->PB0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KEY2_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KEY2_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* KEY3->PC5 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line=EXTI_Line5;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KEY3_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KEY3_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* KEY4->PC4 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_Init(&EXTI_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = KEY4_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = KEY4_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return HW_ERR_OK;
}


