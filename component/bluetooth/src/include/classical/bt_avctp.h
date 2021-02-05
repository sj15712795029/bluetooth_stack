/******************************************************************************
  * @file           bt_avctp.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt avctp header file
******************************************************************************/

#ifndef BT_AVCTP_H_H_H
#define BT_AVCTP_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"

#define AVCTP_NON_FRAG_HDR_SIZE 3

struct avctp_pcb_t
{
    struct avctp_pcb_t *next; /* For the linked list */
    l2cap_pcb_t *l2cappcb; /* The L2CAP connection */
    struct bd_addr_t remote_bdaddr;
    uint8_t local_transaction_label;

};

typedef enum {
    AVCTP_SINGLE_PACKET= 0,
    AVCTP_START_PACKET    ,
    AVCTP_CONTINUE_PACKET ,
    AVCTP_END_PACKET
} avctp_packet_type_t;

typedef enum {
    AVCTP_COMMAND_FRAME = 0,
    AVCTP_RESPONSE_FRAME    
} avctp_frame_type_t;

typedef enum
{
	AVCTP_EVT_CONNECT_IND,
	AVCTP_EVT_CONNECT_CFM,
	AVCTP_EVT_DISCON_IND,
	AVCTP_EVT_DISCON_CFM,    
} avctp_event_identifier_e;


typedef err_t (*avctp_event_handle)(struct avctp_pcb_t *avctp_pcb,uint32_t msg_id,struct bt_pbuf_t *p);
typedef err_t (*avctp_data_handle)(struct avctp_pcb_t *avctp_pcb,struct bt_pbuf_t *p);

err_t avctp_init(avctp_event_handle avctp_evt_handle,avctp_data_handle avctp_data_handle);
err_t avctp_datawrite(struct avctp_pcb_t *pcb, struct bt_pbuf_t *p,uint16_t pid);


#endif
