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

struct avctp_pcb_t *temp_avcrp;

avctp_recv avctp_upper_handle;
struct avctp_pcb_t *
avctp_new()
{
    struct avctp_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_AVDTP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(struct avctp_pcb_t));
        return pcb;
    }

	        BT_AVCTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

    return NULL;
}

void
avctp_close(struct avctp_pcb_t *pcb)
{
    bt_memp_free(MEMP_AVDTP_PCB,pcb);
    pcb = NULL;
}

static err_t avctp_disconnect_ind(void *arg, struct l2cap_pcb_t *pcb, err_t err)
{
		        BT_AVCTP_TRACE_DEBUG("avctp_disconnect_ind psm 0x%x\n",pcb->psm);


    l2cap_close(pcb);
    return BT_ERR_OK;
}

err_t
avctp_input(void *arg, struct l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
    uint8_t *cmd_resp = p->payload;
    uint8_t local_transaction_label = (cmd_resp[0] >> 4) & 0xf;
    uint8_t packet_type = (cmd_resp[0] >> 2)&0x3;
    uint8_t cr = (cmd_resp[0] >> 1)&0x1;
    uint8_t ipid = cmd_resp[0] & 0x1;
    uint16_t pid = (cmd_resp[1] << 8) | cmd_resp[2];

			        BT_AVCTP_TRACE_DEBUG("local_transaction_label %d\n", local_transaction_label);
							        BT_AVCTP_TRACE_DEBUG("packet_type %d\n",packet_type);
											        BT_AVCTP_TRACE_DEBUG("cr %d ipid %d\n", cr, ipid);
															        BT_AVCTP_TRACE_DEBUG("pid 0x%x\n", pid);


BT_AVCTP_TRACE_DEBUG("avctp_input: p->len == %d p->tot_len == %d\n", p->len, p->tot_len);
    bt_hex_dump(p->payload,p->len);

    bt_pbuf_header(p, -AVCTP_NON_FRAG_HDR_SIZE);


    avctp_upper_handle(p);

    return BT_ERR_OK;
}

static err_t avctp_connect_ind(void *arg, struct l2cap_pcb_t *pcb, err_t err)
{
	struct avctp_pcb_t *avctppcb;
	BT_AVCTP_TRACE_DEBUG("avctp_connect_indn");
    
	BT_AVCTP_TRACE_DEBUG("avdtp_connect_ind pcb case 1 0x%p\n",pcb);
    temp_avcrp = avctppcb = avctp_new();
    avctppcb->l2cappcb= pcb;
    AVCTP_PCB_REG(&avctp_active_pcbs,avctppcb);
    l2cap_register_disconnect_ind(avctppcb->l2cappcb, avctp_disconnect_ind);
    l2cap_register_recv(avctppcb->l2cappcb, avctp_input);
    return BT_ERR_OK;
}

err_t
avctp_init(void)
{
    struct l2cap_pcb_t *l2cappcb;

    if((l2cappcb = l2cap_new()) == NULL)
    {
    		BT_AVCTP_TRACE_DEBUG("avctp_init: Could not alloc L2CAP PCB for AVCTP_PSM\n");

        return BT_ERR_MEM;
    }
    l2cap_register_connect_ind(l2cappcb, AVCTP_PSM, avctp_connect_ind);
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

err_t avctp_register_callback(avctp_recv recv)
{
    avctp_upper_handle = recv;
    return BT_ERR_OK;
}
