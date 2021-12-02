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

typedef enum
{
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
#if PROFILE_HFP_HF_ENABLE
    MEMP_HFP_PCB,
#endif
#if PROFILE_A2DP_ENABLE
    MEMP_A2DP_PCB,
#endif
#if PROFILE_AVRCP_ENABLE
    MEMP_AVRCP_PCB,
#endif
#if PROFILE_SPP_ENABLE
    MEMP_SPP_PCB,
#endif
#if PROFILE_PBAP_ENABLE
    MEMP_PBAP_PCB,
#endif
#if PROFILE_HID_ENABLE
    MEMP_HID_PCB,
#endif

#if BT_BLE_ENABLE
	MEMP_SMP_PCB,
    MEMP_ATT_PCB,
    MEMP_GATT_PCB,
#endif

    MEMP_BT_PBUF_POOL,
    MEMP_BT_MAX
} bt_memp_t;


void bt_memp_init(void);
void *bt_memp_malloc(bt_memp_t type);
void bt_memp_free(bt_memp_t type, void *mem);

#endif

