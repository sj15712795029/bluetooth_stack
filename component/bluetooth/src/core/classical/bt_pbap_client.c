/******************************************************************************
  * @file           bt_pbap_client.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt pbap client source file
******************************************************************************/


#include "bt_pbap_client.h"

#if PROFILE_PBAP_ENABLE



static  uint8_t pbap_client_service_record[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST>>8&0xff, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST&0xff, /* Service class ID list attribute */
            SDP_DES_SIZE8, 3,
            SDP_UUID16, BT_SERVICE_CLASS_PHONEBOOK_ACCESS_PCE>>8&0xff, BT_SERVICE_CLASS_PHONEBOOK_ACCESS_PCE&0xff,

            SDP_DES_SIZE8, 0xd,
            SDP_UINT16, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST>>8&0xff, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST&0xff, /* profile descriptor List */
            SDP_DES_SIZE8, 0x8,
            SDP_DES_SIZE8,0x06,
            SDP_UUID16,BT_SERVICE_CLASS_PHONEBOOK_ACCESS>>8&0xff, BT_SERVICE_CLASS_PHONEBOOK_ACCESS&0xff,
            SDP_UINT16,0x01,0x02,			/* V1.2 */
};

struct pbap_pcb_t *pbap_active_pcbs;  /* List of all active PBAP PCBs */
struct pbap_pcb_t *pbap_tmp_pcb;

#define PBAP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define PBAP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(pbap_tmp_pcb = *pcbs; pbap_tmp_pcb != NULL; pbap_tmp_pcb = pbap_tmp_pcb->next) { \
                               if(pbap_tmp_pcb->next != NULL && pbap_tmp_pcb->next == npcb) { \
                                  pbap_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)


uint8_t pbap_app_para[PBAP_APP_PARA_MAX_SIZE] = {0};
uint8_t pbap_app_para_offset = 0;


pbap_client_cbs_t *pbap_client_cbs;
uint8_t pbap_client_vcard_format = PBAP_DN_VCARD_FORMAT;

static const uint8_t pbap_target_id[] = { 0x79, 0x61, 0x35, 0xf0, 0xf0, 0xc5, 0x11, 0xd8, 0x09, 0x66, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66};
static const uint8_t pbap_phonebook_type[] = "x-bt/phonebook";
static const uint8_t pbap_vcardlisting_type[] =  "x-bt/vcard-listing";
static const uint8_t pbap_vcardentry_type[] = "x-bt/vcard";

static const uint8_t pbap_telecom_repositories[] = "telecom";
static const uint8_t pbap_sim_repositories[] = "SIM1";
static const uint8_t pbap_pb_name[] = "pb";
static const uint8_t pbap_ich_name[] = "ich";
static const uint8_t pbap_och_name[] = "och";
static const uint8_t pbap_mch_name[] = "mch";
static const uint8_t pbap_cch_name[] = "cch";
static const uint8_t pbap_vcard_suffix[] = ".vcf";

static struct pbap_pcb_t *pbap_new(rfcomm_pcb_t *rfcommpcb);
static struct pbap_pcb_t *pbap_get_active_pcb(struct bd_addr_t *bdaddr);
static void pbap_close(struct pbap_pcb_t *pcb);
static err_t pbap_client_parse_pull_phonebook_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status);
static err_t pbap_client_parse_get_phonebook_size_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status);
static err_t pbap_client_parse_pull_vcard_list_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status);
static err_t pbap_client_parse_pull_vcard_entry_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status);
static err_t pbap_client_pull_phone_book(struct pbap_pcb_t *pcb,uint8_t repositories,uint8_t type,uint16_t count,uint16_t offset,uint64_t property_mask,uint8_t vcard_format);

static err_t pbap_client_run(struct pbap_pcb_t *pcb);


err_t pbap_app_para_append(uint8_t para_id,uint8_t *para_data,uint8_t para_data_len)
{
    pbap_app_para[pbap_app_para_offset++] = para_id;
    pbap_app_para[pbap_app_para_offset++] = para_data_len;
    memcpy(pbap_app_para+pbap_app_para_offset,para_data,para_data_len);
    pbap_app_para_offset += para_data_len;
    return BT_ERR_OK;
}


err_t pbap_app_para_reset()
{
    memset(pbap_app_para,0,PBAP_APP_PARA_MAX_SIZE);
    pbap_app_para_offset = 0;
    return BT_ERR_OK;
}


