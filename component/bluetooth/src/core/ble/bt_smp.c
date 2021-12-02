#include "bt_smp.h"

smp_cbs_t *smp_cbs;

smp_pcb_t *smp_active_pcbs;  /* List of all active SMP PCBs */
smp_pcb_t *smp_tmp_pcb;


#define SMP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define SMP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(smp_tmp_pcb = *pcbs; smp_tmp_pcb != NULL; smp_tmp_pcb = smp_tmp_pcb->next) { \
                               if(smp_tmp_pcb->next != NULL && smp_tmp_pcb->next == npcb) { \
                                  smp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)

							   

static err_t smp_send_data(smp_pcb_t *smp_pcb,struct bt_pbuf_t *p);
static err_t smp_handle_pairing_req(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p);

char* dump_smp_code(uint8_t smp_code)
{
    switch(smp_code)
    {
        CASE_RETURN_STR(SMP_OPCODE_PAIRING_REQ)
        CASE_RETURN_STR(SMP_OPCODE_PAIRING_RSP)
        CASE_RETURN_STR(SMP_OPCODE_CONFIRM)
        CASE_RETURN_STR(SMP_OPCODE_RAND)
        CASE_RETURN_STR(SMP_OPCODE_PAIRING_FAILED)
        CASE_RETURN_STR(SMP_OPCODE_ENCRYPT_INFO)
        CASE_RETURN_STR(SMP_OPCODE_MASTER_ID)
        CASE_RETURN_STR(SMP_OPCODE_IDENTITY_INFO)
        CASE_RETURN_STR(SMP_OPCODE_ID_ADDR)
        CASE_RETURN_STR(SMP_OPCODE_SIGN_INFO)
        CASE_RETURN_STR(SMP_OPCODE_SEC_REQ)
        CASE_RETURN_STR(SMP_OPCODE_PAIR_PUBLIC_KEY)
        CASE_RETURN_STR(SMP_OPCODE_PAIR_DHKEY_CHECK)
        CASE_RETURN_STR(SMP_OPCODE_PAIR_KEYPR_NOTIF)
    default:
        return "UNKNOWN SMP CODE";
    }
}


static smp_pcb_t *smp_pcb_new(l2cap_pcb_t *l2cap_pcb)
{
    smp_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_SMP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(smp_pcb_t));
        pcb->l2cappcb = l2cap_pcb;
        return pcb;
    }

    BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}
static smp_pcb_t *smp_get_active_pcb(struct bd_addr_t *bdaddr)
{
    smp_pcb_t *pcb = NULL;
    for(pcb = smp_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}
static void smp_pcb_close(smp_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_SMP_PCB, pcb);
        pcb = NULL;
    }
}



err_t l2cap_smp_connect(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    smp_pcb_t *smp_pcb = NULL;
    BT_SMP_TRACE_DEBUG("l2cap_smp_connect\n");

    if((smp_pcb = smp_pcb_new(l2cap_pcb)) == NULL)
    {
        BT_SMP_TRACE_DEBUG("l2cap_smp_connect: Could not alloc smp pcb\n");

        return BT_ERR_MEM;
    }

    bd_addr_set(&(smp_pcb->remote_addr),&(l2cap_pcb->remote_bdaddr));
    SMP_PCB_REG(&smp_active_pcbs, smp_pcb);

    return BT_ERR_OK;
}

err_t l2cap_smp_disconnect(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
    BT_SMP_TRACE_DEBUG("l2cap_smp_disconnect\n");

    smp_pcb_t *smp_pcb = smp_get_active_pcb(&l2cap_pcb->remote_bdaddr);
    if(smp_pcb)
    {
        SMP_PCB_RMV(&smp_active_pcbs, smp_pcb);
        smp_pcb_close(smp_pcb);
        return BT_ERR_OK;
    }
    else
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] l2cap_smp_disconnect fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }
}

err_t l2cap_smp_input(void *arg, l2cap_pcb_t *l2cap_pcb, struct bt_pbuf_t *p, err_t err)
{

    smp_pcb_t *smp_pcb = smp_get_active_pcb(&l2cap_pcb->remote_bdaddr);
    if(smp_pcb)
    {
        uint8_t smp_code = ((uint8_t *)p->payload)[0];
        BT_SMP_TRACE_DEBUG("l2cap_smp_input code:(%s=0x%x)\n",dump_smp_code(smp_code),smp_code);
        bt_hex_dump(p->payload,p->len);

        switch(smp_code)
        {
        case SMP_OPCODE_PAIRING_REQ:
            smp_handle_pairing_req(smp_pcb,p);
            break;
        case SMP_OPCODE_PAIRING_RSP:
            break;
        case SMP_OPCODE_CONFIRM:
            break;
        case SMP_OPCODE_RAND:
            break;
        case SMP_OPCODE_PAIRING_FAILED:
            break;
        case SMP_OPCODE_ENCRYPT_INFO:
            break;
        case SMP_OPCODE_MASTER_ID:
            break;
        case SMP_OPCODE_IDENTITY_INFO:
            break;
        case SMP_OPCODE_ID_ADDR:
            break;
        case SMP_OPCODE_SIGN_INFO:
            break;
        case SMP_OPCODE_SEC_REQ:
            break;
        case SMP_OPCODE_PAIR_PUBLIC_KEY:
            break;
        case SMP_OPCODE_PAIR_DHKEY_CHECK:
            break;
        case SMP_OPCODE_PAIR_KEYPR_NOTIF:
            break;
        default:
            break;
        }
    }
    else
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] l2cap_smp_input fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    return BT_ERR_OK;
}


