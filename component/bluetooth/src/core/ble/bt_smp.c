#include "bt_smp.h"
#include "bt_smp_key.h"

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


static const uint8_t smp_legacy_pair_table[2][5][5] =
{
    /* display only */	 /* Display Yes/No */	/* keyboard only */
    /* No Input/Output */ /* keyboard display */

    /* initiator */
    /* model = tbl[peer_io_caps][loc_io_caps] */
    {
        /* Display Only */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* Display Yes/No */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* Keyboard only */
        {
            SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* No Input No Output */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY
        },

        /* keyboard display */
        {
            SMP_MODEL_KEY_NOTIF, SMP_MODEL_KEY_NOTIF, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        }
    },

    /* responder */
    /* model = tbl[loc_io_caps][peer_io_caps] */
    {
        /* Display Only */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* Display Yes/No */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_KEY_NOTIF
        },

        /* keyboard only */
        {
            SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        },

        /* No Input No Output */
        {
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_ENCRYPTION_ONLY
        },

        /* keyboard display */
        {
            SMP_MODEL_PASSKEY, SMP_MODEL_PASSKEY, SMP_MODEL_KEY_NOTIF,
            SMP_MODEL_ENCRYPTION_ONLY, SMP_MODEL_PASSKEY
        }
    }
};

static const uint8_t smp_sc_pair_table[2][5][5] =
{
    /* display only */    /* Display Yes/No */   /* keyboard only */
    /* No InputOutput */  /* keyboard display */

    /* initiator */
    /* model = tbl[peer_io_caps][loc_io_caps] */   
    {   
		/* Display Only */
		{
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT
        },

        /* Display Yes/No */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        },

        /* keyboard only */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP
        },

        /* No Input No Output */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS
        },

        /* keyboard display */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_DISP, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        }
    },

    /* responder */
    /* model = tbl[loc_io_caps][peer_io_caps] */   
    {   
		/* Display Only */
		{
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_DISP
        },

        /* Display Yes/No */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        },

        /* keyboard only */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_PASSKEY_ENT,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_PASSKEY_ENT
        },

        /* No Input No Output */
        {
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_JUSTWORKS
        },

        /* keyboard display */
        {
            SMP_MODEL_SEC_CONN_PASSKEY_ENT, SMP_MODEL_SEC_CONN_NUM_COMP, SMP_MODEL_SEC_CONN_PASSKEY_DISP,
            SMP_MODEL_SEC_CONN_JUSTWORKS, SMP_MODEL_SEC_CONN_NUM_COMP
        }
    }
};


static void smp_c1_test(void);

static err_t smp_send_data(smp_pcb_t *smp_pcb,struct bt_pbuf_t *p);
static err_t smp_send_pair_rsp(smp_pcb_t *smp_pcb);
static err_t smp_send_pair_confirm(smp_pcb_t *smp_pcb,uint8_t confirm[16]);
static err_t smp_send_pair_fail(smp_pcb_t *smp_pcb,uint8_t reason);
static err_t smp_send_pair_random(smp_pcb_t *smp_pcb,uint8_t random[16]);
static err_t smp_send_enc_info(smp_pcb_t *smp_pcb,uint8_t ltk[16]);
static err_t smp_send_master_id(smp_pcb_t *smp_pcb,uint16_t ediv,uint8_t rand[8]);
static err_t smp_send_id_info(smp_pcb_t *smp_pcb,uint8_t irk[16]);
static err_t smp_send_id_addr_info(smp_pcb_t *smp_pcb,uint8_t addr_type,uint8_t addr[6]);
static err_t smp_handle_pairing_req(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p);
static err_t smp_handle_pairing_confirm(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p);
static err_t smp_handle_pairing_random(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p);
static err_t smp_distribution_key(smp_pcb_t *smp_pcb);
static uint8_t smp_select_pairing_method(smp_pcb_t *smp_pcb);


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

	if(smp_cbs && smp_cbs->smp_connect_set_up)
        smp_cbs->smp_connect_set_up(&smp_pcb->remote_addr,BT_ERR_OK);

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
    }
    else
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] l2cap_smp_disconnect fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

	if(smp_cbs && smp_cbs->smp_connect_realease)
        smp_cbs->smp_connect_realease(&smp_pcb->remote_addr,BT_ERR_OK);
	
	return BT_ERR_OK;
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
            smp_handle_pairing_confirm(smp_pcb,p);
            break;
        case SMP_OPCODE_RAND:
            smp_handle_pairing_random(smp_pcb,p);
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

