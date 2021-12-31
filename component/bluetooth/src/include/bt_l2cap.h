/******************************************************************************
  * @file           bt_l2cap.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt l2cap header file
******************************************************************************/

#ifndef BT_L2CAP_H_H_H
#define BT_L2CAP_H_H_H

#include "bt_common.h"
#include "bt_hci.h"
#include "bt_pbuf.h"


/* Protocol and service multiplexor */
#define SDP_PSM 0x0001
#define RFCOMM_PSM 0x0003
#define TCS_BIN_PSM 0x0005
#define TCS_BIN_CORDLESS_PSM 0x0007
#define BNEP_PSM 0x000F
#define HID_CONTROL_PSM 0x0011
#define HID_INTERRUPT_PSM 0x0013
#define UPNP_PSM 0x0015
#define AVCTP_PSM 0x0017
#define AVDTP_PSM 0x0019
#define AVCTP_BROWSING_PSM 0x001B
#define UDI_C_PLANE 0x001D
#define ATT_PSM 0x001F
#define DSP_3_PSM 0x0021
#define LE_PSM_IPSP_PSM 0x0023
#define OTS_PSM 0x0025
#define EATT_PSM 0x0027


/* Packet header lengths */
#define L2CAP_HDR_LEN 4
#define L2CAP_SIGHDR_LEN 4
#define L2CAP_CFGOPTHDR_LEN 2

/* Signals sizes */
#define L2CAP_CONN_REQ_SIZE 4
#define L2CAP_CONN_RSP_SIZE 8
#define L2CAP_CFG_RSP_SIZE 6
#define L2CAP_INFO_MTU_RSP_SIZE 8
#define L2CAP_INFO_EXFEATURE_RSP_SIZE 8
#define L2CAP_INFO_FIXED_CHNL_RSP_SIZE 12
#define L2CAP_DISCONN_RSP_SIZE 4

#define L2CAP_CFG_REQ_SIZE 4

#define L2CAP_DISCONN_REQ_SIZE 4
#define L2CAP_CMD_REJ_SIZE 2

/* Signal codes */
#define L2CAP_CMD_REJ 0x01
#define L2CAP_CONN_REQ 0x02
#define L2CAP_CONN_RSP 0x03
#define L2CAP_CFG_REQ 0x04
#define L2CAP_CFG_RSP 0x05
#define L2CAP_DISCONN_REQ 0x06
#define L2CAP_DISCONN_RSP 0x07
#define L2CAP_ECHO_REQ 0x08
#define L2CAP_ECHO_RSP 0x09
#define L2CAP_INFO_REQ 0x0A
#define L2CAP_INFO_RSP 0x0B
#define L2CAP_CREATE_CHANNEL_REQ 0x0C
#define L2CAP_CREATE_CHANNEL_RSP 0x0D
#define L2CAP_MOVE_CHANNEL_REQ 0x0E
#define L2CAP_MOVE_CHANNEL_RSP 0x0F
#define L2CAP_MOVE_CHANNEL_CONFIRMATION_REQ 0x10
#define L2CAP_MOVE_CHANNEL_CONFIRMATION_RSP 0x11
#define L2CAP_CONN_PARAM_UPDATE_REQ 0x12
#define L2CAP_CONN_PARAM_UPDATE_RSP 0x13
#define L2CAP_LE_CREDIT_BASED_CONN_REQ 0x14
#define L2CAP_LE_CREDIT_BASED_CONN_RSP 0x15
#define L2CAP_FLOW_CONTROL_CREDIT_IND 0x16
#define L2CAP_CREDIT_BASED_CONN_REQ 0x17
#define L2CAP_CREDIT_BASED_CONN_RSP 0x18
#define L2CAP_CREDIT_BASED_RECONFIGURE_REQ 0x19
#define L2CAP_CREDIT_BASED_RECONFIGURE_RSP 0x1A

/* Permanent channel identifiers */
#define L2CAP_NULL_CID 0x0000
#define L2CAP_SIG_CID 0x0001
#define L2CAP_CONNLESS_CID 0x0002
#define L2CAP_AMP_MANAGER_CID 0x0003
#define L2CAP_ATT_CID 0x0004
#define L2CAP_L2_SIG_CID 0x0005
#define L2CAP_SM_CID 0x0006
#define L2CAP_BREDR_SM_CID 0x0007

/* Channel identifiers values */
#define L2CAP_MIN_CID 0x0040
#define L2CAP_MAX_CID 0xFFFF

