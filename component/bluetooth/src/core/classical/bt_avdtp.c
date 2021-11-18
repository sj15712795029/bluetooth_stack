/******************************************************************************
  * @file           bt_avdtp.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt avdtp source file
******************************************************************************/

#include "bt_avdtp.h"



uint8_t local_seid = 0;
uint8_t avdtp_signal_open = 0;

struct avdtp_pcb_t *avdtp_active_pcbs;  /* List of all active AVDTP PCBs */
struct avdtp_pcb_t *avdtp_tmp_pcb;
struct avdtp_sep_t *avdtp_local_sep; /* List of all active avdtp seps */
struct avdtp_sep_t *avdtp_tmp_sep;      /* Only used for temporary storage. */

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



avdtp_event_handle avdtp_event_handler;
avdtp_media_handle avdtp_media_handler;

static err_t avdtp_connect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err);
static err_t avdtp_signal_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err);
static err_t avdtp_stream_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err);

err_t avdtp_signal_input(void *arg, l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err);
err_t avdtp_media_input(void *arg, l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err);


struct avdtp_pcb_t *avdtp_new(void)
{
    struct avdtp_pcb_t *avdtp_pcb;

    avdtp_pcb = bt_memp_malloc(MEMP_AVDTP_PCB);
    if(avdtp_pcb != NULL)
    {
        memset(avdtp_pcb, 0, sizeof(struct avdtp_pcb_t));
        return avdtp_pcb;
    }

    BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void avdtp_close(struct avdtp_pcb_t *avdtp_pcb)
{
    if(avdtp_pcb != NULL)
    {
        bt_memp_free(MEMP_AVDTP_PCB,avdtp_pcb);
        avdtp_pcb = NULL;
    }
}


struct avdtp_sep_t *avdtp_get_sep(uint8_t seid)
{
    struct avdtp_sep_t *avdtp_sep = NULL;

    for(avdtp_sep = avdtp_local_sep; avdtp_sep != NULL; avdtp_sep = avdtp_sep->next)
    {
        if(seid == avdtp_sep->seid)
			break;
    }
    return avdtp_sep;
}


static err_t avdtp_handle_discovery(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;

    struct bt_pbuf_t *p,*q;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    cmd_resp = p->payload;
    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_DISCOVER;

    for(avdtp_tmp_sep = avdtp_local_sep; avdtp_tmp_sep != NULL; avdtp_tmp_sep = avdtp_tmp_sep->next)
    {

        if((q = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_DIS_PER_EP_SIZE, BT_PBUF_RAM)) == NULL)
        {
            /* Could not allocate memory for bt_pbuf_t */
            BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM;
        }
        cmd_resp_pos = 0;
        cmd_resp = q->payload;
        cmd_resp[cmd_resp_pos++] = (avdtp_tmp_sep->seid << 2)|(avdtp_tmp_sep->in_use << 1);
        /* TODO :media type和sep type要根据注册发送封包 */
        cmd_resp[cmd_resp_pos++] = (AVDTP_AUDIO << 4) | (AVDTP_SINK << 3);

        bt_pbuf_cat(p, q);
    }

    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


static err_t avdtp_handle_get_capabilities(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label,uint8_t seid)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p,*q;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    cmd_resp = p->payload;
    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_GET_CAPABILITIES;