err_t pbap_client_init(pbap_client_cbs_t *cb)
{
    sdp_record_t *record;

    uint8_t pbap_client_sdp_size = sizeof(pbap_client_service_record);
    uint32_t pbap_client_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)pbap_client_service_record,pbap_client_sdp_size,pbap_client_record_hdl)) == NULL)
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_init: Could not alloc SDP record\n");

        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)pbap_client_service_record,sizeof(pbap_client_service_record));


    pbap_client_cbs = cb;

    return BT_ERR_OK;
}


err_t pbap_client_connect(struct bd_addr_t *addr)
{
    struct pbap_pcb_t *pbappcb;

    if((pbappcb = pbap_new(NULL)) == NULL)
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_connect: Could not alloc rfcomm pcb\n");
        return BT_ERR_MEM;
    }

    pbappcb->state = PBAP_W2_SDP_CONNECTED;

    bd_addr_set(&(pbappcb->remote_addr),addr );
    pbap_client_run(pbappcb);

    return BT_ERR_OK;
}
err_t pbap_client_disconnect(struct bd_addr_t *addr)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    pbappcb->state = PBAP_W2_DISCONNECT_RFCOMM;
    pbap_client_run(pbappcb);

    return BT_ERR_OK;
}

err_t pbap_client_download_phonebook(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    pbappcb->state = PBAP_OPERATE_PULL_PHONEBOOK;
    pbappcb->dn_pb_repositories = repositories;
    pbappcb->dn_pb_type = type;
	pbappcb->dn_operate = PBAP_OPERATE_PULL_PHONEBOOK;
    pbap_client_pull_phone_book(pbappcb,repositories,type,0xffff,0x0,PBAP_PROPERTY_MASK_DEFAULT,pbap_client_vcard_format);

    if(pbap_client_cbs && pbap_client_cbs->pbap_download_phonebook_status)
        pbap_client_cbs->pbap_download_phonebook_status(&pbappcb->remote_addr,pbappcb->dn_pb_repositories,pbappcb->dn_pb_type,PBAP_DN_PB_START);

    return BT_ERR_OK;
}

err_t pbap_client_query_phonebook_size(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    pbappcb->state = PBAP_OPERATE_QUERY_PHONEBOOK_SIZE;
    pbappcb->query_repositories = repositories;
    pbappcb->query_type = type;
    pbap_client_pull_phone_book(pbappcb,repositories,type,0x0,0x0,PBAP_PROPERTY_MASK_DEFAULT,pbap_client_vcard_format);

    return BT_ERR_OK;

}

err_t pbap_client_set_path(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    uint8_t index = 0;
    uint8_t name_string_temp[20] = {0};
    uint8_t name_string[40] = {0};
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    pbappcb->state = PBAP_OPERATE_SET_PATH;

    obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pbappcb->cid),sizeof(pbappcb->cid));

    if(repositories == PB_LOCAL_REPOSITORY)
    {
        switch(type)
        {
        case PBAP_NONE_TYPE:
            sprintf((char *)name_string_temp, "%s", pbap_telecom_repositories);
            break;
        case PB_PHONEBOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_telecom_repositories,pbap_pb_name);
            break;
        case PB_INCOMING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_telecom_repositories,pbap_ich_name);
            break;
        case PB_OUTGOING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_telecom_repositories,pbap_och_name);
            break;
        case PB_MISSING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_telecom_repositories,pbap_mch_name);
            break;
        case PB_COMBINE_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_telecom_repositories,pbap_cch_name);
            break;
        default:

            break;
        }
    }
    else if(repositories == PB_SIM_REPOSITORY)
    {
        switch(type)
        {
        case PBAP_NONE_TYPE:
            sprintf((char *)name_string_temp, "%s", pbap_sim_repositories);
            break;
        case PB_PHONEBOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_sim_repositories,pbap_pb_name);
            break;
        case PB_INCOMING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_sim_repositories,pbap_ich_name);
            break;
        case PB_OUTGOING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_sim_repositories,pbap_och_name);
            break;
        case PB_MISSING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_sim_repositories,pbap_mch_name);
            break;
        case PB_COMBINE_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s", pbap_sim_repositories,pbap_cch_name);
            break;
        default:
            break;
        }
    }
    for(index = 0; index < strlen((const char *)name_string_temp) + 1; index++)
    {
        name_string[index*2] = 0;
        name_string[index*2+1] = name_string_temp[index];
    }
    obex_header_para_append(OBEX_HEADER_NAME,name_string,(strlen((const char *)name_string_temp) + 1)*2);

    pbappcb->current_repositories = repositories;
    pbappcb->current_type = type;
    obex_client_setpath(pbappcb->rfcommpcb);

    return BT_ERR_OK;

}

