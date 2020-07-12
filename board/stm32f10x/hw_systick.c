/******************************************************************************
  * @file    bsp_systick.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-6-11
  * @brief   systick source file
******************************************************************************/
#include "hw_systick.h"

/******************************************************************************
 * func name   : hw_systick_init
 * para        : ticks(IN)  -> The number of ticks that will fire the interrupt
 * return      : NULL
 * description : The number of ticks that will fire the interrupt.
                 Cortexm3 is basically 72M Hz, so the time to run it once
                 That's 1/72m, so the time of 1ms is 1/72000
******************************************************************************/
uint8_t hw_systick_init(uint32_t ticks)
{
    SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    /* Reset the value of systick */
    SysTick->VAL   = 0;
    /* Set to internal clock, turn on downward counting interrupt, 
     turn on repeat technology function */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
    return HW_ERR_OK;

}
