#include "hw_misc.h"
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_4)
    {
        HW_DEBUG("Press user button\n");
    }
		if (GPIO_Pin == GPIO_PIN_9)
		{
			HAL_PCDEx_BCD_VBUSDetect(&hpcd_USB_OTG_FS);
		}
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