err_t pbap_client_download_vcard_list(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    if((pbappcb->current_repositories!= repositories) || (pbappcb->current_type != type))
        return BT_ERR_ARG;

    pbappcb->state = PBAP_OPERATE_PULL_VCARD_LIST;
	pbappcb->dn_operate = PBAP_OPERATE_PULL_VCARD_LIST;

    obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pbappcb->cid),sizeof(pbappcb->cid));
    obex_header_para_append(OBEX_HEADER_TYPE,(uint8_t *)pbap_vcardlisting_type,sizeof(pbap_vcardlisting_type));

    obex_header_para_append(OBEX_HEADER_NAME,NULL,0);

    obex_client_get(pbappcb->rfcommpcb);

    if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardlist_status)
        pbap_client_cbs->pbap_download_vcardlist_status(&pbappcb->remote_addr,pbappcb->current_repositories,pbappcb->current_type,PBAP_DN_VCARD_LIST_START);

    return BT_ERR_OK;
}

err_t pbap_client_download_vcard_entry(struct bd_addr_t *addr,uint8_t repositories,uint8_t type,uint16_t entry_number)
{
    uint8_t index = 0;
    uint8_t name_string_temp[20] = {0};
    uint8_t name_string[40] = {0};
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;

    if((pbappcb->current_repositories!= repositories) || (pbappcb->current_type != type))
        return BT_ERR_ARG;

    pbappcb->state = PBAP_OPERATE_PULL_VCARD_ENTRY;
    pbappcb->dn_ventry_number = entry_number;

    obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pbappcb->cid),sizeof(pbappcb->cid));
    obex_header_para_append(OBEX_HEADER_TYPE,(uint8_t *)pbap_vcardentry_type,sizeof(pbap_vcardentry_type));
    sprintf((char *)name_string_temp, "%d%s", entry_number,pbap_vcard_suffix);
    for(index = 0; index < strlen((const char *)name_string_temp) + 1; index++)
    {
        name_string[index*2] = 0;
        name_string[index*2+1] = name_string_temp[index];
    }
    obex_header_para_append(OBEX_HEADER_NAME,name_string,(strlen((const char *)name_string_temp) + 1)*2);

    pbap_app_para_append(PBAP_APP_PARAM_FORMAT,&pbap_client_vcard_format,sizeof(pbap_client_vcard_format));
    obex_header_para_append(OBEX_HEADER_APPLICATION_PARAMETERS,pbap_app_para,pbap_app_para_offset);
    pbap_app_para_reset();

    obex_client_get(pbappcb->rfcommpcb);

    return BT_ERR_OK;

}

err_t pbap_client_download_abort(struct bd_addr_t *addr)
{
	struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(addr);
    if(!pbappcb)
        return BT_ERR_CONN;
	
	pbappcb->state = PBAP_OPERATE_ABORT;

	obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pbappcb->cid),sizeof(pbappcb->cid));
	obex_client_abort(pbappcb->rfcommpcb);
    return BT_ERR_OK;
}

err_t pbap_client_pull_next(struct pbap_pcb_t *pcb)
{
    obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pcb->cid),sizeof(pcb->cid));
    obex_client_get(pcb->rfcommpcb);
    return BT_ERR_OK;
}


