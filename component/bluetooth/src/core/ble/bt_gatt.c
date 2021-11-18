#include "bt_gatt.h"


gatt_pcb_t *gatt_active_pcbs;  /* List of all active ATT PCBs */
gatt_pcb_t *gatt_tmp_pcb;


#define GATT_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define GATT_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(gatt_tmp_pcb = *pcbs; gatt_tmp_pcb != NULL; gatt_tmp_pcb = gatt_tmp_pcb->next) { \
                               if(gatt_tmp_pcb->next != NULL && gatt_tmp_pcb->next == npcb) { \
                                  gatt_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)




/* TODO:1. Queued writes 2.Server initiated中的ATT_MULTIPLE_HANDLE_VALUE_NTF  */
gatt_server_pri_service_t gatt_server_pri_service[GATT_PRI_SERVICE_MAX_COUNT] = {0};

gatt_client_manager_t gatt_client_manager = {0};
gatt_server_manager_t gatt_server_manager = {0};

gatt_cbs_t *gatt_cbs;




uint8_t gatt_gap_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GAP_SERVER)};
uint8_t gatt_gap_characteristic[] = {GATT_CHAR_PROP_BIT_READ,BT_LE_U16_TO_ARRAY(GATT_GAP_NAME_HANDLE),BT_LE_U16_TO_ARRAY(GATT_UUID_GAP_DEVICE_NAME)};
uint8_t gatt_server_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GATT_SERVER)};

gatt_server_service_t gap_service[] =
{
    {
        GATT_GAP_SERVICE_HANDLE,GATT_UUID_PRI_SERVICE,NULL,
        gatt_gap_uuid,sizeof(gatt_gap_uuid),GATT_PERM_READ
    },
    {
        GATT_GAP_CHARACTERISTIC_HANDLE,GATT_UUID_CHAR_DECLARE,NULL,
        gatt_gap_characteristic,sizeof(gatt_gap_characteristic),GATT_PERM_READ
    },
    {
        GATT_GAP_NAME_HANDLE,GATT_UUID_GAP_DEVICE_NAME,NULL,
        (uint8_t *)GATT_GAP_NAME,sizeof(GATT_GAP_NAME),GATT_PERM_READ
    },

};

gatt_server_service_t gatt_service[] =
{
    {
        GATT_SERVICE_HANLE,GATT_UUID_PRI_SERVICE,NULL,
        gatt_server_uuid,sizeof(gatt_server_uuid),GATT_PERM_READ
    },
};


static err_t gatts_handle_mtu_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_find_info_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_find_info_value_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_read_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_read_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_read_blob_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_read_multi_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_read_group_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_write_cmd(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_sig_write_cmd(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_pre_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_exc_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatts_handle_value_cfm(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);



static err_t gatt_handle_mtu_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_read_type_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_read_group_type_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_find_type_value_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);


static gatt_pcb_t *gatt_pcb_new(att_pcb_t *att_pcb)
{
    gatt_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_GATT_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(gatt_pcb_t));
        pcb->att_pcb = att_pcb;
        return pcb;
    }

    BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}
static gatt_pcb_t *gatt_get_active_pcb(struct bd_addr_t *bdaddr)
{
    gatt_pcb_t *pcb = NULL;
    for(pcb = gatt_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}
static void gatt_pcb_close(gatt_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_GATT_PCB, pcb);
        pcb = NULL;
    }
}



void att_gatt_connect_set_up(att_pcb_t *att_pcb,uint8_t status)
{
    gatt_pcb_t *gatt_pcb = NULL;
    BT_GATT_TRACE_DEBUG("att_gatt_connect_set_up status(%d)\n",status);
    bd_addr_debug_print(&att_pcb->remote_addr);


    if((gatt_pcb = gatt_pcb_new(att_pcb)) == NULL)
    {
        BT_GATT_TRACE_DEBUG("att_gatt_connect_set_up: Could not alloc rfcomm pcb\n");

        return;
    }

    gatt_pcb->att_pcb = att_pcb;
    bd_addr_set(&(gatt_pcb->remote_addr),&(att_pcb->remote_addr));
    GATT_PCB_REG(&gatt_active_pcbs, gatt_pcb);

    if(gatt_cbs && gatt_cbs->gatt_connect_set_up)
        gatt_cbs->gatt_connect_set_up(&gatt_pcb->remote_addr,BT_ERR_OK);

}

