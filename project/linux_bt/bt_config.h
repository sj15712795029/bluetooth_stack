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
#define BT_ENABLE_SNOOP 1
/** BT_PBUF_TRANSPORT_H2 = 0x01,BT_PBUF_TRANSPORT_H4 = 0x02,BT_PBUF_TRANSPORT_H5 = 0x03,BT_PBUF_TRANSPORT_BCSP = 0x04,*/
#define BT_TRANSPORT_TYPE 0x02
#define BT_ENABLE_SSP 0
#define BT_PAGE_TIMEOUT 0x4000 /* value*0.625ms */
/* IO_CAP_DISPLAY_ONLY->0x00 IO_CAP_DISPLAY_YES_NO->0x01 IO_CAP_KEYBOARD_ONLY->0x02 IO_CAP_NOINPUT_OUTPUT->0x03 */
#define BT_SSP_IOCAP_CONF 1
#define BT_LOCAL_NAME "BT_DEMO"
#define BT_PIN_CODE "0000"
#define BT_TIMER_COUNT 64 /* TIMER COUNT */
#define BT_BAUDRATE_1 115200
#define BT_BAUDRATE_2 921600
/* CHIP select,!!!must select one(only) */
#define BT_VENDOR_CSR8X11_SUPPORT 1
#define BT_VENDOR_BCM43430A1_SUPPORT 0
#define BT_VENDOR_BCM4345C5_SUPPORT 0
#define BT_VENDOR_CYW43438_SUPPORT 0
#define BT_VENDOR_CYW54591_SUPPORT 0
#define BT_VENDOR_COMMON_NO_FW_SUPPORT 0 /* Support ESP32 */

#define PROFILE_DID_ENABLE 1
#define PROFILE_HFP_HF_ENABLE 1
#define PROFILE_SPP_ENABLE 1
#define PROFILE_PBAP_ENABLE 0
#define PROFILE_A2DP_ENABLE 1
#define PROFILE_AVRCP_ENABLE 0
#define PROFILE_HID_ENABLE 0
#if PROFILE_HID_ENABLE > 0
#define BT_CLASS_OF_DEVICE 0x002540 /**/
#undef BT_ENABLE_SSP
#define BT_ENABLE_SSP 1
#else
#define BT_CLASS_OF_DEVICE 0x200408
#endif

#define PROFILE_BAS_ENABLE 1


/******************************************************************************************************************************/



/*********** debug switch *****************************************************************************************************/
/*****************************************************************************
			BT_TRACE_LEVEL_NONE->0  BT_TRACE_LEVEL_ERROR->1
			BT_TRACE_LEVEL_WARNING->2   BT_TRACE_LEVEL_DEBUG->3
******************************************************************************/
#define BT_DEBUG printf
#define BT_HEX_TRACE_COLOR 1
#define BT_HEX_TRACE_LEVEL 1
#define BT_PBUF_TRACE_LEVEL 0
#define BT_MEMORY_TRACE_LEVEL 0
#define BT_SNOOP_TRACE_LEVEL 0
#define BT_TRANSPORT_TRACE_LEVEL 0
#define BT_VENDOR_TRACE_LEVEL 0
#define BT_HCI_TRACE_LEVEL 3
#define BT_L2CAP_TRACE_LEVEL 0
#define BT_SDP_TRACE_LEVEL 0
#define BT_RFCOMM_TRACE_LEVEL 0
#define BT_HFP_TRACE_LEVEL 0
#define BT_HSP_TRACE_LEVEL 0
#define BT_OBEX_TRACE_LEVEL 0
#define BT_PBAP_TRACE_LEVEL 0
#define BT_MAP_TRACE_LEVEL 0
#define BT_DID_TRACE_LEVEL 0
#define BT_SPP_TRACE_LEVEL 3
#define BT_AVDTP_TRACE_LEVEL 3
#define BT_AVCTP_TRACE_LEVEL 3
#define BT_A2DP_TRACE_LEVEL 3
#define BT_AVRCP_TRACE_LEVEL 3
#define BT_HID_TRACE_LEVEL 3
#define BT_SMP_TRACE_LEVEL 3
#define BT_ATT_TRACE_LEVEL 3
#define BT_GATT_TRACE_LEVEL 3
#define BT_BAS_TRACE_LEVEL 3
/******************************************************************************************************************************/