static err_t pbap_client_pull_phone_book(struct pbap_pcb_t *pcb,uint8_t repositories,uint8_t type,uint16_t count,uint16_t offset,
                                  uint64_t property_mask,uint8_t vcard_format)
{
    uint8_t index = 0;
    uint8_t name_string_temp[20] = {0};
    uint8_t name_string[40] = {0};
    uint8_t max_list_count[2] = {0};
    uint8_t offset_buf[2] = {0};
	uint8_t property_buf[8] = {0};
    obex_header_para_append(OBEX_HEADER_CONNECTION_ID,(uint8_t *)&(pcb->cid),sizeof(pcb->cid));
    obex_header_para_append(OBEX_HEADER_TYPE,(uint8_t *)pbap_phonebook_type,sizeof(pbap_phonebook_type));
    if(repositories == PB_LOCAL_REPOSITORY)
    {
        switch(type)
        {
        case PB_PHONEBOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_telecom_repositories,pbap_pb_name,pbap_vcard_suffix);
            break;
        case PB_INCOMING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_telecom_repositories,pbap_ich_name,pbap_vcard_suffix);
            break;
        case PB_OUTGOING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_telecom_repositories,pbap_och_name,pbap_vcard_suffix);
            break;
        case PB_MISSING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_telecom_repositories,pbap_mch_name,pbap_vcard_suffix);
            break;
        case PB_COMBINE_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_telecom_repositories,pbap_cch_name,pbap_vcard_suffix);
            break;
        default:
            break;
        }
    }
    else if(repositories == PB_SIM_REPOSITORY)
    {
        switch(type)
        {
        case PB_PHONEBOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_sim_repositories,pbap_pb_name,pbap_vcard_suffix);
            break;
        case PB_INCOMING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_sim_repositories,pbap_ich_name,pbap_vcard_suffix);
            break;
        case PB_OUTGOING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_sim_repositories,pbap_och_name,pbap_vcard_suffix);
            break;
        case PB_MISSING_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_sim_repositories,pbap_mch_name,pbap_vcard_suffix);
            break;
        case PB_COMBINE_BOOK_TYPE:
            sprintf((char *)name_string_temp, "%s/%s%s", pbap_sim_repositories,pbap_cch_name,pbap_vcard_suffix);
            break;
        default:
            break;
        }
    }

    for(index = 0; index < strlen((const char *)name_string_temp) + 1; index++)
    {
        name_string[index*2] = 0;
        name_string[index*2+1] = name_string_temp[index];
    }

    obex_header_para_append(OBEX_HEADER_NAME,name_string,(strlen((const char *)name_string_temp) + 1)*2);

    /* PBAP app para ass */
    bt_be_store_16(max_list_count,0,count);
    pbap_app_para_append(PBAP_APP_PARAM_MAX_LIST_COUNT,max_list_count,sizeof(max_list_count));

    if(count != 0)
    {
        bt_be_store_16(offset_buf,0,offset);
        pbap_app_para_append(PBAP_APP_PARAM_LIST_START_OFFSET,offset_buf,sizeof(offset_buf));
		bt_be_store_64(property_buf,0,property_mask);
		pbap_app_para_append(PBAP_APP_PARAM_PROPERTY_SELECTOR,property_buf,sizeof(property_buf));
    }
    pbap_app_para_append(PBAP_APP_PARAM_FORMAT,&vcard_format,sizeof(vcard_format));
    obex_header_para_append(OBEX_HEADER_APPLICATION_PARAMETERS,pbap_app_para,pbap_app_para_offset);

    pbap_app_para_reset();

    obex_client_get(pcb->rfcommpcb);

    return BT_ERR_OK;
}

void pbap_obex_client_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status,uint32_t cid)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(remote_addr);
    if(!pbappcb)
        return;
    BT_PBAP_TRACE_DEBUG("PBAP << OBEX:obex_client_connect_set_up,cid 0x%xaddress is :\n",cid);

    bt_addr_dump(remote_addr->addr);

    pbappcb->cid = cid;

    if(pbappcb)
    {
        BT_PBAP_TRACE_DEBUG("pbap_obex_client_connect_set_up\n");

        pbappcb->state = PBAP_OBEX_CONNECTED;
        pbap_client_run(pbappcb);

    }

}
void pbap_obex_client_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    struct pbap_pcb_t *pbappcb;
    BT_PBAP_TRACE_DEBUG("PBAP << OBEX:obex_client_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    pbappcb = pbap_get_active_pcb(remote_addr);
    if(pbappcb)
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_disconnected: close pbap active pcb\n");

        pbappcb->state = PBAP_W4_RFCOMM_DISCONNECTED_AND_RESTART;
        pbap_client_run(pbappcb);

    }
}

