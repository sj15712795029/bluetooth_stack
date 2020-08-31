/******************************************************************************
  * @file           bt_obex_client.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt obex client header file
******************************************************************************/

#ifndef BT_OBEX_CLIENT_H_H_H
#define BT_OBEX_CLIENT_H_H_H

#include "bt_common.h"
#include "bt_rfcomm.h"


#define OBEX_HEADER_MAX_SIZE 128
#define OBEX_CONNECT_FIELD_LEN 7
#define OBEX_GET_FIELD_LEN 3
#define OBEX_SETPATH_FIELD_LEN 5

#define OBEX_OPCODE_CONNECT                0x80
#define OBEX_OPCODE_DISCONNECT             0x81
#define OBEX_OPCODE_PUT                    0x82
#define OBEX_OPCODE_CLOSE                  0x82
#define OBEX_OPCODE_GET                    0x83
#define OBEX_OPCODE_SETPATH                0x85
#define OBEX_OPCODE_SESSION                0x87
#define OBEX_OPCODE_ABORT                  0xFF

#define OBEX_OPCODE_FINAL_BIT_MASK         0x80

#define OBEX_RESP_SUCCESS                  0xA0
#define OBEX_RESP_CONTINUE                 0x90
#define OBEX_RESP_BAD_REQUEST              0xC0
#define OBEX_RESP_UNAUTHORIZED             0xC1
#define OBEX_RESP_FORBIDDEN                0xC3
#define OBEX_RESP_NOT_FOUND                0xC4
#define OBEX_RESP_NOT_ACCEPTABLE           0xC6

#define OBEX_HEADER_BODY                           0x48
#define OBEX_HEADER_END_OF_BODY                    0x49
#define OBEX_HEADER_COUNT                          0xC0
#define OBEX_HEADER_NAME                           0x01
#define OBEX_HEADER_TYPE                           0x42
#define OBEX_HEADER_LENGTH                         0xC3
#define OBEX_HEADER_TIME_ISO_8601                  0x44
#define OBEX_HEADER_TIME_4_BYTE                    0xC4
#define OBEX_HEADER_DESCRIPTION                    0x05
#define OBEX_HEADER_TARGET                         0x46
#define OBEX_HEADER_HTTP                           0x47
#define OBEX_HEADER_WHO                            0x4A
#define OBEX_HEADER_OBJECT_CLASS                   0x4F
#define OBEX_HEADER_APPLICATION_PARAMETERS         0x4C
#define OBEX_HEADER_CONNECTION_ID                  0xCB
#define OBEX_HEADER_AUTHENTICATION_CHALLENGE       0x4D
#define OBEX_HEADER_AUTHENTICATION_RESPONSE        0x4E
#define OBEX_HEADER_SINGLE_RESPONSE_MODE           0x97
#define OBEX_HEADER_SINGLE_RESPONSE_MODE_PARAMETER 0x98


#define OBEX_VERSION                       0x13


#define OBEX_CONNECTION_ID_INVALID         0xFFFFFFFF

/* SRM header values */
#define OBEX_SRM_DISABLE                            0x00
#define OBEX_SRM_ENABLE                             0x01
#define OBEX_SRM_INDICATE                           0x02

/* SRMP header values */
#define OBEX_SRMP_NEXT                              0x00
#define OBEX_SRMP_WAIT                              0x01
#define OBEX_SRMP_NEXT_WAIT                         0x02


typedef struct
{
    void (*obex_client_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status,uint32_t cid);
    void (*obex_client_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*obex_client_data_ind)(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len,uint8_t status);
} obex_client_cbs_t;

struct obex_pcb_t
{
    struct obex_pcb_t *next; /* For the linked list */
    struct rfcomm_pcb_t *rfcommpcb;
    struct sdp_pcb_t *sdppcb;
    struct l2cap_pcb_t*l2cappcb;

	uint8_t last_opcode;
	uint8_t remote_cn;
	uint32_t connect_id;
	obex_client_cbs_t *obex_client_cbs;
    struct bd_addr_t remote_addr;
};

err_t obex_header_para_append(uint8_t hdr_id,uint8_t *hdr_data,uint8_t hdr_data_len);
err_t obex_client_connect(struct rfcomm_pcb_t *rfcommpcb,obex_client_cbs_t *cb,uint16_t mtu,uint8_t scn);
err_t obex_client_get(struct rfcomm_pcb_t *rfcommpcb);
err_t obex_client_setpath(struct rfcomm_pcb_t *rfcommpcb);

extern struct obex_pcb_t *obex_active_pcbs;  /* List of all active PBAP PCBs */
extern struct obex_pcb_t *obex_tmp_pcb;


#define OBEX_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define OBEX_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(obex_tmp_pcb = *pcbs; obex_tmp_pcb != NULL; obex_tmp_pcb = obex_tmp_pcb->next) { \
                               if(obex_tmp_pcb->next != NULL && obex_tmp_pcb->next == npcb) { \
                                  obex_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)
#endif