err_t smp_ltk_request_handle(struct bd_addr_t *bdaddr,uint8_t *random,uint16_t ediv)
{
    smp_pcb_t *smp_pcb = smp_get_active_pcb(bdaddr);
    BT_SMP_TRACE_DEBUG("smp_ltk_request_handle\n");
    bt_addr_dump((uint8_t *)bdaddr);
    BT_SMP_TRACE_DEBUG("random\n");
    bt_hex_dump(random,8);
    BT_SMP_TRACE_DEBUG("ediv:0x%x\n",ediv);

    if(!smp_pcb)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] smp_pcb is NULL\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }
    hci_le_ltk_req_reply(bdaddr,smp_pcb->stk);

    return BT_ERR_OK;
}

err_t smp_enc_change_handle(struct bd_addr_t *bdaddr,uint8_t enc)
{
    smp_pcb_t *smp_pcb = smp_get_active_pcb(bdaddr);
    BT_SMP_TRACE_DEBUG("smp_enc_change_handle\n");
    bt_addr_dump((uint8_t *)bdaddr);
    BT_SMP_TRACE_DEBUG("enc:%d\n",enc);

    if(!smp_pcb)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] smp_pcb is NULL\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_CONN;
    }

    smp_distribution_key(smp_pcb);
    return BT_ERR_OK;
}


err_t smp_init(smp_cbs_t *cb)
{
    smp_cbs = cb;

    hci_register_ltk_req(smp_ltk_request_handle);
    hci_register_enc_change(smp_enc_change_handle);
    l2cap_fixed_channel_register_recv(L2CAP_SM_CID,l2cap_smp_connect,l2cap_smp_disconnect,l2cap_smp_input);

    return BT_ERR_OK;

}

static err_t smp_send_pair_rsp(smp_pcb_t *smp_pcb)
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

    memcpy(smp_pcb->pair_rsp_buf,send_pbuf->payload,SMP_PAIR_RSP_PACK_LEN);
    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}


static err_t smp_send_pair_confirm(smp_pcb_t *smp_pcb,uint8_t confirm[16])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_PAIR_CONFIRM_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_CONFIRM;
    memcpy(((uint8_t *)send_pbuf->payload)+1,confirm,16);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}

static err_t smp_send_pair_random(smp_pcb_t *smp_pcb,uint8_t random[16])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_PAIR_RANDOM_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_RAND;
    memcpy(((uint8_t *)send_pbuf->payload)+1,random,16);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}

static err_t smp_send_enc_info(smp_pcb_t *smp_pcb,uint8_t ltk[16])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_ENC_INFO_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_ENCRYPT_INFO;
    memcpy(((uint8_t *)send_pbuf->payload)+1,ltk,16);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}

static err_t smp_send_master_id(smp_pcb_t *smp_pcb,uint16_t ediv,uint8_t rand[8])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_MASTER_ID_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_MASTER_ID;
    bt_le_store_16((uint8_t *)send_pbuf->payload,1,ediv);
    memcpy(((uint8_t *)send_pbuf->payload)+3,rand,8);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}

static err_t smp_send_id_info(smp_pcb_t *smp_pcb,uint8_t irk[16])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_ID_INFO_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_IDENTITY_INFO;
    memcpy(((uint8_t *)send_pbuf->payload)+1,irk,16);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}


static err_t smp_send_id_addr_info(smp_pcb_t *smp_pcb,uint8_t addr_type,uint8_t addr[6])
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_ID_ADDR_INFO_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_ID_ADDR;
    ((uint8_t *)send_pbuf->payload)[1] = addr_type;
    memcpy(((uint8_t *)send_pbuf->payload)+2,addr,6);

    smp_send_data(smp_pcb,send_pbuf);
    bt_pbuf_free(send_pbuf);

    return BT_ERR_OK;
}