void pbap_obex_client_data_ind(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len,uint8_t status)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(remote_addr);
    if(!pbappcb)
        return;

    BT_PBAP_TRACE_DEBUG("PBAP << OBEX:obex_client_data_ind, pbappcb->state(%d) status 0x%x address is :\n",pbappcb->state,status);
    bt_addr_dump(remote_addr->addr);

    //BT_PBAP_TRACE_DEBUG("PBAP << OBEX:obex_client_data is :\n");
    //bt_hex_dump(data,data_len);

    switch(pbappcb->state)
    {
    case PBAP_OPERATE_PULL_PHONEBOOK:
    {
        pbap_client_parse_pull_phonebook_resp(pbappcb,data,data_len,status);
        break;
    }
    case PBAP_OPERATE_QUERY_PHONEBOOK_SIZE:
    {
        pbap_client_parse_get_phonebook_size_resp(pbappcb,data,data_len,status);
        break;
    }
    case PBAP_OPERATE_PULL_VCARD_LIST:
    {
        pbap_client_parse_pull_vcard_list_resp(pbappcb,data,data_len,status);
        break;
    }
    case PBAP_OPERATE_PULL_VCARD_ENTRY:
    {
        pbap_client_parse_pull_vcard_entry_resp(pbappcb,data,data_len,status);
        break;
    }
    case PBAP_OPERATE_SET_PATH:
        break;
    default:
        break;

    }

}

void pbap_obex_client_abort(struct bd_addr_t *remote_addr,uint8_t status)
{
	struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(remote_addr);
    if(!pbappcb)
        return;

    BT_PBAP_TRACE_DEBUG("PBAP << OBEX:pbap_obex_client_abort, pbappcb->dn_operate(%d) status 0x%x address is :\n",pbappcb->dn_operate,status);
    bt_addr_dump(remote_addr->addr);

	switch(pbappcb->dn_operate)
    {
    case PBAP_OPERATE_PULL_PHONEBOOK:
    {
        if(pbap_client_cbs && pbap_client_cbs->pbap_download_phonebook_status)
        	pbap_client_cbs->pbap_download_phonebook_status(&pbappcb->remote_addr,pbappcb->dn_pb_repositories,pbappcb->dn_pb_type,PBAP_DN_PB_ABORT);
        break;
    }
    case PBAP_OPERATE_PULL_VCARD_LIST:
    {
        if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardlist_status)
        	pbap_client_cbs->pbap_download_vcardlist_status(&pbappcb->remote_addr,pbappcb->current_repositories,pbappcb->current_type,PBAP_DN_VCARD_LIST_ABORT);
        break;
    }
    default:
        break;

    }
}


obex_client_cbs_t pbap_obex_client_cbs =
{
    pbap_obex_client_connect_set_up,
    pbap_obex_client_connect_realease,
    pbap_obex_client_data_ind,
    pbap_obex_client_abort,
};

static struct pbap_pcb_t *pbap_new(rfcomm_pcb_t *rfcommpcb)
{
    struct pbap_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_PBAP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(struct pbap_pcb_t));
        pcb->rfcommpcb = rfcommpcb;
        return pcb;
    }

    BT_PBAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

static struct pbap_pcb_t *pbap_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct pbap_pcb_t *pcb = NULL;
    for(pcb = pbap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}


static void pbap_close(struct pbap_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_PBAP_PCB, pcb);
        pcb = NULL;
    }
}


static err_t l2cap_connect_cfm(void *arg,l2cap_pcb_t *l2cappcb, uint16_t result, uint16_t status)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(&(l2cappcb->remote_bdaddr));

    if(pbappcb)
    {

        if(result == L2CAP_CONN_SUCCESS)
        {
            BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP connected pcb->state = %d,psm %d\n", l2cappcb->state,l2cap_psm(l2cappcb));

            /* Tell L2CAP that we wish to be informed of a disconnection request */
            switch(l2cap_psm(l2cappcb))
            {
            case SDP_PSM:
                BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: SDP L2CAP configured. Result = %d\n", result);

                if((pbappcb->sdppcb= sdp_new(l2cappcb)) == NULL)
                {
                    BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a SDP PCB\n");
                    return BT_ERR_MEM;
                }
                pbappcb->state = PBAP_W2_SDP_QUERY_RFCOMM_CHANNEL;
                pbappcb->sdppcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, sdp_recv);
                pbap_client_run(pbappcb);
                return BT_ERR_OK;
            case RFCOMM_PSM:
                if((pbappcb->rfcommpcb = rfcomm_new(l2cappcb)) == NULL)
                {
                    BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a RFCOMM PCB\n");
                    return BT_ERR_MEM;
                }
                pbappcb->rfcommpcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, rfcomm_input);
                pbappcb->state = PBAP_W2_SERVER_CN_CONNECTED;
                pbap_client_run(pbappcb);
                break;
            default:
                return BT_ERR_VAL;
            }
        }
        else
        {
            BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP not connected. Redo inquiry\n");

            l2cap_close(l2cappcb);
        }

        return BT_ERR_OK;
    }
    else
    {
        BT_PBAP_TRACE_DEBUG("l2cap_connect_cfm: connect fail\n");

        PBAP_PCB_RMV(&pbap_active_pcbs, pbappcb);
        pbap_close(pbappcb);
        return BT_ERR_CONN;
    }

}


