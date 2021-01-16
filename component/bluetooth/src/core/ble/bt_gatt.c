#include "bt_gatt.h"

uint8_t gatt_server_pri_service_count = 0;
gatt_server_pri_service_t gatt_server_pri_service[GATT_PRI_SERVICE_MAX_COUNT] = {0};


uint8_t gatt_gap_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GAP_SERVER)};
uint8_t gatt_gap_characteristic[] = {GATT_CHAR_PROP_BIT_READ,BT_LE_U16_TO_ARRAY(GATT_GAP_NAME_HANDLE),BT_LE_U16_TO_ARRAY(GATT_UUID_GAP_DEVICE_NAME)};
uint8_t gatt_server_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_GATT_SERVER)};

gatt_server_service_t gap_service[] =
{
    {GATT_GAP_SERVICE_HANDLE,GATT_UUID_PRI_SERVICE,gatt_gap_uuid,sizeof(gatt_gap_uuid),GATT_PERM_READ},
    {GATT_GAP_CHARACTERISTIC_HANDLE,GATT_UUID_CHAR_DECLARE,gatt_gap_characteristic,sizeof(gatt_gap_characteristic),GATT_PERM_READ},
    {GATT_GAP_NAME_HANDLE,GATT_UUID_GAP_DEVICE_NAME,GATT_GAP_NAME,sizeof(GATT_GAP_NAME),GATT_PERM_READ},

};

gatt_server_service_t gatt_service[] =
{
    {GATT_SERVICE_HANLE,GATT_UUID_PRI_SERVICE,gatt_server_uuid,sizeof(gatt_server_uuid),GATT_PERM_READ},
};

static err_t gatt_handle_mtu_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_read_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_find_info_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_read_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);
static err_t gatt_handle_read_group_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p);



void gatt_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{

}

void gatt_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{

}

void gatt_data_recv(struct bd_addr_t *remote_addr,struct bt_pbuf_t *p)
{
    uint8_t opcode = ((uint8_t *)p->payload)[0];
    BT_GATT_TRACE_DEBUG("gatt_data_recv opcode(0x%x)\n",opcode);
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
		gatt_handle_mtu_req(NULL,p);
        break;
    }
    case ATT_RSP_MTU:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_MTU\n");
        break;
    }
    case ATT_REQ_FIND_INFO:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_FIND_INFO\n");
		gatt_handle_find_info_req(NULL,p);
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
        break;
    }
    case ATT_RSP_FIND_TYPE_VALUE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_FIND_TYPE_VALUE\n");
        break;
    }
    case ATT_REQ_READ_BY_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ_BY_TYPE\n");
        gatt_handle_read_type_req(NULL,p);
        break;
    }
    case ATT_RSP_READ_BY_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_BY_TYPE\n");
        break;
    }
    case ATT_REQ_READ:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_READ\n");
        gatt_handle_read_req(NULL,p);
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
        gatt_handle_read_group_type_req(NULL,p);
        break;
    }
    case ATT_RSP_READ_BY_GRP_TYPE:
    {
        BT_GATT_TRACE_DEBUG("ATT_RSP_READ_BY_GRP_TYPE\n");
        break;
    }
    case ATT_REQ_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_WRITE\n");
        gatt_handle_write_req(NULL,p);
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
        break;
    }
    case ATT_REQ_PREPARE_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_REQ_PREPARE_WRITE\n");
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
        break;
    }
    case ATT_SIGN_CMD_WRITE:
    {
        BT_GATT_TRACE_DEBUG("ATT_SIGN_CMD_WRITE\n");
        break;
    }
    default:
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow opcode(0x%x)\n",__FILE__,__FUNCTION__,__LINE__,opcode);
        break;

    }
}

static att_cbs_t gatt_cb =
{
    gatt_connect_set_up,
    gatt_connect_realease,
    gatt_data_recv,
};