void att_gatt_connect_realease(att_pcb_t *att_pcb,uint8_t status)
{
    BT_GATT_TRACE_DEBUG("att_gatt_connect_realease status(%d)\n",status);
    bd_addr_debug_print(&att_pcb->remote_addr);

    gatt_pcb_t *gatt_pcb = gatt_get_active_pcb(&att_pcb->remote_addr);
    if(gatt_pcb)
    {
        if(gatt_cbs && gatt_cbs->gatt_connect_set_up)
            gatt_cbs->gatt_connect_realease(&gatt_pcb->remote_addr,BT_ERR_OK);

        GATT_PCB_RMV(&gatt_active_pcbs, gatt_pcb);
        gatt_pcb_close(gatt_pcb);
        return;
    }
    else
    {
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] att_gatt_connect_realease fail\n",__FILE__,__FUNCTION__,__LINE__);
        return;
    }

}

void att_gatt_data_recv(att_pcb_t *att_pcb,struct bt_pbuf_t *p)
{
    uint8_t opcode = ((uint8_t *)p->payload)[0];
    BT_GATT_TRACE_DEBUG("gatt_data_recv opcode(0x%x)\n",opcode);
    bd_addr_debug_print(&att_pcb->remote_addr);
    bt_hex_dump(p->payload,p->len);


    switch(opcode)
    {
    case ATT_RSP_ERROR:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_ERROR\n");
        break;
    }
    case ATT_REQ_MTU:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_MTU\n");
        gatts_handle_mtu_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_MTU:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_MTU\n");
        gatt_handle_mtu_rsp(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_REQ_FIND_INFO:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_FIND_INFO\n");
        gatts_handle_find_info_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_FIND_INFO:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_FIND_INFO\n");
        break;
    }
    case ATT_REQ_FIND_TYPE_VALUE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_FIND_TYPE_VALUE\n");
        gatts_handle_find_info_value_type_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_FIND_TYPE_VALUE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_FIND_TYPE_VALUE\n");
        gatt_handle_find_type_value_rsp(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_REQ_READ_BY_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ_BY_TYPE\n");
        gatts_handle_read_type_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_READ_BY_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_BY_TYPE\n");
        gatt_handle_read_type_rsp(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_REQ_READ:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ\n");
        gatts_handle_read_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_READ:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ\n");

        break;
    }
    case ATT_REQ_READ_BLOB:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ_BLOB\n");
        gatts_handle_read_blob_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_READ_BLOB:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_BLOB\n");
        break;
    }
    case ATT_REQ_READ_MULTI:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ_MULTI\n");
        gatts_handle_read_multi_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_READ_MULTI:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_MULTI\n");
        break;
    }
    case ATT_REQ_READ_BY_GRP_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ_BY_GRP_TYPE\n");
        gatts_handle_read_group_type_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_READ_BY_GRP_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_BY_GRP_TYPE\n");
        gatt_handle_read_group_type_rsp(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_REQ_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_WRITE\n");
        gatts_handle_write_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_WRITE\n");

        break;
    }
    case ATT_CMD_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_CMD_WRITE\n");
        gatts_handle_write_cmd(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_SIGN_CMD_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_SIGN_CMD_WRITE\n");
        gatts_handle_sig_write_cmd(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_REQ_PREPARE_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_PREPARE_WRITE\n");
        gatts_handle_pre_write_req(&att_pcb->remote_addr,p);
        break;
    }
    case ATT_RSP_PREPARE_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_PREPARE_WRITE\n");
        break;
    }
    case ATT_REQ_EXEC_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_EXEC_WRITE\n");
        gatts_handle_exc_write_req(&att_pcb->remote_addr,p);;
        break;
    }
    case ATT_RSP_EXEC_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_EXEC_WRITE\n");
        break;
    }
    case ATT_HANDLE_VALUE_NOTIF:
    {
        BT_GATT_TRACE_DEBUG("ATT_HANDLE_VALUE_NOTIF\n");
        break;
    }
    case ATT_HANDLE_VALUE_IND:
    {
        BT_GATT_TRACE_DEBUG("ATT_HANDLE_VALUE_IND\n");
        break;
    }
    case ATT_HANDLE_VALUE_CONF:
    {
        BT_GATT_TRACE_DEBUG("ATT_HANDLE_VALUE_CONF\n");
        gatts_handle_value_cfm(&att_pcb->remote_addr,p);
        break;
    }

    default:
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow opcode(0x%x)\n",__FILE__,__FUNCTION__,__LINE__,opcode);
        break;

    }
}