static err_t l2cap_disconnect_cfm(void *arg,l2cap_pcb_t *pcb)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(&(pcb->remote_bdaddr));

    if(!pbappcb)
        return BT_ERR_CONN;

    BT_PBAP_TRACE_DEBUG("l2cap_disconnected_cfm psm 0x%02x\n",l2cap_psm(pcb));

    switch(l2cap_psm(pcb))
    {
    case SDP_PSM:
        pbappcb->state = PBAP_W2_RFCOMM_CONNECTED;
        pbap_client_run(pbappcb);
        break;
    case RFCOMM_PSM:
        PBAP_PCB_RMV(&pbap_active_pcbs, pbappcb);
        rfcomm_close(pbappcb->rfcommpcb);
        pbap_close(pbappcb);
        break;
    default:
        break;
    }
    l2cap_close(pcb);
    return BT_ERR_OK;
}



static uint8_t pbap_get_rfcomm_cn(uint16_t attribl_bc, struct bt_pbuf_t *attribute_list)
{
    uint8_t i;
    for(i = 0; i < attribl_bc; i++)
    {
        if(((uint8_t *)attribute_list->payload)[i] == (SDP_DE_TYPE_UUID | SDP_DE_SIZE_16))
        {
            if(bt_ntohs(*((uint16_t *)(((uint8_t *)attribute_list->payload)+i+1))) == 0x0003)
            {
                return *(((uint8_t *)attribute_list->payload)+i+4);
            }
        }
    }
    return 0;
}


static void pbap_client_sdp_attributes_recv(void *arg, sdp_pcb_t *sdppcb, uint16_t attribl_bc, struct bt_pbuf_t *p)
{
    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(&(sdppcb->l2cappcb->remote_bdaddr));

    if(!pbappcb)
        return;

    /* Get the RFCOMM channel identifier from the protocol descriptor list */
    if((pbappcb->remote_cn = pbap_get_rfcomm_cn(attribl_bc, p)) != 0)
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_sdp_attributes_recv:remote cn %d\n",pbappcb->remote_cn);

    }

    pbappcb->state = PBAP_W2_SDP_DISCONNECTD;
    pbap_client_run(pbappcb);
}


static err_t pbap_client_rf_connect_cfm(void *arg, rfcomm_pcb_t *pcb, err_t err)
{

    struct pbap_pcb_t *pbappcb = pbap_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));

    if(err == BT_ERR_OK)
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_rf_connect_cfm. CN = %d\n", rfcomm_cn(pcb));

        pbappcb->rfcommpcb = pcb;
        pbappcb->state = PBAP_W2_OBEX_CONNECTED;
        pbap_client_run(pbappcb);
    }
    else
    {
        BT_PBAP_TRACE_DEBUG("pbap_client_rf_connect_cfm. Connection attempt failed CN = %d\n", rfcomm_cn(pcb));

        l2cap_close(pcb->l2cappcb);
        rfcomm_close(pcb);
    }
    return BT_ERR_OK;
}

static err_t pbap_client_parse_pull_vcard_list_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status)
{
    uint16_t vcard_length;
    uint16_t data_offset;

    if(status == OBEX_RESP_CONTINUE)
    {
        if(obex_header_para_get(OBEX_HEADER_BODY,data,data_len,&data_offset,&vcard_length) == BT_ERR_OK)
        {
            if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardlist_data)
                pbap_client_cbs->pbap_download_vcardlist_data(&pcb->remote_addr,pcb->current_repositories,pcb->current_type,data+data_offset+3,vcard_length-3);
        }
        pbap_client_pull_next(pcb);
    }
    else if(status == OBEX_RESP_SUCCESS)
    {

        if(obex_header_para_get(OBEX_HEADER_BODY,data,data_len,&data_offset,&vcard_length) == BT_ERR_OK)
        {
            if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardlist_data)
                pbap_client_cbs->pbap_download_vcardlist_data(&pcb->remote_addr,pcb->current_repositories,pcb->current_type,data+data_offset+3,vcard_length-3);
        }
        if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardlist_status)
            pbap_client_cbs->pbap_download_vcardlist_status(&pcb->remote_addr,pcb->current_repositories,pcb->current_type,PBAP_DN_VCARD_LIST_END);
    }
    else
    {
        /* TODO: error 处理 */
        BT_PBAP_TRACE_DEBUG("pbap_client_parse_pull_vcard_list_resp: err++++++++++++++\n");

    }
    return BT_ERR_OK;
}

