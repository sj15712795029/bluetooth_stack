#include "hw_misc.h"


void EXTI4_IRQHandler(void)
{
    hw_delay_ms(10);
    if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4) == 0)
    {
        /* USER CODE */
        HW_DEBUG("BUTTON KEY \n");
    }
    EXTI_ClearITPendingBit(EXTI_Line4);
}
