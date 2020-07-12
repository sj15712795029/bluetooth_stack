/******************************************************************************
  * @file           bt_fcs.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt fcs header file
******************************************************************************/

#ifndef BT_FCS_H_H_H
#define BT_FCS_H_H_H
#include "bt_common.h"

uint8_t fcs8_crc_check(struct bt_pbuf_t *p, uint8_t len, uint8_t check_sum);
uint8_t fcs8_crc_calc(struct bt_pbuf_t *p, uint8_t len);
uint8_t fcs16_crc_check(struct bt_pbuf_t *p, uint16_t len);
uint16_t fcs16_crc_calc(struct bt_pbuf_t *p, uint16_t len);

#endif
