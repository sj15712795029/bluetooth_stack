/******************************************************************************
  * @file           bt_rfcomm.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt rfcomm header file
******************************************************************************/

#ifndef BT_RFCOMM_H_H_H
#define BT_RFCOMM_H_H_H

#include "bt_l2cap.h"




#define rfcomm_cn(pcb) ((pcb)->cn)
#define rfcomm_cl(pcb) ((pcb)->cl)
#define rfcomm_local_credits(pcb) ((pcb)->k)
#define rfcomm_remote_credits(pcb) ((pcb)->rk)
#define rfcomm_fc(pcb) ((pcb)->fc)
#define rfcomm_mfs(pcb) ((pcb)->n)



/* Control field values */
#define RFCOMM_SABM 0x3F
#define RFCOMM_UA 0x73
#define RFCOMM_DM 0x0F
#define RFCOMM_DM_PF 0x1F
#define RFCOMM_DISC 0x53
#define RFCOMM_UIH 0xEF
#define RFCOMM_UIH_PF 0xFF

/* Multiplexer message types */
#define RFCOMM_PN_CMD 0x83
#define RFCOMM_PN_RSP 0x81
#define RFCOMM_TEST_CMD 0x23
#define RFCOMM_TEST_RSP 0x21
#define RFCOMM_FCON_CMD 0xA3
#define RFCOMM_FCON_RSP 0xA1
#define RFCOMM_FCOFF_CMD 0x63
#define RFCOMM_FCOFF_RSP 0x61
#define RFCOMM_MSC_CMD 0xE3
#define RFCOMM_MSC_RSP 0xE1
#define RFCOMM_RPN_CMD 0x93
#define RFCOMM_RPN_RSP 0x91
#define RFCOMM_RLS_CMD 0x53
#define RFCOMM_RLS_RSP 0x51
#define RFCOMM_NSC_RSP 0x11

/* Length of RFCOMM hdr with 1 or 2 byte lengh field excluding credit */
#define RFCOMM_HDR_LEN_1 3
#define RFCOMM_HDR_LEN_2 4

/* Length of a multiplexer message */
#define RFCOMM_MSGHDR_LEN 2
#define RFCOMM_PNMSG_LEN 8
#define RFCOMM_MSCMSG_LEN 2
#define RFCOMM_RPNMSG_LEN 8
#define RFCOMM_NCMSG_LEN 1

/* Length of a frame */
#define RFCOMM_DM_LEN 4
#define RFCOMM_SABM_LEN 4
#define RFCOMM_DISC_LEN 4
#define RFCOMM_UA_LEN 4
#define RFCOMM_UIH_LEN 3
#define RFCOMM_UIHCRED_LEN 4

/* Default convergence layer */
#define RFCOMM_CL 0xF /* Credit based flow control */

/* Default port settings for a communication link */
#define RFCOMM_COM_BR 0x03 /* Baud rate (9600 bit/s)*/
#define RFCOMM_COM_CFG 0x03 /* Data bits (8 bits), stop bits (1), parity (no parity) 
			       and parity type */
#define RFCOMM_COM_FC 0x00 /* Flow control (no flow ctrl) */
#define RFCOMM_COM_XON 0x00 /* No flow control (default DC1) */
#define RFCOMM_COM_XOFF 0x00 /* No flow control (default DC3) */

/* FCS calc */
#define RFCOMM_CODE_WORD 0xE0 /* pol = x8+x2+x1+1 */
#define RFCOMM_CRC_CHECK_LEN 3
#define RFCOMM_UIHCRC_CHECK_LEN 2

/* RFCOMM configuration parameter masks */
#define RFCOMM_CFG_IR 0x01
#define RFCOMM_CFG_FC 0x02
#define RFCOMM_CFG_MSC_OUT 0x04
#define RFCOMM_CFG_MSC_IN 0x08

enum rfcomm_state_e
{
    RFCOMM_CLOSED,
    RFCOMM_LISTEN,
    W4_RFCOMM_MULTIPLEXER,
    W4_RFCOMM_SABM_RSP,
    RFCOMM_CFG,
    RFCOMM_OPEN,
    W4_RFCOMM_DISC_RSP
};

#pragma pack (1) 

/* The RFCOMM frame header */
typedef struct
{
    uint8_t addr;
    uint8_t ctrl;
    uint16_t len;
    uint8_t k;
}  rfcomm_hdr_t;

typedef struct
{
    uint8_t type;
    uint8_t len;
}  rfcomm_msg_hdr_t;

typedef struct
{
    uint8_t dlci; /* Data link connection id */
    uint8_t i_cl; /* Type frame for information and Convergece layer to use */
    uint8_t p; /* Priority */
    uint8_t t; /* Value of acknowledgement timer */
    uint16_t n; /* Maximum frame size */
    uint8_t na; /* Maximum number of retransmissions */
    uint8_t k; /* Initial credit value */
}  rfcomm_pn_msg_t;

typedef struct
{
    uint8_t dlci; /* Data link connection id */
    uint8_t rs232; /* RS232 control signals */
}  rfcomm_msc_msg_t;

typedef struct
{
    uint8_t dlci; /* Data link connection id */
    uint8_t br; /* Baud Rate */
    uint8_t cfg; /* Data bits, Stop bits, Parity, Parity type */
    uint8_t fc; /* Flow control */
    uint8_t xon;
    uint8_t xoff;
    uint16_t mask;
}  rfcomm_rpn_msg_t;
#pragma pack () 


