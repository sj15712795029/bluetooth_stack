#include "bt_smp.h"

smp_cbs_t *smp_cbs;

err_t l2cap_smp_connect(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    BT_SMP_TRACE_DEBUG("l2cap_smp_connect\n");

    return BT_ERR_OK;
}

err_t l2cap_smp_disconnect(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    BT_SMP_TRACE_DEBUG("l2cap_smp_disconnect\n");

	return BT_ERR_OK;
}

err_t l2cap_smp_input(void *arg, l2cap_pcb_t *l2cap_pcb, struct bt_pbuf_t *p, err_t err)
{
    BT_SMP_TRACE_DEBUG("l2cap_smp_input\n");

	bt_hex_dump(p->payload,p->len);
    return BT_ERR_OK;
}


err_t smp_init(smp_cbs_t *cb)
{
	smp_cbs = cb;

	l2cap_fixed_channel_register_recv(L2CAP_SM_CID,l2cap_smp_connect,l2cap_smp_disconnect,l2cap_smp_input);

    return BT_ERR_OK;
	
}

