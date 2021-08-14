/******************************************************************************
  * @file           bt_rfcomm.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt rfcomm source file
******************************************************************************/

#include "bt_rfcomm.h"
#include "bt_fcs.h"
#include "bt_common.h"
#include "bt_pbuf.h"

rfcomm_pcb_listen_t *rfcomm_listen_pcbs; /* List of all RFCOMM PCBs listening for
					      an incomming connection on a specific
					      server channel */
rfcomm_pcb_t *rfcomm_active_pcbs;  /* List of all active RFCOMM PCBs */
rfcomm_pcb_t *rfcomm_tmp_pcb;



#define RFCOMM_EVENT_CONNECTED(pcb,err) \
                              if((pcb)->connected != NULL) \
                              ((pcb)->connected((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_ACCEPT(pcb,err) \
                              if((pcb)->accept != NULL) \
                              ((pcb)->accept((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_DISCONNECTED(pcb,err) \
                                 if((pcb)->disconnected != NULL) { \
                                   ((pcb)->disconnected((pcb)->callback_arg,(pcb),(err))); \
                                 } else { \
                                   rfcomm_close(pcb); \
				 }
#define RFCOMM_EVENT_PN_RSP(pcb,err) \
                       if((pcb)->pn_rsp != NULL) \
                       ((pcb)->pn_rsp((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_TEST(pcb,err) \
                       if((pcb)->test_rsp != NULL) \
                       ((pcb)->test_rsp((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_MSC(pcb,err) \
                        if((pcb)->msc_rsp != NULL) \
                        ((pcb)->msc_rsp((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_RPN(pcb,err) \
                        if((pcb)->rpn_rsp != NULL) \
                        ((pcb)->rpn_rsp((pcb)->callback_arg,(pcb),(err)))
#define RFCOMM_EVENT_RECV(pcb,err,p) \
                          if((pcb)->recv != NULL) { \
                            ((pcb)->recv((pcb)->callback_arg,(pcb),(p),(err))); \
                          } else { \
                            bt_pbuf_free(p); \
                          }


/* Define two macros, RFCOMM_REG and RFCOMM_RMV that registers a RFCOMM PCB
   with a PCB list or removes a PCB from a list, respectively. */

#define RFCOMM_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define RFCOMM_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(rfcomm_tmp_pcb = *pcbs; rfcomm_tmp_pcb != NULL; rfcomm_tmp_pcb = rfcomm_tmp_pcb->next) { \
                               if(rfcomm_tmp_pcb->next != NULL && rfcomm_tmp_pcb->next == npcb) { \
                                  rfcomm_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)



/* Forward declarations */
rfcomm_pcb_t *rfcomm_get_active_pcb(uint8_t cn, struct bd_addr_t *bdaddr);

static err_t rfcomm_connect_ind(void *arg, l2cap_pcb_t *pcb, err_t err);
static err_t rfcomm_disconnect_ind(void *arg, l2cap_pcb_t *pcb, err_t err);

static err_t rfcomm_connect_ind(void *arg, l2cap_pcb_t *pcb, err_t err)
{
    BT_RFCOMM_TRACE_DEBUG("rfcomm_connect_ind\n");

    l2cap_register_disconnect_ind(pcb, rfcomm_disconnect_ind);
    l2cap_register_recv(pcb, rfcomm_input);
    return BT_ERR_OK;
}


static err_t rfcomm_disconnect_ind(void *arg, l2cap_pcb_t *pcb, err_t err)
{
    BT_RFCOMM_TRACE_DEBUG("bt_disconnect_ind\n");

    if(pcb->psm == RFCOMM_PSM)
    {
        rfcomm_lp_disconnected(pcb);
    }

    l2cap_close(pcb);
    return BT_ERR_OK;
}


err_t rfcomm_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    err_t ret = BT_ERR_OK;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_disconnected: CN = %d\n", rfcomm_cn(pcb));
    rfcomm_close(pcb);

    return ret;
}


static err_t rfcomm_accept(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    BT_RFCOMM_TRACE_DEBUG("rfcomm_accept: CN = %d\n", rfcomm_cn(pcb));


    rfcomm_register_disc(pcb, rfcomm_disconnected);

    return BT_ERR_OK;
}


err_t rfcomm_init(void)
{
    /* Clear globals */
    rfcomm_listen_pcbs = NULL;
    rfcomm_active_pcbs = NULL;
    rfcomm_tmp_pcb = NULL;

    l2cap_register_connect_ind(RFCOMM_PSM, rfcomm_connect_ind);

    BT_RFCOMM_TRACE_DEBUG("rfcomm_init: Allocate RFCOMM PCB for CN 0\n");

    rfcomm_listen(0, rfcomm_accept);

    return BT_ERR_OK;

}

void rfcomm_tmr(void)
{
    rfcomm_pcb_t *pcb, *tpcb;
    err_t ret = BT_ERR_OK;

	BT_UNUSED_ARG(ret);

    /* Step through all of the active pcbs */
    for(pcb = rfcomm_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(pcb->to != 0)
        {
            --pcb->to;
            BT_RFCOMM_TRACE_DEBUG("rfcomm_tmr: %d\n", pcb->to);
            if(pcb->to == 0)
            {
                /* Timeout */
                if(pcb->cn == 0)
                {
                    /* If DLC 0 timed out, disconnect all other DLCs on this multiplexer session first */
                    for(tpcb = rfcomm_active_pcbs; tpcb != NULL; tpcb = tpcb->next)
                    {
                        if(tpcb->cn != 0 && bd_addr_cmp(&(tpcb->l2cappcb->remote_bdaddr), &(pcb->l2cappcb->remote_bdaddr)))
                        {
                            //RFCOMM_RMV(&rfcomm_active_pcbs, tpcb); /* Remove pcb from active list */
                            tpcb->state = RFCOMM_CLOSED;
                            RFCOMM_EVENT_DISCONNECTED(tpcb,BT_ERR_OK); /* Notify upper layer */
                        }
                    }
                }
                /* Disconnect this DLC */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_tmr: Timeout! Disconnect this DLC. State = %d\n", pcb->state);
                //RFCOMM_RMV(&rfcomm_active_pcbs, pcb); /* Remove pcb from active list */
                pcb->state = RFCOMM_CLOSED;
                RFCOMM_EVENT_DISCONNECTED(pcb,BT_ERR_OK); /* Notify upper layer */
            }
        }
    }
}

err_t rfcomm_lp_disconnected(l2cap_pcb_t *l2cappcb)
{
    rfcomm_pcb_t *pcb, *tpcb;
    err_t ret = BT_ERR_OK;

    pcb = rfcomm_active_pcbs;
    while(pcb != NULL)
    {
        tpcb = pcb->next;
        if(bd_addr_cmp(&(l2cappcb->remote_bdaddr), &(pcb->l2cappcb->remote_bdaddr)))
        {
            pcb->state = RFCOMM_CLOSED;
            RFCOMM_EVENT_DISCONNECTED(pcb,BT_ERR_OK); /* Notify upper layer */
        }
        pcb = tpcb;
    }

    return ret;
}

rfcomm_pcb_t *rfcomm_new(l2cap_pcb_t *l2cappcb)
{
    rfcomm_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_RFCOMM_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(rfcomm_pcb_t));
        pcb->l2cappcb = l2cappcb;

        pcb->cl = RFCOMM_CL; /* Default convergence layer */
        pcb->n = RFCOMM_N; /* Default maximum frame size */

        pcb->state = RFCOMM_CLOSED;
        return pcb;
    }

    BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void rfcomm_close(rfcomm_pcb_t *pcb)
{

    if(pcb->state == RFCOMM_LISTEN)
    {
        RFCOMM_RMV((rfcomm_pcb_t **)&rfcomm_listen_pcbs, pcb);
        bt_memp_free(MEMP_RFCOMM_PCB_LISTEN, pcb);
    }
    else
    {
#if RFCOMM_FLOW_QUEUEING
    if(pcb->buf != NULL)
    {
        bt_pbuf_free(pcb->buf);
    }
#endif
        RFCOMM_RMV(&rfcomm_active_pcbs, pcb);
        bt_memp_free(MEMP_RFCOMM_PCB, pcb);
    }
    pcb = NULL;
}

void rfcomm_deinit(void)
{
    rfcomm_pcb_t *pcb, *tpcb;
    rfcomm_pcb_listen_t *lpcb, *tlpcb;

    for(pcb = rfcomm_active_pcbs; pcb != NULL;)
    {
        tpcb = pcb->next;
        rfcomm_close(pcb);
        pcb = tpcb;
    }

    for(lpcb = rfcomm_listen_pcbs; lpcb != NULL;)
    {
        tlpcb = lpcb->next;
        rfcomm_close((rfcomm_pcb_t *)lpcb);
        lpcb = tlpcb;
    }

    /* Clear globals */
    rfcomm_listen_pcbs = NULL;
    rfcomm_active_pcbs = NULL;
    rfcomm_tmp_pcb = NULL;
}

rfcomm_pcb_t *rfcomm_get_active_pcb(uint8_t cn, struct bd_addr_t *bdaddr)
{
    rfcomm_pcb_t *pcb;
    for(pcb = rfcomm_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(pcb->cn == cn && bd_addr_cmp(&(pcb->l2cappcb->remote_bdaddr),
                                        bdaddr))
        {
            break;
        }
    }
    return pcb;
}

static err_t rfcomm_dm(l2cap_pcb_t *pcb, rfcomm_hdr_t *hdr)
{
    struct bt_pbuf_t *p;
    rfcomm_hdr_t *rfcommhdr;
    err_t ret;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_dm\n");

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_DM_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    rfcommhdr = p->payload;
    rfcommhdr->addr = hdr->addr & 0xFB; /* Set direction bit to 0 for the response */
    rfcommhdr->ctrl = RFCOMM_DM;
    rfcommhdr->len = 1; /* EA bit set to 1 to indicate a 7 bit length field */
    ((uint8_t *)p->payload)[RFCOMM_HDR_LEN_1] = fcs8_crc_calc(p, RFCOMM_CRC_CHECK_LEN);

    ret = l2cap_datawrite(pcb, p);
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_connect(rfcomm_pcb_t *pcb, uint8_t cn, rfcomm_connected_cb connected)
{
    rfcomm_hdr_t *hdr;
    struct bt_pbuf_t *p;
    err_t ret;
    rfcomm_pcb_t *tpcb;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_connect\n");

    pcb->connected = connected;
    pcb->cn = cn;
    pcb->rfcommcfg |= RFCOMM_CFG_IR; /* Set role to initiator */

    /* Create multiplexer session if one does not already exist */
    if(cn != 0)
    {
        tpcb = rfcomm_get_active_pcb(0, &pcb->l2cappcb->remote_bdaddr);

        if(tpcb == NULL)
        {
            pcb->state = W4_RFCOMM_MULTIPLEXER;
            RFCOMM_REG(&rfcomm_active_pcbs, pcb);
            pcb = rfcomm_new(pcb->l2cappcb);
            pcb->rfcommcfg |= RFCOMM_CFG_IR; /* Set role to initiator */
        }
    }

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_SABM_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    hdr = p->payload;

    hdr->addr = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (((pcb->rfcommcfg & RFCOMM_CFG_IR) ^ 1) << 2) | (pcb->cn << 3);

	hdr->ctrl = RFCOMM_SABM;
    hdr->len = (1 << 0) | (0 << 1);
    ((uint8_t *)p->payload)[RFCOMM_HDR_LEN_1] = fcs8_crc_calc(p, RFCOMM_CRC_CHECK_LEN);

    if((ret = l2cap_datawrite(pcb->l2cappcb, p)) == BT_ERR_OK)
    {
        pcb->state = W4_RFCOMM_SABM_RSP;
        pcb->to = 5*RFCOMM_TO; /* Set acknowledgement timer, 50-300s (5*10-60s) */
    }

    if((tpcb = rfcomm_get_active_pcb(pcb->cn, &pcb->l2cappcb->remote_bdaddr)) == NULL)
    {
        RFCOMM_REG(&rfcomm_active_pcbs, pcb);
    }

    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_disconnect(rfcomm_pcb_t *pcb)
{
    rfcomm_hdr_t *hdr;
    struct bt_pbuf_t *p;
    err_t ret;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_disconnect\n");

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_DISC_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    hdr = p->payload;
    hdr->addr = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (((pcb->rfcommcfg & RFCOMM_CFG_IR) == RFCOMM_CFG_IR) << 2) | (pcb->cn << 3);

	hdr->ctrl = RFCOMM_DISC;
    hdr->len = (1 << 0) | (0 << 1);
    ((uint8_t *)p->payload)[RFCOMM_HDR_LEN_1] = fcs8_crc_calc(p, RFCOMM_CRC_CHECK_LEN);
    pcb->state = W4_RFCOMM_DISC_RSP;

    if((ret = l2cap_datawrite(pcb->l2cappcb, p)) == BT_ERR_OK)
    {
        pcb->to = RFCOMM_TO; /* Set acknowledgement timer, 10-60s */
    }
    bt_pbuf_free(p);
    return ret;
}

static err_t rfcomm_ua(l2cap_pcb_t *pcb, rfcomm_hdr_t *hdr)
{
    struct bt_pbuf_t *p;
    rfcomm_hdr_t *rfcommhdr;
    err_t ret;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_ua\n");

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UA_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
		BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    rfcommhdr = p->payload;
    rfcommhdr->addr = hdr->addr & 0xFB; /* Set direction bit to 0 for the response */
    rfcommhdr->ctrl = RFCOMM_UA;
    rfcommhdr->len = 1; /* EA bit set to 1 to indicate a 7 bit length field */
    ((uint8_t *)p->payload)[RFCOMM_HDR_LEN_1] = fcs8_crc_calc(p, RFCOMM_CRC_CHECK_LEN);

    ret = l2cap_datawrite(pcb, p);
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_pn(rfcomm_pcb_t *pcb,rfcomm_pn_rsp_cbs pn_rsp)
{
    struct bt_pbuf_t *p;
    rfcomm_msg_hdr_t *cmdhdr;
    rfcomm_pn_msg_t *pnmsg;
    err_t ret;
    rfcomm_pcb_t *opcb;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_pn\n");

    opcb = rfcomm_get_active_pcb(0, &pcb->l2cappcb->remote_bdaddr);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_MSGHDR_LEN + RFCOMM_PNMSG_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Set multiplexer parameter negotiation command header */
    cmdhdr = p->payload;
    cmdhdr->type = RFCOMM_PN_CMD;
    cmdhdr->len = 1 | (RFCOMM_PNMSG_LEN << 1);

    /* Set multiplexer parameter negotiation command paramenters */
    pnmsg = (void *)(((uint8_t *)p->payload) + RFCOMM_MSGHDR_LEN);
    pnmsg->dlci = (((opcb->rfcommcfg & RFCOMM_CFG_IR) ^ 1) << 0) | (pcb->cn << 1);
    pnmsg->i_cl = 0 | (RFCOMM_CL << 4);
    pnmsg->p = 0;
    pnmsg->t = 0;
    pnmsg->n = RFCOMM_N;
    pnmsg->na = 0;
    pnmsg->k = RFCOMM_RK_INIT;
    pcb->rk = RFCOMM_RK_INIT;
    if((ret = rfcomm_uih(opcb, 0, p)) == BT_ERR_OK)
    {
        pcb->pn_rsp = pn_rsp;
        opcb->to = RFCOMM_TO; /* Set acknowledgement timer, 10-60s */
    }
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_test(rfcomm_pcb_t *pcb, rfcomm_test_rsp_cb test_rsp)
{
    struct bt_pbuf_t *p;
    rfcomm_msg_hdr_t *cmdhdr;
    err_t ret;
    rfcomm_pcb_t *opcb;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_test\n");

    opcb = rfcomm_get_active_pcb(0, &pcb->l2cappcb->remote_bdaddr);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_MSGHDR_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Set multiplexer modem status command header */
    cmdhdr = p->payload;
    cmdhdr->type = RFCOMM_TEST_CMD;
    cmdhdr->len = 1 | (0 << 1);

    if((ret = rfcomm_uih(opcb, 0, p)) == BT_ERR_OK)
    {
        opcb->test_rsp = test_rsp;
        opcb->to = RFCOMM_TO; /* Set acknowledgement timer, 10-60s */
    }
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_msc(rfcomm_pcb_t *pcb, uint8_t fc,rfcomm_msc_rsp_cb msc_rsp)
{
    struct bt_pbuf_t *p;
    rfcomm_msg_hdr_t *cmdhdr;
    rfcomm_msc_msg_t *mscmsg;
    err_t ret;
    rfcomm_pcb_t *opcb;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_msc\n");

    opcb = rfcomm_get_active_pcb(0, &pcb->l2cappcb->remote_bdaddr);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_MSGHDR_LEN + RFCOMM_MSCMSG_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Set multiplexer modem status command header */
    cmdhdr = p->payload;
    cmdhdr->type = RFCOMM_MSC_CMD;
    cmdhdr->len = 1 | (RFCOMM_MSCMSG_LEN << 1);

    /* Set multiplexer parameter negotiation command paramenters */
    mscmsg = (void *)(((uint8_t *)p->payload) + RFCOMM_MSGHDR_LEN);
    // mscmsg->dlci = (1 << 0) | (1 << 1) | (((pcb->rfcommcfg & RFCOMM_CFG_IR) ^ 1) << 2) | (pcb->cn << 3);
    mscmsg->dlci = (1 << 0) | (1 << 1) | (0 << 2) | (pcb->cn << 3);
    mscmsg->rs232 = (1 << 0) | (fc << 1) | (0x23 << 2);

    if((ret = rfcomm_uih(opcb, 0, p)) == BT_ERR_OK)
    {
        pcb->msc_rsp = msc_rsp;
        opcb->to = RFCOMM_TO; /* Set acknowledgement timer, 10-60s */
    }
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_rpn(rfcomm_pcb_t *pcb, uint8_t br,rfcomm_rpn_rsp_cb rpn_rsp)
{
    struct bt_pbuf_t *p;
    rfcomm_msg_hdr_t *cmdhdr;
    rfcomm_rpn_msg_t *rpnmsg;
    err_t ret;
    rfcomm_pcb_t *opcb;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_rpn\n");


    opcb = rfcomm_get_active_pcb(0, &pcb->l2cappcb->remote_bdaddr);

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_MSGHDR_LEN + RFCOMM_RPNMSG_LEN, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    /* Set remote port negotiation command header */
    cmdhdr = p->payload;
    cmdhdr->type = RFCOMM_RPN_CMD;
    cmdhdr->len = 1 | (RFCOMM_RPNMSG_LEN << 1);

    /* Set remote port negotiation command paramenters */
    rpnmsg = (void *)(((uint8_t *)p->payload) + RFCOMM_MSGHDR_LEN);
    rpnmsg->dlci = (1 << 0) | (1 << 1) | (((opcb->rfcommcfg & RFCOMM_CFG_IR) ^ 1) << 2) | (pcb->cn << 3);
    rpnmsg->br = br;
    rpnmsg->mask = 1;

    if((ret = rfcomm_uih(opcb, 0, p)) == BT_ERR_OK)
    {
        pcb->rpn_rsp = rpn_rsp;
        opcb->to = RFCOMM_TO; /* Set acknowledgement timer, 10-60s */

    }
    bt_pbuf_free(p);
    return ret;
}

err_t rfcomm_uih(rfcomm_pcb_t *pcb, uint8_t cn, struct bt_pbuf_t *q)
{
    struct bt_pbuf_t *p, *r;
    err_t ret;
    uint16_t tot_len = 0;

    /* Decrease local credits */
    if(pcb->cl == 0xF && pcb->state == RFCOMM_OPEN && pcb->cn != 0)
    {
        if(pcb->k != 0)
        {
            --pcb->k;
            BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: local credit %d\n",pcb->k);

        }
        else
        {
            BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Out of local credits\n");
#if RFCOMM_FLOW_QUEUEING
            if(q != NULL)
            {
                /* Packet can be queued? */
                if(pcb->buf != NULL)
                {
                    return BT_ERR_OK; /* Drop packet */
                }
                else
                {
                    pcb->buf = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_FLOW_QUEUEING_SIZE, BT_PBUF_RAM);
                    if(q->len <=  pcb->buf->len)
                    {
                        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Queued packet %p on channel %d\n", (void *)q, pcb->cn);
                        /* Copy PBUF_REF referenced payloads into BT_PBUF_RAM */
                        bt_pbuf_take(pcb->buf,q->payload,q->len);
                    }
                    else
                    {
                        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Queued q->len >  pcb->buf->len\n");

                    }
                }
            }
#else
            BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Dropped packet.\n");
            return BT_ERR_OK; /* Drop packet */
#endif /* RFCOMM_FLOW_QUEUEING */
        }
    }

    if(q != NULL)
    {
        tot_len = q->tot_len;
    }

    /* Size of information must be less than maximum frame size */
    if(tot_len > pcb->n)
    {
        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Size of information must be less than maximum frame size tot_len[%d]>pcb->n[%d]\n",tot_len,pcb->n);
        return BT_ERR_MEM;
    }

    if(tot_len < 127)
    {
        if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UIH_LEN, BT_PBUF_RAM)) == NULL)
        {
            BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
        }
    }
    else
    {
        if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UIH_LEN+1, BT_PBUF_RAM)) == NULL)
        {
            BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
        }
    }

    /* Setup RFCOMM header */
    if(cn == 0)
    {
        ((uint8_t *)p->payload)[0] = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (0 << 2) | (0 << 3);
    }
    else
    {
        ((uint8_t *)p->payload)[0] = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (0 << 2) | (cn << 3);
    }
    ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
    if(q != NULL)
    {
        if(q->tot_len < 127)
        {
            ((uint8_t *)p->payload)[2] = (1 << 0) | (q->tot_len << 1);
        }
        else
        {
            ((uint16_t *)p->payload)[1] = (0 << 0) | (q->tot_len << 1);
        }
        /* Add information data to bt_pbuf_t */
        bt_pbuf_chain(p, q);
    }
    else
    {
        ((uint8_t *)p->payload)[2] = (1 << 0) | (0 << 1); /* Empty UIH frame */
    }
    /* Add information FCS to bt_pbuf_t */
    if((r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        bt_pbuf_free(p);
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    if(cn == 0)
    {
        ((uint8_t *)r->payload)[0] = pcb->uih0_out_fcs;
    }
    else
    {
        	((uint8_t *)r->payload)[0] = pcb->uih_out_fcs;
    }

    bt_pbuf_chain(p, r);
    bt_pbuf_free(r);

    ret = l2cap_datawrite(pcb->l2cappcb, p);

    /* Dealloc the RFCOMM header. Lower layers will handle rest of packet */
    if(q != NULL)
    {
        bt_pbuf_dechain(p); /* Have q point to information + FCS */
        bt_pbuf_realloc(q, q->tot_len-1); /* Remove FCS from packet */
    }
    bt_pbuf_free(p);

    return ret;
}

err_t rfcomm_uih_credits(rfcomm_pcb_t *pcb, uint8_t credits, struct bt_pbuf_t *q)
{
    struct bt_pbuf_t *p, *r;
    err_t ret;
    uint16_t tot_len = 0;

    BT_RFCOMM_TRACE_DEBUG("give remote credits %d,local credit %d,remote credit %d\n",credits,pcb->k,pcb->rk);

    /* Decrease local credits */
    if(pcb->cl == 0xF && pcb->state == RFCOMM_OPEN && pcb->cn != 0)
    {
        if(pcb->k != 0)
        {
            --pcb->k;
        }
        else
        {
            BT_RFCOMM_TRACE_DEBUG("rfcomm_uih_credits: Out of local credits\n");
#if RFCOMM_FLOW_QUEUEING
            if(q != NULL)
            {
                /* Packet can be queued? */
                if(pcb->buf != NULL)
                {
                    return BT_ERR_OK; /* Drop packet */
                }
                else
                {
                    pcb->buf = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_FLOW_QUEUEING_SIZE, BT_PBUF_RAM);
                    if(q->len <=  pcb->buf->len)
                    {
                        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Queued packet %p on channel %d\n", (void *)q, pcb->cn);
                        /* Copy PBUF_REF referenced payloads into BT_PBUF_RAM */
                        bt_pbuf_take(pcb->buf,q->payload,q->len);
                    }
                    else
                    {
                        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih: Queued q->len >  pcb->buf->len\n");
                    }
                }
            }
#else
            BT_RFCOMM_TRACE_DEBUG("rfcomm_uih_credits: Dropped packet\n");
#endif /* RFCOMM_FLOW_QUEUEING */
            return BT_ERR_OK;
        }
    }

    if(q != NULL)
    {
        tot_len = q->tot_len;
    }

    /* Size of information must be less than maximum frame size */
    if(tot_len > pcb->n)
    {
        BT_RFCOMM_TRACE_DEBUG("rfcomm_uih_credits: Size of information must be less than maximum frame size = %d Packet lenght = %d\n", pcb->n, q->tot_len);
        return BT_ERR_MEM;
    }

    if(tot_len < 127)
    {
        if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UIHCRED_LEN, BT_PBUF_RAM)) == NULL)
        {
            BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
        }
    }
    else
    {
        if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UIHCRED_LEN+1, BT_PBUF_RAM)) == NULL)
        {
            BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
        }
    }

    /* Setup RFCOMM header */
    ((uint8_t *)p->payload)[0] = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (0  << 2) | (pcb->cn << 3);
    ((uint8_t *)p->payload)[1] = RFCOMM_UIH_PF;
    if(q != NULL)
    {
        if(q->tot_len < 127)
        {
            ((uint8_t *)p->payload)[2] = (1 << 0) | (q->tot_len << 1);
            ((uint8_t *)p->payload)[3] = credits;
        }
        else
        {
            ((uint16_t *)p->payload)[1] = (0 << 0) | (q->tot_len << 1);
            ((uint8_t *)p->payload)[4] = credits;
        }
        /* Add information data to bt_pbuf_t */
        bt_pbuf_chain(p, q);
    }
    else
    {
        /* Credit only UIH frame */
        ((uint8_t *)p->payload)[2] = (1 << 0) | (0 << 1);
    }

    /* Add information FCS to bt_pbuf_t */
    if((r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        bt_pbuf_free(p);
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)r->payload)[0] = pcb->uihpf_out_fcs;
    bt_pbuf_chain(p, r);
    bt_pbuf_free(r);

    /* Increase remote credits */
    pcb->rk += credits;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_uih_credits: p->tot_len = %d pcb->k = %d pcb->rk = %d\n", p->tot_len, pcb->k, pcb->rk);

    ret = l2cap_datawrite(pcb->l2cappcb, p);

    /* Free RFCOMM header. Higher layers will handle rest of packet */
    if(q != NULL)
    {
        bt_pbuf_dechain(p);
        bt_pbuf_realloc(q, q->tot_len-1); /* Remove FCS from packet */
    }
    bt_pbuf_free(p);

    return ret;
}

err_t rfcomm_issue_credits(rfcomm_pcb_t *pcb, uint8_t credits)
{
    struct bt_pbuf_t *p, *r;
    err_t ret;


    if((p = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_UIHCRED_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }


    /* Setup RFCOMM header */
    ((uint8_t *)p->payload)[0] = (1 << 0) | ((pcb->rfcommcfg & RFCOMM_CFG_IR) << 1) | (0  << 2) | (pcb->cn << 3);
    ((uint8_t *)p->payload)[1] = RFCOMM_UIH_PF;
    ((uint8_t *)p->payload)[2] = (1 << 0);
    ((uint8_t *)p->payload)[3] = credits;

    /* Add information FCS to bt_pbuf_t */
    if((r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        bt_pbuf_free(p);
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM; /* Could not allocate memory for bt_pbuf_t */
    }

    ((uint8_t *)r->payload)[0] = pcb->uihpf_out_fcs;
    bt_pbuf_chain(p, r);
    bt_pbuf_free(r);

    /* Increase remote credits */
    pcb->rk += credits;

    BT_RFCOMM_TRACE_DEBUG("rfcomm_issue_credits: p->tot_len = %d pcb->k = %d pcb->rk = %d\n", p->tot_len, pcb->k, pcb->rk);

    ret = l2cap_datawrite(pcb->l2cappcb, p);

    bt_pbuf_free(p);

    return ret;
}

void rfcomm_process_msg(rfcomm_pcb_t *pcb, rfcomm_hdr_t *rfcommhdr, l2cap_pcb_t *l2cappcb,
                   struct bt_pbuf_t *p)
{
    rfcomm_msg_hdr_t *cmdhdr, *rsphdr;
    rfcomm_pn_msg_t *pnreq;
    rfcomm_msc_msg_t *mscreq;
    rfcomm_rpn_msg_t *rpnreq;
    rfcomm_pcb_t *tpcb; /* Temp pcb */
    rfcomm_pcb_listen_t *lpcb; /* Listen pcb */
    struct bt_pbuf_t *q;
    err_t ret = BT_ERR_OK;
	
	BT_UNUSED_ARG(ret);
    cmdhdr = p->payload;
    bt_pbuf_header(p, -RFCOMM_MSGHDR_LEN);

    switch(cmdhdr->type)
    {
    case RFCOMM_PN_CMD:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM PN command\n");

        pnreq = p->payload;

        /* Check if the DLC is already established */
        tpcb = rfcomm_get_active_pcb((pnreq->dlci >> 1), &pcb->l2cappcb->remote_bdaddr);

        if(tpcb == NULL)
        {
            /* Check if the server channel exists */
            for(lpcb = rfcomm_listen_pcbs; lpcb != NULL; lpcb = lpcb->next)
            {
                if(lpcb->cn == (pnreq->dlci >> 1))
                {
                    break;
                }
            }
            if(lpcb != NULL)
            {
                /* Found a listening pcb with a matching server channel number, now initiate a new PCB
                   with default configuration */

                BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: Allocate RFCOMM PCB for CN %d\n", lpcb->cn);
                if((tpcb = rfcomm_new(pcb->l2cappcb)) == NULL)
                {
                    BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: could not allocate PCB\n");

                    return;
                }
                tpcb->cn = lpcb->cn;
                tpcb->callback_arg = lpcb->callback_arg;
                tpcb->accept = lpcb->accept;
                tpcb->state = RFCOMM_CFG;

                RFCOMM_REG(&rfcomm_active_pcbs, tpcb);
            }
            else
            {
                /* Channel does not exist, refuse connection with DM frame */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: Channel does not exist, refuse connection with DM frame CN %d\n", (pnreq->dlci >> 1));
                rfcomm_dm(pcb->l2cappcb, rfcommhdr);
                break;
            }
        }
        /* Get suggested parameters */
        tpcb->cl = pnreq->i_cl >> 4;
        tpcb->p = pnreq->p;
        if(tpcb->n > pnreq->n)
        {
            tpcb->n = pnreq->n;
        }
        tpcb->k = RFCOMM_RK_INIT;
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_PN_CMD. tpcb->k = %d\n", tpcb->k);


        /* Send PN response */
        cmdhdr->type = cmdhdr->type & 0xFD; /* Set C/R to response */
        if(tpcb->cl == 0xF)
        {
            pnreq->i_cl = 0 | (0xE << 4); /* Credit based flow control */
        }
        else
        {
            pnreq->i_cl = 0; /* Remote device conforms to bluetooth version 1.0B. No flow control */
        }
        pnreq->p = tpcb->p;
        pnreq->n = tpcb->n;
        pnreq->k = tpcb->k;

        /* recored the number of remote device send packet */
        tpcb->rk = pnreq->k;

        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: remote credit %d %d\n",tpcb->rk,pcb->rk);
        bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

        rfcomm_uih(pcb, 0, p);
        break;
    case RFCOMM_PN_RSP:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM PN response\n");

        pcb->to = 0; /* Reset response timer */
        pnreq = p->payload;
        /* Find PCB with matching server channel number and bluetooth address */
        tpcb = rfcomm_get_active_pcb((pnreq->dlci >> 1), &pcb->l2cappcb->remote_bdaddr);

        if(tpcb != NULL)
        {
            /* Use negotiated settings that may have changed from the default ones */
            if((pnreq->i_cl >> 4) == 0xE)
            {
                tpcb->cl = 0xF; /* Credit based flow control */
                tpcb->k = pnreq->k; /* Inital credit value */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_PN_RSP. tpcb->k = %d\n", tpcb->k);
                BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: Credit based flow control is used for outgoing packets 0x%x %d %d\n", (pnreq->i_cl >> 4), pnreq->k, pnreq->n);
            }
            else
            {
                BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: No flow control used for outgoing packets 0x%x\n", (pnreq->i_cl >> 4));

                tpcb->cl = 0; /* Remote device conform to bluetooth version 1.0B. No flow control */
            }
            tpcb->n = pnreq->n; /* Maximum frame size */

            if(tpcb->state == W4_RFCOMM_MULTIPLEXER)
            {
                rfcomm_connect(tpcb, tpcb->cn, tpcb->connected); /* Create a connection for a channel that
							    waits for the multiplexer connection to
							    be established */
            }

            pcb->state = RFCOMM_OPEN;
            RFCOMM_EVENT_PN_RSP(tpcb,BT_ERR_OK);
        } /* else silently discard */
        break;
    case RFCOMM_TEST_CMD:
        /* Send TEST response */
        cmdhdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
        bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

        rfcomm_uih(pcb, 0, p);
        break;
    case RFCOMM_TEST_RSP:
        pcb->to = 0; /* Reset response timer */
        RFCOMM_EVENT_TEST(pcb,BT_ERR_OK);
        break;
    case RFCOMM_FCON_CMD:
        /* Enable transmission of data on all channels in session except cn 0 */
        for(tpcb = rfcomm_active_pcbs; tpcb != NULL; tpcb = tpcb->next)
        {
            if(bd_addr_cmp(&(tpcb->l2cappcb->remote_bdaddr), &(l2cappcb->remote_bdaddr)) &&
                    tpcb->cn != 0)
            {
                tpcb->rfcommcfg |= RFCOMM_CFG_FC;
            }
        }
        /* Send FC_ON response */
        cmdhdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
        bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

        rfcomm_uih(pcb, 0, p);
        break;
    case RFCOMM_FCON_RSP:
        break;
    case RFCOMM_FCOFF_CMD:
        /* Disable transmission of data on all channels in session except cn 0 */
        for(tpcb = rfcomm_active_pcbs; tpcb != NULL; tpcb = tpcb->next)
        {
            if(bd_addr_cmp(&(tpcb->l2cappcb->remote_bdaddr), &(l2cappcb->remote_bdaddr)) &&
                    tpcb->cn != 0)
            {
                tpcb->rfcommcfg &= ~RFCOMM_CFG_FC;
            }
        }
        /* Send FC_OFF response */
        cmdhdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
        bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

        rfcomm_uih(pcb, 0, p);
        break;
    case RFCOMM_FCOFF_RSP:
        break;
    case RFCOMM_MSC_CMD:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_MSC_CMD\n");

        mscreq = p->payload;
        /* Find DLC */
        tpcb = rfcomm_get_active_pcb((mscreq->dlci >> 3), &pcb->l2cappcb->remote_bdaddr);

        if(tpcb != NULL)
        {
            /* Set flow control bit. Ignore remaining fields in the MSC since this is a type 1
            device */
            if((mscreq->rs232 >> 1) & 0x01)
            {
                tpcb->rfcommcfg |= RFCOMM_CFG_FC;
            }
            else
            {
                tpcb->rfcommcfg &= ~RFCOMM_CFG_FC;
            }
            BT_RFCOMM_TRACE_DEBUG("rfcommm_process_msg: fc bit = %d\n", (mscreq->rs232 >> 1) & 0x01);

            /* Send MSC response */
            cmdhdr->type = cmdhdr->type & 0xFD; /* Set C/R to response */
            bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

            if(!(tpcb->rfcommcfg & RFCOMM_CFG_IR) && !(tpcb->rfcommcfg & RFCOMM_CFG_MSC_IN))   /* We are the responder and should send a MSC command before responding to one */
            {
                BT_RFCOMM_TRACE_DEBUG("rfcommm_process_msg: We are the responder and should send a MSC command before responding to one\n");

                rfcomm_msc(tpcb, 0, NULL);
            }

            rfcomm_uih(pcb, 0, p);

            tpcb->rfcommcfg |= RFCOMM_CFG_MSC_OUT;

            if((tpcb->rfcommcfg & RFCOMM_CFG_MSC_IN) && tpcb->state != RFCOMM_OPEN)
            {
                tpcb->state = RFCOMM_OPEN;
                if(tpcb->rfcommcfg & RFCOMM_CFG_IR)
                {
                    RFCOMM_EVENT_CONNECTED(tpcb,BT_ERR_OK);
                }
                else
                {
                    RFCOMM_EVENT_ACCEPT(tpcb,BT_ERR_OK);
                }
            }
        } /* else silently discard */
        break;
    case RFCOMM_MSC_RSP:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_MSC_RSP\n");

        /* Information received in response is only a copy of the signals that where sent in the command */
        pcb->to = 0; /* Reset response timer */

        mscreq = p->payload;

        /* Find PCB with matching server channel number and Bluetooth address */
        tpcb = rfcomm_get_active_pcb((mscreq->dlci >> 3), &pcb->l2cappcb->remote_bdaddr);

        if(tpcb != NULL)
        {

            if(tpcb->rfcommcfg & RFCOMM_CFG_MSC_IN)
            {
                RFCOMM_EVENT_MSC(tpcb,BT_ERR_OK); /* We have sent a MSC after initial configuration of
					      the connection was done */
            }
            else
            {
                tpcb->rfcommcfg |= RFCOMM_CFG_MSC_IN;
                if(tpcb->rfcommcfg & RFCOMM_CFG_MSC_OUT)
                {
                    tpcb->state = RFCOMM_OPEN;
                    if(tpcb->rfcommcfg & RFCOMM_CFG_IR)
                    {
                        RFCOMM_EVENT_CONNECTED(tpcb,BT_ERR_OK);
                    }
                    else
                    {
                        RFCOMM_EVENT_ACCEPT(tpcb,BT_ERR_OK);
                    }
                }
            }
        } /* else silently discard */
        break;
    case RFCOMM_RPN_CMD:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_RPN_CMD\n");

        /* Send RPN response */
        if(cmdhdr->len == 8)
        {
            /* RPN command was a request to set up the link's parameters */
            /* All parameters accepted since this is a type 1 device */
            cmdhdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
            bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);
            //rfcomm_uih(pcb->l2cappcb, rfcommhdr, p);
            rfcomm_uih(pcb, 0, p);
        }
        else if(cmdhdr->len == 1)
        {
            /* RPN command was a request for the link's parameters */
            q = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_RPNMSG_LEN+RFCOMM_MSGHDR_LEN, BT_PBUF_RAM);
            rsphdr = q->payload;
            rsphdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
            rsphdr->len = RFCOMM_RPNMSG_LEN;
            bt_pbuf_header(q, -RFCOMM_MSGHDR_LEN);
            rpnreq = q->payload;
            rpnreq->dlci = ((uint8_t *)p->payload)[0]; /* Copy DLCI from command to response */
            rpnreq->br = RFCOMM_COM_BR; /* Default baud rate */
            rpnreq->cfg = RFCOMM_COM_CFG; /* Default data bits, stop bits, parity and parity type */
            rpnreq->fc = RFCOMM_COM_FC; /* Default flow control */
            rpnreq->xon = RFCOMM_COM_XON; /* Default */
            rpnreq->xoff = RFCOMM_COM_XOFF; /* Default */
            rpnreq->mask = 0xFFFF; /* All parameters are valid */
            bt_pbuf_header(q, RFCOMM_MSGHDR_LEN);

            rfcomm_uih(pcb, 0, q);
            bt_pbuf_free(q);
        }
        else
        {
            //SHOULD NOT HAPPEN. LENGTH SHOULD ALWAYS BE 1 OR 8
        }
        break;
    case RFCOMM_RPN_RSP:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_RPN_CMD\n");

        pcb->to = 0; /* Reset response timer */
        rpnreq = p->payload;
        BT_RFCOMM_TRACE_DEBUG("rfcomm_msc: rpn response received 0x%x\n", rpnreq->br);


        /* Find PCB with matching server channel number and bluetooth address */
        tpcb = rfcomm_get_active_pcb((rpnreq->dlci >> 3), &pcb->l2cappcb->remote_bdaddr);

        if(tpcb != NULL)
        {
            RFCOMM_EVENT_RPN(tpcb,BT_ERR_OK);
        }
        break;
    case RFCOMM_RLS_CMD:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_process_msg: RFCOMM_RLS_CMD\n");
        /* Send RLS response */
        cmdhdr->type = cmdhdr->type & 0xBF; /* Set C/R to response */
        bt_pbuf_header(p, RFCOMM_MSGHDR_LEN);

        rfcomm_uih(pcb, 0, p);
        break;
    case RFCOMM_RLS_RSP:
        break;
    case RFCOMM_NSC_RSP:
        break;
    default:
        /* Send NSC response */
        q = bt_pbuf_alloc(BT_PBUF_RAW, RFCOMM_MSGHDR_LEN, BT_PBUF_RAM);
        rsphdr = q->payload;
        rsphdr->type = ((cmdhdr->type & 0x03) << 0) | (RFCOMM_NSC_RSP << 2);
        rsphdr->len = 0;

        rfcomm_uih(pcb, 0, q);
        bt_pbuf_free(q);
        break;
    }
}

err_t rfcomm_input(void *arg, l2cap_pcb_t *l2cappcb, struct bt_pbuf_t *p, err_t err)
{
    rfcomm_hdr_t rfcommhdr;
    rfcomm_pcb_t *pcb, *tpcb;
    rfcomm_pcb_listen_t *lpcb;

    int16_t len = 0;
    uint8_t hdrlen;
    uint8_t fcs;

    struct bt_pbuf_t *q;
      err_t ret = BT_ERR_OK;

		BT_UNUSED_ARG(ret);
    rfcommhdr.addr = *((uint8_t *)p->payload);
    rfcommhdr.ctrl = ((uint8_t *)p->payload)[1];

    /* Find PCB with matching server channel number and bluetooth address */
    pcb = rfcomm_get_active_pcb((rfcommhdr.addr >> 3), &l2cappcb->remote_bdaddr);

    if(pcb == NULL && rfcommhdr.ctrl != RFCOMM_SABM)
    {
        /* Channel does not exist */
        if(rfcommhdr.ctrl != RFCOMM_DM_PF && rfcommhdr.ctrl != RFCOMM_DM)
        {
            /* Send a DM response */
            BT_RFCOMM_TRACE_DEBUG("Send a DM response to CN %d rfcomm.ctrl == 0x%x\n", (rfcommhdr.addr >> 3), rfcommhdr.ctrl);

            rfcomm_dm(l2cappcb, &rfcommhdr);
        } /* else silently discard packet */
        bt_pbuf_free(p);
        return BT_ERR_OK;
    }

    /* Check if length field is 1 or 2 bytes long and remove EA bit */
    if((((uint8_t *)p->payload)[2] & 0x01) == 1)
    {
        hdrlen = RFCOMM_HDR_LEN_1;
        rfcommhdr.len = (((uint8_t *)p->payload)[2] >> 1) & 0x007F;
    }
    else
    {
        hdrlen = RFCOMM_HDR_LEN_2;
        rfcommhdr.len = (((uint16_t *)p->payload)[1] >> 1) & 0x7FFF;
    }

    if(rfcommhdr.ctrl == RFCOMM_UIH_PF)
    {
        if(pcb->cl == 0xF)
        {
            rfcommhdr.k = ((uint8_t *)p->payload)[hdrlen++];
        }
    }

    /* Frame check */
    for(q = p; q != NULL; q = q->next)
    {
        len += q->len;
        if(len > (rfcommhdr.len + hdrlen))
        {
            len -= q->len;
            len = rfcommhdr.len - len;
            len += hdrlen;
            break;
        }
    }

    fcs = ((uint8_t *)q->payload)[len];
    if(rfcommhdr.ctrl == RFCOMM_UIH)
    {
        if(pcb->cn == 0)
        {
            if(fcs != pcb->uih0_in_fcs)   /* Check against the precalculated fcs */
            {
                //if(fcs8_crc_check(p, RFCOMM_UIHCRC_CHECK_LEN, fcs) != 0) {
                /* Packet discarded due to failing frame check sequence */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_input: UIH packet discarded due to failing frame check sequence\n");
                bt_pbuf_free(p);
                return BT_ERR_OK;
            }
        }
    }
    else if(rfcommhdr.ctrl == RFCOMM_UIH_PF)
    {
        if(fcs != pcb->uihpf_in_fcs)   /* Check against the precalculated fcs */
        {
            //if(fcs8_crc_check(p, RFCOMM_UIHCRC_CHECK_LEN, fcs) != 0) {
            /* Packet discarded due to failing frame check sequence */
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: UIH_PF packet discarded due to failing frame check sequence RFCS = 0x%x LFCS = 0x%x\n",
                                  fcs, pcb->uihpf_in_fcs);

            bt_pbuf_free(p);
            return BT_ERR_OK;
        }
    }
    else
    {
        if(fcs8_crc_check(p, hdrlen, fcs) != 0)
        {
            /* Packet discarded due to failing frame check sequence */
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Packet discarded due to failing frame check sequence\n");
            bt_pbuf_free(p);
            return BT_ERR_OK;
        }
    }

    bt_pbuf_header(p, -hdrlen); /* Adjust information pointer */
    bt_pbuf_realloc(p, rfcommhdr.len); /* Remove fcs from packet */

    switch(rfcommhdr.ctrl)
    {
    case RFCOMM_SABM:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_input: RFCOMM_SABM\n");

        if(pcb == NULL)
        {
            /* Check if the server channel exists */
            lpcb = NULL;
            if(rfcommhdr.addr >> 3 == 0)
            {
                /* Only the multiplexer channel can be connected without first
                			configuring it */
                for(lpcb = rfcomm_listen_pcbs; lpcb != NULL; lpcb = lpcb->next)
                {
                    if(lpcb->cn == (rfcommhdr.addr >> 3))
                    {
                        break;
                    }
                }
            }
            if(lpcb != NULL)
            {
                /* Found a listening pcb with a matching server channel number, now initiate a new active PCB
                   with default configuration */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Allocate RFCOMM PCB for CN %d******************************\n", lpcb->cn);
                if((pcb = rfcomm_new(l2cappcb)) == NULL)
                {
                    /* No memory to allocate PCB. Refuse connection attempt */
                    BT_RFCOMM_TRACE_DEBUG("rfcomm_input: No memory to allocate PCB. Refuse connection attempt CN %d******************************\n", lpcb->cn);

                    rfcomm_dm(l2cappcb, &rfcommhdr);
                    bt_pbuf_free(p);
                    return BT_ERR_OK;
                }
                pcb->cn = lpcb->cn;
                pcb->callback_arg = lpcb->callback_arg;
                pcb->accept = lpcb->accept;

                RFCOMM_REG(&rfcomm_active_pcbs, pcb);
            }
            else
            {
                /* Channel does not exist or multiplexer is not connected, refuse connection with DM frame */
                BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Channel does not exist, refuse connection with DM frame CN %d\n", (rfcommhdr.addr >> 3));
                rfcomm_dm(l2cappcb, &rfcommhdr);
                bt_pbuf_free(p);
                break;
            }
        }
        /* Set role to responder */
        pcb->rfcommcfg &= ~RFCOMM_CFG_IR;

        /* Send UA frame as response to SABM frame */
        rfcomm_ua(l2cappcb, &rfcommhdr);

        /* FCS precalculation for UIH frames */
        bt_pbuf_header(p, hdrlen); /* Reuse the buffer for the current header */

        /* Change header values to refelct an UIH frame sent to the initiator */
        *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0. We are the responder */
        *((uint8_t *)p->payload) &= 0xFD; /* Set C/R bit to 0. We are the responder */
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
        pcb->uih_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH_PF;
        pcb->uihpf_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

        /* Change header values to refelct an UIH frame from to the initiator */
        //*((uint8_t *)p->payload) |= 0x04; /* Set direction bit to 1. We are the responder */
        *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0. We are the responder */
        *((uint8_t *)p->payload) |= 0x02; /* Set C/R bit to 1. We are the responder */
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
        pcb->uih_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH_PF;
        pcb->uihpf_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

        /* UIH frame received on the control channel */
        *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0 */
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
        pcb->uih0_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

        /* Change header values to reflect an UIH frame sent on the control channel */
        *((uint8_t *)p->payload) &= 0xF9; /* Set C/R bit and direction bit to 0 */
        ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
        pcb->uih0_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

        bt_pbuf_free(p);
        break;
    case RFCOMM_UA:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_input: RFCOMM_UA\n");

        pcb->to = 0;
        if(pcb->state ==  W4_RFCOMM_SABM_RSP)
        {
            pcb->state = RFCOMM_CFG;
            /* FCS precalculation for UIH frames */
            bt_pbuf_header(p, hdrlen); /* Reuse the buffer for the current header */
            /* Change header values to refelct an UIH frame sent to the responder */
            *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0. We are the initiator */
            *((uint8_t *)p->payload) |= 0x02; /* Set C/R bit to 1. We are the intitiator */
            ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
            pcb->uih_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);
            ((uint8_t *)p->payload)[1]= RFCOMM_UIH_PF;
            pcb->uihpf_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

            /* Change header values to reflect an UIH frame sent to the responder */
            *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0. We are the intitiator */
            *((uint8_t *)p->payload) &= 0xFD; /* Set C/R bit to 0. We are the initiator */
            ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
            pcb->uih_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);
            ((uint8_t *)p->payload)[1] = RFCOMM_UIH_PF;
            pcb->uihpf_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

            /* UIH frame sent on the control channel */
            *((uint8_t *)p->payload) &= 0xFB; /* Set direction bit to 0 */
            *((uint8_t *)p->payload) |= 0x02; /* Set C/R bit to 1. We are the intitiator */
            ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
            pcb->uih0_out_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

            /* Change header values to reflect an UIH frame received on the control channel */
            *((uint8_t *)p->payload) &= 0xF9; /* Set C/R bit and direction bit to 0 */
            ((uint8_t *)p->payload)[1] = RFCOMM_UIH;
            pcb->uih0_in_fcs = fcs8_crc_calc(p, RFCOMM_UIHCRC_CHECK_LEN);

            if(pcb->cn == 0)
            {
                for(tpcb = rfcomm_active_pcbs; tpcb != NULL; tpcb = tpcb->next)
                {
                    if(bd_addr_cmp(&(tpcb->l2cappcb->remote_bdaddr), &(pcb->l2cappcb->remote_bdaddr)) &&
                            tpcb->state == W4_RFCOMM_MULTIPLEXER)
                    {
                        rfcomm_pn(tpcb, NULL); /* Send a parameter negotiation command to negotiate the
						     connection settings for a channel that waits for the
						     multiplexer connection to be established */
                        break;
                    }
                }
            }
            else
            {
                rfcomm_msc(pcb, 0, NULL); /* Send a modem status command to set V.24 control signals for
				     the RFCOMM connection */
            }
        }
        else if (pcb->state == W4_RFCOMM_DISC_RSP)
        {
            //RFCOMM_RMV(&rfcomm_active_pcbs, pcb);
            pcb->state = RFCOMM_CLOSED;
            RFCOMM_EVENT_DISCONNECTED(pcb,BT_ERR_OK);
        }
        else
        {
            /* A response without an outstanding request is silently discarded */
        }
        bt_pbuf_free(p);
        break;
    case RFCOMM_DM_PF:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_input: RFCOMM_DM_PF\n");
    case RFCOMM_DM:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_input: RFCOMM_DM\n");

        pcb->to = 0;
        //RFCOMM_RMV(&rfcomm_active_pcbs, pcb);
        if(pcb->state ==  W4_RFCOMM_SABM_RSP)
        {
            pcb->state = RFCOMM_CLOSED;
            RFCOMM_EVENT_CONNECTED(pcb,BT_ERR_CONN);
        }
        else
        {
            pcb->state = RFCOMM_CLOSED;
            RFCOMM_EVENT_DISCONNECTED(pcb,BT_ERR_CONN);
        }
        bt_pbuf_free(p);
        break;
    case RFCOMM_DISC:
        BT_RFCOMM_TRACE_DEBUG("rfcomm_input: RFCOMM_DISC\n");
        //RFCOMM_RMV(&rfcomm_active_pcbs, pcb);
        /* Send UA frame as response to DISC frame */
        ret = rfcomm_ua(l2cappcb, &rfcommhdr);
        pcb->state = RFCOMM_CLOSED;
        RFCOMM_EVENT_DISCONNECTED(pcb,BT_ERR_OK);
        bt_pbuf_free(p);
        break;
    case RFCOMM_UIH_PF:
        if((rfcommhdr.addr >> 3) == 0)
        {
            /* Process multiplexer command/response */
            rfcomm_process_msg(pcb, &rfcommhdr, l2cappcb, p);
            bt_pbuf_free(p);
        }
        else if(pcb->cl == 0xF)
        {
            /* Process credit based frame */
            if(pcb->rk != 0 && rfcommhdr.len > 0)
            {
                --pcb->rk; /* Decrease remote credits */
            }

            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Received local credits: %d Existing local credits: %d\n", rfcommhdr.k, pcb->k);
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: remote credits %d\n", pcb->rk);



            if(pcb->rk <= RFCOMM_RK_MIN)
            {
                rfcomm_issue_credits(pcb,RFCOMM_RK_MAX-pcb->rk);
            }

            if((pcb->k + rfcommhdr.k) < 255)
            {
                pcb->k += rfcommhdr.k; /* Increase local credits */
#if RFCOMM_FLOW_QUEUEING
                /* Queued packet present? */
                if (pcb->buf != NULL)
                {
                    /* NULL attached buffer immediately */
                    BT_RFCOMM_TRACE_DEBUG("rfcomm_input: sending queued packet.\n");
                    /* Send the queued packet */
                    rfcomm_uih(pcb, pcb->cn, pcb->buf);
                    /* Free the queued packet */
                    bt_pbuf_free(pcb->buf);
                    pcb->buf = NULL;
                }
#endif /* RFCOMM_FLOW_QUEUEING */
            }
            else
            {
                pcb->k = 255;
            }

            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Forward RFCOMM_UIH_PF credit packet to higher layer\n");
            RFCOMM_EVENT_RECV(pcb,BT_ERR_OK,p); /* Process information. Application must free bt_pbuf_t */
        }
        else
        {
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Forward RFCOMM_UIH_PF non credit packet to higher layer\n");

            RFCOMM_EVENT_RECV(pcb,BT_ERR_OK,p); /* Process information. Application must free bt_pbuf_t */
        }
        break;
    case RFCOMM_UIH:
        if((rfcommhdr.addr >> 3) == 0)
        {
            /* Process multiplexer command/response */
            rfcomm_process_msg(pcb, &rfcommhdr, l2cappcb, p);
            bt_pbuf_free(p);
        }
        else
        {
            if(pcb->rk != 0)
            {
                --pcb->rk; /* Decrease remote credits */
            }

            if(pcb->rk <= RFCOMM_RK_MIN)
            {
                rfcomm_issue_credits(pcb,RFCOMM_RK_MAX-pcb->rk);
            }
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Forward RFCOMM_UIH packet to higher layer\n");
            BT_RFCOMM_TRACE_DEBUG("rfcomm_input: Rremote credit %d\n",pcb->rk);
            RFCOMM_EVENT_RECV(pcb,BT_ERR_OK,p); /* Process information. Application must free bt_pbuf_t */
        }
        break;
    default:
        /* Unknown or illegal frame type. Throw it away! */
        bt_pbuf_free(p);
        break;
    }
    return BT_ERR_OK;
}



void rfcomm_register_recv(rfcomm_pcb_t *pcb,rfcomm_recv_cb recv)
{
    pcb->recv = recv;
}

void rfcomm_register_disc(rfcomm_pcb_t *pcb,rfcomm_disconnected_cb disconnected)
{
    pcb->disconnected = disconnected;
}

err_t rfcomm_listen(uint8_t cn, rfcomm_accept_cb connet_ind)
{
    rfcomm_pcb_listen_t *listen_pcb;

    if((listen_pcb = bt_memp_malloc(MEMP_RFCOMM_PCB_LISTEN)) == NULL)
    {
        BT_RFCOMM_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    listen_pcb->cn = cn;
    listen_pcb->accept = connet_ind;
    listen_pcb->state = RFCOMM_LISTEN;

    RFCOMM_REG(&rfcomm_listen_pcbs, listen_pcb);
    return BT_ERR_OK;
}