err_t gatt_server_init(void)
{
    BT_GATT_TRACE_DEBUG("gatt_server_init\n");

    att_register_cb(&gatt_cb);

    gatt_server_add_pri_service(&gap_service,GATT_GAP_SERVICE_HANDLE,GATT_GAP_NAME_HANDLE,sizeof(gap_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_GAP_SERVER);
    gatt_server_add_pri_service(&gatt_service,GATT_SERVICE_HANLE,GATT_SERVICE_HANLE,sizeof(gatt_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_GATT_SERVER);

    return BT_ERR_OK;
}

err_t gatt_server_add_pri_service(gatt_server_service_t *service,uint16_t start_handle,uint16_t end_handle,uint8_t service_count,uint16_t pri_uuid)
{
    gatt_server_pri_service[gatt_server_pri_service_count].serivce_count = service_count;
    gatt_server_pri_service[gatt_server_pri_service_count].start_handle = start_handle;
    gatt_server_pri_service[gatt_server_pri_service_count].end_handle = end_handle;
    gatt_server_pri_service[gatt_server_pri_service_count].pri_uuid = pri_uuid;
    gatt_server_pri_service[gatt_server_pri_service_count].gatt_server_service = service;
    gatt_server_pri_service_count++;
}

err_t gatt_server_notification(uint16_t handle,uint8_t *value,uint8_t value_length)
{
	struct bt_pbuf_t *send_pbuf;
	if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, 3+value_length, BT_PBUF_RAM)) == NULL)
    {
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = ATT_HANDLE_VALUE_NOTIF;
    bt_le_store_16((uint8_t *)send_pbuf->payload,1,handle);
	memcpy(((uint8_t *)send_pbuf->payload)+3, value, value_length);
	
    att_send_data(send_pbuf);
    bt_pbuf_free(send_pbuf);

	return BT_ERR_OK;
}

static err_t gatt_handle_mtu_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint16_t mtu;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};

    att_parse_mtu_req(p,&mtu);
    BT_GATT_TRACE_DEBUG("gatt_handle_mtu_req handle(%d)\n",mtu);

	att_send_mtu_rsp(GATT_BLE_MTU_SIZE);

	return BT_ERR_OK;
}

static err_t gatt_handle_read_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index;
    int8_t find_index = -1;
    uint16_t handle;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};
    struct bt_pbuf_t *send_pbuf;

    att_parse_read_req(p,&handle);
    BT_GATT_TRACE_DEBUG("gatt_handle_read_req handle(%d)\n",handle);

    for(index = 0; index < gatt_server_pri_service_count; index++)
    {
        if((handle >= gatt_server_pri_service[index].start_handle) && (handle <= gatt_server_pri_service[index].end_handle))
        {
            find_index = index;
            break;
        }
    }

    BT_GATT_TRACE_DEBUG("find_index(%d)\n",find_index);

    for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
    {
        if(gatt_server_pri_service[find_index].gatt_server_service[index].handle == handle)
        {
            memcpy(rsp_buf+rsp_buf_len,gatt_server_pri_service[find_index].gatt_server_service[index].value,gatt_server_pri_service[find_index].gatt_server_service[index].value_length);
            rsp_buf_len += gatt_server_pri_service[find_index].gatt_server_service[index].value_length;
        }
    }

    att_send_read_rsp(rsp_buf,rsp_buf_len);

    return BT_ERR_OK;
}


static err_t gatt_handle_write_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
	/* TODO:把service后面增加callback */
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }
    ((uint8_t *)send_pbuf->payload)[0] = ATT_RSP_WRITE;

    att_send_data(send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}


static err_t gatt_handle_find_info_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
	uint8_t index = 0;
	int8_t find_index = -1;
    uint16_t start_handle;
    uint16_t end_handle;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};
    struct bt_pbuf_t *send_pbuf;

    att_parse_find_info_req(p,&start_handle,&end_handle);

	for(index = 0; index < gatt_server_pri_service_count; index++)
	{
		if((gatt_server_pri_service[index].start_handle <= end_handle) && (gatt_server_pri_service[index].end_handle >= start_handle))
		{
			find_index = index;
			break;
		}
	}
	
	BT_GATT_TRACE_DEBUG("gatt_handle_find_info_req find_index(%d)\n",find_index);

	for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
    {
    	/* TODO1:如果>MTU怎么处理 TODO2:如果是UUID128怎么处理 TODO3:多handle处理 */
    	if(start_handle == gatt_server_pri_service[find_index].gatt_server_service[index].handle)
    	{
	        bt_le_store_16(rsp_buf,0,start_handle);
	        bt_le_store_16(rsp_buf,2,gatt_server_pri_service[find_index].gatt_server_service[index].uuid16);
			rsp_buf_len = 4;
			break;
    	}
    }

	att_send_find_info_rsp(ATT_UUID16_FORMAT,rsp_buf,rsp_buf_len);

    return BT_ERR_OK;
}