err_t smp_init(smp_cbs_t *cb)
{
    smp_cbs = cb;

    l2cap_fixed_channel_register_recv(L2CAP_SM_CID,l2cap_smp_connect,l2cap_smp_disconnect,l2cap_smp_input);

    return BT_ERR_OK;

}

err_t smp_send_pair_rsp(smp_pcb_t *smp_pcb)
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_PAIR_RSP_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_PAIRING_RSP;
    ((uint8_t *)send_pbuf->payload)[1] = smp_pcb->local_io_cap;
    ((uint8_t *)send_pbuf->payload)[2] = smp_pcb->local_oob_flag;
    ((uint8_t *)send_pbuf->payload)[3] = smp_pcb->local_auth_req;
    ((uint8_t *)send_pbuf->payload)[4] = smp_pcb->local_enc_size;
    ((uint8_t *)send_pbuf->payload)[5] = smp_pcb->local_i_key;
    ((uint8_t *)send_pbuf->payload)[6] = smp_pcb->local_r_key;

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

	return BT_ERR_OK;
}


static err_t smp_send_data(smp_pcb_t *smp_pcb,struct bt_pbuf_t *p)
{
    BT_SMP_TRACE_DEBUG("smp_send_data\n");
    bt_hex_dump(p->payload,p->tot_len);

    l2cap_fixed_channel_datawrite(smp_pcb->l2cappcb,p,L2CAP_SM_CID);

    return BT_ERR_OK;
}


static err_t smp_handle_pairing_req(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p)
{
    uint8_t hci_version = hci_get_version();
    BT_SMP_TRACE_DEBUG("smp_handle_pairing_req\n");

    BT_SMP_TRACE_DEBUG("hci_get_version(0x%x)\n",hci_version);

    smp_pcb->remote_io_cap = ((uint8_t *)p->payload)[1];
    smp_pcb->remote_oob_flag = ((uint8_t *)p->payload)[2];
    smp_pcb->remote_auth_req = ((uint8_t *)p->payload)[3];
    smp_pcb->remote_enc_size = ((uint8_t *)p->payload)[4];
    smp_pcb->remote_i_key = ((uint8_t *)p->payload)[5];
    smp_pcb->remote_r_key = ((uint8_t *)p->payload)[6];

    BT_SMP_TRACE_DEBUG("remote_io_cap(0x%x)\n",smp_pcb->remote_io_cap);
    BT_SMP_TRACE_DEBUG("remote_oob_flag(0x%x)\n",smp_pcb->remote_oob_flag);
    BT_SMP_TRACE_DEBUG("remote_auth_req(0x%x)\n",smp_pcb->remote_auth_req);
    BT_SMP_TRACE_DEBUG("remote_enc_size(0x%x)\n",smp_pcb->remote_enc_size);
    BT_SMP_TRACE_DEBUG("remote_i_key(0x%x)\n",smp_pcb->remote_i_key);
    BT_SMP_TRACE_DEBUG("remote_r_key(0x%x)\n",smp_pcb->remote_r_key);

    smp_pcb->local_io_cap = SMP_IO_CAP_CONF;
    smp_pcb->local_oob_flag = SMP_OOB_PRESENT_CONF;
    smp_pcb->local_enc_size = SMP_MAX_ENC_KEY_SIZE;
    smp_pcb->local_i_key = smp_pcb->remote_i_key;
    smp_pcb->local_r_key = smp_pcb->remote_r_key;

    smp_pcb->local_auth_req |= SMP_BONDING;
	smp_pcb->local_auth_req |= SMP_AUTH_MIMT_BIT;
    if(hci_version >= HCI_PROTO_VERSION_4_2)
    {
        smp_pcb->local_auth_req |= SMP_SC_SUPPORT_BIT;
        if(hci_version >= HCI_PROTO_VERSION_5_0)
            smp_pcb->local_auth_req |= SMP_H7_SUPPORT_BIT;
    }
    else
    {
        smp_pcb->local_auth_req &= ~SMP_SC_SUPPORT_BIT;
        smp_pcb->local_auth_req &= ~SMP_H7_SUPPORT_BIT;
    }

	smp_send_pair_rsp(smp_pcb);

    return BT_ERR_OK;
}