static err_t pbap_client_parse_pull_vcard_entry_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status)
{
    uint16_t vcard_length;
    uint16_t data_offset;

    if(status == OBEX_RESP_SUCCESS)
    {
        if(obex_header_para_get(OBEX_HEADER_END_OF_BODY,data,data_len,&data_offset,&vcard_length) == BT_ERR_OK)
        {
            if(pbap_client_cbs && pbap_client_cbs->pbap_download_vcardentry_data)
                pbap_client_cbs->pbap_download_vcardentry_data(&pcb->remote_addr,pcb->current_repositories,pcb->current_type,pcb->dn_ventry_number,data+data_offset+3,vcard_length-3);
        }
    }
    else
    {
        /* TODO: error 处理 */
        BT_PBAP_TRACE_DEBUG("pbap_client_parse_pull_vcard_list_resp: err++++++++++++++\n");

    }
    return BT_ERR_OK;
}


static err_t pbap_client_parse_pull_phonebook_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status)
{
    uint16_t vcard_length;
    uint16_t data_offset;

    if(status == OBEX_RESP_CONTINUE)
    {

        if(obex_header_para_get(OBEX_HEADER_BODY,data,data_len,&data_offset,&vcard_length) == BT_ERR_OK)
        {
            if(pbap_client_cbs && pbap_client_cbs->pbap_download_phonebook_data)
                pbap_client_cbs->pbap_download_phonebook_data(&pcb->remote_addr,pcb->dn_pb_repositories,pcb->dn_pb_type,data+data_offset+3,vcard_length-3);
        }
        pbap_client_pull_next(pcb);
    }
    else if(status == OBEX_RESP_SUCCESS)
    {
        if(obex_header_para_get(OBEX_HEADER_END_OF_BODY,data,data_len,&data_offset,&vcard_length) == BT_ERR_OK)
        {
            if(pbap_client_cbs && pbap_client_cbs->pbap_download_phonebook_data)
                pbap_client_cbs->pbap_download_phonebook_data(&pcb->remote_addr,pcb->dn_pb_repositories,pcb->dn_pb_type,data+data_offset+3,vcard_length-3);
        }
        BT_PBAP_TRACE_DEBUG("pbap_client_parse_pull_phonebook_resp download complete\n");
        if(pbap_client_cbs && pbap_client_cbs->pbap_download_phonebook_status)
            pbap_client_cbs->pbap_download_phonebook_status(&pcb->remote_addr,pcb->dn_pb_repositories,pcb->dn_pb_type,PBAP_DN_PB_END);
    }
    else
    {
        /* TODO: error 处理 */
        BT_PBAP_TRACE_DEBUG("PBAP_OPERATE_PULL_PHONEBOOK: err++++++++++++++\n");
    }
    return BT_ERR_OK;
}

