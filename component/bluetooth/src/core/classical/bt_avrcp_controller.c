/******************************************************************************
  * @file           bt_avrcp_controller.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt avrcp controller source file
******************************************************************************/

#include "bt_avrcp_controller.h"


extern struct avctp_pcb_t *temp_avcrp;
uint8_t wait_press_resp;

static  uint8_t avrcp_controller_service_record[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x0, /* Service record handle attribute */
    SDP_UINT32, 0x00, 0x01, 0x00, 0x01, /*dummy vals, filled in on xmit*/

    SDP_DES_SIZE8, 0xb,
    SDP_UINT16, 0x0, 0x1, /* Service class ID list attribute */
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x11,0x0e,
    SDP_UUID16, 0x11,0x0f,

    SDP_DES_SIZE8, 0x15,
    SDP_UINT16, 0x0, 0x4, /* Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0x10,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_UINT16, 0x0, 0x17,/* AVCTP */
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x0, 0x17, /* AVCTP UUID */
    SDP_UINT16, 0x1, 0x04,/* AVCTP version */

    SDP_DES_SIZE8, 0xd,
    SDP_UINT16, 0x0, 0x9, /* profile descriptor List */
    SDP_DES_SIZE8, 0x8,
    SDP_DES_SIZE8,0x06,
    SDP_UUID16,0x11,0x0e,
    SDP_UINT16,0x01,0x04,

    SDP_DES_SIZE8, 0x6,
    SDP_UINT16, 0x3, 0x11, /* support feature 1:speaker */
    SDP_UINT16,0x00,0x01,

};


static err_t avrcp_controller_get_capabilities(uint8_t capability_id)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;
struct bt_pbuf_t *p;
	if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+1, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
	BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
	

    cmd = p->payload;
    cmd[cmd_pos++] = AVRCP_CTYPE_STATUS;
    cmd[cmd_pos++] = (AVRCP_SUBUNIT_TYPE_PANEL << 3) |AVRCP_SUBUNIT_ID;
    cmd[cmd_pos++] = AVRCP_CMD_OPCODE_VENDOR_DEPENDENT;
    bt_be_store_24(cmd, cmd_pos, BT_SIG_COMPANY_ID);
    cmd_pos += 3;
    cmd[cmd_pos++] = AVRCP_PDU_ID_GET_CAPABILITIES;
    cmd[cmd_pos++] = 0;
    bt_be_store_16(cmd, cmd_pos, 1);
    cmd_pos += 2;
    cmd[cmd_pos++] = capability_id;

    avctp_datawrite(temp_avcrp, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_get_supported_company_ids()
{
    return avrcp_controller_get_capabilities(AVRCP_CAPABILITY_ID_COMPANY);
}

err_t avrcp_controller_get_supported_events()
{
    return avrcp_controller_get_capabilities(AVRCP_CAPABILITY_ID_EVENT);
}

err_t avrcp_controller_pass_through_press_cmd(avrcp_operation_id_e operation_id)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;
struct bt_pbuf_t *p;
	if((p  =bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_PASS_THROUGH_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
	BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }


    cmd = p->payload;
    cmd[cmd_pos++] = AVRCP_CTYPE_CONTROL;
    cmd[cmd_pos++] = (AVRCP_SUBUNIT_TYPE_PANEL << 3) |AVRCP_SUBUNIT_ID;
    cmd[cmd_pos++] = AVRCP_CMD_OPCODE_PASS_THROUGH;
    cmd[cmd_pos++] = operation_id;
    cmd[cmd_pos++] = 0;

    wait_press_resp = 1;
    avctp_datawrite(temp_avcrp, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_pass_through_realease_cmd(avrcp_operation_id_e operation_id)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;

	struct bt_pbuf_t *p;
	if((p  =bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_PASS_THROUGH_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
	BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
	

    cmd = p->payload;
    cmd[cmd_pos++] = AVRCP_CTYPE_CONTROL;
    cmd[cmd_pos++] = (AVRCP_SUBUNIT_TYPE_PANEL << 3) |AVRCP_SUBUNIT_ID;
    cmd[cmd_pos++] = AVRCP_CMD_OPCODE_PASS_THROUGH;
    cmd[cmd_pos++] = operation_id | 0x80;
    cmd[cmd_pos++] = 0;

    wait_press_resp = 0;
    avctp_datawrite(temp_avcrp, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);

    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_channel_up()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_CHANNEL_UP);
    return BT_ERR_OK;
}

err_t avrcp_controller_channel_down()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_CHANNEL_DOWN);
    return BT_ERR_OK;
}

err_t avrcp_controller_select()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_SELECT);
    return BT_ERR_OK;
}

err_t avrcp_controller_up()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_UP);
    return BT_ERR_OK;
}

err_t avrcp_controller_down()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_DOWN);
    return BT_ERR_OK;
}

err_t avrcp_controller_left()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_LEFT);
    return BT_ERR_OK;
}

err_t avrcp_controller_right()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_RIGHT);
    return BT_ERR_OK;
}

err_t avrcp_controller_root_menu()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_ROOT_MENU);
    return BT_ERR_OK;
}

err_t avrcp_controller_root_skip()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_SKIP);
    return BT_ERR_OK;
}