static err_t gatt_handle_find_info_value_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
	uint8_t index = 0;
	int8_t find_index = -1;
    uint16_t start_handle;
    uint16_t end_handle;
	uint16_t att_type;
	uint8_t req_buf_len = 0;
    uint8_t rsp_buf_len = 0;
    uint8_t req_buf[GATT_BLE_MTU_SIZE] = {0};
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};
	
    struct bt_pbuf_t *send_pbuf;

    att_parse_find_info_type_value_req(p,&start_handle,&end_handle,&att_type,req_buf,&req_buf_len);

	/* TODO:做处理 */
	//att_send_find_info_value_type_rsp

    return BT_ERR_OK;
}


static err_t gatt_handle_read_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
{
    uint8_t index = 0;
    int8_t find_index = -1;
    uint8_t find_uuid = 0;
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid;
    uint8_t rsp_buf_len = 0;
    uint8_t rsp_buf[GATT_BLE_MTU_SIZE] = {0};
    
    att_parse_read_type_req(p,&start_handle,&end_handle,&uuid);


	for(index = 0; index < gatt_server_pri_service_count; index++)
	{
		if((gatt_server_pri_service[index].start_handle <= end_handle) && (gatt_server_pri_service[index].end_handle >= start_handle))
		{
			find_index = index;
			break;
		}
	}
	

    BT_GATT_TRACE_DEBUG("gatt_handle_read_type_req find_index(%d)\n",find_index);
    for(index = 0; index < gatt_server_pri_service[find_index].serivce_count; index++)
    {
    	/* TODO1:uuid128的扩展 */
		if((gatt_server_pri_service[find_index].gatt_server_service[index].handle >= start_handle) && 
			(gatt_server_pri_service[find_index].gatt_server_service[index].handle <= end_handle))
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
    }

    if(find_uuid)
    {
		att_send_read_type_rsp(rsp_buf,rsp_buf_len);
    }
    else
    {
    	att_send_err_rsp(ATT_REQ_READ_BY_TYPE,start_handle,ATT_NOT_FOUND);
    }
    return BT_ERR_OK;
}

static err_t gatt_handle_read_group_type_req(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p)
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
        for(index = 0; index < gatt_server_pri_service_count; index++)
        {
            if(gatt_server_pri_service[index].start_handle >= start_handle)
            {
                printf("start(%d) end(%d) uuid(0x%x)\r\n",gatt_server_pri_service[index].start_handle,gatt_server_pri_service[index].end_handle,gatt_server_pri_service[index].pri_uuid);
                bt_le_store_16(rsp_buf,0,gatt_server_pri_service[index].start_handle);
                bt_le_store_16(rsp_buf,2,gatt_server_pri_service[index].end_handle);
                bt_le_store_16(rsp_buf,4,gatt_server_pri_service[index].pri_uuid);
                rsp_buf_len = 6;
                bt_hex_dump(rsp_buf,6);
                break;
            }
        }

        if(rsp_buf_len > 0)
        {
        	struct bt_pbuf_t *send_pbuf;
            if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, 8, BT_PBUF_RAM)) == NULL)
            {
                BT_GATT_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

                return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
            }
            ((uint8_t *)send_pbuf->payload)[0] = ATT_RSP_READ_BY_GRP_TYPE;
            ((uint8_t *)send_pbuf->payload)[1] = 6;
            memcpy(((uint8_t *)send_pbuf->payload)+2, rsp_buf, 6);
			att_send_data(send_pbuf);
    		bt_pbuf_free(send_pbuf);

        }
        else
        {
        	att_send_err_rsp(ATT_REQ_READ_BY_GRP_TYPE,start_handle,ATT_NOT_FOUND);
        }
    }

    return BT_ERR_OK;
}