    for(avdtp_tmp_sep = avdtp_local_sep; avdtp_tmp_sep != NULL; avdtp_tmp_sep = avdtp_tmp_sep->next)
    {
        if(seid == avdtp_tmp_sep->seid)
        {
            if((avdtp_tmp_sep->service_categories_bitmap & AVDTP_MEDIA_TRANSPORT) == AVDTP_MEDIA_TRANSPORT)
            {
                if((q = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_CAP_HDR_SIZE, BT_PBUF_RAM)) == NULL)
                {
                    /* Could not allocate memory for bt_pbuf_t */
                    BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

                    return BT_ERR_MEM;
                }
                cmd_resp_pos = 0;
                cmd_resp = q->payload;
                cmd_resp[cmd_resp_pos++] = AVDTP_MEDIA_TRANSPORT;
                cmd_resp[cmd_resp_pos++] = 0;
                bt_pbuf_cat(p, q);
            }

			if((avdtp_tmp_sep->service_categories_bitmap & AVDTP_MEDIA_CODEC_MASK) == AVDTP_MEDIA_CODEC_MASK)
            {

                if((q = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_CAP_HDR_SIZE+avdtp_tmp_sep->cap.media_codec.media_info_len, BT_PBUF_RAM)) == NULL)
                {
                    /* Could not allocate memory for bt_pbuf_t */
                    BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                    return BT_ERR_MEM;
                }
                cmd_resp_pos = 0;
                cmd_resp = q->payload;
                cmd_resp[cmd_resp_pos++] = AVDTP_MEDIA_CODEC;
                cmd_resp[cmd_resp_pos++] = avdtp_tmp_sep->cap.media_codec.media_info_len;
                memcpy(cmd_resp+cmd_resp_pos,avdtp_tmp_sep->cap.media_codec.media_info,avdtp_tmp_sep->cap.media_codec.media_info_len);
                bt_pbuf_cat(p, q);
            }
        }
    }
    /* TODO:增加其他caplitily */

    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


static err_t avdtp_handle_get_all_capabilities(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p,*q;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    cmd_resp = p->payload;
    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_GET_ALL_CAPABILITIES;


    for(avdtp_tmp_sep = avdtp_local_sep; avdtp_tmp_sep != NULL; avdtp_tmp_sep = avdtp_tmp_sep->next)
    {
        if((avdtp_tmp_sep->service_categories_bitmap & AVDTP_MEDIA_TRANSPORT_MASK) == AVDTP_MEDIA_TRANSPORT_MASK)
        {
            if((q = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_CAP_HDR_SIZE, BT_PBUF_RAM)) == NULL)
            {
                /* Could not allocate memory for bt_pbuf_t */
                BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                return BT_ERR_MEM;
            }
            cmd_resp_pos = 0;
            cmd_resp = q->payload;
            cmd_resp[cmd_resp_pos++] = AVDTP_MEDIA_TRANSPORT;
            cmd_resp[cmd_resp_pos++] = 0;
            bt_pbuf_cat(p, q);
        }

        if((avdtp_tmp_sep->service_categories_bitmap & AVDTP_MEDIA_CODEC_MASK) == AVDTP_MEDIA_CODEC_MASK)
        {

            if((q = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_CAP_HDR_SIZE+avdtp_tmp_sep->cap.media_codec.media_info_len, BT_PBUF_RAM)) == NULL)
            {
                /* Could not allocate memory for bt_pbuf_t */
                BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

                return BT_ERR_MEM;
            }
            cmd_resp_pos = 0;
            cmd_resp = q->payload;
            cmd_resp[cmd_resp_pos++] = AVDTP_MEDIA_CODEC;
            cmd_resp[cmd_resp_pos++] = avdtp_tmp_sep->cap.media_codec.media_info_len;
            memcpy(cmd_resp+cmd_resp_pos,avdtp_tmp_sep->cap.media_codec.media_info,avdtp_tmp_sep->cap.media_codec.media_info_len);
            bt_pbuf_cat(p, q);
        }
    }
    /* TODO:增加其他caplitily */

    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

uint8_t *avdtp_get_spec_cap_value(uint8_t category_id,uint8_t *cap,uint16_t cap_len,uint16_t *spec_cap_len)
{
	uint8_t *cap_value = NULL;
	uint8_t remain_len = cap_len;
	struct service_category_hdr *service_category = (struct service_category_hdr *)cap;

	while(remain_len)
	{
		if(service_category->service_category == category_id)
		{
			*spec_cap_len = service_category->losc;
			cap_value = (uint8_t *)service_category + sizeof(struct service_category_hdr);
			break;
		}
		remain_len -= service_category->losc + sizeof(struct service_category_hdr);
		service_category = (struct service_category_hdr *)((uint8_t *)service_category + service_category->losc + sizeof(struct service_category_hdr));
	}

    return cap_value;
}

uint8_t *avdtp_parse_media_codec_cap(uint8_t *cap,uint8_t *media_type,uint8_t *media_codec_type)
{
	*media_type = cap[0] >> 4;
	*media_codec_type = cap[1];
	return (cap+2);
}