err_t avrcp_controller_vol_up()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_VOLUME_UP);
    return BT_ERR_OK;
}

err_t avrcp_controller_vol_down()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_VOLUME_DOWN);
    return BT_ERR_OK;
}

err_t avrcp_controller_mute()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_MUTE);
    return BT_ERR_OK;
}

err_t avrcp_controller_play()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_PLAY);
    return BT_ERR_OK;
}

err_t avrcp_controller_stop()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_STOP);
    return BT_ERR_OK;
}

err_t avrcp_controller_pause()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_PAUSE);
    return BT_ERR_OK;
}

err_t avrcp_controller_rewind()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_REWIND);
    return BT_ERR_OK;
}

err_t avrcp_controller_fast_forward()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_FAST_FORWARD);
    return BT_ERR_OK;
}

err_t avrcp_controller_forward()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_FORWARD);
    return BT_ERR_OK;
}

err_t avrcp_controller_backward()
{
    avrcp_controller_pass_through_press_cmd(AVRCP_OPERATION_ID_BACKWARD);
    return BT_ERR_OK;
}

err_t avrcp_controller_enable_notification(avrcp_notification_event_id_e event_id)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;

	struct bt_pbuf_t *p;
	if((p  =bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+5, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
	BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
	

    cmd = p->payload;
    cmd[cmd_pos++] = AVRCP_CTYPE_NOTIFY;
    cmd[cmd_pos++] = (AVRCP_SUBUNIT_TYPE_PANEL << 3) |AVRCP_SUBUNIT_ID;
    cmd[cmd_pos++] = AVRCP_CMD_OPCODE_VENDOR_DEPENDENT;
    bt_be_store_24(cmd, cmd_pos, BT_SIG_COMPANY_ID);
    cmd_pos += 3;
    cmd[cmd_pos++] = AVRCP_PDU_ID_REGISTER_NOTIFICATION;
    cmd[cmd_pos++] = 0;
    bt_be_store_16(cmd, cmd_pos, 5);
    cmd_pos += 2;
    cmd[cmd_pos++] = event_id;
    bt_be_store_32(cmd,cmd_pos,0);
    avctp_datawrite(temp_avcrp, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_get_element_attributes(uint8_t *element_attr, uint8_t element_size)
{
    uint8_t *cmd;
    uint8_t cmd_pos = 0;
    uint8_t index;

	struct bt_pbuf_t *p;
	if((p  = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+9+4*element_size, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
	BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    cmd = p->payload;
    cmd[cmd_pos++] = AVRCP_CTYPE_STATUS;
    cmd[cmd_pos++] = (AVRCP_SUBUNIT_TYPE_PANEL << 3) |AVRCP_SUBUNIT_ID;
    cmd[cmd_pos++] = AVRCP_CMD_OPCODE_VENDOR_DEPENDENT;
    bt_be_store_24(cmd, cmd_pos, BT_SIG_COMPANY_ID);
    cmd_pos += 3;
    cmd[cmd_pos++] = AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES;
    cmd[cmd_pos++] = 0;
    bt_be_store_16(cmd, cmd_pos, 9+4*element_size);
    cmd_pos += 2;
    memset(cmd+cmd_pos,0,8);		/* PLAYING */
    cmd_pos += 8;
    for(index = 0; index < element_size; index++)
    {
        bt_be_store_32(cmd,cmd_pos,element_attr[index]);
        cmd_pos += 4;
    }

    avctp_datawrite(temp_avcrp, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t avrcp_controller_input(struct bt_pbuf_t *p)
{
    uint8_t *data = p->payload;
    uint8_t opcode = data[2];


    BT_AVRCP_TRACE_DEBUG("avrcp_controller_input: p->len == %d p->tot_len == %d\n", p->len, p->tot_len);

    bt_hex_dump(p->payload,p->len);

    switch(opcode)
    {
    case AVRCP_CMD_OPCODE_VENDOR_DEPENDENT:
    {
        uint8_t pdu_id = data[6];
        uint16_t param_len = bt_be_read_16(data, 8);
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_input: pdu_id == 0x%x param_len == %d\n", pdu_id, param_len);

        break;
    }
    case AVRCP_CMD_OPCODE_UNIT_INFO:
    {
        break;
    }
    case AVRCP_CMD_OPCODE_SUBUNIT_INFO:
    {
        break;
    }
    case AVRCP_CMD_OPCODE_PASS_THROUGH:
    {
        avrcp_operation_id_e id = (avrcp_operation_id_e)data[3];
        if(wait_press_resp == 1)
            avrcp_controller_pass_through_realease_cmd(id);
        break;
    }
    default:
        break;
    }
    return BT_ERR_OK;
}

err_t avrcp_controller_init(void)
{
    struct sdp_record_t *record;

    uint32_t controller_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)avrcp_controller_service_record,sizeof(avrcp_controller_service_record),controller_record_hdl)) == NULL)
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_init: Could not alloc SDP record\n");
        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)avrcp_controller_service_record,sizeof(avrcp_controller_service_record));

    avctp_init();
    avctp_register_callback(avrcp_controller_input);
    return BT_ERR_OK;
}
