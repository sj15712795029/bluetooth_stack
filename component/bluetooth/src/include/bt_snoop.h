/******************************************************************************
  * @file           bt_snoop.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-21
  * @brief          bt snoop header file
******************************************************************************/
#ifndef BT_SNOOP_H_H_H
#define BT_SNOOP_H_H_H

#include "bt_common.h"

#if BT_ENABLE_SNOOP > 0

#define BT_SNOOP_PACKET_TYPE_CMD  0x01
#define BT_SNOOP_PACKET_TYPE_ACL_DATA  0x02
#define BT_SNOOP_PACKET_TYPE_SCO_DATA  0x03
#define BT_SNOOP_PACKET_TYPE_EVT  0x04

err_t bt_snoop_init(void);
err_t bt_snoop_deinit(void);
err_t bt_snoop_write(uint8_t  packet_type, uint8_t  in, uint8_t *packet, uint16_t len);
#endif

#endif
