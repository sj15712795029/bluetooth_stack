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

/***********   debug switch **************************************************/
/*****************************************************************************
			BT_TRACE_LEVEL_NONE->0  BT_TRACE_LEVEL_ERROR->1
			BT_TRACE_LEVEL_WARNING->2   BT_TRACE_LEVEL_DEBUG->3
******************************************************************************/
#define BT_DEBUG printf
#define BT_PBUF_TRACE_LEVEL 1
#define BT_MEMORY_TRACE_LEVEL 1
#define BT_SNOOP_TRACE_LEVEL 1
#define BT_TRANSPORT_TRACE_LEVEL 3
#define BT_VENDOR_TRACE_LEVEL 3
#define BT_HCI_TRACE_LEVEL 3
#define BT_L2CAP_TRACE_LEVEL 3
#define BT_SDP_TRACE_LEVEL 3
#define BT_RFCOMM_TRACE_LEVEL 3
#define BT_HFP_TRACE_LEVEL 3
#define BT_HSP_TRACE_LEVEL 3
#define BT_OBEX_TRACE_LEVEL 3
#define BT_PBAP_TRACE_LEVEL 3
#define BT_MAP_TRACE_LEVEL 3
#define BT_DID_TRACE_LEVEL 3
#define BT_SPP_TRACE_LEVEL 3
#define BT_AVDTP_TRACE_LEVEL 3
#define BT_AVCTP_TRACE_LEVEL 3
#define BT_A2DP_TRACE_LEVEL 3
#define BT_AVRCP_TRACE_LEVEL 3




/***********   memory & memory pool option ************************************/
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
#define MEMP_NUM_SPP 7
#define MEMP_NUM_PBAP 7
#define MEMP_NUM_BT_PBUF 3

#define MEM_ALIGNMENT 4
#define MEM_LIBC_MALLOC 1

#define PBUF_POOL_BUFSIZE               MEM_ALIGN_SIZE(1024)
#define BT_PACK_END	__attribute__ ((packed))



/***********   bluetooth function option **************************************/
#define BT_CLASSICAL_ENABLE 1
#define BT_BLE_ENABLE 1
/** BT_PBUF_TRANSPORT_H2 = 0x01,BT_PBUF_TRANSPORT_H4 = 0x02,BT_PBUF_TRANSPORT_H5 = 0x03,BT_PBUF_TRANSPORT_BCSP = 0x04,*/
#define BT_ENABLE_SNOOP 1
#define BT_TRANSPORT_TYPE 0x02
#define BT_ENABLE_SSP 1
/* IO_CAP_DISPLAY_ONLY->0x00 IO_CAP_DISPLAY_YES_NO->0x01 IO_CAP_KEYBOARD_ONLY->0x02 IO_CAP_NOINPUT_OUTPUT->0x03 */
#define BT_SSP_IOCAP_CONF 1
#define BT_CLASS_OF_DEVICE 0x200408;
#define BT_LOCAL_NAME "BT_DEMO"
#define BT_PIN_CODE "0000"
#define BT_TIMER_COUNT 64 /* TIMER COUNT */
#define BT_BAUDRATE_1 921600


#define PROFILE_DID_ENABLE 0
#define PROFILE_HFP_ENABLE 0
#define PROFILE_SPP_ENABLE 1
#define PROFILE_PBAP_ENABLE 0
#define PROFILE_A2DP_ENABLE 0
#define PROFILE_AVRCP_ENABLE 0


#define SBC_SNK_MIN_BITPOOL 2
#define SBC_SNK_MAX_BITPOOL 53

/* HCI_HOST_MAX_NUM_ACL: The maximum number of ACL packets that the host can buffer */
#define HCI_HOST_MAX_NUM_ACL 8 //TODO: Should be equal to PBUF_POOL_SIZE/2??? */
/* HCI_HOST_ACL_MAX_LEN: The maximum size of an ACL packet that the host can buffer */
#define HCI_HOST_ACL_MAX_LEN (RFCOMM_N + 14) /* Default: RFCOMM MFS + ACL header size, L2CAP header size, 
                                                RFCOMM header size and RFCOMM FCS size */
/* HCI_PACKET_TYPE: The set of packet types which may be used on the connection. In order to 
   maximize packet throughput, it is recommended that RFCOMM should make use of the 3 and 5 
   slot baseband packets.*/