/* Configuration types */
#define L2CAP_CFG_MTU 0x01
#define L2CAP_FLUSHTO 0x02
#define L2CAP_QOS 0x03
#define L2CAP_CFG_RETRANSMISSION_FLOW_CONTROL 0x04
#define L2CAP_CFG_FCS 0x05
#define L2CAP_CFG_EXT_FLOW 0x06
#define L2CAP_CFG_EXT_WINDOW_SIZE 0x07

/* Info type */
#define L2CAP_CONLESS_MTU 0x01
#define L2CAP_EXFEATURE_SUPPORT 0x02
#define L2CAP_FIXED_CHNL_SUPPORT 0x03

/* Configuration types length */
#define L2CAP_MTU_LEN 2
#define L2CAP_FLUSHTO_LEN 2
#define L2CAP_QOS_LEN 22
#define L2CAP_RETRA_FLOW_CTL_LEN 9
#define L2CAP_FCS_LEN 1
#define L2CAP_EXT_FLOW_SPEC_LEN 16
#define L2CAP_EXT_WINDOWS_SIZE 2

/* Configuration response types */
#define L2CAP_CFG_SUCCESS 0x0000
#define L2CAP_CFG_UNACCEPT 0x0001
#define L2CAP_CFG_REJ 0x0002
#define L2CAP_CFG_UNKNOWN 0x0003
#define L2CAP_CFG_PENDING 0x0004
#define L2CAP_CFG_FLOW_SPEC_REJ 0x0005
#define L2CAP_CFG_TIMEOUT 0xEEEE

/* QoS types */
#define L2CAP_QOS_NO_TRAFFIC 0x00
#define L2CAP_QOS_BEST_EFFORT 0x01
#define L2CAP_QOS_GUARANTEED 0x02

/* Command reject reasons */
#define L2CAP_CMD_NOT_UNDERSTOOD 0x0000
#define L2CAP_MTU_EXCEEDED 0x0001
#define L2CAP_INVALID_CID 0x0002

/* Connection response results */
#define L2CAP_CONN_SUCCESS 0x0000
#define L2CAP_CONN_PND 0x0001
#define L2CAP_CONN_REF_PSM 0x0002
#define L2CAP_CONN_REF_SEC 0x0003
#define L2CAP_CONN_REF_RES 0x0004
#define L2CAP_CONN_CFG_TO 0x0005 /* Implementation specific result */
#define L2CAP_CONN_REF_CID 0x0006
#define L2CAP_CONN_REF_HAS_CID 0x0007

/* Extended features mask bits
*/
#define L2CAP_EXTFEA_FC             0x00000001    /* Flow Control Mode   (Not Supported)    */
#define L2CAP_EXTFEA_RTRANS         0x00000002    /* Retransmission Mode (Not Supported)    */
#define L2CAP_EXTFEA_QOS            0x00000004
#define L2CAP_EXTFEA_ENH_RETRANS    0x00000008    /* Enhanced retransmission mode           */
#define L2CAP_EXTFEA_STREAM_MODE    0x00000010    /* Streaming Mode                         */
#define L2CAP_EXTFEA_NO_CRC         0x00000020    /* Optional FCS (if set No FCS desired)   */
#define L2CAP_EXTFEA_EXT_FLOW_SPEC  0x00000040    /* Extended flow spec                     */
#define L2CAP_EXTFEA_FIXED_CHNLS    0x00000080    /* Fixed channels                         */
#define L2CAP_EXTFEA_EXT_WINDOW     0x00000100    /* Extended Window Size                   */
#define L2CAP_EXTFEA_UCD_RECEPTION  0x00000200    /* Unicast Connectionless Data Reception  */
#define L2CAP_EXTFEA_ENH_CREDIT_BASE_FC 0x00000400 /* Enhanced Credit Based Flow Control Mode */
#define L2CAP_EXTFEA_SUPPORTED_MASK (L2CAP_EXTFEA_ENH_RETRANS  | L2CAP_EXTFEA_NO_CRC | L2CAP_EXTFEA_FIXED_CHNLS |L2CAP_EXTFEA_FIXED_CHNLS | L2CAP_EXTFEA_UCD_RECEPTION)

/* L2CAP mode */
#define L2CAP_MODE_BASIC	0x00
#define L2CAP_MODE_RETRANS	0x01
#define L2CAP_MODE_FLOWCTL	0x02
#define L2CAP_MODE_ERTM		0x03
#define L2CAP_MODE_STREAMING	0x04

/* Echo response results */
#define L2CAP_ECHO_RCVD 0x00
#define L2CAP_ECHO_TO 0x01

/* Info request results */
#define L2CAP_INFO_REQ_SUCCESS 0x0

/* L2CAP segmentation */
#define L2CAP_ACL_START 0x02
#define L2CAP_ACL_CONT 0x01

