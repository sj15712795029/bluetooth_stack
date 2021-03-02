/******************************************************************************
  * @file           bt_spp.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt spp header file
******************************************************************************/

#ifndef BT_SPP_H_H_H
#define BT_SPP_H_H_H


#include "bt_common.h"

#if PROFILE_SPP_ENABLE

#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"

typedef struct
{
    void (*spp_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*spp_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*spp_data_ind)(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len);
} spp_cbs_t;


typedef enum
{
    SPP_IDLE = 0,
    SPP_W2_SDP_CONNECTED,
    SPP_W2_SDP_QUERY_RFCOMM_CHANNEL,
    SPP_W4_SDP_QUERY_COMPLETE,
    SPP_W2_SDP_DISCONNECTD,
    SPP_W4_SDP_DISCONNECTD,
    SPP_W2_RFCOMM_CONNECTED,
    SPP_W4_RFCOMM_CONNECTED,

    SPP_W2_SERVER_CN_CONNECTED,
    SPP_W4_SERVER_CN_CONNECTED,

    SPP_SERVER_CN_CONNECTED,

    SPP_W2_DISCONNECT_RFCOMM,
    SPP_W4_RFCOMM_DISCONNECTED,
    SPP_W4_RFCOMM_DISCONNECTED_AND_RESTART,
    SPP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN
} spp_state_e;

struct spp_pcb_t
{
    struct spp_pcb_t *next; /* For the linked list */
    rfcomm_pcb_t *rfcommpcb;
    sdp_pcb_t *sdppcb;
    l2cap_pcb_t*l2cappcb;

    uint8_t remote_cn;

    spp_state_e state;
    struct bd_addr_t remote_addr;
};
err_t spp_init(spp_cbs_t *cb);
err_t spp_connect(struct bd_addr_t *addr);
err_t spp_disconnect(struct bd_addr_t *addr);
err_t spp_send_data(struct bd_addr_t *addr,uint8_t *data,uint16_t data_len);

extern struct spp_pcb_t *spp_active_pcbs;  /* List of all active SPP PCBs */
extern struct spp_pcb_t *spp_tmp_pcb;

#define SPP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define SPP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(spp_tmp_pcb = *pcbs; spp_tmp_pcb != NULL; spp_tmp_pcb = spp_tmp_pcb->next) { \
                               if(spp_tmp_pcb->next != NULL && spp_tmp_pcb->next == npcb) { \
                                  spp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)

#endif

#endif

