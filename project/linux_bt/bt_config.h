/******************************************************************************
  * @file    bt_config.h
  * @author  Yu-ZhongJun(124756828@qq.com)
  * @version V0.0.1
  * @date    2020-3-31
  * @brief   bt config header file
******************************************************************************/
#ifndef BT_CONFIG_H_H_H
#define BT_CONFIG_H_H_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

/***********   bluetooth function option *************************************************************************************/
#define BT_CLASSICAL_ENABLE 1
#define BT_BLE_ENABLE 1
/** BT_PBUF_TRANSPORT_H2 = 0x01,BT_PBUF_TRANSPORT_H4 = 0x02,BT_PBUF_TRANSPORT_H5 = 0x03,BT_PBUF_TRANSPORT_BCSP = 0x04,*/
#define BT_ENABLE_SNOOP 1
#define BT_TRANSPORT_TYPE 0x02
#define BT_ENABLE_SSP 0
/* IO_CAP_DISPLAY_ONLY->0x00 IO_CAP_DISPLAY_YES_NO->0x01 IO_CAP_KEYBOARD_ONLY->0x02 IO_CAP_NOINPUT_OUTPUT->0x03 */
#define BT_SSP_IOCAP_CONF 1
#define BT_CLASS_OF_DEVICE 0x200408;
#define BT_LOCAL_NAME "BT_DEMO"
#define BT_PIN_CODE "0000"
#define BT_TIMER_COUNT 64 /* TIMER COUNT */
#define BT_BAUDRATE_1 115200
#define BT_BAUDRATE_2 921600
/* CHIP select,!!!must select one(only) */
#define BT_VENDOR_CSR8X11_SUPPORT 1
#define BT_VENDOR_BCM43430A1_SUPPORT 0
#define PROFILE_DID_ENABLE 1
#define PROFILE_HFP_ENABLE 0
#define PROFILE_SPP_ENABLE 0
#define PROFILE_PBAP_ENABLE 0
#define PROFILE_A2DP_ENABLE 1
#define PROFILE_AVRCP_ENABLE 0
/******************************************************************************************************************************/



/*********** debug switch *****************************************************************************************************/
/*****************************************************************************
			BT_TRACE_LEVEL_NONE->0  BT_TRACE_LEVEL_ERROR->1
			BT_TRACE_LEVEL_WARNING->2   BT_TRACE_LEVEL_DEBUG->3
******************************************************************************/
#define BT_DEBUG printf
#define BT_HEX_TRACE_LEVEL 1
#define BT_PBUF_TRACE_LEVEL 0
#define BT_MEMORY_TRACE_LEVEL 0
#define BT_SNOOP_TRACE_LEVEL 0
#define BT_TRANSPORT_TRACE_LEVEL 0
#define BT_VENDOR_TRACE_LEVEL 0
#define BT_HCI_TRACE_LEVEL 0
#define BT_L2CAP_TRACE_LEVEL 0
#define BT_SDP_TRACE_LEVEL 0
#define BT_RFCOMM_TRACE_LEVEL 0
#define BT_HFP_TRACE_LEVEL 0
#define BT_HSP_TRACE_LEVEL 0
#define BT_OBEX_TRACE_LEVEL 0
#define BT_PBAP_TRACE_LEVEL 0
#define BT_MAP_TRACE_LEVEL 0
#define BT_DID_TRACE_LEVEL 0
#define BT_SPP_TRACE_LEVEL 0
#define BT_AVDTP_TRACE_LEVEL 3
#define BT_AVCTP_TRACE_LEVEL 3
#define BT_A2DP_TRACE_LEVEL 3
#define BT_AVRCP_TRACE_LEVEL 3
/******************************************************************************************************************************/



/***********   memory & memory pool option ************************************************************************************/
#define BT_MEMORY_C_LIB 1
#define MEMP_NUM_HCI_PCB 1
#define MEMP_NUM_HCI_LINK 7
#define MEMP_NUM_HCI_INQ 8
#define MEMP_NUM_L2CAP_PCB 7
#define MEMP_NUM_L2CAP_PCB_LISTEN 7
#define MEMP_NUM_L2CAP_SIG 7
#define MEMP_NUM_L2CAP_SEG 7
#define MEMP_NUM_SDP_PCB 7
#define MEMP_NUM_SDP_RECORD 7
#define MEMP_NUM_RFCOMM_PCB 7
#define MEMP_NUM_RFCOMM_PCB_LISTEN 7
#define MEMP_NUM_OBEX 7
#define MEMP_NUM_AVDTP 7
#define MEMP_NUM_AVDTP_SEP 7
#define MEMP_NUM_AVCTP 7
#define MEMP_NUM_HFP 7
#define MEMP_NUM_A2DP 7
#define MEMP_NUM_SPP 7
#define MEMP_NUM_PBAP 7
#define MEMP_NUM_BT_PBUF 3
#define MEM_ALIGNMENT 4
#define MEM_LIBC_MALLOC 1
#define PBUF_POOL_BUFSIZE               MEM_ALIGN_SIZE(1024)
#define BT_PACK_END	__attribute__ ((packed))
typedef uintptr_t mem_ptr_t;
/******************************************************************************************************************************/


/***********   HCI protocol config ********************************************************************************************/
#define HCI_HOST_MAX_NUM_ACL 8 /* HCI_HOST_MAX_NUM_ACL: The maximum number of ACL packets that the host can buffer */
#define HCI_HOST_ACL_MAX_LEN 1024
#define HCI_PACKET_TYPE 0xCC18 /* Default DM1, DH1, DM3, DH3, DM5, DH5 */
#define HCI_ALLOW_ROLE_SWITCH 1 /* Default 1 */
#define HCI_FLOW_QUEUEING 0 /* Default: 0 */
/******************************************************************************************************************************/



/***********   L2CAP protocol config ******************************************************************************************/
#define L2CAP_CFG_QOS 0
#define L2CAP_MTU 1024
#define L2CAP_OUT_FLUSHTO 0xFFFF
#define L2CAP_RTX 60
#define L2CAP_ERTX 300
#define L2CAP_MAXRTX 0
#define L2CAP_CFG_TO 30
/******************************************************************************************************************************/



/***********   SDP protocol config ********************************************************************************************/
#define RFCOMM_SPP_SERVER_CHNL 8
#define RFCOMM_HFP_HF_CHNL 9
/******************************************************************************************************************************/


/***********   RFCOMM protocol config *****************************************************************************************/
#define RFCOMM_N 1000
#define RFCOMM_K 0 /* RFCOMM_K: Initial amount of credits issued to the peer (min 0, max 7) */
#define RFCOMM_RK_MIN 4
#define RFCOMM_RK_INIT 7
#define RFCOMM_RK_MAX 10
#define RFCOMM_TO 20
#define RFCOMM_FLOW_QUEUEING 1 /* Default: 0 */
#define RFCOMM_FLOW_QUEUEING_SIZE 32
/******************************************************************************************************************************/




/***********   DID profile config ********************************************************************************************/
#define DID_VENDOR_ID_VALUE 0xffff
#define DID_PRODUCT_ID_VALUE 0xffff
#define DID_VERSION_ID_VALUE 0x0100
#define DID_PRIMARY_RECORD_VALUE 0x01
#define DID_VENDOR_ID_SOURCE_VALUE DID_VENDOR_ID_SOURCE_BLUETOOTH
/******************************************************************************************************************************/



/***********   A2DP profile config *******************************************************************************************/
#define SBC_SNK_MIN_BITPOOL 2
#define SBC_SNK_MAX_BITPOOL 53
/******************************************************************************************************************************/


#endif

