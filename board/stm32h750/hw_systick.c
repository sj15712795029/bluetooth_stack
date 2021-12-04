/******************************************************************************
  * @file    bsp_systick.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-25
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
    HAL_SetTickFreq(1000/ticks);
    return HW_ERR_OK;
}