static att_cbs_t att_gatt_cb =
{
    att_gatt_connect_set_up,
    att_gatt_connect_realease,
    att_gatt_data_recv,
};

err_t gatt_init(gatt_cbs_t *cbs)
{
    BT_GATT_TRACE_DEBUG("gatt_init\n");

    gatt_cbs = cbs;
    att_init();
    att_register_cb(&att_gatt_cb);

    return BT_ERR_OK;
}


err_t gatt_server_init(void)
{
    gatt_server_manager.server_mtu = GATT_BLE_MTU_SIZE;
    gatt_server_manager.gatt_server_pri_service_count = 0;

    gatt_server_add_pri_service((gatt_server_service_t *)&gap_service,GATT_GAP_SERVICE_HANDLE,GATT_GAP_NAME_HANDLE,
                                sizeof(gap_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_GAP_SERVER,NULL,NULL);
    gatt_server_add_pri_service((gatt_server_service_t *)&gatt_service,GATT_SERVICE_HANLE,GATT_SERVICE_HANLE,
                                sizeof(gatt_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_GATT_SERVER,NULL,NULL);

    return BT_ERR_OK;
}

err_t gatt_server_add_pri_service(gatt_server_service_t *service,uint16_t start_handle,uint16_t end_handle,
                                  uint8_t service_count,uint16_t pri_uuid,uint8_t *pri_uuid128,gatt_pri_service_cbs_t *cb)
{
    uint8_t server_pri_service_cnt = gatt_server_manager.gatt_server_pri_service_count;
    gatt_server_pri_service[server_pri_service_cnt].serivce_count = service_count;
    gatt_server_pri_service[server_pri_service_cnt].start_handle = start_handle;
    gatt_server_pri_service[server_pri_service_cnt].end_handle = end_handle;
    gatt_server_pri_service[server_pri_service_cnt].pri_uuid = pri_uuid;
    gatt_server_pri_service[server_pri_service_cnt].gatt_server_service = service;
    gatt_server_pri_service[server_pri_service_cnt].cb = cb;
    if(pri_uuid128)
        memcpy(gatt_server_pri_service[server_pri_service_cnt].pri_uuid128,pri_uuid128,16);
    gatt_server_manager.gatt_server_pri_service_count++;

    return BT_ERR_OK;
}

err_t gatt_server_notification(uint16_t handle,uint8_t *value,uint8_t value_length)
{
    att_notification(handle,value,value_length);

    return BT_ERR_OK;
}

err_t gatt_server_indication(uint16_t handle,uint8_t *value,uint8_t value_length)
{
    att_indication(handle,value,value_length);

    return BT_ERR_OK;
}


static err_t gatts_handle_mtu_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint16_t mtu;

    att_parse_mtu_req(p,&mtu);
    BT_GATT_TRACE_DEBUG("gatts_handle_mtu_req handle(%d)\n",mtu);

    gatt_server_manager.client_mtu = gatt_server_manager.server_mtu = mtu;
    att_mtu_rsp(gatt_server_manager.server_mtu);

	if(gatt_cbs && gatt_cbs->gatt_server_cbs && gatt_cbs->gatt_server_cbs->gatt_mtu_value)
    	gatt_cbs->gatt_server_cbs->gatt_mtu_value(bdaddr,mtu);

    return BT_ERR_OK;
}

static err_t gatt_handle_mtu_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint16_t mtu;
    att_parse_mtu_rsp(p,&mtu);
    BT_GATT_TRACE_DEBUG("gatt_handle_mtu_rsp handle(%d)\n",mtu);

	if(gatt_cbs && gatt_cbs->gatt_client_cbs && gatt_cbs->gatt_client_cbs->gatt_mtu_value)
    	gatt_cbs->gatt_client_cbs->gatt_mtu_value(bdaddr,mtu);

    return BT_ERR_OK;
}