/* L2CAP config default parameters */
#define L2CAP_CFG_DEFAULT_INMTU 672 /* Two Baseband DH5 packets (2*341=682) minus the Baseband ACL 
				       headers (2*2=4) and L2CAP header (6) */
#define L2CAP_CFG_DEFAULT_OUTFLUSHTO 0xFFFF

/* L2CAP configuration parameter masks */
#define L2CAP_CFG_IR 0x01
#define L2CAP_CFG_IN_SUCCESS 0x02
#define L2CAP_CFG_OUT_SUCCESS 0x04
#define L2CAP_CFG_OUT_REQ 0x08

#pragma pack (1) 

typedef struct 
{
    uint16_t len;
    uint16_t cid;
}l2cap_hdr_t;

typedef struct 
{
    uint8_t code;
    uint8_t id;
    uint16_t len;
}l2cap_sig_hdr_t;

typedef struct 
{
    uint8_t type;
    uint8_t len;
}l2cap_cfgopt_hdr_t;
#pragma pack () 


enum l2cap_state_e
{
    L2CAP_CLOSED, L2CAP_LISTEN, W4_L2CAP_CONNECT_RSP, W4_L2CA_CONNECT_RSP, L2CAP_CONFIG,
    L2CAP_OPEN, W4_L2CAP_DISCONNECT_RSP, W4_L2CA_DISCONNECT_RSP
};

typedef struct _l2cap_acl_link_t
{
    struct _l2cap_acl_link_t *next;
    struct bd_addr_t bdaddr;
}l2cap_acl_link_t;

/* This structure is used to represent L2CAP signals. */
typedef struct _l2cap_sig_t
{
    struct _l2cap_sig_t *next;    /* for the linked list, used when putting signals
				on a queue */
    struct bt_pbuf_t *p;          /* buffer containing data + L2CAP header */
    uint16_t sigid; /* Identification */
    uint16_t ertx; /* extended response timeout expired */
    uint8_t rtx; /* response timeout expired */
    uint8_t nrtx; /* number of retransmissions */
}l2cap_sig_t;

typedef struct 
{
    uint16_t inmtu; /* Maximum transmission unit this channel can accept */
    uint16_t outmtu; /* Maximum transmission unit that can be sent on this channel */
    uint16_t influshto; /* In flush timeout */
    uint16_t outflushto; /* Out flush timeout */

    struct bt_pbuf_t *opt; /* Any received non-hint unknown option(s) or option(s) with unacceptable parameters
		       will be temporarily stored here */

    uint8_t cfgto; /* Configuration timeout */
    uint8_t l2capcfg; /* Bit 1 indicates if we are the initiator of this connection
		  * Bit 2 indicates if a successful configuration response has been received
		  * Bit 3 indicates if a successful configuration response has been sent
		  * Bit 4 indicates if an initial configuration request has been sent
		  */
}l2cap_cfg_t;


struct _l2cap_pcb_t;
typedef err_t (* l2ca_connect_ind_cb)(void *arg, struct _l2cap_pcb_t *pcb, err_t err);
typedef err_t (* l2ca_disconnect_ind_cb)(void *arg, struct _l2cap_pcb_t *pcb, err_t err);
typedef err_t (* l2ca_connect_cfm_cb)(void *arg, struct _l2cap_pcb_t *pcb, uint16_t result, uint16_t status);
typedef err_t (* l2ca_timeout_ind_cb)(void *arg, struct _l2cap_pcb_t *newpcb, err_t err);
typedef err_t (* l2ca_recv_cb)(void *arg, struct _l2cap_pcb_t *pcb, struct bt_pbuf_t *p, err_t err);
typedef err_t (* l2ca_disconnect_cfm_cb)(void *arg, struct _l2cap_pcb_t *pcb);
typedef err_t (* l2ca_ping_cb)(void *arg, struct _l2cap_pcb_t *pcb, uint8_t result);