static err_t avdtp_handle_set_configuration(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label,uint8_t acp_seid,uint8_t int_seid,uint8_t *cap,uint16_t cap_len)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;

    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_SET_CONFIGURATION;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t avdtp_handle_open(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_OPEN;
    avdtp_signal_open = 1;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t avdtp_handle_start(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_START;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t avdtp_handle_suspend(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW,AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_SUSPEND;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


static err_t avdtp_handle_close(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW,AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_CLOSE;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t avdtp_handle_abort(struct avdtp_pcb_t *avdtp_pcb,uint8_t transaction_label)
{
    uint8_t *cmd_resp;
    uint8_t cmd_resp_pos = 0;
    struct bt_pbuf_t *p;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW,AVDTP_SIG_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    cmd_resp = p->payload;

    cmd_resp[cmd_resp_pos++] = 	(transaction_label << 4) | (AVDTP_SINGLE_PACKET<<2) |AVDTP_RESPONSE_ACCEPT_MSG;
    cmd_resp[cmd_resp_pos++] = AVDTP_SI_ABORT;
    ret = l2cap_datawrite(avdtp_pcb->avdtp_signal_l2cappcb, p);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}



struct avdtp_pcb_t *avdtp_get_active_pcb( struct bd_addr_t *bdaddr)
{
    struct avdtp_pcb_t *avdtp_pcb;
    for(avdtp_pcb = avdtp_active_pcbs; avdtp_pcb != NULL; avdtp_pcb = avdtp_pcb->next)
    {
        if(bd_addr_cmp(&(avdtp_pcb->avdtp_signal_l2cappcb->remote_bdaddr),
                       bdaddr))
        {
            break;
        }
    }
    return avdtp_pcb;
}

static err_t avdtp_connect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    struct avdtp_pcb_t *avdtp_pcb;

    if((avdtp_pcb = avdtp_get_active_pcb(&l2cap_pcb->remote_bdaddr)) == NULL)
    {
        avdtp_pcb = (struct avdtp_pcb_t *)avdtp_new();
        avdtp_pcb->avdtp_signal_l2cappcb = l2cap_pcb;
		memcpy(&avdtp_pcb->remote_bdaddr,&l2cap_pcb->remote_bdaddr,BD_ADDR_LEN);
        AVDTP_PCB_REG(&avdtp_active_pcbs,avdtp_pcb);
        l2cap_register_disconnect_ind(avdtp_pcb->avdtp_signal_l2cappcb, avdtp_signal_disconnect_ind);
        l2cap_register_recv(avdtp_pcb->avdtp_signal_l2cappcb, avdtp_signal_input);
        avdtp_event_handler(avdtp_pcb,AVDTP_SI_SIGNAL_CONNECT_IND,NULL);
    }
    else
    {
        avdtp_pcb->avdtp_media_l2cappcb = l2cap_pcb;
        l2cap_register_disconnect_ind(avdtp_pcb->avdtp_media_l2cappcb, avdtp_stream_disconnect_ind);
        l2cap_register_recv(avdtp_pcb->avdtp_media_l2cappcb, avdtp_media_input);
		avdtp_event_handler(avdtp_pcb,AVDTP_SI_STREAM_CONNECT_IND,NULL);
    }

    return BT_ERR_OK;
}


static err_t avdtp_signal_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
	struct avdtp_pcb_t *avdtp_pcb = NULL;
    BT_AVDTP_TRACE_DEBUG("avdtp_signal_disconnect_ind psm 0x%x\n",l2cap_pcb->psm);
	if((avdtp_pcb = avdtp_get_active_pcb(&l2cap_pcb->remote_bdaddr)) != NULL)
    {
    	avdtp_event_handler(avdtp_pcb,AVDTP_SI_SIGNAL_DISCON_IND,NULL);
		AVDTP_PCB_RMV(&avdtp_active_pcbs,avdtp_pcb);   
		avdtp_close(avdtp_pcb);
    }
    l2cap_close(l2cap_pcb);
    return BT_ERR_OK;
}


static err_t avdtp_stream_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
	struct avdtp_pcb_t *avdtp_pcb = NULL;
    BT_AVDTP_TRACE_DEBUG("avdtp_stream_disconnect_ind psm 0x%x\n",l2cap_pcb->psm);

	if((avdtp_pcb = avdtp_get_active_pcb(&l2cap_pcb->remote_bdaddr)) != NULL)
    {
    	avdtp_event_handler(avdtp_pcb,AVDTP_SI_STREAM_DISCON_IND,NULL);
    }
	
	l2cap_close(l2cap_pcb);
    return BT_ERR_OK;
}



err_t avdtp_media_input(void *arg, l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
    uint8_t marker,payload_type;
    uint16_t sequence_number;
    uint32_t timestamp,synchronization_source;

    struct avdtp_pcb_t *avdtp_pcb =  avdtp_get_active_pcb(&l2cappcb->remote_bdaddr);
    uint8_t *data = (uint8_t *)p->payload;
    uint8_t data_pos = 0;
    uint8_t version = data[data_pos]&0x03;
    uint8_t padding = (data[data_pos] >> 2)&0x1;
    uint8_t extension = (data[data_pos] >> 3)&0x1;
    uint8_t csrc_count = (data[data_pos]>>4)&0x0f;
    data_pos += 1;
    marker = (data[data_pos] )&0x1;
    payload_type = (data[data_pos] >> 1) & 0x7F;
    data_pos += 1;

    sequence_number = bt_be_read_16(data, data_pos);
    data_pos += 2;
    timestamp = bt_be_read_32(data, data_pos);
    data_pos += 4;
    synchronization_source = bt_be_read_32(data, data_pos);

    data_pos += 4;

    BT_AVDTP_INFO_DEBUG("avdtp_media_input: p->len == %d p->tot_len == %d,data pos %d\n", p->len, p->tot_len,data_pos+4*csrc_count);
    BT_AVDTP_INFO_DEBUG("version %d,padding %d,extension %d,csrc_count %d,marker %d,payload_type %d\n",version,padding,extension,csrc_count,marker,payload_type);
    BT_AVDTP_INFO_DEBUG("sequence_number 0x%x,timestamp %d,synchronization_source %d\n",sequence_number,timestamp,synchronization_source);


    bt_pbuf_header(p, -(data_pos));

    avdtp_media_handler(avdtp_pcb,p);

    return BT_ERR_OK;
}

err_t avdtp_signal_input(void *arg, l2cap_pcb_t *l2cap_pcb, struct bt_pbuf_t *p, err_t err)
{
    uint8_t *data = (uint8_t *)p->payload;
    uint8_t transaction_label = data[0] >> 4;
    uint8_t packet_type = ((data[0] >> 2) & 0x03);
    uint8_t message_type = data[0] & 0x03;

    struct avdtp_pcb_t *avdtp_pcb =  avdtp_get_active_pcb(&l2cap_pcb->remote_bdaddr);

    BT_AVDTP_TRACE_DEBUG("avdtp_signal_input len:%d transaction_label:%d,packet_type:%d,message_type:%d\n",p->len,transaction_label,packet_type,message_type);
    BT_AVDTP_TRACE_DEBUG("----------------------------------\n");
    bt_hex_dump(p->payload,p->len);
    BT_AVDTP_TRACE_DEBUG("----------------------------------\n");

    /* TODO :分隔封包 AVDTP_SINGLE_PACKET */
    if(packet_type == AVDTP_SINGLE_PACKET)
    {
        uint8_t signal_identifier = data[1] & 0x3f;
        switch(signal_identifier)
        {
        case AVDTP_SI_DISCOVER:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_DISCOVER\n");

            avdtp_handle_discovery(avdtp_pcb,transaction_label);

            break;
        }
        case AVDTP_SI_GET_CAPABILITIES:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_GET_CAPABILITIES\n");

            avdtp_handle_get_capabilities(avdtp_pcb,transaction_label,data[2]>>2);
            break;
        }
        case AVDTP_SI_SET_CONFIGURATION:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_SET_CONFIGURATION\n");

            avdtp_handle_set_configuration(avdtp_pcb,transaction_label,data[2]>>2,data[3]>>2,data+4,p->len-4);
            bt_pbuf_header(p,-4);

            avdtp_event_handler(avdtp_pcb,AVDTP_SI_SET_CONFIGURATION,p);
            break;
        }
        case AVDTP_SI_GET_CONFIGURATION:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_GET_CONFIGURATION\n");
            break;
        case AVDTP_SI_RECONFIGURE:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_RECONFIGURE\n");
            break;
        case AVDTP_SI_OPEN:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_OPEN\n");
            avdtp_handle_open(avdtp_pcb,transaction_label);
            avdtp_event_handler(avdtp_pcb,AVDTP_SI_OPEN,NULL);

        }
        break;
        case AVDTP_SI_START:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_START\n");
            avdtp_handle_start(avdtp_pcb,transaction_label);
			avdtp_event_handler(avdtp_pcb,AVDTP_SI_START,NULL);
            break;
        }
        case AVDTP_SI_CLOSE:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_CLOSE\n");
			avdtp_handle_close(avdtp_pcb,transaction_label);
			avdtp_event_handler(avdtp_pcb,AVDTP_SI_CLOSE,NULL);
            break;
        case AVDTP_SI_SUSPEND:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_SUSPEND\n");
            avdtp_handle_suspend(avdtp_pcb,transaction_label);
			avdtp_event_handler(avdtp_pcb,AVDTP_SI_SUSPEND,NULL);
            break;
        }
        case AVDTP_SI_ABORT:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_ABORT\n");
			avdtp_handle_abort(avdtp_pcb,transaction_label);
			avdtp_event_handler(avdtp_pcb,AVDTP_SI_ABORT,NULL);
            break;
        case AVDTP_SI_SECURITY_CONTROL:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_SECURITY_CONTROL\n");
            break;
        case AVDTP_SI_GET_ALL_CAPABILITIES:
        {
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_GET_ALL_CAPABILITIES\n");
            avdtp_handle_get_all_capabilities(avdtp_pcb,transaction_label);
            break;
        }
        case AVDTP_SI_DELAYREPORT:
            BT_AVDTP_TRACE_DEBUG("AVDTP_SI_DELAYREPORT\n");
            break;
        default:
            break;
        }
    }
    return BT_ERR_OK;
}

