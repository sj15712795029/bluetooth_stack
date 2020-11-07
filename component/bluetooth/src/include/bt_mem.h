/******************************************************************************
  * @file           bt_mem.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt memory header file
******************************************************************************/

#ifndef BT_MEM_H_H_H
#define BT_MEM_H_H_H

#include "bt_common.h"


#if MEM_LIBC_MALLOC
typedef size_t mem_size_t;
#define bt_mem_init()
#define bt_mem_free free
#define bt_mem_malloc malloc
#define bt_mem_calloc calloc

#endif

void* bt_mem_trim(void *mem, mem_size_t size);

#endif