static err_t gatts_handle_read_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index;
    int8_t find_index = -1;
    uint16_t handle;
    uint8_t err_code;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};


    att_parse_read_req(p,&handle);
    BT_GATT_TRACE_DEBUG("gatts_handle_read_req handle(%d)\n",handle);


    for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
    {
        if((handle >= gatt_server_pri_service[index].start_handle) && (handle <= gatt_server_pri_service[index].end_handle))
        {
            find_index = index;
            break;
        }
    }

    BT_GATT_TRACE_DEBUG("find_index(%d)\n",find_index);

    if(gatt_server_pri_service[find_index].cb && gatt_server_pri_service[find_index].cb->gatt_db_read)
    {
        gatt_server_pri_service[find_index].cb->gatt_db_read(bdaddr,handle,rsp_buf,&rsp_buf_len,&err_code);
        BT_GATT_TRACE_DEBUG("db has callback,rsp_buf_len(%d)\n",rsp_buf_len);
        bt_hex_dump(rsp_buf,rsp_buf_len);
    }
    else
    {
        BT_GATT_TRACE_DEBUG("db has not callback\n");
        for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
        {
            if(gatt_server_pri_service[find_index].gatt_server_service[index].handle == handle)
            {
                memcpy(rsp_buf+rsp_buf_len,gatt_server_pri_service[find_index].gatt_server_service[index].value,gatt_server_pri_service[find_index].gatt_server_service[index].value_length);
                rsp_buf_len += gatt_server_pri_service[find_index].gatt_server_service[index].value_length;
            }
        }
    }

    att_read_rsp(rsp_buf,rsp_buf_len);

    return BT_ERR_OK;
}

static err_t gatts_handle_read_blob_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint16_t offset;
    uint16_t handle;


    att_parse_read_blob_req(p,&handle,&offset);

    /* TODO:做处理 */
    //att_read_blob_rsp

    return BT_ERR_OK;
}

static err_t gatts_handle_read_multi_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    /* TODO */
    att_parse_read_multi_req();
    return BT_ERR_OK;
}



static err_t gatts_handle_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index;
    int8_t find_index = -1;
    uint16_t handle;
    uint8_t req_buf_len = 0;
    uint8_t req_buf[GATT_BLE_MTU_SIZE] = {0};
    uint8_t err_code;
    att_parse_write_req(p,&handle,req_buf,&req_buf_len);

    for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
    {
        if((handle >= gatt_server_pri_service[index].start_handle) && (handle <= gatt_server_pri_service[index].end_handle))
        {
            find_index = index;
            break;
        }
    }

    BT_GATT_TRACE_DEBUG("find_index(%d)\n",find_index);

    if(gatt_server_pri_service[find_index].cb && gatt_server_pri_service[find_index].cb->gatt_db_write)
    {
        gatt_server_pri_service[find_index].cb->gatt_db_write(bdaddr,handle,req_buf,req_buf_len,&err_code);
    }

    att_write_rsp();

    return BT_ERR_OK;
}

static err_t gatts_handle_write_cmd(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index;
    int8_t find_index = -1;
    uint16_t handle;
    uint8_t err_code;
    uint8_t req_buf_len = 0;
    uint8_t req_buf[GATT_BLE_MTU_SIZE] = {0};

    att_parse_write_cmd(p,&handle,req_buf,&req_buf_len);

    for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
    {
        if((handle >= gatt_server_pri_service[index].start_handle) && (handle <= gatt_server_pri_service[index].end_handle))
        {
            find_index = index;
            break;
        }
    }

    BT_GATT_TRACE_DEBUG("find_index(%d)\n",find_index);

    if(gatt_server_pri_service[index].cb && gatt_server_pri_service[index].cb->gatt_db_write)
    {
        gatt_server_pri_service[index].cb->gatt_db_write(bdaddr,handle,req_buf,req_buf_len,&err_code);
    }

    return BT_ERR_OK;
}

static err_t gatts_handle_sig_write_cmd(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    att_parse_sig_write_cmd();
    return BT_ERR_OK;
}

static err_t gatts_handle_pre_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    att_parse_pre_write_req();
    att_pre_write_rsp();
    return BT_ERR_OK;
}
static err_t gatts_handle_exc_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    att_parse_exc_write_req();
    att_exc_write_rsp();
    return BT_ERR_OK;
}

static err_t gatts_handle_value_cfm(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    return BT_ERR_OK;
}



