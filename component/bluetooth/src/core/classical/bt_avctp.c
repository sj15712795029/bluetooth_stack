/******************************************************************************
  * @file           bt_avctp.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt avctp source file
******************************************************************************/

#include "bt_avctp.h"


struct avctp_pcb_t *avctp_active_pcbs;  /* List of all active AVDTP PCBs */
struct avctp_pcb_t *avctp_tmp_pcb;

avctp_event_handle avctp_event_handler;
avctp_data_handle avctp_data_handler;

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



struct avctp_pcb_t *avctp_new()
{
    struct avctp_pcb_t *avctp_pcb;

    avctp_pcb = bt_memp_malloc(MEMP_AVCTP_PCB);
    if(avctp_pcb != NULL)
    {
        memset(avctp_pcb, 0, sizeof(struct avctp_pcb_t));
        return avctp_pcb;
    }

    BT_AVCTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void avctp_close(struct avctp_pcb_t *avctp_pcb)
{
    if(avctp_pcb != NULL)
    {
        bt_memp_free(MEMP_AVCTP_PCB,avctp_pcb);
        avctp_pcb = NULL;
    }
}

struct avctp_pcb_t *avctp_get_active_pcb( struct bd_addr_t *bdaddr)
{
    struct avctp_pcb_t *avctp_pcb;
    for(avctp_pcb = avctp_active_pcbs; avctp_pcb != NULL; avctp_pcb = avctp_pcb->next)
    {
        if(bd_addr_cmp(&(avctp_pcb->remote_bdaddr),bdaddr))
        {
            break;
        }
    }
    return avctp_pcb;
}


static err_t avctp_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    struct avctp_pcb_t *avctp_pcb = NULL;
    BT_AVCTP_TRACE_DEBUG("avctp_disconnect_ind psm 0x%x\n",l2cap_pcb->psm);
    if((avctp_pcb = avctp_get_active_pcb(&l2cap_pcb->remote_bdaddr)) != NULL)
    {
        avctp_event_handler(avctp_pcb,AVCTP_EVT_DISCON_IND,NULL);
        AVCTP_PCB_RMV(&avctp_active_pcbs,avctp_pcb);
        avctp_close(avctp_pcb);
    }
    l2cap_close(l2cap_pcb);
    return BT_ERR_OK;
}

err_t avctp_data_input(void *arg,l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
    uint8_t *cmd_resp = p->payload;
    uint8_t local_transaction_label = (cmd_resp[0] >> 4) & 0xf;
    uint8_t packet_type = (cmd_resp[0] >> 2)&0x3;
    uint8_t cr = (cmd_resp[0] >> 1)&0x1;
    uint8_t ipid = cmd_resp[0] & 0x1;
    uint16_t pid = (cmd_resp[1] << 8) | cmd_resp[2];

	struct avctp_pcb_t *avctp_pcb =  avctp_get_active_pcb(&l2cappcb->remote_bdaddr);
	
    BT_AVCTP_TRACE_DEBUG("local_transaction_label %d\n", local_transaction_label);
    BT_AVCTP_TRACE_DEBUG("packet_type %d\n",packet_type);
    BT_AVCTP_TRACE_DEBUG("cr %d ipid %d\n", cr, ipid);
    BT_AVCTP_TRACE_DEBUG("pid 0x%x\n", pid);


    BT_AVCTP_TRACE_DEBUG("avctp_data_input: p->len == %d p->tot_len == %d\n", p->len, p->tot_len);
    bt_hex_dump(p->payload,p->len);

    bt_pbuf_header(p, -AVCTP_NON_FRAG_HDR_SIZE);


    avctp_data_handler(avctp_pcb,p);

    return BT_ERR_OK;
}

static err_t avctp_connect_ind(void *arg,l2cap_pcb_t *l2cap_pcb, err_t err)
{
    struct avctp_pcb_t *avctp_pcb;
    BT_AVCTP_TRACE_DEBUG("avctp_connect_ind\n");

    avctp_pcb = avctp_new();
    avctp_pcb->l2cappcb= l2cap_pcb;
    memcpy(&avctp_pcb->remote_bdaddr,&l2cap_pcb->remote_bdaddr,BD_ADDR_LEN);
    AVCTP_PCB_REG(&avctp_active_pcbs,avctp_pcb);
    l2cap_register_disconnect_ind(avctp_pcb->l2cappcb, avctp_disconnect_ind);
    l2cap_register_recv(avctp_pcb->l2cappcb, avctp_data_input);
    avctp_event_handler(avctp_pcb,AVCTP_EVT_CONNECT_IND,NULL);
    return BT_ERR_OK;
}

err_t avctp_init(avctp_event_handle avctp_evt_handle,avctp_data_handle avctp_data_handle)
{
    avctp_event_handler = avctp_evt_handle;
    avctp_data_handler = avctp_data_handle;

    l2cap_register_connect_ind(AVCTP_PSM, avctp_connect_ind);
    return BT_ERR_OK;
}

err_t avctp_datawrite(struct avctp_pcb_t *pcb, struct bt_pbuf_t *p,uint16_t pid)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;
    struct bt_pbuf_t *avc_hdr;

    if((avc_hdr = bt_pbuf_alloc(BT_PBUF_RAW, AVCTP_NON_FRAG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        BT_AVCTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }


    cmd = avc_hdr->payload;
    cmd[cmd_pos++] = ((pcb->local_transaction_label++) << 4) |(AVCTP_SINGLE_PACKET << 2) | (AVCTP_COMMAND_FRAME << 1) | 0;;
    cmd[cmd_pos++] = pid >> 8;
    cmd[cmd_pos++] = pid & 0x00FF;

    bt_pbuf_chain(avc_hdr, p);

    l2cap_datawrite(pcb->l2cappcb, avc_hdr);
    bt_pbuf_dechain(avc_hdr);
    bt_pbuf_free(avc_hdr);
    return BT_ERR_OK;
}