#define HCI_PACKET_TYPE 0xCC18 /* Default DM1, DH1, DM3, DH3, DM5, DH5 */
/* HCI_ALLOW_ROLE_SWITCH: Tells the host controller whether to accept a Master/Slave switch 
   during establishment of a connection */
#define HCI_ALLOW_ROLE_SWITCH 1 /* Default 1 */
/* HCI_FLOW_QUEUEING: Control if a packet should be queued if the host controller is out of 
   bufferspace for outgoing packets. Only the first packet sent when out of credits will be 
   queued */
#define HCI_FLOW_QUEUEING 0 /* Default: 0 */

/* ---------- L2CAP options ---------- */
/* L2CAP_HCI: Option for including HCI to access the Bluetooth baseband capabilities */
#define L2CAP_HCI 1 //TODO: NEEDED?
/* L2CAP_CFG_QOS: Control if a flow specification similar to RFC 1363 should be used */
#define L2CAP_CFG_QOS 0
/* L2CAP_MTU: Maximum transmission unit for L2CAP packet payload (min 48) */
#define L2CAP_MTU (RFCOMM_N + 6)/* Default for this implementation is RFCOMM MFS + RFCOMM header size and 
				   RFCOMM FCS size while the L2CAP default is 672 */
/* L2CAP_OUT_FLUSHTO: For some networking protocols, such as many real-time protocols, guaranteed delivery
   is undesirable. The flush time-out value SHALL be set to its default value 0xffff for a reliable L2CAP 
   channel, and MAY be set to other values if guaranteed delivery is not desired. (min 1) */
#define L2CAP_OUT_FLUSHTO 0xFFFF /* Default: 0xFFFF. Infinite number of retransmissions (reliable channel)
				    The value of 1 implies no retransmissions at the Baseband level 
				    should be performed since the minimum polling interval is 1.25 ms.*/ 
/* L2CAP_RTX: The Responsive Timeout eXpired timer is used to terminate
   the channel when the remote endpoint is unresponsive to signalling
   requests (min 1s, max 60s) */
#define L2CAP_RTX 60
/* L2CAP_ERTX: The Extended Response Timeout eXpired timer is used in
   place of the RTC timer when a L2CAP_ConnectRspPnd event is received
   (min 60s, max 300s) */
#define L2CAP_ERTX 300
/* L2CAP_MAXRTX: Maximum number of Request retransmissions before
   terminating the channel identified by the request. The decision
   should be based on the flush timeout of the signalling link. If the
   flush timeout is infinite, no retransmissions should be performed */
#define L2CAP_MAXRTX 0
/* L2CAP_CFG_TO: Amount of time spent arbitrating the channel parameters
   before terminating the connection (max 120s) */  
#define L2CAP_CFG_TO 30

/* ---------- SDP options ---------- */

#define RFCOMM_SPP_SERVER_CHNL 8
#define RFCOMM_HFP_HF_CHNL 9
/* ---------- RFCOMM options ---------- */
/* RFCOMM_N: Maximum frame size for RFCOMM segments (min 23, max 32767)*/
#define RFCOMM_N ((576 * 2) + 8) /* Default: Worst case byte stuffed PPP packet size + 
					   non-compressed PPP header size and FCS size */
/* RFCOMM_K: Initial amount of credits issued to the peer (min 0, max 7) */
#define RFCOMM_K 0

#define RFCOMM_RK_MIN 4
#define RFCOMM_RK_INIT 7
#define RFCOMM_RK_MAX 10
/* RFCOMM_TO: Acknowledgement timer (T1) and response timer for multiplexer control channel (T2).
   T1 is the timeout for frames sent with the P/F bit set to 1 (SABM and DISC) and T2 is the timeout
   for commands sent in UIH frames on DLCI 0 (min 10s, max 60s) */
#define RFCOMM_TO 20
/* RFCOMM_FLOW_QUEUEING: Control if a packet should be queued if a channel is out of credits for 
   outgoing packets. Only the first packet sent when out of credits will be queued */
#define RFCOMM_FLOW_QUEUEING 1 /* Default: 0 */
#define RFCOMM_FLOW_QUEUEING_SIZE 32


/***********   misc config  ***************************************************/
typedef uintptr_t mem_ptr_t;


#endif

