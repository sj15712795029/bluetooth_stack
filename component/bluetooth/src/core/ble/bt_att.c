#include "bt_att.h"

att_cbs_t *att_cbs;

struct l2cap_pcb_t *att_l2cap;

err_t att_input(void *arg, struct l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
        BT_ATT_TRACE_DEBUG("att_input\n");

    bt_hex_dump(p->payload,p->len);
	att_l2cap = l2cappcb;
    if(att_cbs && att_cbs->att_data_recv)
        att_cbs->att_data_recv(NULL,p);

	return BT_ERR_OK;
}


err_t att_init(void)
{
    l2cap_fixed_channel_register_recv(L2CAP_ATT_CID,att_input);

	return BT_ERR_OK;
}

err_t att_register_cb(att_cbs_t *cb)
{
	att_cbs = cb;

	return BT_ERR_OK;
}

err_t att_parse_read_req(struct bt_pbuf_t *p,uint16_t *handle)
{
	uint8_t *data = p->payload;
	uint8_t data_len = p->len;

	BT_ATT_TRACE_DEBUG("att_parse_read_req data_len(%d)\n",data_len);
	*handle = bt_le_read_16(data,1);

	return BT_ERR_OK;
}


err_t att_parse_find_info_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle)
{
	uint8_t *data = p->payload;
	uint8_t data_len = p->len;

	BT_ATT_TRACE_DEBUG("att_parse_find_info_req data_len(%d)\n",data_len);
	*s_handle = bt_le_read_16(data,1);
	*e_handle = bt_le_read_16(data,3);

	return BT_ERR_OK;
}


err_t att_parse_read_group_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,uint16_t *uuid)
{
	uint8_t *data = p->payload;
	uint8_t data_len = p->len;

	BT_ATT_TRACE_DEBUG("att_parse_read_group_type_req data_len(%d)\n",data_len);
	*s_handle = bt_le_read_16(data,1);
	*e_handle = bt_le_read_16(data,3);

	if(data_len ==  7)
		*uuid = bt_le_read_16(data,5);
	else
	{
		/* TODO:UUID128->UUID16 */
	}
		
	
	return BT_ERR_OK;
}

err_t att_parse_read_type_req(struct bt_pbuf_t *p,uint16_t *s_handle,uint16_t *e_handle,uint16_t *uuid)
{
	uint8_t *data = p->payload;
	uint8_t data_len = p->len;

	BT_ATT_TRACE_DEBUG("att_parse_read_type_req data_len(%d)\n",data_len);
	*s_handle = bt_le_read_16(data,1);
	*e_handle = bt_le_read_16(data,3);

	if(data_len ==  7)
		*uuid = bt_le_read_16(data,5);
	else
	{
		/* TODO:UUID128->UUID16 */
	}
		
	
	return BT_ERR_OK;
}


err_t att_send_data(struct bt_pbuf_t *p)
{
	BT_ATT_TRACE_DEBUG("att_send_data\n");

    bt_hex_dump(p->payload,p->tot_len);

	l2cap_fixed_channel_datawrite(att_l2cap,p,L2CAP_ATT_CID);
}



