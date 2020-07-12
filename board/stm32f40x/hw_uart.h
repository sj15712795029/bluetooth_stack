/******************************************************************************
  * @file    hw_usart.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2019-3-12
  * @brief   usart header file
******************************************************************************/
#ifndef HW_UART_H_H_H
#define HW_UART_H_H_H
#include "hw_misc.h"

uint8_t hw_uart_debug_init(uint32_t baud_rate);
uint8_t hw_uart_bt_init(uint32_t baud_rate,uint8_t reconfig);
uint8_t* bt_get_tx_buffer(void);
void uart_bt_send(uint8_t *buf,uint8_t len);

#endif