static err_t gatts_handle_find_info_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index = 0;
    int8_t find_index = -1;
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};
    uint8_t uuid_format;


    att_parse_find_info_req(p,&start_handle,&end_handle);


    BT_GATT_TRACE_DEBUG("gatts_handle_find_info_req start handle(%d) end handle\n",end_handle);
    for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
    {
        if((gatt_server_pri_service[index].start_handle <= end_handle) && (gatt_server_pri_service[index].end_handle >= start_handle))
        {
            find_index = index;
            break;
        }
    }

    BT_GATT_TRACE_DEBUG("gatts_handle_find_info_req find_index(%d)\n",find_index);

    for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
    {
        if(start_handle == gatt_server_pri_service[find_index].gatt_server_service[index].handle)
        {
            bt_le_store_16(rsp_buf,0,start_handle);
            if(gatt_server_pri_service[find_index].gatt_server_service[index].uuid16 == 0)
            {
                uuid_format = ATT_UUID128_FORMAT;
                memcpy(rsp_buf+2,gatt_server_pri_service[find_index].gatt_server_service[index].uuid128,16);
                rsp_buf_len = 18;
            }
            else
            {
                uuid_format = ATT_UUID16_FORMAT;
                bt_le_store_16(rsp_buf,2,gatt_server_pri_service[find_index].gatt_server_service[index].uuid16);
                rsp_buf_len = 4;
            }
            break;
        }
    }

    att_find_info_rsp(uuid_format,rsp_buf,rsp_buf_len);

    return BT_ERR_OK;
}


static err_t gatts_handle_find_info_value_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{

    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t att_type;
    uint8_t req_buf_len = 0;
    uint8_t req_buf[GATT_BLE_MTU_SIZE] = {0};


    att_parse_find_info_type_value_req(p,&start_handle,&end_handle,&att_type,req_buf,&req_buf_len);

    /* TODO:做处理 */
    //att_find_info_value_type_rsp

    return BT_ERR_OK;
}


static err_t gatts_handle_read_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index = 0;
    uint8_t uuid_format;
    uint8_t uuid128[16];
    int8_t find_index = -1;
    uint8_t find_uuid = 0;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};

    att_parse_read_type_req(p,&start_handle,&end_handle,&uuid_format,&uuid,(uint8_t **)&uuid128);


    for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
    {
        if((gatt_server_pri_service[index].start_handle <= end_handle) && (gatt_server_pri_service[index].end_handle >= start_handle))
        {
            find_index = index;
            break;
        }
    }


    BT_GATT_TRACE_DEBUG("gatts_handle_read_type_req find_index(%d)\n",find_index);
    for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
    {
        if((gatt_server_pri_service[find_index].gatt_server_service[index].handle >= start_handle) &&
                (gatt_server_pri_service[find_index].gatt_server_service[index].handle <= end_handle))
        {
            if(uuid_format == ATT_UUID16_FORMAT)
            {
                if(gatt_server_pri_service[find_index].gatt_server_service[index].uuid16 == uuid)
                {
                    find_uuid = 1;
                    bt_le_store_16(rsp_buf+rsp_buf_len,rsp_buf_len,gatt_server_pri_service[find_index].gatt_server_service[index].handle);
                    rsp_buf_len+=2;
                    memcpy(rsp_buf+rsp_buf_len,gatt_server_pri_service[find_index].gatt_server_service[index].value,gatt_server_pri_service[find_index].gatt_server_service[index].value_length);
                    rsp_buf_len += gatt_server_pri_service[find_index].gatt_server_service[index].value_length;
                }
            }
            else
            {
                /* UUID128扩展 */
            }
        }
    }

    if(find_uuid)
    {
        att_read_type_rsp(rsp_buf,rsp_buf_len);
    }
    else
    {
        att_err_rsp(ATT_REQ_READ_BY_TYPE,start_handle,ATT_NOT_FOUND);
    }
    return BT_ERR_OK;
}

static err_t gatts_handle_read_group_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index = 0;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};

    att_parse_read_group_type_req(p,&start_handle,&end_handle,&uuid);

    if(GATT_UUID_PRI_SERVICE == uuid)
    {
        for(index = 0; index < gatt_server_manager.gatt_server_pri_service_count; index++)
        {
            if(gatt_server_pri_service[index].start_handle >= start_handle)
            {
                bt_le_store_16(rsp_buf,0,gatt_server_pri_service[index].start_handle);
                bt_le_store_16(rsp_buf,2,gatt_server_pri_service[index].end_handle);

                if(gatt_server_pri_service[index].pri_uuid == 0)
                {
                    memcpy(rsp_buf+4,gatt_server_pri_service[index].pri_uuid128,16);
                    rsp_buf_len = 20;
                }
                else
                {
                    bt_le_store_16(rsp_buf,4,gatt_server_pri_service[index].pri_uuid);
                    rsp_buf_len = 6;
                }
                break;
            }
        }

        if(rsp_buf_len > 0)
        {
            att_read_group_type_rsp(rsp_buf,rsp_buf_len);

        }
        else
        {
            att_err_rsp(ATT_REQ_READ_BY_GRP_TYPE,start_handle,ATT_NOT_FOUND);
        }
    }

    return BT_ERR_OK;
}


