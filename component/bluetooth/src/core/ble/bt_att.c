#include "bt_att.h"



err_t att_input(void *arg, struct l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
    BT_ATT_TRACE_DEBUG("att_input\n");

	bt_hex_dump(p->payload,p->len);
}




err_t att_init(void)
{
	l2cap_fixed_channel_register_recv(L2CAP_ATT_CID,att_input);
}

