/******************************************************************************
  * @file    hw_misc.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   hw misc source file
******************************************************************************/

#include "hw_misc.h"

/* 函数声明区 */
static void hw_timer_reset_count(TIM_TypeDef * timer);


#define MAX_COL 16
#define SHOW_LINE_SIZE 16
void hw_hex_dump(uint8_t *data,int len)
{
#if HW_DEBUG_ENABLE > 0
    int line;
    int curline = 0;
    int curcol = 0;
    char showline[SHOW_LINE_SIZE];
    int data_pos = 0;

    if(len % MAX_COL)
    {
        line = len/MAX_COL+1;
    }
    else
    {
        line = len/MAX_COL;
    }

    for(curline = 0; curline < line; curline++)
    {
        hw_sprintf(showline,"%08xh:",curline*MAX_COL);
        HW_DEBUG("%s",showline);
        for(curcol = 0; curcol < MAX_COL; curcol++)
        {
            if(data_pos < len)
            {
                HW_DEBUG("%02x ",data[data_pos]);
                data_pos++;
                continue;
            }
            else
            {
                break;
            }
        }
        HW_DEBUG("\n");
    }
#endif
}




void hw_delay_ms(uint32_t cms)
{
    uint16_t temp = 0;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    /* 使能timer2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_Period = cms * 2;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM2, ENABLE);

    do
    {
        temp = TIM_GetCounter(TIM2);
    }
    while(temp != 0x01);

    /* 时间到，失能timer2 */
    TIM_Cmd(TIM2, DISABLE);
    hw_timer_reset_count(TIM2);

}

static void hw_timer_reset_count(TIM_TypeDef * timer)
{
    timer->CNT = 0;
}

void hw_delay_us(uint32_t cus)
{
	while(cus--);
}
