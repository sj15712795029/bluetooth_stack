/******************************************************************************
  * @file           bt_timer.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt timer source file
******************************************************************************/

#include "bt_timer.h"

utimer_t timer[BT_TIMER_COUNT];

/******************************************************************************
 * func name   : find_idle_timer
 * para        : NULL
 * return      : 返回timer handle
 * description : 寻找空闲的timer
******************************************************************************/
static int32_t find_idle_timer()
{
    uint8_t index = 0;
    for(index = 0; index < BT_TIMER_COUNT; index++)
    {
        if(timer[index].used == 0)
        {
            return index;
        }
    }
    return -1;
}

/******************************************************************************
 * func name   : utimer_create
 * para        : ticks(IN) -> 触发timeout的tick个数
 					 cb(IN) -> timeout回调函数 
 					 para(IN) -> 传给timeout回调函数的参数,需要全局变量
 * return      : 返回timer handle
 * description : 创建软件定时器timer
******************************************************************************/
int32_t  utimer_create(uint32_t ticks, timer_cb cb,void *para)
{
    int32_t idle_timer;

    idle_timer = find_idle_timer();
    if(-1 == idle_timer)
    {
        return -1;
    }

    timer[idle_timer].used = 1;
    timer[idle_timer].cb = cb;
    timer[idle_timer].para = para;
    timer[idle_timer].count = ticks;
    return idle_timer;
}

/******************************************************************************
 * func name   : utimer_cancel
 * para        : timerHandle(IN) -> 软件定时器的handle
 * return      : VOID
 * description : 取消软件定时器timer
******************************************************************************/
void utimer_cancel(int32_t timerHandle)
{
    if(1 == timer[timerHandle].used)
    {
        memset(&timer[timerHandle],0,sizeof(utimer_t));
    }
}

/******************************************************************************
 * func name   : utimer_polling
 * para        : VOID
 * return      : VOID
 * description : 轮询软件定时器，查找是否有timeout的定时器
******************************************************************************/
void utimer_polling()
{
    int index = 0;
    for(index = 0; index < BT_TIMER_COUNT; index++)
    {
        if(1 == timer[index].used)
        {
            timer[index].count -= 1;
            if(0 == timer[index].count)
            {
                timer[index].cb(timer[index].para);
                memset(&timer[index],0,sizeof(utimer_t));
            }
        }
    }
}

/******************************************************************************
 * func name   : utimer_init
 * para        : VOID
 * return      : VOID
 * description : 初始化软件定时器
******************************************************************************/
void utimer_init()
{
    memset(&timer,0,sizeof(utimer_t)*BT_TIMER_COUNT);
}