/***********   memory & memory pool option ************************************************************************************/
#define BT_MEMORY_C_LIB 1
#define MEMP_NUM_HCI_PCB 2
#define MEMP_NUM_HCI_LINK 7
#define MEMP_NUM_HCI_INQ 32
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
#define MEMP_NUM_AVRCP 7
#define MEMP_NUM_SPP 7
#define MEMP_NUM_PBAP 7
#define MEMP_NUM_HID 7
#define MEMP_NUM_SMP 7
#define MEMP_NUM_ATT 7
#define MEMP_NUM_GATT 7
#define MEMP_NUM_BT_PBUF 3
#define MEM_ALIGNMENT 4
#define MEM_LIBC_MALLOC 1
#define PBUF_POOL_BUFSIZE               MEM_ALIGN_SIZE(1024)
typedef uintptr_t mem_ptr_t;
/******************************************************************************************************************************/


/***********   HCI protocol config ********************************************************************************************/
#define HCI_HOST_MAX_NUM_ACL 8 /* HCI_HOST_MAX_NUM_ACL: The maximum number of ACL packets that the host can buffer */
#define HCI_HOST_ACL_MAX_LEN 1024
#define HCI_PACKET_TYPE 0xCC18 /* Default DM1, DH1, DM3, DH3, DM5, DH5 */
#define HCI_ALLOW_ROLE_SWITCH 1 /* Default 1 */
#define HCI_FLOW_QUEUEING 0 /* Default: 0 */
#define HCI_RESET_TIMEOUT	10
#define HCI_REMOTE_NAME_LEN 32
#define HCI_LE_ADV_MAX_SIZE 32
#define HCI_INQUIRY_MAX_DEV MEMP_NUM_HCI_INQ
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
#define A2DP_SBC_SNK_MIN_BITPOOL 2
#define A2DP_SBC_SNK_MAX_BITPOOL 53
/******************************************************************************************************************************/

/***********   OBEX profile config ********************************************************************************************/
#define OBEX_VERSION                       0x13
/******************************************************************************************************************************/


/***********   PBAP profile config *******************************************************************************************/
#define PBAP_APP_PARA_MAX_SIZE 128
#define PBAP_CONNECT_MTU 640
#define PBAP_DN_VCARD_FORMAT 1 /* 0:vCard2.1 1:vCard3.0 */
/******************************************************************************************************************************/



/***********  AVRCP profile config *******************************************************************************************/
#define AVRCP_ID3_TITIL_MAX_SIZE 64
#define AVRCP_ID3_ARTIST_MAX_SIZE 64
#define AVRCP_ID3_ALBUM_MAX_SIZE 64
/******************************************************************************************************************************/

/***********  HID profile config *******************************************************************************************/
/* !!!!You must only select one, if you want to select multiple, then you need to assemble your own HID Descriptor */
#define HID_DEVICE_MOUSE_ENABLE 0
#define HID_DEVICE_KEYBAORD_ENABLE 1
#define HID_DEVICE_GAMEPAD_ENABLE 0
#define HID_DEVICE_JOYSTICK_ENABLE 0
/******************************************************************************************************************************/

/***********   GATT protocol config ********************************************************************************************/
#define GATT_PRI_SERVICE_MAX_COUNT 10
#define GATT_BLE_MTU_SIZE 23
#define GATT_GAP_SERVICE_HANDLE 0x0001
#define GATT_GAP_CHARACTERISTIC_HANDLE 0x0002
#define GATT_GAP_NAME_HANDLE 0x0003
#define GATT_SERVICE_HANLE 0x0004
#define GATT_GAP_NAME BT_LOCAL_NAME
#define GATT_BAS_SERVICE_HANDLE 0x0005
#define GATT_BAS_CHARACTERISTIC_HANDLE 0x0006
#define GATT_BAS_LEVEL_HANDLE 0x0007
#define GATT_BAS_CLIENT_CHARA_CONF_HANDLE 0x0008
/******************************************************************************************************************************/

/***********   SMP protocol config ********************************************************************************************/
#define SMP_MAX_ENC_KEY_SIZE 16
/* SMP_IO_CAP_DISPLAY_ONLY->0 SMP_IO_CAP_DISPLAY_YESNO->1 SMP_IO_CAP_KEYBORAD_ONLY->2 SMP_IO_CAP_NOIO->3 SMP_IO_CAP_KBDISP->4 */
#define SMP_IO_CAP_CONF 3
#define SMP_PAIR_MIMT 1
#define SMP_OOB_PRESENT_CONF 0
/******************************************************************************************************************************/



#endif

