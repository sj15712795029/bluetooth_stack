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
    struct l2cap_pcb_t *l2cappcb; /* The L2CAP connection */
    void *callback_arg;
    uint8_t local_transaction_label;

    /* Callback */
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


typedef err_t (*avctp_recv)(struct bt_pbuf_t *p);
err_t avctp_init(void);
err_t avctp_datawrite(struct avctp_pcb_t *pcb, struct bt_pbuf_t *p,uint16_t pid);
err_t avctp_register_callback(avctp_recv recv);
extern struct avctp_pcb_t *avctp_active_pcbs;  /* List of all active AVDTP PCBs */
extern struct avctp_pcb_t *avctp_tmp_pcb;

#define AVCTP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define AVCTP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(avctp_tmp_pcb = *pcbs; avctp_tmp_pcb != NULL; avctp_tmp_pcb = avctp_tmp_pcb->next) { \
                               if(avctp_tmp_pcb->next != NULL && avctp_tmp_pcb->next == npcb) { \
                                  avctp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)


#endif