struct _rfcomm_pcb_t;
typedef err_t (* rfcomm_connected_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_accept_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_disconnected_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_pn_rsp_cbs)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_test_rsp_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_msc_rsp_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_rpn_rsp_cb)(void *arg, struct _rfcomm_pcb_t *pcb, err_t err);
typedef err_t (* rfcomm_recv_cb)(void *arg, struct _rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err);



/* The RFCOMM protocol control block */
typedef struct  _rfcomm_pcb_t
{
    struct _rfcomm_pcb_t *next; /* For the linked list */

    enum rfcomm_state_e state; /* RFCOMM state */

    l2cap_pcb_t *l2cappcb; /* The L2CAP connection */

    uint8_t cn; /* Channel number */

    uint8_t cl; /* Convergence layer */
    uint8_t p; /* Connection priority */
    uint16_t n; /* Maximum frame size */
    uint8_t k; /* Number of local credits:The number of packets we can send */

    uint8_t rk; /* Number of remote credits:The number of packets that the other party can send */

    uint8_t rfcommcfg; /* Bit 1 indicates if we are the initiator of this connection
		   * Bit 2 indicates if the flow control bit is set so that we are allowed to send data
		   * Bit 3 indicates if modem status has been configured for the incoming direction
		   * Bit 4 indicates if modem status has been configured for the outgoing direction
		   */

    uint16_t to; /* Frame and cmd timeout */

    uint8_t uih_in_fcs; /* Frame check sequence for uih frames (P/F bit = 0) */
    uint8_t uihpf_in_fcs; /* Frame check sequence for uih frames (P/F bit = 1) */
    uint8_t uih_out_fcs; /* Frame check sequence for uih frames (P/F bit = 0) */
    uint8_t uihpf_out_fcs; /* Frame check sequence for uih frames (P/F bit = 1) */

    uint8_t uih0_in_fcs; /* Frame check sequence for uih frames on the control channel (P/F bit = 0) */
    uint8_t uih0_out_fcs; /* Frame check sequence for uih frames on the control channel (P/F bit = 0) */

#if RFCOMM_FLOW_QUEUEING
    struct bt_pbuf_t *buf;
#endif
    void *callback_arg;

    /* RFCOMM Frame commands and responses */
    rfcomm_connected_cb connected;
    rfcomm_accept_cb accept;
    rfcomm_disconnected_cb disconnected;

    /* RFCOMM Multiplexer responses */
    rfcomm_pn_rsp_cbs pn_rsp;
    rfcomm_test_rsp_cb test_rsp;
    rfcomm_msc_rsp_cb msc_rsp;
    rfcomm_rpn_rsp_cb rpn_rsp;

    rfcomm_recv_cb recv;
} rfcomm_pcb_t;

/* The server channel */
typedef struct  _rfcomm_pcb_listen_t
{
    struct _rfcomm_pcb_listen_t *next; /* For the linked list */

    enum rfcomm_state_e state; /* RFCOMM state */

    uint8_t cn; /* Channel number */

    void *callback_arg;

    rfcomm_accept_cb accept;
} rfcomm_pcb_listen_t;



/* Functions for interfacing with RFCOMM: */

/* Lower layer interface to RFCOMM: */
err_t rfcomm_init(void); /* Must be called first to initialize RFCOMM */
void rfcomm_deinit(void);
void rfcomm_tmr(void); /* Must be called every 1s interval */
/* Application program's interface: */
rfcomm_pcb_t *rfcomm_new(l2cap_pcb_t *pcb);
void rfcomm_close(rfcomm_pcb_t *pcb);
void rfcomm_register_recv(rfcomm_pcb_t *pcb,rfcomm_recv_cb recv);
void rfcomm_register_disc(rfcomm_pcb_t *pcb,rfcomm_disconnected_cb disconnected);
err_t rfcomm_input(void *arg, l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err);
err_t rfcomm_connect(rfcomm_pcb_t *pcb, uint8_t cn,rfcomm_connected_cb connected);
err_t rfcomm_disconnect(rfcomm_pcb_t *pcb);
err_t rfcomm_listen(uint8_t cn, rfcomm_accept_cb connet_ind);
err_t rfcomm_pn(rfcomm_pcb_t *pcb,rfcomm_pn_rsp_cbs pn_rsp);
err_t rfcomm_test(rfcomm_pcb_t *pcb,rfcomm_test_rsp_cb test_rsp);
err_t rfcomm_msc(rfcomm_pcb_t *pcb, uint8_t fc,rfcomm_msc_rsp_cb msc_rsp);
err_t rfcomm_rpn(rfcomm_pcb_t *pcb, uint8_t br,rfcomm_rpn_rsp_cb rpn_rsp);
err_t rfcomm_uih(rfcomm_pcb_t *pcb, uint8_t cn, struct bt_pbuf_t *q);
err_t rfcomm_uih_credits(rfcomm_pcb_t *pcb, uint8_t credits, struct bt_pbuf_t *q);
err_t rfcomm_issue_credits(rfcomm_pcb_t *pcb, uint8_t credits);
err_t rfcomm_lp_disconnected(l2cap_pcb_t *pcb);

#endif

