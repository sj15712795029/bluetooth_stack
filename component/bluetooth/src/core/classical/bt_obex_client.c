/******************************************************************************
  * @file           bt_obex_client.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt obex client header file
******************************************************************************/

#include "bt_sdp.h"
#include "bt_obex_client.h"
#include "bt_pbuf.h"

struct obex_pcb_t *obex_active_pcbs;  /* List of all active PBAP PCBs */
struct obex_pcb_t *obex_tmp_pcb;


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


uint8_t obex_header_para[OBEX_HEADER_MAX_SIZE] = {0};
uint8_t obex_header_offset = 0;



static struct obex_pcb_t *obex_new(rfcomm_pcb_t *rfcommpcb);
static struct obex_pcb_t *obex_get_active_pcb(struct bd_addr_t *bdaddr);
static void obex_close(struct obex_pcb_t *pcb);


err_t obex_header_para_append(uint8_t hdr_id,uint8_t *hdr_data,uint8_t hdr_data_len)
{
    uint8_t hi = hdr_id >> 6;

    switch(hi)
    {
    case 0:
    case 1:
    {
        BT_OBEX_TRACE_DEBUG("obex_header_para_append:hi 0/1\n");
        obex_header_para[obex_header_offset++] = hdr_id;
        bt_be_store_16(obex_header_para,obex_header_offset,3+hdr_data_len);
        obex_header_offset += 2;
        if(hdr_data_len != 0)
        {
            memcpy(obex_header_para+obex_header_offset,hdr_data,hdr_data_len);
        }
        obex_header_offset += hdr_data_len;
        break;
    }

    case 2:
        BT_OBEX_TRACE_DEBUG("obex_header_para_append:hi 2\n");
        break;
    case 3:
    {
        BT_OBEX_TRACE_DEBUG("obex_header_para_append:hi 3\n");
        obex_header_para[obex_header_offset++] = hdr_id;
        bt_be_store_32(obex_header_para,obex_header_offset,*(uint32_t *)hdr_data);
        obex_header_offset += 4;
        break;
    }
    default:
    {
        BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] obex_header_para_append invalid id\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_ARG;
    }

    }

    return BT_ERR_OK;
}

err_t obex_header_para_get(uint8_t hdr_id,uint8_t *data_in,uint16_t data_in_len,uint16_t *find_offset,uint16_t *hdr_data_len)
{
    uint16_t index = 0;
    uint8_t hi;
    uint16_t temp_offset = 0;

    for(index = 0; index < data_in_len; index += temp_offset)
    {
        hi = data_in[index]>>6;
        if((hi == 0) || (hi == 1))
            temp_offset = (data_in[index+1]<<8) + data_in[index+2];
        else if(hi == 2)
            temp_offset = 2;
        else if(hi == 3)
            temp_offset = 5;

        if(data_in[index] == hdr_id)
        {
            *find_offset = index;
            *hdr_data_len = temp_offset;
            return BT_ERR_OK;
        }
    }

    return BT_ERR_VAL;
}



err_t obex_reset_header_para()
{
    memset(obex_header_para,0,OBEX_HEADER_MAX_SIZE);
    obex_header_offset = 0;

    return BT_ERR_OK;
}

static err_t obex_client_parse_connect_resp(struct obex_pcb_t *pcb, struct bt_pbuf_t *p)
{
    BT_OBEX_TRACE_DEBUG("obex_client_parse_connect_resp:in\n");

    if(((uint8_t *)p->payload)[0] == OBEX_RESP_SUCCESS)
    {
        uint8_t index = 0;
        uint8_t *hdr_para = ((uint8_t *)p->payload) + OBEX_CONNECT_FIELD_LEN;
        uint8_t data_len = p->len - OBEX_CONNECT_FIELD_LEN;
        for(index = 0; index < data_len; index++)
        {
            if(hdr_para[index] == OBEX_HEADER_CONNECTION_ID)
            {
                pcb->connect_id = bt_be_read_32(hdr_para,index+1);
                if(pcb->obex_client_cbs && pcb->obex_client_cbs->obex_client_connect_set_up)
                    pcb->obex_client_cbs->obex_client_connect_set_up(&pcb->remote_addr,BT_ERR_OK,pcb->connect_id);
            }
        }
    }

    return BT_ERR_OK;
}

