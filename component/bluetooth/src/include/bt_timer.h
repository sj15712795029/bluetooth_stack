/******************************************************************************
  * @file           bt_timer.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt timer header file
******************************************************************************/

#ifndef BT_TIMER_H_H_H
#define BT_TIMER_H_H_H

#include "bt_common.h"


typedef err_t(*timer_cb)(void *para);
typedef struct
{
    uint8_t used;
    uint32_t count;
    void *para;
    timer_cb cb;
} utimer_t;

void utimer_init(void);
int32_t  utimer_create(uint32_t ticks, timer_cb cb,void *para);
void utimer_cancel(int32_t timerHandle);
void utimer_polling(void);
#endif