static err_t gatt_handle_read_type_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t each_len;
    uint8_t data_num;
    uint8_t *data_list;
    att_parse_read_type_rsp(p,&each_len,&data_num,&data_list);

    BT_GATT_TRACE_DEBUG("gatt_handle_read_type_rsp each_len(%d) data_num(%d)\n",each_len,data_num);

    return BT_ERR_OK;
}


static err_t gatt_handle_read_group_type_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index;
    uint8_t each_len;
    uint8_t data_num;
    uint8_t *data_list;
    uint8_t uuid_type;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid;
    att_parse_read_group_type_rsp(p,&each_len,&data_num,&data_list,&uuid_type);

    BT_GATT_TRACE_DEBUG("gatt_handle_read_group_type_rsp each_len(%d) data_num(%d) uuid_type(%d)\n",each_len,data_num,uuid_type);

    for(index = 0; index < data_num; index++)
    {
        start_handle = bt_le_read_16(data_list,index*each_len);
        end_handle = bt_le_read_16(data_list,index*each_len+2);
        uuid = bt_le_read_16(data_list,index*each_len+4);
        BT_GATT_TRACE_DEBUG("s_handle(0x%x) e_handle(0x%x) uuid(0x%x)\n",start_handle,end_handle,uuid);
        /* TODO */
    }

    return BT_ERR_OK;
}


static err_t gatt_handle_find_type_value_rsp(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index = 0;
    uint8_t info_num;
    uint8_t *info_list;
    uint16_t start_handle;
    uint16_t end_handle;
    att_parse_find_type_value_rsp(p,&info_num,&info_list);

    BT_GATT_TRACE_DEBUG("gatt_handle_find_type_value_rsp info_num(%d)\n",info_num);

    for(index = 0; index < info_num; index++)
    {
        start_handle = bt_le_read_16(info_list,index*4);
        end_handle = bt_le_read_16(info_list,index*4+2);
        BT_GATT_TRACE_DEBUG("s_handle(0x%x) e_handle(0x%x)\n",start_handle,end_handle);
        /* TODO */
    }

    return BT_ERR_OK;
}



/* Gatt client API */

err_t gatt_client_init(void)
{
    gatt_client_manager.client_mtu= GATT_BLE_MTU_SIZE;

    return BT_ERR_OK;
}


err_t gatt_client_exchange_mtu(struct bd_addr_t *remote_addr,uint16_t mtu)
{
	gatt_pcb_t *gatt_pcb = gatt_get_active_pcb(remote_addr);
    if(gatt_pcb)
    {
        att_mtu_req(gatt_pcb->att_pcb,mtu);
    }
    else
    {
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] gatt_client_exchange_mtu fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    
    return BT_ERR_OK;
}

err_t gatt_client_discovery_pri_service(uint16_t start_handle,uint16_t end_handle)
{
    gatt_client_manager.last_opcode = GATT_CLIENT_OP_DISCOVERY;
    att_read_group_type_req(start_handle,end_handle,GATT_UUID_PRI_SERVICE);

    return BT_ERR_OK;
}

err_t gatt_client_discovery_pri_service_uuid(uint16_t start_handle,uint16_t end_handle,uint16_t uuid)
{
    uint8_t uuid_array[] = {BT_LE_U16_TO_ARRAY(uuid)};
    gatt_client_manager.last_opcode = GATT_CLIENT_OP_DISCOVERY;
    att_find_type_value_req(start_handle,end_handle,GATT_UUID_PRI_SERVICE,uuid_array,sizeof(uuid_array));

    return BT_ERR_OK;
}

err_t gatt_client_find_include(uint16_t start_handle,uint16_t end_handle)
{
    uint8_t uuid_array[] = {BT_LE_U16_TO_ARRAY(GATT_UUID_INCLUDE_SERVICE)};
    att_read_type_req(start_handle,end_handle,uuid_array,sizeof(uuid_array));

    return BT_ERR_OK;
}

err_t gatt_client_find_characteristics(uint16_t start_handle,uint16_t end_handle)
{
    uint8_t uuid_array[] = {BT_LE_U16_TO_ARRAY(GATT_UUID_CHAR_DECLARE)};
    att_read_type_req(start_handle,end_handle,uuid_array,sizeof(uuid_array));

    return BT_ERR_OK;
}

err_t gatt_client_find_characteristics_uuid(uint16_t start_handle,uint16_t end_handle,uint16_t uuid)
{
    return BT_ERR_OK;
}




