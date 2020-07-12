/******************************************************************************
  * @file           bt_avdtp.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt avdtp header file
******************************************************************************/

#ifndef BT_AVDTP_H_H_H
#define BT_AVDTP_H_H_H

#include "bt_common.h"
#include "bt_l2cap.h"

typedef enum
{
    AVDTP_SINGLE_PACKET= 0,
    AVDTP_START_PACKET,
    AVDTP_CONTINUE_PACKET,
    AVDTP_END_PACKET
} avdtp_packet_type_e;

typedef enum
{
    AVDTP_CMD_MSG = 0,
    AVDTP_GENERAL_REJECT_MSG,
    AVDTP_RESPONSE_ACCEPT_MSG,
    AVDTP_RESPONSE_REJECT_MSG
} avdtp_message_type_e;

typedef enum
{
    AVDTP_AUDIO = 0,
    AVDTP_VIDEO,
    AVDTP_MULTIMEDIA
} avdtp_media_type_e;

typedef enum
{
    AVDTP_CODEC_SBC             = 0x00,
    AVDTP_CODEC_MPEG_1_2_AUDIO  = 0x01,
    AVDTP_CODEC_MPEG_2_4_AAC    = 0x02,
    AVDTP_CODEC_ATRAC_FAMILY    = 0x04,
    AVDTP_CODEC_NON_A2DP        = 0xFF
} avdtp_media_codec_type_e;

typedef enum
{
    AVDTP_SOURCE = 0,
    AVDTP_SINK
} avdtp_sep_type_e;

typedef enum
{
    AVDTP_SERVICE_CATEGORY_INVALID_0 = 0x00,
    AVDTP_MEDIA_TRANSPORT = 0x01,
    AVDTP_REPORTING = 0x02,
    AVDTP_RECOVERY = 0x03,
    AVDTP_CONTENT_PROTECTION = 0x04, //4
    AVDTP_HEADER_COMPRESSION = 0x05, //5
    AVDTP_MULTIPLEXING = 0x06,       //6
    AVDTP_MEDIA_CODEC = 0x07,        //7
    AVDTP_DELAY_REPORTING = 0x08,    //8
} avdtp_service_category_e;


// Signal Identifier fields
typedef enum
{
    /* SPEC DEFINE */
    AVDTP_SI_NONE = 0x00,
    AVDTP_SI_DISCOVER = 0x01,
    AVDTP_SI_GET_CAPABILITIES,
    AVDTP_SI_SET_CONFIGURATION,
    AVDTP_SI_GET_CONFIGURATION,
    AVDTP_SI_RECONFIGURE, //5
    AVDTP_SI_OPEN,  //6
    AVDTP_SI_START, //7
    AVDTP_SI_CLOSE,
    AVDTP_SI_SUSPEND,
    AVDTP_SI_ABORT, //10
    AVDTP_SI_SECURITY_CONTROL,
    AVDTP_SI_GET_ALL_CAPABILITIES, //12
    AVDTP_SI_DELAYREPORT,
    /* USER DEFINE */
    AVDTP_SI_SIGNAL_CONNECT_IND,
    AVDTP_SI_SIGNAL_CONNECT_CFM,
    AVDTP_SI_SIGNAL_DISCON_IND,
    AVDTP_SI_SIGNAL_DISCON_CFM,
    AVDTP_SI_STREAM_CONNECT_IND,
    AVDTP_SI_STREAM_CONNECT_CFM,
    AVDTP_SI_STREAM_DISCON_IND,
    AVDTP_SI_STREAM_DISCON_CFM,
} avdtp_signal_identifier_e;


#define AVDTP_SIG_HDR_SIZE 2
#define AVDTP_DIS_PER_EP_SIZE 2
#define AVDTP_CAP_HDR_SIZE 2

struct avdtp_pcb_t
{
    struct avdtp_pcb_t *next; /* For the linked list */
    uint8_t stream_open;
    struct l2cap_pcb_t *avdtp_signal_l2cappcb; /* The L2CAP connection */
    struct l2cap_pcb_t *avdtp_media_l2cappcb;
    void *callback_arg;
    /* Callback */
};



typedef struct
{
    uint8_t *media_info;
    uint16_t media_info_len;
} adtvp_media_codec_capabilities_t;

typedef struct
{
    adtvp_media_codec_capabilities_t media_codec;
} avdtp_capabilities_t;

struct avdtp_sep_t
{
    struct avdtp_sep_t *next; /* For the linked list */
    uint8_t seid;
    uint8_t in_use;
    uint8_t service_categories_bitmap;

    avdtp_capabilities_t cap;
};

typedef err_t (*avdtp_event_handle)(struct avdtp_pcb_t *pcb,uint32_t msg_id,struct bt_pbuf_t *p);
typedef err_t (*avdtp_media_handle)(struct avdtp_pcb_t *pcb,struct bt_pbuf_t *p);
err_t avdtp_init(avdtp_event_handle avdtp_evt_handle,avdtp_media_handle avdtp_media_handle);
err_t avdtp_create_sep(uint8_t * media_codec_info, uint16_t media_codec_info_len);


extern struct avdtp_sep_t *avdtp_local_sep; /* List of all active avdtp seps */
extern struct avdtp_sep_t *avdtp_tmp_sep;      /* Only used for temporary storage. */
extern struct avdtp_pcb_t *avdtp_active_pcbs;  /* List of all active AVDTP PCBs */
extern struct avdtp_pcb_t *avdtp_tmp_pcb;

#define AVDTP_SEP_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define AVDTP_SEP_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(avdtp_tmp_sep = *pcbs; avdtp_tmp_sep != NULL; avdtp_tmp_sep = avdtp_tmp_sep->next) { \
                               if(avdtp_tmp_sep->next != NULL && avdtp_tmp_sep->next == npcb) { \
                                  avdtp_tmp_sep->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)

#define AVDTP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define AVDTP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(avdtp_tmp_pcb = *pcbs; avdtp_tmp_pcb != NULL; avdtp_tmp_pcb = avdtp_tmp_pcb->next) { \
                               if(avdtp_tmp_pcb->next != NULL && avdtp_tmp_pcb->next == npcb) { \
                                  avdtp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)


#endif