typedef struct _l2cap_pcb_t
{
    struct _l2cap_pcb_t *next; /* For the linked list */

	uint8_t conn_role;
	
    enum l2cap_state_e state; /* L2CAP state */

    void *callback_arg;

    uint16_t scid; /* Source CID */
    uint16_t dcid; /* Destination CID */

    uint16_t psm; /* Protocol/Service Multiplexer */
	uint16_t fixed_cid;
	
    uint16_t ursp_id; /* Signal id to respond to */
    uint8_t encrypt; /* encryption mode */

    l2cap_sig_t *unrsp_sigs;  /* List of sent but unresponded signals */

    struct bd_addr_t remote_bdaddr;

    l2cap_cfg_t cfg; /* Configuration parameters */

    uint8_t mode;
    /* Upper layer to L2CAP confirmation functions */

    /* Function to be called when a connection has been set up */
	l2ca_connect_cfm_cb l2ca_connect_cfm;
    /* Function to be called when a connection has been closed */
	l2ca_disconnect_cfm_cb l2ca_disconnect_cfm;
    /* Function to be called when a echo reply has been received */
	l2ca_ping_cb l2ca_ping;

    /* L2CAP to upper layer indication functions */

    /* Function to be called when a connection indication event occurs */
	l2ca_connect_ind_cb l2ca_connect_ind;
    /* Function to be called when a disconnection indication event occurs */
	l2ca_disconnect_ind_cb l2ca_disconnect_ind;
    /* Function to be called when a timeout indication event occurs */
    l2ca_timeout_ind_cb l2ca_timeout_ind;
    /* Function to be called when a L2CAP connection receives data */
	l2ca_recv_cb l2ca_recv;
}l2cap_pcb_t;



typedef struct _l2cap_seg_t
{
    struct _l2cap_seg_t *next; /* For the linked list */

    struct bd_addr_t bdaddr;

    struct bt_pbuf_t *p;          /* Buffer containing data + L2CAP header */
    uint16_t len;               /* The L2CAP length of this segment */
    l2cap_hdr_t *l2caphdr;  /* The L2CAP header */
    l2cap_pcb_t *pcb; /* The L2CAP Protocol Control Block */
}l2cap_seg_t;




typedef struct _l2cap_pcb_listen_t
{
    struct _l2cap_pcb_listen_t *next; /* for the linked list */

    enum l2cap_state_e state; /* L2CAP state */

    void *callback_arg;

    uint16_t psm; /* Protocol/Service Multiplexer */
    /* Function to call when a connection request has been received
       from a remote device. */
    l2ca_connect_ind_cb l2ca_connect_ind;
}l2cap_pcb_listen_t;


#define l2cap_psm(pcb) ((pcb)->psm)


/* Functions for interfacing with L2CAP */
void l2cap_init(void); /* Must be called first to initialize L2CAP */
void l2cap_deinit(void);
void l2cap_tmr(void); /* Must be called every 1s */
l2cap_pcb_t *l2cap_new(void);
err_t l2cap_close(l2cap_pcb_t *pcb);
err_t l2cap_register_connect_ind(uint8_t psm,l2ca_connect_ind_cb l2ca_connect_ind);
void l2cap_register_disconnect_ind(l2cap_pcb_t *pcb,l2ca_disconnect_ind_cb l2ca_disconnect_ind);
void l2cap_register_timeout_ind(l2cap_pcb_t *pcb,l2ca_timeout_ind_cb l2ca_timeout_ind);
void l2cap_register_recv(l2cap_pcb_t *pcb,l2ca_recv_cb l2ca_recv);
err_t l2cap_fixed_channel_register_recv(uint16_t cid,
							l2ca_connect_ind_cb l2ca_connect_ind,
							l2ca_disconnect_ind_cb l2ca_disconnect_ind,
							l2ca_recv_cb l2ca_recv);
err_t l2cap_connect_req(l2cap_pcb_t *pcb, struct bd_addr_t *bdaddr, uint16_t psm, uint8_t role_switch,
                        l2ca_connect_cfm_cb l2ca_connect_cfm);
err_t l2cap_ertm_connect_req(l2cap_pcb_t *pcb, struct bd_addr_t *bdaddr, uint16_t psm, uint8_t role_switch,
                             l2ca_connect_cfm_cb l2ca_connect_cfm);
err_t l2cap_disconnect_req(l2cap_pcb_t *pcb,l2ca_disconnect_cfm_cb l2ca_disconnect_cfm);
err_t l2cap_datawrite(l2cap_pcb_t *pcb, struct bt_pbuf_t *p);
err_t l2cap_fixed_channel_datawrite(l2cap_pcb_t *pcb, struct bt_pbuf_t *p,uint16_t cid);
err_t l2cap_ping(struct bd_addr_t *bdaddr, l2cap_pcb_t *tpcb,l2ca_ping_cb l2ca_ping);
void l2cap_acl_input(struct bt_pbuf_t *p, struct bd_addr_t *bdaddr);
void lp_connect_cfm(struct bd_addr_t *bdaddr, uint8_t encrypt_mode, err_t err);
void lp_connect_ind(struct bd_addr_t *bdaddr);
void lp_disconnect_ind(struct bd_addr_t *bdaddr);
void le_connect_handler(struct bd_addr_t *bdaddr,uint8_t conn_role);




#endif