static err_t obex_client_parse_get_resp(struct obex_pcb_t *pcb, struct bt_pbuf_t *p)
{
    uint8_t status = ((uint8_t *)p->payload)[0];
    BT_OBEX_TRACE_DEBUG("obex_client_parse_get_resp:in\n");

    if(pcb->obex_client_cbs && pcb->obex_client_cbs->obex_client_data_ind)
        pcb->obex_client_cbs->obex_client_data_ind(&pcb->remote_addr,((uint8_t *)p->payload) + OBEX_GET_FIELD_LEN,p->tot_len - OBEX_GET_FIELD_LEN,status);

    return BT_ERR_OK;
}

static err_t obex_client_parse_abort_resp(struct obex_pcb_t *pcb, struct bt_pbuf_t *p)
{
    uint8_t status = ((uint8_t *)p->payload)[0];
    BT_OBEX_TRACE_DEBUG("obex_client_parse_abort_resp\n");

	if(status == OBEX_RESP_SUCCESS)
	{
	    if(pcb->obex_client_cbs && pcb->obex_client_cbs->obex_client_abort)
	        pcb->obex_client_cbs->obex_client_abort(&pcb->remote_addr,status);
	}

    return BT_ERR_OK;
}


static err_t obex_client_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    struct obex_pcb_t *obex_pcb;

    BT_PBAP_TRACE_DEBUG("pbap_client_disconnected: CN = %d\n", rfcomm_cn(pcb));
    obex_pcb = obex_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(obex_pcb)
    {
        obex_close(obex_pcb);
    }

    if(obex_pcb->obex_client_cbs && obex_pcb->obex_client_cbs->obex_client_connect_realease)
        obex_pcb->obex_client_cbs->obex_client_connect_realease(&obex_pcb->remote_addr,BT_ERR_OK);

    rfcomm_close(pcb);


    return BT_ERR_OK;
}


static err_t obex_client_recv(void *arg, rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err)
{
    struct obex_pcb_t *obexpcb = obex_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(!obexpcb)
        return BT_ERR_CONN;

    BT_OBEX_TRACE_DEBUG("obex_client_recv: p->len == %d p->tot_len == %d,last op 0x%x\n", p->len, p->tot_len,obexpcb->last_opcode);
    //bt_hex_dump(p->payload,p->len);

    switch(obexpcb->last_opcode)
    {
    case OBEX_OPCODE_CONNECT:
    {
        obex_client_parse_connect_resp(obexpcb,p);
        break;
    }
    case OBEX_OPCODE_GET:
    {
        obex_client_parse_get_resp(obexpcb,p);
        break;
    }
    case OBEX_OPCODE_ABORT:
    {
        obex_client_parse_abort_resp(obexpcb,p);
        break;
    }
    default:
        break;
    }

    return BT_ERR_OK;
}