static err_t smp_send_pair_fail(smp_pcb_t *smp_pcb,uint8_t reason)
{
    struct bt_pbuf_t *send_pbuf;
    if((send_pbuf = bt_pbuf_alloc(BT_PBUF_RAW, SMP_PAIR_FAIL_PACK_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_SMP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)send_pbuf->payload)[0] = SMP_OPCODE_PAIRING_FAILED;
    ((uint8_t *)send_pbuf->payload)[1] = reason;

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

    memcpy(smp_pcb->pair_req_buf,p->payload,SMP_PAIR_REQ_PACK_LEN);
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
    //smp_pcb->local_i_key = smp_pcb->remote_i_key;
    //smp_pcb->local_r_key = smp_pcb->remote_r_key;

    smp_pcb->local_i_key = 0x03;
    smp_pcb->local_r_key = 0x03;

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

	smp_pcb->pairing_method = smp_select_pairing_method(smp_pcb);
	BT_SMP_TRACE_DEBUG("pairing_method(%d)\n",smp_pcb->pairing_method);

    smp_send_pair_rsp(smp_pcb);

    return BT_ERR_OK;
}

static err_t smp_handle_pairing_confirm(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p)
{
    uint8_t rsp[16];
    BT_SMP_TRACE_DEBUG("smp_handle_pairing_confirm\n");

    memcpy(smp_pcb->remote_confirm,((uint8_t *)p->payload)+1,16);

    /* Generate legency pairing random */
    *(uint32_t *)(smp_pcb->local_random) = rand();
    *(uint32_t *)(smp_pcb->local_random+4) = rand();
    *(uint32_t *)(smp_pcb->local_random+8) = rand();
    *(uint32_t *)(smp_pcb->local_random+12) = rand();

	bt_hex_dump(smp_pcb->tk,sizeof(smp_pcb->tk)); 


	switch(smp_pcb->pairing_method)
	{
		case SMP_MODEL_ENCRYPTION_ONLY:
			memset(smp_pcb->tk,0,sizeof(smp_pcb->tk));
			break;
		case SMP_MODEL_PASSKEY:
		{
			uint32_t input_passkey;

			if(smp_cbs && smp_cbs->smp_passkey_input)
        		smp_cbs->smp_passkey_input(&smp_pcb->remote_addr,&input_passkey);
			BT_SMP_TRACE_DEBUG("Input passkey:%d\n",input_passkey);
			
			bt_le_store_32(smp_pcb->tk,0,input_passkey);
			break;
		}
		case SMP_MODEL_OOB:
			break;
		case SMP_MODEL_KEY_NOTIF:
		{
			uint32_t gen_passkey = rand() & 999999;

			bt_le_store_32(smp_pcb->tk,0,gen_passkey);
			
			if(smp_cbs && smp_cbs->smp_passkey_display)
        		smp_cbs->smp_passkey_display(&smp_pcb->remote_addr,gen_passkey);
			break;
		}
		default:
			break;
	}
			
    smp_c1(smp_pcb->tk, smp_pcb->local_random, smp_pcb->pair_req_buf,smp_pcb->pair_rsp_buf,
           (uint8_t *)&smp_pcb->remote_addr,1, hci_get_local_addr(),0, rsp);

    smp_send_pair_confirm(smp_pcb,rsp);

    return BT_ERR_OK;
}

static err_t smp_handle_pairing_random(smp_pcb_t *smp_pcb, struct bt_pbuf_t *p)
{
    uint8_t confirm[16];
    uint8_t *remote_random = ((uint8_t *)p->payload)+1;
    BT_SMP_TRACE_DEBUG("smp_handle_pairing_random\n");

    memcpy(smp_pcb->remote_random,remote_random,16);
    smp_c1(smp_pcb->tk, remote_random, smp_pcb->pair_req_buf,smp_pcb->pair_rsp_buf,
           (uint8_t *)&smp_pcb->remote_addr,1, hci_get_local_addr(),0, confirm);

    if(memcmp(confirm,smp_pcb->remote_confirm,16))
    {
        BT_SMP_TRACE_ERROR("pairing confirm check fail\n");
        smp_send_pair_fail(smp_pcb,SMP_CONFIRM_VALUE_ERR);
        return BT_ERR_VAL;
    }
    else
    {
        BT_SMP_TRACE_DEBUG("pairing confirm check pass\n");
    }

    smp_send_pair_random(smp_pcb,smp_pcb->local_random);

    smp_s1(smp_pcb->tk,smp_pcb->local_random,remote_random, smp_pcb->stk);
    BT_SMP_TRACE_DEBUG("STK:%s\n",bt_hex_string(smp_pcb->stk,16));


    return BT_ERR_OK;
}

static err_t smp_distribution_key(smp_pcb_t *smp_pcb)
{
    uint8_t ltk[16] = {0};
    uint16_t ediv;
    uint8_t random[8] = {0};
    uint8_t irk[16] = {0};
    /* Generate ltk */
    *(uint32_t *)(ltk) = rand();
    *(uint32_t *)(ltk+4) = rand();
    *(uint32_t *)(ltk+8) = rand();
    *(uint32_t *)(ltk+12) = rand();

    ediv = rand() & 0xffff;

    *(uint32_t *)(random) = rand();
    *(uint32_t *)(random+4) = rand();

    *(uint32_t *)(irk) = rand();
    *(uint32_t *)(irk+4) = rand();
    *(uint32_t *)(irk+8) = rand();
    *(uint32_t *)(irk+12) = rand();

    smp_send_enc_info(smp_pcb,ltk);
    smp_send_master_id(smp_pcb,ediv,random);
    smp_send_id_info(smp_pcb,irk);
    smp_send_id_addr_info(smp_pcb,0,hci_get_local_addr());

    return BT_ERR_OK;
}


static uint8_t smp_select_pairing_method(smp_pcb_t *smp_pcb)
{
    uint8_t pairing_method = SMP_MODEL_OUT_OF_RANGE;
    BT_SMP_TRACE_DEBUG("%s remote_io_cap(%d) local_io_cap(%d)\n",__FUNCTION__, smp_pcb->remote_io_cap, smp_pcb->local_io_cap);
    BT_SMP_TRACE_DEBUG("%s remote_oob_flag(%d) local_oob_flag(%d)\n",__FUNCTION__, smp_pcb->remote_oob_flag, smp_pcb->local_oob_flag);
    BT_SMP_TRACE_DEBUG("%s remote_auth_req(0x%02x) local_auth_req(0x%02x)\n",__FUNCTION__, smp_pcb->remote_auth_req, smp_pcb->local_auth_req);
 
    if ((smp_pcb->remote_auth_req & SMP_SC_SUPPORT_BIT) && (smp_pcb->local_auth_req & SMP_SC_SUPPORT_BIT))
    {
        smp_pcb->use_sc = 1;
    }

    BT_SMP_TRACE_DEBUG("use security connection(%d)\n", smp_pcb->use_sc);

    if (smp_pcb->use_sc)
    {
        /* base on Core5.3 page 1573:Table 2.7: Rules for using Out-of-Band and MITM flags for LE Secure Connections pairing */
        if (smp_pcb->remote_oob_flag == SMP_OOB_PRESENT || smp_pcb->local_oob_flag == SMP_OOB_PRESENT)
        {
        	BT_SMP_TRACE_DEBUG("return pair method SMP_MODEL_SEC_CONN_OOB\n");
            return SMP_MODEL_SEC_CONN_OOB;
        }

        /* base on Core5.3 page 1573:Table 2.7: Rules for using Out-of-Band and MITM flags for LE Secure Connections pairing */
        if ((!(smp_pcb->remote_auth_req & SMP_AUTH_MIMT_BIT)) && (!(smp_pcb->local_auth_req & SMP_AUTH_MIMT_BIT)))
        {
        	BT_SMP_TRACE_DEBUG("return pair method SMP_MODEL_SEC_CONN_JUSTWORKS\n");
            return SMP_MODEL_SEC_CONN_JUSTWORKS;
        }

        if (smp_pcb->l2cappcb->conn_role == HCI_ROLE_MASTER)
        {
            pairing_method = smp_sc_pair_table[smp_pcb->l2cappcb->conn_role ][smp_pcb->remote_io_cap][smp_pcb->local_io_cap];
        }
        else
        {
            pairing_method = smp_sc_pair_table[smp_pcb->l2cappcb->conn_role ][smp_pcb->local_io_cap][smp_pcb->remote_io_cap];
        }
    }
    else
    {
        /* base on Core5.3 page 1572:Table 2.6: Rules for using Out-of-Band and MITM flags for LE legacy pairing */
        if ((smp_pcb->remote_oob_flag == SMP_OOB_PRESENT) && (smp_pcb->local_oob_flag == SMP_OOB_PRESENT))
        {
        	BT_SMP_TRACE_DEBUG("return pair method SMP_MODEL_OOB\n");
            return SMP_MODEL_OOB;
        }

        /* base on Core5.3 page 1572:Table 2.6: Rules for using Out-of-Band and MITM flags for LE legacy pairing */
        if ((!(smp_pcb->remote_auth_req & SMP_AUTH_MIMT_BIT)) && (!(smp_pcb->local_auth_req & SMP_AUTH_MIMT_BIT)))
        {
        	BT_SMP_TRACE_DEBUG("return pair method SMP_MODEL_ENCRYPTION_ONLY\n");
            return SMP_MODEL_ENCRYPTION_ONLY;
		}

        if (smp_pcb->l2cappcb->conn_role == HCI_ROLE_MASTER)
        {
            pairing_method = smp_legacy_pair_table[smp_pcb->l2cappcb->conn_role ][smp_pcb->remote_io_cap][smp_pcb->local_io_cap];
        }
        else
        {
            pairing_method = smp_legacy_pair_table[smp_pcb->l2cappcb->conn_role ][smp_pcb->local_io_cap][smp_pcb->remote_io_cap];
        }

    }

    BT_SMP_TRACE_DEBUG("conn_role(%d) pairing_method(%d)\n", smp_pcb->l2cappcb->conn_role,pairing_method);

    return pairing_method;
}


static void smp_c1_test(void)
{
    /* Confirm Value	1AFA6675:F2324666:4F6CAA84:68B1ED92 */
    /* Random Value	7CF7ED7D:681EB48D:A11EA4DE:505A90F5 */
    uint8_t confirm[16];
    uint8_t tk[16] = {0};

    uint8_t request_cmd[7] = {0x01,0x04,0x00,0x05,0x10,0x07,0x07};
    uint8_t response_cmd[7] = {0x02,0x03,0x00,0x01,0x10,0x07,0x07};

    uint8_t local_random[16] = {0xf5,0x90,0x5a,0x50,0xde,0xa4,0x1e,0xa1,\
                                0x8d,0xb4,0x1e,0x68,0x7d,0xed,0xf7,0x7c
                               };
    uint8_t ra[6] = {0x98,0x1a,0x20,0x86,0x1d,0x00};
    uint8_t rat = 0;
    uint8_t ia[6] = {0x52,0x24,0x6c,0xf0,0x03,0x6a};
    uint8_t iat = 1;

    BT_SMP_TRACE_DEBUG("request_cmd: %s\n", bt_hex_string(request_cmd, 7));
    BT_SMP_TRACE_DEBUG("response_cmd: %s\n", bt_hex_string(response_cmd, 7));
    BT_SMP_TRACE_DEBUG("local_random: %s\n", bt_hex_string(local_random, 16));
    BT_SMP_TRACE_DEBUG("ia: %s\n", bt_hex_string(ia, 6));
    BT_SMP_TRACE_DEBUG("ra: %s\n", bt_hex_string(ra, 6));
    BT_SMP_TRACE_DEBUG("==================================================\n");
    smp_c1(tk, (uint8_t *)&local_random, request_cmd, response_cmd,
           ia,iat, ra,rat, confirm);

    BT_SMP_TRACE_DEBUG("\n==================================================\n");
    BT_SMP_TRACE_DEBUG("data %s\n", bt_hex_string(confirm, 16));
}


static void smp_s1_test(void)
{
    /*
    TK(LSB ~ MSB):
    [000]: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

    R1(LSB ~ MSB):
    [000]: 19 d7 14 88 d1 d8 16 52 2c 58 00 8d 94 19 da 6b

    R2(LSB ~ MSB):
    [000]: ad 1a aa bb c9 d4 41 6d ce bb 13 8e 39 29 e4 23

    S1(LSB ~ MSB):
    [000]: 97 3b a7 52 bc be e9 3e 33 4e 19 49 42 ff 1f 00
    */

    uint8_t s1[16];
    uint8_t tk[16] = {0};
    uint8_t r1[16] = {0x19,0xd7,0x14,0x88,0xd1,0xd8,0x16,0x52,0x2c,0x58,0x00,0x8d,0x94,0x19,0xda,0x6b};
    uint8_t r2[16] = {0xad,0x1a,0xaa,0xbb,0xc9,0xd4,0x41,0x6d,0xce,0xbb,0x13,0x8e,0x39,0x29,0xe4,0x23};

    BT_SMP_TRACE_DEBUG("-------------------------------------------------\n");
    smp_s1(tk,r1,r2,s1);
    BT_SMP_TRACE_DEBUG("data %s\n", bt_hex_string(s1, 16));
    BT_SMP_TRACE_DEBUG("-------------------------------------------------\n");
}



