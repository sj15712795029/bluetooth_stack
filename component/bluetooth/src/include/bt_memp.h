/******************************************************************************
  * @file           bt_memp.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt memory pool header file
******************************************************************************/

#ifndef BT_MEMP_H_H_H
#define BT_MEMP_H_H_H

#include "bt_common.h"

typedef enum {
  MEMP_HCI_PCB,
  MEMP_HCI_LINK,
  MEMP_HCI_INQ,
  MEMP_L2CAP_PCB,
  MEMP_L2CAP_PCB_LISTEN,
  MEMP_L2CAP_SIG,
  MEMP_L2CAP_SEG,
  MEMP_SDP_PCB,
  MEMP_SDP_RECORD,
  MEMP_RFCOMM_PCB,
  MEMP_RFCOMM_PCB_LISTEN,
  MEMP_OBEX_PCB,
  MEMP_AVDTP_PCB,
  MEMP_AVDTP_SEP,
  MEMP_AVCTP_PCB,
  MEMP_HFP_PCB,
  MEMP_A2DP_PCB,
  MEMP_AVRCP_PCB,
  MEMP_SPP_PCB,
  MEMP_PBAP_PCB,
  MEMP_BT_PBUF_POOL,
  MEMP_BT_MAX
} bt_memp_t;


void bt_memp_init(void);
void *bt_memp_malloc(bt_memp_t type);
void bt_memp_free(bt_memp_t type, void *mem);

#endif