static err_t pbap_client_parse_get_phonebook_size_resp(struct pbap_pcb_t *pcb,uint8_t *data,uint16_t data_len,uint8_t status)
{
    if(status == OBEX_RESP_SUCCESS)
    {
        uint16_t data_offset = 0;
        uint16_t hdr_length = 0;
        BT_PBAP_TRACE_DEBUG("pbap_client_parse_get_phonebook_size_resp: data\n");
        bt_hex_dump(data,data_len);
        if(obex_header_para_get(OBEX_HEADER_APPLICATION_PARAMETERS,data,data_len,&data_offset,&hdr_length) == BT_ERR_OK)
        {
            uint8_t index = 0;
            uint8_t temp_offset;
            uint8_t *hdr_data = data+data_offset;
            BT_PBAP_TRACE_DEBUG("data_offset(%d) hdr_length(%d)\n",data_offset,hdr_length);
            bt_hex_dump(hdr_data,hdr_length);
            for(index = 3; index < hdr_length; index+=temp_offset)
            {
                temp_offset = hdr_data[index+1]+2;
                if(hdr_data[index] == PBAP_APP_PARAM_PHONEBOOK_SIZE)
                {
                    uint16_t phonebook_size = bt_be_read_16(hdr_data+index,2);
                    BT_PBAP_TRACE_DEBUG("repositories(%d)type(%d) size(%d)\n",pcb->query_repositories,pcb->query_type,phonebook_size);
                    if(pbap_client_cbs && pbap_client_cbs->pbap_query_repositories_size)
                        pbap_client_cbs->pbap_query_repositories_size(&pcb->remote_addr,pcb->query_repositories,pcb->query_type,phonebook_size);
                }
            }
        }

    }
    else
    {
        BT_PBAP_TRACE_DEBUG("PBAP_OPERATE_QUERY_PHONEBOOK_SIZE: err++++++++++++++\n");
		if(pbap_client_cbs && pbap_client_cbs->pbap_query_repositories_size)
			pbap_client_cbs->pbap_query_repositories_size(&pcb->remote_addr,pcb->query_repositories,pcb->query_type,0);
    }
    return BT_ERR_OK;
}

static err_t pbap_client_run(struct pbap_pcb_t *pcb)
{
    BT_PBAP_TRACE_DEBUG("pbap_client_run: state %d\n", pcb->state);
    switch(pcb->state)
    {
    case PBAP_W2_SDP_CONNECTED:
        PBAP_PCB_REG(&pbap_active_pcbs, pcb);
        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_PBAP_TRACE_DEBUG("pbap_client_run: Could not alloc L2CAP pcb\n");
            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), SDP_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case PBAP_W2_SDP_QUERY_RFCOMM_CHANNEL:
    {
        uint8_t pbap[] = {0x35, 0x03, 0x19, 0x11, 0x2f}; /* Service search pattern with PBAP UUID is default */
        uint8_t attrids[] = {0x35, 0x03, 0x09, 0x00, 0x04}; /* Attribute IDs to search for in data element sequence form */
        sdp_service_search_attrib_req(pcb->sdppcb, 0xFFFF, pbap, sizeof(pbap),
                                      attrids, sizeof(attrids), pbap_client_sdp_attributes_recv);
        break;
    }
    case PBAP_W2_SDP_DISCONNECTD:
        l2cap_disconnect_req(pcb->sdppcb->l2cappcb, l2cap_disconnect_cfm);
        sdp_free(pcb->sdppcb);
        break;
    case PBAP_W2_RFCOMM_CONNECTED:

        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_PBAP_TRACE_DEBUG("pbap_client_run: Could not alloc L2CAP pcb\n")
            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), RFCOMM_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case PBAP_W2_SERVER_CN_CONNECTED:
        rfcomm_connect(pcb->rfcommpcb, pcb->remote_cn, pbap_client_rf_connect_cfm);
        break;

    case PBAP_W2_OBEX_CONNECTED:
        obex_header_para_append(OBEX_HEADER_TARGET,(uint8_t *)pbap_target_id,sizeof(pbap_target_id));
        obex_client_connect(pcb->rfcommpcb,&pbap_obex_client_cbs,PBAP_CONNECT_MTU,pcb->remote_cn);
        break;
    case PBAP_OBEX_CONNECTED:
        if(pbap_client_cbs && pbap_client_cbs->pbap_client_connect_set_up)
            pbap_client_cbs->pbap_client_connect_set_up(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = PBAP_OPERATE_IDLE;
        break;
    case PBAP_W2_DISCONNECT_RFCOMM:
        rfcomm_disconnect(pcb->rfcommpcb);
        //l2cap_disconnect_req(pcb->rfcommpcb->l2cappcb, l2cap_disconnect_cfm);
        break;
    case PBAP_W4_RFCOMM_DISCONNECTED_AND_RESTART:
        if(pbap_client_cbs && pbap_client_cbs->pbap_client_connect_set_up)
            pbap_client_cbs->pbap_client_connect_realease(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = PBAP_IDLE;
        PBAP_PCB_RMV(&pbap_active_pcbs, pcb);
        pbap_close(pcb);
        break;

    default:
        break;
    }

    return BT_ERR_OK;
}

#endif