err_t obex_client_connect(rfcomm_pcb_t *rfcommpcb,obex_client_cbs_t *cb,uint16_t mtu,uint8_t scn)
{
    uint8_t offset = 0;
    struct bt_pbuf_t *p = NULL;
    struct obex_pcb_t *obexpcb;
    if((obexpcb = obex_new(rfcommpcb)) == NULL)
    {
        BT_OBEX_TRACE_DEBUG("obex_client_connect: Could not alloc rfcomm pcb\n");

        return BT_ERR_MEM;
    }
    bd_addr_set(&(obexpcb->remote_addr),&(rfcommpcb->l2cappcb->remote_bdaddr));
    obexpcb->remote_cn = scn;
    obexpcb->obex_client_cbs = cb;
    obexpcb->last_opcode = OBEX_OPCODE_CONNECT;
    OBEX_PCB_REG(&obex_active_pcbs, obexpcb);
    rfcomm_register_recv(rfcommpcb, obex_client_recv);
    rfcomm_register_disc(rfcommpcb, obex_client_disconnected);
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, OBEX_CONNECT_FIELD_LEN+obex_header_offset, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    ((uint8_t *)p->payload)[offset++] = OBEX_OPCODE_CONNECT;
    bt_be_store_16((uint8_t *)p->payload,offset,OBEX_CONNECT_FIELD_LEN+obex_header_offset);
    offset += 2;
    ((uint8_t *)p->payload)[offset++] = OBEX_VERSION;
    ((uint8_t *)p->payload)[offset++] = 0;
    bt_be_store_16((uint8_t *)p->payload,offset,mtu);
    offset += 2;
    memcpy((uint8_t *)p->payload+offset, obex_header_para, obex_header_offset);

    rfcomm_uih(rfcommpcb, obexpcb->remote_cn, p);

    obex_reset_header_para();
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t obex_client_get(rfcomm_pcb_t *rfcommpcb)
{
    uint8_t offset = 0;
    struct bt_pbuf_t *p = NULL;
    struct obex_pcb_t *obexpcb = obex_get_active_pcb(&(rfcommpcb->l2cappcb->remote_bdaddr));
    if(!obexpcb)
        return BT_ERR_CONN;

    obexpcb->last_opcode = OBEX_OPCODE_GET;
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, OBEX_GET_FIELD_LEN+obex_header_offset, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    ((uint8_t *)p->payload)[offset++] = OBEX_OPCODE_GET;
    bt_be_store_16((uint8_t *)p->payload,offset,OBEX_GET_FIELD_LEN+obex_header_offset);
    offset += 2;
    memcpy((uint8_t *)p->payload+offset, obex_header_para, obex_header_offset);

    rfcomm_uih(rfcommpcb, obexpcb->remote_cn, p);

    obex_reset_header_para();
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t obex_client_setpath(rfcomm_pcb_t *rfcommpcb)
{
    uint8_t offset = 0;
    struct bt_pbuf_t *p = NULL;
    struct obex_pcb_t *obexpcb = obex_get_active_pcb(&(rfcommpcb->l2cappcb->remote_bdaddr));
    if(!obexpcb)
        return BT_ERR_CONN;

    obexpcb->last_opcode = OBEX_OPCODE_SETPATH;
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, OBEX_SETPATH_FIELD_LEN+obex_header_offset, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    ((uint8_t *)p->payload)[offset++] = OBEX_OPCODE_SETPATH;
    bt_be_store_16((uint8_t *)p->payload,offset,OBEX_SETPATH_FIELD_LEN+obex_header_offset);
    offset += 2;
    ((uint8_t *)p->payload)[offset++] = 0x2;/* do not backup and do not create folder */
    ((uint8_t *)p->payload)[offset++] = 0;

    memcpy((uint8_t *)p->payload+offset, obex_header_para, obex_header_offset);

    rfcomm_uih(rfcommpcb, obexpcb->remote_cn, p);

    obex_reset_header_para();
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

err_t obex_client_abort(rfcomm_pcb_t *rfcommpcb)
{
    uint8_t offset = 0;
    struct bt_pbuf_t *p = NULL;
    struct obex_pcb_t *obexpcb = obex_get_active_pcb(&(rfcommpcb->l2cappcb->remote_bdaddr));
    if(!obexpcb)
        return BT_ERR_CONN;

    obexpcb->last_opcode = OBEX_OPCODE_ABORT;
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, OBEX_ABORT_FIELD_LEN+obex_header_offset, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    ((uint8_t *)p->payload)[offset++] = OBEX_OPCODE_ABORT;
    bt_be_store_16((uint8_t *)p->payload,offset,OBEX_ABORT_FIELD_LEN+obex_header_offset);
    offset += 2;
    memcpy((uint8_t *)p->payload+offset, obex_header_para, obex_header_offset);

    rfcomm_uih(rfcommpcb, obexpcb->remote_cn, p);
    obex_reset_header_para();

    bt_pbuf_free(p);
		
		return BT_ERR_OK;
}


static struct obex_pcb_t *obex_new(rfcomm_pcb_t *rfcommpcb)
{
    struct obex_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_OBEX_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(struct obex_pcb_t));
        pcb->rfcommpcb = rfcommpcb;
        return pcb;
    }
    BT_OBEX_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

    return NULL;
}

static struct obex_pcb_t *obex_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct obex_pcb_t *pcb = NULL;
    for(pcb = obex_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}


static void obex_close(struct obex_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_OBEX_PCB, pcb);
        pcb = NULL;
    }
}