static uint16_t avdtp_get_next_local_seid()
{
    local_seid++;
    if (local_seid == 0)
    {
        local_seid = 1;
    }
    return local_seid;
}

struct avdtp_sep_t *avdtp_sep_new(void)
{
    struct avdtp_sep_t *sep;

    sep = bt_memp_malloc(MEMP_AVDTP_SEP);
    if(sep != NULL)
    {
        memset(sep, 0, sizeof(struct avdtp_sep_t));
        return sep;
    }
    BT_AVDTP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void avdtp_sep_close(struct avdtp_sep_t *sep)
{
    bt_memp_free(MEMP_AVDTP_SEP,sep);
    sep = NULL;
}


void avdtp_register_media_transport_category(struct avdtp_sep_t* sep)
{
    sep->service_categories_bitmap |= AVDTP_MEDIA_TRANSPORT_MASK;
}

void avdtp_register_media_codec_category(struct avdtp_sep_t* sep,uint8_t * media_codec_info, uint16_t media_codec_info_len)
{
    sep->service_categories_bitmap |= AVDTP_MEDIA_CODEC_MASK;
    sep->cap.media_codec.media_info = media_codec_info;
    sep->cap.media_codec.media_info_len = media_codec_info_len;
}


err_t avdtp_init(avdtp_event_handle avdtp_evt_handle,avdtp_media_handle avdtp_media_handle)
{
    /* Clear globals */
    avdtp_tmp_pcb = NULL;

    avdtp_event_handler = avdtp_evt_handle;
    avdtp_media_handler = avdtp_media_handle;

    l2cap_register_connect_ind(AVDTP_PSM, avdtp_connect_ind);
    return BT_ERR_OK;
}

err_t avdtp_create_sep(uint8_t codec_type,uint8_t * media_codec_info, uint16_t media_codec_info_len)
{
    struct avdtp_sep_t * sep = avdtp_sep_new();
    sep->seid = avdtp_get_next_local_seid();
	sep->codec_type = codec_type;
    avdtp_register_media_transport_category(sep);
    avdtp_register_media_codec_category(sep, media_codec_info, media_codec_info_len);

    AVDTP_SEP_REG(&avdtp_local_sep,sep);
    return BT_ERR_OK;
}

