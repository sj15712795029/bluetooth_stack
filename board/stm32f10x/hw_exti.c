#include "hw_misc.h"
#include "hw_ov7670.h"

void EXTI1_IRQHandler(void)
{
    hw_delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
    {
        /* USER CODE */
        HW_DEBUG("BUTTON KEY 1\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line1);
}

void EXTI0_IRQHandler(void)
{
    hw_delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0) == 0)
    {
        /* USER CODE */
        HW_DEBUG("BUTTON KEY 2\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line0);
}


void EXTI4_IRQHandler(void)
{
    hw_delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == 0)
    {
        /* USER CODE */
        HW_DEBUG("BUTTON KEY 4\n");
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
}
