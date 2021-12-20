/******************************************************************************
  * @file           bt_l2cap.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt l2cap source file
******************************************************************************/

#include "bt_l2cap.h"
#include "bt_common.h"
#include "bt_pbuf.h"
/* Next Identifier to be sent */
uint8_t sigid_nxt;

/* The L2CAP PCB lists. */
l2cap_pcb_listen_t *l2cap_listen_pcbs;  /* List of all L2CAP PCBs in CLOSED state
						but awaiting an incoming conn req */
l2cap_pcb_t *l2cap_active_pcbs;  /* List of all L2CAP PCBs that are in a
					 state in which they accept or send
					 data */
l2cap_pcb_t *l2cap_tmp_pcb;

/* Temp signal */
l2cap_sig_t *l2cap_tmp_sig;

/* Global variable involved in input processing of l2cap data segements */
l2cap_seg_t *l2cap_insegs;
l2cap_seg_t *l2cap_tmp_inseg;


/* Internal functions and global variables */
#define L2CA_ACTION_CONN_CFM(pcb,result,status) if((pcb)->l2ca_connect_cfm != NULL) ((pcb)->l2ca_connect_cfm((pcb)->callback_arg,(pcb),(result),(status)))
#define L2CA_ACTION_DISCONN_CFM(pcb) if((pcb)->l2ca_disconnect_cfm != NULL) ((pcb)->l2ca_disconnect_cfm((pcb)->callback_arg,(pcb)))
#define L2CA_ACTION_PING_CFM(pcb,result) if((pcb)->l2ca_ping != NULL) ((pcb)->l2ca_ping((pcb)->callback_arg,(pcb),(result)))
#define L2CA_ACTION_CONN_IND(pcb,err) if((pcb)->l2ca_connect_ind != NULL) ((pcb)->l2ca_connect_ind((pcb)->callback_arg,(pcb),(err)))
#define L2CA_ACTION_DISCONN_IND(pcb,err) if((pcb)->l2ca_disconnect_ind != NULL) { ((pcb)->l2ca_disconnect_ind((pcb)->callback_arg,(pcb),(err))); } else { l2cap_close(pcb);}
#define L2CA_ACTION_TO_IND(pcb,err) if((pcb)->l2ca_timeout_ind != NULL) ((pcb)->l2ca_timeout_ind((pcb)->callback_arg,(pcb),(err)))
#define L2CA_ACTION_RECV(pcb,p,err) if((pcb)->l2ca_recv != NULL) { ((pcb)->l2ca_recv((pcb)->callback_arg,(pcb),(p),(err))); } else { bt_pbuf_free(p); }

#define L2CAP_OPTH_TYPE(hdr) (((hdr)->type) & 0x7f)
#define L2CAP_OPTH_TOA(hdr) (((hdr)->type) >> 7)

#define L2CAP_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define L2CAP_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(l2cap_tmp_pcb = *pcbs; l2cap_tmp_pcb != NULL; l2cap_tmp_pcb = l2cap_tmp_pcb->next) { \
                               if(l2cap_tmp_pcb->next != NULL && l2cap_tmp_pcb->next == npcb) { \
                                  l2cap_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)


#define L2CAP_SIG_REG(ursp_sigs, nsig) do { \
                            nsig->next = *ursp_sigs; \
                            *ursp_sigs = nsig; \
                            } while(0)
#define L2CAP_SIG_RMV(ursp_sigs, nsig) do { \
                            if(*ursp_sigs == nsig) { \
                               *ursp_sigs = (*ursp_sigs)->next; \
                            } else for(l2cap_tmp_sig = *ursp_sigs; l2cap_tmp_sig != NULL; l2cap_tmp_sig = l2cap_tmp_sig->next) { \
                               if(l2cap_tmp_sig->next != NULL && l2cap_tmp_sig->next == nsig) { \
                                  l2cap_tmp_sig->next = nsig->next; \
                                  break; \
                               } \
                            } \
                            nsig->next = NULL; \
                            } while(0)

#define L2CAP_SEG_REG(segs, nseg) do { \
                            nseg->next = *segs; \
                            *segs = nseg; \
                            } while(0)
#define L2CAP_SEG_RMV(segs, nseg) do { \
                            if(*segs == nseg) { \
                               *segs = (*segs)->next; \
                            } else for(l2cap_tmp_inseg = *segs; l2cap_tmp_inseg != NULL; l2cap_tmp_inseg = l2cap_tmp_inseg->next) { \
                               if(l2cap_tmp_inseg->next != NULL && l2cap_tmp_inseg->next == nseg) { \
                                  l2cap_tmp_inseg->next = nseg->next; \
                                  break; \
                               } \
                            } \
                            nseg->next = NULL; \
                            } while(0)


static err_t l2cap_config_req(l2cap_pcb_t *pcb);

static err_t l2cap_rexmit_signal(l2cap_pcb_t *pcb,l2cap_sig_t *sig);

static uint16_t _l2cap_cid_alloc(void);
static uint8_t _l2cap_next_sigid(void);
static err_t _l2cap_signal_send(l2cap_pcb_t *pcb, uint8_t code, uint16_t ursp_id, struct bd_addr_t *remote_bdaddr, struct bt_pbuf_t *data);;
static err_t _l2cap_null_cid_process(struct bt_pbuf_t *p,struct bd_addr_t *bdaddr);
static err_t _l2cap_classical_sig_cid_process(struct bt_pbuf_t *p,l2cap_hdr_t *l2cap_hdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_connless_cid_process(struct bt_pbuf_t *p,struct bd_addr_t *bdaddr);
static err_t _l2cap_fixed_cid_process(uint16_t cid,struct bt_pbuf_t *p,struct bd_addr_t *bdaddr);
static err_t _l2cap_dynamic_cid_process(l2cap_pcb_t *l2cap_pcb,struct bt_pbuf_t *p,l2cap_hdr_t *l2cap_hdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_sig_conn_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_sig_conn_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_t *sig);
static err_t _l2cap_sig_cfg_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_sig_cfg_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,l2cap_sig_t *sig);
static err_t _l2cap_sig_disconn_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_sig_disconn_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_t *sig);
static err_t _l2cap_sig_echo_req_process(l2cap_pcb_t *pcb,l2cap_sig_hdr_t *sighdr);
static err_t _l2cap_sig_echo_rsp_process(l2cap_pcb_t *pcb,l2cap_sig_t *sig);
static err_t _l2cap_sig_info_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr);
static err_t _l2cap_sig_info_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p);
static err_t _l2cap_sig_unsupport_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr);

void l2cap_init(void)
{
    /* Clear globals */
    l2cap_listen_pcbs = NULL;
    l2cap_active_pcbs = NULL;
    l2cap_tmp_pcb = NULL;
    l2cap_tmp_sig = NULL;
    l2cap_insegs = NULL;
    l2cap_tmp_inseg = NULL;

    /* Initialize the signal identifier (0x00 shall never be used) */
    sigid_nxt = 0x00;
}

void l2cap_tmr(void)
{
    l2cap_sig_t *sig;
    l2cap_pcb_t *pcb;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    /* Step through all of the active pcbs */
    for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        /* Step through any unresponded signals */
        for(sig = pcb->unrsp_sigs; sig != NULL; sig = sig->next)
        {
            /* Check if channel is not reliable */
            if(pcb->cfg.outflushto < 0xFFFF)
            {
                /* Check if rtx is active. Otherwise ertx is active */
                if(sig->rtx > 0)
                {
                    /* Adjust rtx timer */
                    --sig->rtx;
                    /* Check if rtx has expired */
                    if(sig->rtx == 0)
                    {
                        if(sig->nrtx == 0)
                        {
                            /* Move pcb to closed state */
                            pcb->state = L2CAP_CLOSED;
                            /* Indicate disconnect to upper layer */
                            BT_L2CAP_TRACE_DEBUG("l2cap_tmr: Max number of retransmissions (rtx) has expired\n");

                            L2CA_ACTION_DISCONN_IND(pcb,BT_ERR_OK);
                        }
                        else
                        {
                            --sig->nrtx;
                            /* Indicate timeout to upper layer */
                            L2CA_ACTION_TO_IND(pcb,BT_ERR_OK);
                            /* Retransmitt signal w timeout doubled */
                            sig->rtx += sig->rtx;
                            ret = l2cap_rexmit_signal(pcb, sig);
                        }
                    } /* if */
                }
                else
                {
                    /* Adjust ertx timer */
                    --sig->ertx;
                    /* Check if ertx has expired */
                    if(sig->ertx == 0)
                    {
                        if(sig->nrtx == 0)
                        {
                            /* Move pcb to closed state */
                            pcb->state = L2CAP_CLOSED;
                            /* Indicate disconnect to upper layer */
                            BT_L2CAP_TRACE_DEBUG("l2cap_tmr: Max number of retransmissions (ertx) has expired\n");
                            L2CA_ACTION_DISCONN_IND(pcb,BT_ERR_OK);
                        }
                        else
                        {
                            --sig->nrtx;
                            /* Indicate timeout to upper layer */
                            L2CA_ACTION_TO_IND(pcb,BT_ERR_OK);
                            /* Disable ertx, activate rtx and retransmitt signal */
                            sig->ertx = 0;
                            sig->rtx = L2CAP_RTX;
                            ret = l2cap_rexmit_signal(pcb, sig);
                        }
                    } /* if */
                } /* else */
            } /* if */
        } /* for */

        /* Check configuration timer */
        if(pcb->state == L2CAP_CONFIG)
        {
            /* Check if configuration timer is active */
            if(pcb->cfg.cfgto > 0)
            {
                --pcb->cfg.cfgto;
                BT_L2CAP_TRACE_DEBUG("l2cap_tmr: Configuration timer = %d\n", pcb->cfg.cfgto);

                /* Check if config timer has expired */
                if(pcb->cfg.cfgto == 0)
                {
                    /* Connection attempt failed. Disconnect */
                    l2cap_disconnect_req(pcb, NULL);
                    /* Notify the application that the connection attempt failed */
                    if(pcb->cfg.l2capcfg & L2CAP_CFG_IR)
                    {
                        L2CA_ACTION_CONN_CFM(pcb, L2CAP_CONN_CFG_TO, 0x0000);
                    }
                    else
                    {
                        L2CA_ACTION_CONN_IND(pcb, BT_ERR_OK);
                    }
                    pcb->cfg.cfgto = L2CAP_CFG_TO; /* Reset timer */
                }
            }
        }
    }
}

err_t l2cap_write(struct bd_addr_t *bdaddr, struct bt_pbuf_t *p, uint16_t len)
{
    uint8_t pb = L2CAP_ACL_START;
    uint16_t maxsize;
    uint16_t outsize;
    err_t ret = BT_ERR_OK;
    struct bt_pbuf_t *q;
    uint16_t i = 0;

    maxsize = hci_pdu_maxsize();
    q = p;

    while(len && ret == BT_ERR_OK)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_write: len %d maxsize %d p->len %d\n", len, maxsize, p->len);
        if(len > maxsize)
        {
            ret = hci_acl_write(bdaddr, q, maxsize, pb);
            len -= maxsize;
            outsize = maxsize;

            BT_L2CAP_TRACE_DEBUG("l2cap_write: Outsize before %d\n", outsize);

            while(q->len < outsize)
            {
                outsize -= q->len;
                q = q->next;
            }
            BT_L2CAP_TRACE_DEBUG("l2cap_write: Outsize after %d\n", outsize);
            if(outsize)
            {
                bt_pbuf_header(q, -outsize);
                i += outsize;
            }
            pb = L2CAP_ACL_CONT;
            BT_L2CAP_TRACE_DEBUG("l2cap_write: FRAG\n");

        }
        else
        {
            ret = hci_acl_write(bdaddr, q, len, pb);
            len = 0;
        }
    }
    bt_pbuf_header(q, i);
    BT_L2CAP_TRACE_DEBUG("l2cap_write: DONE\n");
    return ret;
}



static err_t _l2cap_sig_cmd_reject_process(l2cap_pcb_t *pcb,l2cap_sig_t *sig)
{
    /* Remove signal from unresponded list and deallocate it */
    L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
    bt_pbuf_free(sig->p);
    bt_memp_free(MEMP_L2CAP_SIG, sig);
    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Our command was rejected so we disconnect\n");
    l2cap_disconnect_req(pcb, NULL);

    return BT_ERR_OK;
}

static err_t _l2cap_sig_conn_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr)
{
    l2cap_pcb_listen_t *lpcb;
    struct bt_pbuf_t *data;
    uint16_t psm = ((uint16_t *)p->payload)[0];
    /* Search for a listening pcb */
    for(lpcb = l2cap_listen_pcbs; lpcb != NULL; lpcb = lpcb->next)
    {
        if(lpcb->psm == psm)
        {
            /* Found a listening pcb with the correct PSM */
            break;
        }
    }
    /* If no matching pcb was found, send a connection rsp neg (PSM) */
    if(lpcb == NULL)
    {
        /* Alloc size of data in conn rsp signal */
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_RSP_SIZE, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = L2CAP_CONN_REF_PSM;
            ((uint16_t *)data->payload)[1] = 0; /* No further info available */
            _l2cap_signal_send(pcb, L2CAP_CONN_RSP, sighdr->id, bdaddr, data);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
    }
    else
    {
        /* Initiate a new active pcb */
        pcb = l2cap_new();
        if(pcb == NULL)
        {

            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: could not allocate PCB\n");
            /* Send a connection rsp neg (no resources available) and alloc size of data in conn rsp
               signal */
            if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_RSP_SIZE, BT_PBUF_RAM)) != NULL)
            {
                ((uint16_t *)data->payload)[0] = L2CAP_CONN_REF_RES;
                ((uint16_t *)data->payload)[1] = 0; /* No further info available */
                _l2cap_signal_send(pcb, L2CAP_CONN_RSP, sighdr->id, &(pcb->remote_bdaddr), data);
            }
            else
            {
                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

                return BT_ERR_MEM;
            }
        }
        bd_addr_set(&(pcb->remote_bdaddr),bdaddr);

        pcb->scid = _l2cap_cid_alloc();
        pcb->dcid = ((uint16_t *)p->payload)[1];
        pcb->psm = psm;
        pcb->callback_arg = lpcb->callback_arg;
        pcb->l2ca_connect_ind = lpcb->l2ca_connect_ind;

        pcb->state = L2CAP_CONFIG;
        L2CAP_REG(&l2cap_active_pcbs, pcb);

        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: A connection request was received. Send a response\n");

        data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_RSP_SIZE, BT_PBUF_RAM);
        if(data == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            return BT_ERR_MEM;
        }
        ((uint16_t *)data->payload)[0] = pcb->scid;
        ((uint16_t *)data->payload)[1] = pcb->dcid;
        ((uint16_t *)data->payload)[2] = L2CAP_CONN_SUCCESS;
        ((uint16_t *)data->payload)[3] = 0x0000; /* No further information available */

        /* Send the response */
        _l2cap_signal_send(pcb, L2CAP_CONN_RSP, sighdr->id, &(pcb->remote_bdaddr), data);

    }

    return BT_ERR_OK;
}


static err_t _l2cap_sig_conn_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_t *sig)
{
    uint16_t result, status;
    if(pcb == NULL)
    {
        return BT_ERR_CONN;
    }
    BT_ASSERT("l2cap_process_sig: conn rsp, active pcb->state == W4_L2CAP_CONNECT_RSP\n",
              pcb->state == W4_L2CAP_CONNECT_RSP);
    result = ((uint16_t *)p->payload)[2];
    status = ((uint16_t *)p->payload)[3];
    switch(result)
    {
    case L2CAP_CONN_SUCCESS:
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Conn_rsp_sucess, status %d\n", status);

        BT_ASSERT("l2cap_process_sig: conn rsp success, pcb->scid == ((uint16_t *)p->payload)[1]\n",
                  pcb->scid == ((uint16_t *)p->payload)[1]);

        /* Set destination connection id */
        pcb->dcid = ((uint16_t *)p->payload)[0];

        /* Remove signal from unresponded list and deallocate it */
        L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
        bt_pbuf_free(sig->p);
        bt_memp_free(MEMP_L2CAP_SIG, sig);

        /* Configure connection */
        pcb->state = L2CAP_CONFIG;

        /* If initiator send a configuration request */
        if(pcb->cfg.l2capcfg & L2CAP_CFG_IR)
        {
            l2cap_config_req(pcb);
            pcb->cfg.l2capcfg |= L2CAP_CFG_OUT_REQ;
        }
        break;
    case L2CAP_CONN_PND:
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Conn_rsp_pnd, status %d\n", status);

        /* Disable rtx and enable ertx */
        sig->rtx = 0;
        sig->ertx = L2CAP_ERTX;
        break;
    default:
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Conn_rsp_neg, result %d\n", result);

        /* Remove signal from unresponded list and deallocate it */
        L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
        bt_pbuf_free(sig->p);
        bt_memp_free(MEMP_L2CAP_SIG, sig);

        L2CA_ACTION_CONN_CFM(pcb,result,status);
        break;
    }

    return BT_ERR_OK;
}

static err_t _l2cap_sig_cfg_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr)
{
    uint16_t siglen;
    l2cap_cfgopt_hdr_t *opthdr;
    uint16_t flags, dcid;
    struct bt_pbuf_t  *data;
    uint16_t rspstate = L2CAP_CFG_SUCCESS;
    struct bt_pbuf_t  *s;

    siglen = sighdr->len;
    dcid = ((uint16_t *)p->payload)[0];
    flags = ((uint16_t *)p->payload)[1];
    siglen -= 4;
    bt_pbuf_header(p, -4);

    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Congfiguration request, flags = %d\n", flags);


    /* Find PCB with matching cid */
    for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: dcid = 0x%x, pcb->scid = 0x%x, pcb->dcid = 0x%x\n\n", dcid, pcb->scid, pcb->dcid);

        if(pcb->scid == dcid)
        {
            /* Matching cid found */
            break;
        }
    }
    /* If no matching cid was found, send a cmd reject (Invalid cid) */
    if(pcb == NULL)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Cfg req: no matching cid was found\n");

        /* Alloc size of reason in cmd rej + data (dcid + scid) */
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CMD_REJ_SIZE+4, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = L2CAP_INVALID_CID;
            ((uint16_t *)data->payload)[1] = dcid; /* Requested local cid */
            ((uint16_t *)data->payload)[2] = L2CAP_NULL_CID; /* Remote cid not known */

            _l2cap_signal_send(NULL, L2CAP_CMD_REJ, sighdr->id, bdaddr, data);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
    }
    else     /* Handle config request */
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Handle configuration request\n");
        pcb->ursp_id = sighdr->id; /* Set id of request to respond to */

        /* Parse options and add to pcb */
        while(siglen > 0)
        {
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Siglen = %d\n", siglen);

            opthdr = p->payload;
            /* Check if type of action bit indicates a non-hint. Hints are ignored */
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Type of action bit = %d\n", L2CAP_OPTH_TOA(opthdr));

            if(L2CAP_OPTH_TOA(opthdr) == 0)
            {
                BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Type = %d\n", L2CAP_OPTH_TYPE(opthdr));
                BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Length = %d\n", opthdr->len);
                switch(L2CAP_OPTH_TYPE(opthdr))
                {
                case L2CAP_CFG_MTU:
                    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Out MTU = %d\n", ((uint16_t *)p->payload)[1]);
                    pcb->cfg.outmtu = ((uint16_t *)p->payload)[1];
                    break;
                case L2CAP_FLUSHTO:
                    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: In flush timeout = %d\n", ((uint16_t *)p->payload)[1]);

                    pcb->cfg.influshto = ((uint16_t *)p->payload)[1];
                    break;
                case L2CAP_QOS:
                    /* If service type is Best Effort or No Traffic the remainder fields will be ignored */
                    if(((uint8_t *)p->payload)[3] == L2CAP_QOS_GUARANTEED)
                    {

                        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: This implementation does not support the guaranteed QOS service type");

                        if(rspstate == L2CAP_CFG_SUCCESS)
                        {
                            rspstate = L2CAP_CFG_UNACCEPT;
                            if(pcb->cfg.opt != NULL)
                            {
                                bt_pbuf_free(pcb->cfg.opt);
                                pcb->cfg.opt = NULL;
                            }
                        }

                        if((s = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFGOPTHDR_LEN + opthdr->len, BT_PBUF_RAM)) == NULL)
                        {

                            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                        }
                        memcpy((uint8_t *)s->payload, (uint8_t *)p->payload, L2CAP_CFGOPTHDR_LEN + opthdr->len);
                        if(pcb->cfg.opt == NULL)
                        {
                            pcb->cfg.opt = s;
                        }
                        else
                        {
                            bt_pbuf_chain(pcb->cfg.opt, s);
                            bt_pbuf_free(s);
                        }
                    }
                    break;
                default:
                    if(rspstate != L2CAP_CFG_REJ)
                    {
                        /* Unknown option. Add to unknown option type buffer */
                        if(rspstate != L2CAP_CFG_UNKNOWN)
                        {
                            rspstate = L2CAP_CFG_UNKNOWN;
                            if(pcb->cfg.opt != NULL)
                            {
                                bt_pbuf_free(pcb->cfg.opt);
                                pcb->cfg.opt = NULL;
                            }
                        }

                        if((s = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFGOPTHDR_LEN + opthdr->len, BT_PBUF_RAM)) == NULL)
                        {

                            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                        }
                        memcpy((uint8_t *)s->payload, (uint8_t *)p->payload, L2CAP_CFGOPTHDR_LEN + opthdr->len);
                        if(pcb->cfg.opt == NULL)
                        {
                            pcb->cfg.opt = s;
                        }
                        else
                        {
                            bt_pbuf_chain(pcb->cfg.opt, s);
                            bt_pbuf_free(s);
                        }
                    }
                    break;
                } /* switch */
            } /* if(L2CAP_OPTH_TOA(opthdr) == 0) */
            bt_pbuf_header(p, -(L2CAP_CFGOPTHDR_LEN + opthdr->len));
            siglen -= L2CAP_CFGOPTHDR_LEN + opthdr->len;
        } /* while */

        /* If continuation flag is set we don't send the final response just yet */
        if((flags & 0x0001) == 1)
        {
            /* Send success result with no options until the full request has been received */
            if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFG_RSP_SIZE, BT_PBUF_RAM)) == NULL)
            {

                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                return BT_ERR_MEM;
            }
            ((uint16_t *)data->payload)[0] = pcb->dcid;
            ((uint16_t *)data->payload)[1] = 0;
            ((uint16_t *)data->payload)[2] = L2CAP_CFG_SUCCESS;
            _l2cap_signal_send(pcb, L2CAP_CFG_RSP, pcb->ursp_id, &(pcb->remote_bdaddr), data);
            return BT_ERR_OK;
        }

        /* Send a configure request for outgoing link if it hasnt been configured */
        if(!(pcb->cfg.l2capcfg & L2CAP_CFG_IR) && !(pcb->cfg.l2capcfg & L2CAP_CFG_OUT_REQ))
        {
            l2cap_config_req(pcb);
            pcb->cfg.l2capcfg |= L2CAP_CFG_OUT_REQ;
        }

        /* Send response to configuration request */
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Send response to configuration request\n");
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFG_RSP_SIZE, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = pcb->dcid;
            ((uint16_t *)data->payload)[1] = 0; /* Flags (No continuation) */
            ((uint16_t *)data->payload)[2] = rspstate; /* Result */
            if(pcb->cfg.opt != NULL)
            {
                BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: pcb->cfg.opt->len = %d\n", pcb->cfg.opt->len);
                bt_pbuf_chain(data, pcb->cfg.opt); /* Add option type buffer to data buffer */
                bt_pbuf_free(pcb->cfg.opt);
                pcb->cfg.opt = NULL;
            }
            _l2cap_signal_send(pcb, L2CAP_CFG_RSP, pcb->ursp_id, &(pcb->remote_bdaddr), data);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }

        if(rspstate == L2CAP_CFG_SUCCESS)
        {
            pcb->cfg.l2capcfg |= L2CAP_CFG_OUT_SUCCESS;
            /* L2CAP connection established if a successful configuration response has been sent */
            if(pcb->cfg.l2capcfg & L2CAP_CFG_IN_SUCCESS)
            {
                /* IPCP connection established, notify upper layer that connection is open */
                pcb->state = L2CAP_OPEN;
                if(pcb->cfg.l2capcfg & L2CAP_CFG_IR)
                {
                    L2CA_ACTION_CONN_CFM(pcb, L2CAP_CONN_SUCCESS, 0x0000);
                }
                else
                {
                    L2CA_ACTION_CONN_IND(pcb, BT_ERR_OK);
                }
            }
        }
    } /* else */

    return BT_ERR_OK;
}

static err_t _l2cap_sig_cfg_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,l2cap_sig_t *sig)
{
    uint16_t result, flags;;
    //uint16_t scid;
    uint16_t siglen;
    l2cap_cfgopt_hdr_t *opthdr;
    struct bt_pbuf_t  *data;

    if(pcb == NULL)
    {
        return BT_ERR_CONN;
    }

    /* Remove signal from unresponded list and deallocate it */
    L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
    bt_pbuf_free(sig->p);
    bt_memp_free(MEMP_L2CAP_SIG, sig);

    BT_ASSERT(("l2cap_process_sig: cfg rsp, active pcb->state == L2CAP_CONFIG\n"),
              pcb->state == L2CAP_CONFIG);

    siglen = sighdr->len;
    //scid = ((uint16_t *)p->payload)[0];
    flags = ((uint16_t *)p->payload)[1];
    result = ((uint16_t *)p->payload)[2];

    if (siglen < 6)
    {
        return BT_ERR_VAL;
    }
    siglen -= 6;
    bt_pbuf_header(p, -6);
    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Outgoing configuration result == %d continuation flag == %d\n", result, flags);

    /* Handle config request */
    switch(result)
    {
    case L2CAP_CFG_SUCCESS:
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Successfull outgoing configuration\n");

        pcb->cfg.l2capcfg |= L2CAP_CFG_IN_SUCCESS; /* Local side of the connection
						      has been configured for outgoing data */
        pcb->cfg.cfgto = L2CAP_CFG_TO; /* Reset configuration timeout */

        if(pcb->cfg.outflushto != L2CAP_CFG_DEFAULT_OUTFLUSHTO)
        {
            hci_write_flush_timeout(&pcb->remote_bdaddr, pcb->cfg.outflushto);
        }

        /* L2CAP connection established if a successful configuration response has been sent */
        if(pcb->cfg.l2capcfg & L2CAP_CFG_OUT_SUCCESS)
        {
            pcb->state = L2CAP_OPEN;
            if(pcb->cfg.l2capcfg & L2CAP_CFG_IR)
            {
                L2CA_ACTION_CONN_CFM(pcb, L2CAP_CONN_SUCCESS, 0x0000);
            }
            else
            {
                L2CA_ACTION_CONN_IND(pcb, BT_ERR_OK);
            }
        }
        break;
    case L2CAP_CFG_UNACCEPT:
        /* Parse and add options to pcb */
        while(siglen > 0)
        {
            opthdr = p->payload;
            /* Check if type of action bit indicates a non-hint. Hints are ignored */
            if(L2CAP_OPTH_TOA(opthdr) == 0)
            {
                switch(L2CAP_OPTH_TYPE(opthdr))
                {
                case L2CAP_CFG_MTU:
                    if(L2CAP_MTU > ((uint16_t *)p->payload)[1])
                    {
                        pcb->cfg.outmtu = ((uint16_t *)p->payload)[1];
                    }
                    else
                    {
                        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Configuration of MTU failed\n");

                        l2cap_disconnect_req(pcb, NULL);
                        return BT_ERR_ARG;
                    }
                    break;
                case L2CAP_FLUSHTO:
                    pcb->cfg.influshto = ((uint16_t *)p->payload)[1];
                    break;
                case L2CAP_QOS:
                    /* If service type Best Effort is not accepted we will close the connection */
                    if(((uint8_t *)p->payload)[3] != L2CAP_QOS_BEST_EFFORT)
                    {

                        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Unsupported service type\n");

                        l2cap_disconnect_req(pcb, NULL);
                        return BT_ERR_ARG;
                    }
                    break;
                default:
                    /* Should not happen, skip option */
                    break;
                } /* switch */
            } /* if(L2CAP_OPTH_TOA(opthdr) == 0) */
            bt_pbuf_header(p, -(L2CAP_CFGOPTHDR_LEN + opthdr->len));
            siglen -= L2CAP_CFGOPTHDR_LEN + opthdr->len;
        } /* while */

        /* Send out a new configuration request if the continuation flag isn't set */
        if((flags & 0x0001) == 0)
        {
            l2cap_config_req(pcb);
        }
        break;
    case L2CAP_CFG_REJ:
    /* Fallthrough */
    case L2CAP_CFG_UNKNOWN:
    /* Fallthrough */
    default:
        if((flags & 0x0001) == 0)
        {
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Configuration failed\n");

            l2cap_disconnect_req(pcb, NULL);
            return BT_ERR_ARG;
        }
        break;
    } /* switch(result) */

    /* If continuation flag is set we must send a NULL configuration request */
    if((flags & 0x0001) == 1)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Continuation flag is set. Send empty (default) config request signal\n");

        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFG_REQ_SIZE, BT_PBUF_RAM)) == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            return BT_ERR_ARG;
        }
        /* Assemble config request packet */
        ((uint16_t *)data->payload)[0] = pcb->scid;
        ((uint16_t *)data->payload)[2] = 0;
        _l2cap_signal_send(pcb, L2CAP_CFG_REQ, 0, &(pcb->remote_bdaddr), data);
    }

    return BT_ERR_OK;
}


static err_t _l2cap_sig_disconn_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr)
{
    uint16_t siglen;
    uint16_t dcid;
    //uint16_t flags;
    struct bt_pbuf_t *data;

    siglen = sighdr->len;
    dcid = ((uint16_t *)p->payload)[0];
    siglen = siglen - 2;
    //flags = ((uint16_t *)p->payload)[1];
    siglen = siglen - 2;
    bt_pbuf_header(p, -4);

    /* Find PCB with matching cid */
    for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(pcb->scid == dcid)
        {
            /* Matching cid found */
            break;
        }
    }
    /* If no matching cid was found, send a cmd reject (Invalid cid) */
    if(pcb == NULL)
    {
        /* Alloc size of reason in cmd rej + data (dcid + scid) */
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CMD_REJ_SIZE+4, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = L2CAP_INVALID_CID;
            ((uint16_t *)data->payload)[1] = dcid; /* Requested local cid */
            ((uint16_t *)data->payload)[2] = L2CAP_NULL_CID; /* Remote cid not known */

            _l2cap_signal_send(NULL, L2CAP_CMD_REJ, sighdr->id, bdaddr, data);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
    }
    else     /* Handle disconnection request */
    {
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_DISCONN_RSP_SIZE, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = pcb->scid;
            ((uint16_t *)data->payload)[1] = pcb->dcid;
            _l2cap_signal_send(pcb, L2CAP_DISCONN_RSP, sighdr->id, &(pcb->remote_bdaddr), data);

            /* Give upper layer indication */
            pcb->state = L2CAP_CLOSED;
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Disconnection request\n");
            L2CA_ACTION_DISCONN_IND(pcb,BT_ERR_OK);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
    }

    return BT_ERR_OK;
}


static err_t _l2cap_sig_disconn_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_t *sig)
{
    if(pcb == NULL)
    {
        return BT_ERR_CONN;
    }
    /* Remove signal from unresponded list and deallocate it */
    L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
    bt_pbuf_free(sig->p);
    bt_memp_free(MEMP_L2CAP_SIG, sig);

    L2CA_ACTION_DISCONN_CFM(pcb); /* NOTE: Application should
					   now close the connection */

    return BT_ERR_OK;
}

static err_t _l2cap_sig_echo_req_process(l2cap_pcb_t *pcb,l2cap_sig_hdr_t *sighdr)
{
    if(pcb == NULL)
    {
        /* A response without a matching request is silently discarded */
        return BT_ERR_OK;
    }
    pcb->ursp_id = sighdr->id;
    _l2cap_signal_send(pcb, L2CAP_ECHO_RSP, sighdr->id, &(pcb->remote_bdaddr), NULL);

    return BT_ERR_OK;
}


static err_t _l2cap_sig_echo_rsp_process(l2cap_pcb_t *pcb,l2cap_sig_t *sig)
{
    if(pcb == NULL)
    {
        /* A response without a matching request is silently discarded */
        return BT_ERR_OK;
    }
    /* Remove signal from unresponded list and deallocate it */
    L2CAP_SIG_RMV(&(pcb->unrsp_sigs), sig);
    bt_pbuf_free(sig->p);
    bt_memp_free(MEMP_L2CAP_SIG, sig);

    /* Remove temporary pcb from active list */
    L2CAP_RMV(&l2cap_active_pcbs, pcb);
    L2CA_ACTION_PING_CFM(pcb,L2CAP_ECHO_RCVD);

    return BT_ERR_OK;
}

static err_t _l2cap_sig_info_req_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *data;
    uint16_t info_type = ((uint16_t *)p->payload)[0];
    BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: infomation type %d\n",info_type);

    switch(info_type)
    {
    case L2CAP_CONLESS_MTU:
        data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_INFO_MTU_RSP_SIZE, BT_PBUF_RAM);
        if(data == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            break;
        }
        bt_le_store_16((uint8_t *)data->payload,0,L2CAP_CONLESS_MTU);
        bt_le_store_16((uint8_t *)data->payload,2,L2CAP_INFO_REQ_SUCCESS);
        bt_le_store_16((uint8_t *)data->payload,4,L2CAP_MTU);

        /* Send the response */
        _l2cap_signal_send(pcb, L2CAP_INFO_RSP, sighdr->id, bdaddr, data);
        break;
    case L2CAP_EXFEATURE_SUPPORT:
    {
        data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_INFO_EXFEATURE_RSP_SIZE, BT_PBUF_RAM);
        if(data == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            break;
        }
        bt_le_store_16((uint8_t *)data->payload,0,L2CAP_EXFEATURE_SUPPORT);
        bt_le_store_16((uint8_t *)data->payload,2,L2CAP_INFO_REQ_SUCCESS);
        bt_le_store_32((uint8_t *)data->payload,4,L2CAP_EXTFEA_SUPPORTED_MASK);
        /* Send the response */
        _l2cap_signal_send(pcb, L2CAP_INFO_RSP, sighdr->id, bdaddr, data);
    }
    break;
    case L2CAP_FIXED_CHNL_SUPPORT:
    {
        uint8_t fixed_channel[8] = {0};
        /* L2CAP Signaling Channel (0x02) + Connectionless reception (0x04)*/
        fixed_channel[0] = 0x06;
        data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_INFO_FIXED_CHNL_RSP_SIZE, BT_PBUF_RAM);
        if(data == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            break;
        }
        bt_le_store_16((uint8_t *)data->payload,0,L2CAP_FIXED_CHNL_SUPPORT);
        bt_le_store_16((uint8_t *)data->payload,2,L2CAP_INFO_REQ_SUCCESS);
        memcpy((uint8_t *)data->payload + 4,fixed_channel,8);

        /* Send the response */
        _l2cap_signal_send(pcb, L2CAP_INFO_RSP, sighdr->id, bdaddr, data);
    }
    default:
        break;
    }

    return BT_ERR_OK;
}

static err_t _l2cap_sig_info_rsp_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p)
{
    return BT_ERR_OK;
}


static err_t _l2cap_sig_unsupport_process(l2cap_pcb_t *pcb,struct bt_pbuf_t *p,l2cap_sig_hdr_t *sighdr,struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *data;

    /* Alloc size of reason in cmd rej */
    if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CMD_REJ_SIZE, BT_PBUF_RAM)) != NULL)
    {
        ((uint16_t *)data->payload)[0] = L2CAP_CMD_NOT_UNDERSTOOD;

        _l2cap_signal_send(NULL, L2CAP_CMD_REJ, sighdr->id, bdaddr, data);
    }
    else
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    }

    return BT_ERR_OK;
}


void l2cap_process_sig(struct bt_pbuf_t *q, l2cap_hdr_t *l2caphdr, struct bd_addr_t *bdaddr)
{
    l2cap_sig_hdr_t *sighdr;
    l2cap_sig_t *sig = NULL;
    l2cap_pcb_t *pcb = NULL;
    uint16_t len;
    struct bt_pbuf_t *p, *data;

    p = q;

    len = l2caphdr->len;

    while(len > 0)
    {
        /* Set up signal header */
        sighdr = p->payload;

        bt_pbuf_header(p, -L2CAP_SIGHDR_LEN);

        /* Check if this is a response/reject signal, and if so, find the matching request */
        if(sighdr->code % 2)   /* if odd this is a resp/rej signal */
        {
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Response/reject signal received id = %d code = %d\n",
                                 sighdr->id, sighdr->code);
            for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
            {
                for(sig = pcb->unrsp_sigs; sig != NULL; sig = sig->next)
                {
                    if(sig->sigid == sighdr->id)
                    {
                        break; /* found */
                    }
                }
                if(sig != NULL)
                {
                    break;
                }
            }
        }
        else
        {
            BT_L2CAP_TRACE_DEBUG("l2cap_process_sig: Request signal received id = %d code = %d\n",
                                 sighdr->id, sighdr->code);
        }

        /* Reject packet if length exceeds MTU */
        if(l2caphdr->len > L2CAP_MTU)
        {
            /* Alloc size of reason in cmd rej + MTU */
            if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CMD_REJ_SIZE+2, BT_PBUF_RAM)) != NULL)
            {
                ((uint16_t *)data->payload)[0] = L2CAP_MTU_EXCEEDED;
                ((uint16_t *)data->payload)[1] = L2CAP_MTU;

                _l2cap_signal_send(NULL, L2CAP_CMD_REJ, sighdr->id, bdaddr, data);
            }
            else
            {
                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            }
            break;
        }

        switch(sighdr->code)
        {
        case L2CAP_CMD_REJ:
            _l2cap_sig_cmd_reject_process(pcb,sig);
            break;
        case L2CAP_CONN_REQ:
            _l2cap_sig_conn_req_process(pcb,p,sighdr,bdaddr);
            break;
        case L2CAP_CONN_RSP:
            _l2cap_sig_conn_rsp_process(pcb,p,sig);
            break;
        case L2CAP_CFG_REQ:
            _l2cap_sig_cfg_req_process(pcb,p,sighdr,bdaddr);
            break;
        case L2CAP_CFG_RSP:
            _l2cap_sig_cfg_rsp_process(pcb,p,sighdr,sig);
            break;
        case L2CAP_DISCONN_REQ:
            _l2cap_sig_disconn_req_process(pcb,p,sighdr,bdaddr);
            break;
        case L2CAP_DISCONN_RSP:
            _l2cap_sig_disconn_rsp_process(pcb,p,sig);
            break;
        case L2CAP_ECHO_REQ:
            _l2cap_sig_echo_req_process(pcb,sighdr);
            break;
        case L2CAP_ECHO_RSP:
            _l2cap_sig_echo_rsp_process(pcb,sig);
            break;
        case L2CAP_INFO_REQ:
            _l2cap_sig_info_req_process(pcb,p,sighdr,bdaddr);
            break;
        case L2CAP_INFO_RSP:
            _l2cap_sig_info_rsp_process(pcb,p);
            break;
        default:
            _l2cap_sig_unsupport_process(pcb,p,sighdr,bdaddr);
            break;
        } /* switch */
        len = len - (sighdr->len + L2CAP_SIGHDR_LEN);
        bt_pbuf_header(p, -(sighdr->len));
    } /* while */
}

static l2cap_seg_t * l2cap_reassembly_data(struct bt_pbuf_t *p, struct bd_addr_t *bdaddr,uint8_t *can_continue)
{

    l2cap_seg_t *inseg;
    hci_acl_hdr_t *aclhdr = p->payload;

    bt_pbuf_header(p, -HCI_ACL_HDR_LEN);
    bt_pbuf_realloc(p, aclhdr->len);


    for(inseg = l2cap_insegs; inseg != NULL; inseg = inseg->next)
    {
        if(bd_addr_cmp(bdaddr, &(inseg->bdaddr)))
        {
            break;
        }
    }

    if(((aclhdr->conhdl_pb_bc >> 12) & 0x03)== L2CAP_ACL_CONT)	 /* Continuing fragment */
    {
        if(inseg == NULL)
        {
            /* Discard packet */
            BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Continuing fragment. Discard packet\n");
            bt_pbuf_free(p);
            *can_continue = 0;
            return inseg;
        }
        else if(inseg->p->tot_len + p->tot_len > inseg->len)
        {
            /* Check if length of
            							segment exceeds
            							l2cap header length */
            /* Discard packet */
            BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Continuing fragment. Length exceeds L2CAP hdr length. Discard packet\n");
            bt_pbuf_free(inseg->p);
            L2CAP_SEG_RMV(&(l2cap_insegs), inseg);
            bt_memp_free(MEMP_L2CAP_SEG, inseg);

            bt_pbuf_free(p);
            *can_continue = 0;
            return inseg;
        }
        /* Add bt_pbuf_t to segement */
        bt_pbuf_chain(inseg->p, p);
        bt_pbuf_free(p);

    }
    else if(((aclhdr->conhdl_pb_bc >> 12) & 0x03) == L2CAP_ACL_START)	  /* Start of L2CAP packet */
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Start of L2CAP packet p->len = %d, p->tot_len = %d\n",
                             p->len, p->tot_len);
        if(inseg != NULL)	/* Check if there are segments missing in a previous packet */
        {
            /* Discard previous packet */
            BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Start of L2CAP packet. Discard previous packet\n");
            bt_pbuf_free(inseg->p);
        }
        else
        {
            inseg = bt_memp_malloc(MEMP_L2CAP_SEG);
            bd_addr_set(&(inseg->bdaddr), bdaddr);
            L2CAP_SEG_REG(&(l2cap_insegs), inseg);
        }
        inseg->p = p;
        inseg->l2caphdr = p->payload;
        inseg->len = inseg->l2caphdr->len + L2CAP_HDR_LEN;
        for(inseg->pcb = l2cap_active_pcbs; inseg->pcb != NULL; inseg->pcb = inseg->pcb->next)
        {
            if(inseg->pcb->scid == inseg->l2caphdr->cid)
            {
                break; /* found */
            }
        }
    }
    else
    {
        /* Discard packet */
        BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Discard packet\n");

        bt_pbuf_free(inseg->p);
        L2CAP_SEG_RMV(&(l2cap_insegs), inseg);
        bt_memp_free(MEMP_L2CAP_SEG, inseg);

        bt_pbuf_free(p);
        *can_continue = 0;
        return inseg;
    }
    if(inseg->p->tot_len < inseg->len)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Get continuing segments\n");
        *can_continue = 0;
        return inseg; /* Get continuing segments */
    }

    *can_continue = 1;
    return inseg;
}


static err_t _l2cap_null_cid_process(struct bt_pbuf_t *p,struct bd_addr_t *bdaddr)
{
    /* Illegal */
    BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Illegal null cid\n");

    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t _l2cap_classical_sig_cid_process(struct bt_pbuf_t *p,l2cap_hdr_t *l2cap_hdr,struct bd_addr_t *bdaddr)
{
    bt_pbuf_header(p, -L2CAP_HDR_LEN);
    l2cap_process_sig(p, l2cap_hdr, bdaddr);
    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static err_t _l2cap_connless_cid_process(struct bt_pbuf_t *p,struct bd_addr_t *bdaddr)
{
    /* Not needed by PAN, LAN access or DUN profiles */
    bt_pbuf_free(p);

    return BT_ERR_OK;
}


static err_t _l2cap_fixed_cid_process(uint16_t cid,struct bt_pbuf_t *p,struct bd_addr_t *bdaddr)
{
    l2cap_pcb_t *l2cap_pcb;

    BT_L2CAP_TRACE_DEBUG("_l2cap_fixed_cid_process\n");

    bt_pbuf_header(p, -L2CAP_HDR_LEN);
    for(l2cap_pcb = l2cap_active_pcbs; l2cap_pcb != NULL; l2cap_pcb = l2cap_pcb->next)
    {
        if(l2cap_pcb->fixed_cid == cid)
        {
            bd_addr_set(&(l2cap_pcb->remote_bdaddr),bdaddr);
            L2CA_ACTION_RECV(l2cap_pcb,p,BT_ERR_OK);
            break;
        }
    }

    bt_pbuf_free(p);

    return BT_ERR_OK;

}

static err_t _l2cap_dynamic_cid_process(l2cap_pcb_t *l2cap_pcb,struct bt_pbuf_t *p,l2cap_hdr_t *l2cap_hdr,struct bd_addr_t *bdaddr)
{
    struct bt_pbuf_t *data;
    if(l2cap_hdr->cid < 0x0040 || l2cap_pcb == NULL)
    {
        BT_L2CAP_TRACE_DEBUG("**********************************\n");
        /* Reserved for specific L2CAP functions or channel does not exist */
        /* Alloc size of reason in cmd rej */
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CMD_REJ_SIZE+4, BT_PBUF_RAM)) != NULL)
        {
            ((uint16_t *)data->payload)[0] = L2CAP_INVALID_CID;
            ((uint16_t *)data->payload)[1] = l2cap_hdr->cid;
            ((uint16_t *)data->payload)[2] = L2CAP_NULL_CID;

            _l2cap_signal_send(NULL, L2CAP_CMD_REJ, _l2cap_next_sigid(), bdaddr, data);
        }
        else
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        }
        bt_pbuf_free(p);
        return BT_ERR_VAL;
    }

    bt_pbuf_header(p, -L2CAP_HDR_LEN);

    /* Forward packet to higher layer */
    BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Forward packet to higher layer\n");
    BT_L2CAP_TRACE_DEBUG("l2cap_acl_input: Remote BD address: 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
                         bdaddr->addr[5],
                         bdaddr->addr[4],
                         bdaddr->addr[3],
                         bdaddr->addr[2],
                         bdaddr->addr[1],
                         bdaddr->addr[0]);

    L2CA_ACTION_RECV(l2cap_pcb,p,BT_ERR_OK);

    return BT_ERR_OK;
}


void l2cap_acl_input(struct bt_pbuf_t *p, struct bd_addr_t *bdaddr)
{
    uint8_t can_contiue;

    l2cap_seg_t *inseg = l2cap_reassembly_data(p,bdaddr,&can_contiue);

    if(!can_contiue)
        return;

    /* Handle packet */
    switch(inseg->l2caphdr->cid)
    {
    case L2CAP_NULL_CID:
        _l2cap_null_cid_process(inseg->p,bdaddr);
        break;
    case L2CAP_SIG_CID:
        _l2cap_classical_sig_cid_process(inseg->p,inseg->l2caphdr,bdaddr);
        break;
    case L2CAP_CONNLESS_CID:
        _l2cap_connless_cid_process(inseg->p,bdaddr);
        break;
    case L2CAP_ATT_CID:
        _l2cap_fixed_cid_process(L2CAP_ATT_CID,p,bdaddr);
        break;
	case L2CAP_SM_CID:
		_l2cap_fixed_cid_process(L2CAP_SM_CID,p,bdaddr);
		break;
    default:
        _l2cap_dynamic_cid_process(inseg->pcb,inseg->p,inseg->l2caphdr,bdaddr);
        break;
    }

    /* Remove input segment */
    L2CAP_SEG_RMV(&(l2cap_insegs), inseg);
    bt_memp_free(MEMP_L2CAP_SEG, inseg);

}



static uint16_t _l2cap_cid_alloc(void)
{
    uint16_t cid;
    l2cap_pcb_t *pcb;

    for (cid = L2CAP_MIN_CID; cid < L2CAP_MAX_CID; ++cid)
    {
        for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
        {
            if(pcb->scid == cid)
            {
                break;
            }
        }
        if(pcb == NULL)
        {
            return cid;
        }
    }
    return 0;
}

l2cap_pcb_t *l2cap_new(void)
{
    l2cap_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_L2CAP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(l2cap_pcb_t));
        pcb->state = L2CAP_CLOSED;

        /* Initialize configuration parameter options with default values */

        /* Maximum Transmission Unit */
        pcb->cfg.inmtu = L2CAP_MTU; /* The MTU that this implementation support */
        pcb->cfg.outmtu = 672; /* Default MTU. Two Baseband DH5 packets minus the Baseband ACL headers and
			      L2CAP header. This can be set here since we will never send any signals
			      larger than the L2CAP sig MTU (48 bytes) before L2CAP has been configured
			   */

        /* Flush Timeout */
        pcb->cfg.influshto = 0xFFFF;
        pcb->cfg.outflushto = 0xFFFF;

        pcb->cfg.cfgto = L2CAP_CFG_TO; /* Maximum time before terminating a negotiation.
				      Cfg shall not last more than 120s */
        pcb->cfg.opt = NULL;
        return pcb;
    }
    BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

err_t l2cap_close(l2cap_pcb_t *pcb)
{
    l2cap_sig_t *tmpsig;

    if(pcb->state == L2CAP_LISTEN)
    {
        L2CAP_RMV((l2cap_pcb_t **)&l2cap_listen_pcbs, pcb);
        bt_memp_free(MEMP_L2CAP_PCB_LISTEN, pcb);
    }
    else
    {
        L2CAP_RMV(&l2cap_active_pcbs, pcb);
        /* Free any unresponded signals */
        while(pcb->unrsp_sigs != NULL)
        {
            tmpsig = pcb->unrsp_sigs;
            pcb->unrsp_sigs = pcb->unrsp_sigs->next;
            bt_memp_free(MEMP_L2CAP_SIG, tmpsig);
        }

        bt_memp_free(MEMP_L2CAP_PCB, pcb);
    }
    pcb = NULL;
    return BT_ERR_OK;
}


void l2cap_deinit(void)
{
    l2cap_pcb_t *pcb, *tpcb;
    l2cap_pcb_listen_t *lpcb, *tlpcb;
    l2cap_seg_t *seg, *tseg;

    for(pcb = l2cap_active_pcbs; pcb != NULL;)
    {
        tpcb = pcb->next;
        l2cap_close(pcb);
        pcb = tpcb;
    }

    for(lpcb = l2cap_listen_pcbs; lpcb != NULL;)
    {
        tlpcb = lpcb->next;
        l2cap_close((l2cap_pcb_t *)lpcb);
        lpcb = tlpcb;
    }

    for(seg = l2cap_insegs; seg != NULL;)
    {
        tseg = seg->next;
        L2CAP_SEG_RMV(&(l2cap_insegs), seg);
        bt_memp_free(MEMP_L2CAP_SEG, seg);
        seg = tseg;
    }

    l2cap_init();
}

static  err_t _l2cap_signal_send(l2cap_pcb_t *pcb, uint8_t code, uint16_t ursp_id, struct bd_addr_t *remote_bdaddr,
                                 struct bt_pbuf_t *data)
{
    l2cap_sig_t *sig;
    l2cap_sig_hdr_t *sighdr;
    l2cap_hdr_t *hdr;
    err_t ret;

    /* Alloc a new signal */
    BT_L2CAP_TRACE_DEBUG("_l2cap_signal_send: Allocate memory for l2cap_sig_t. Code = 0x%x\n", code);
    if((sig = bt_memp_malloc(MEMP_L2CAP_SIG)) == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Alloc a bt_pbuf_t for signal */
    if((sig->p = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_HDR_LEN+L2CAP_SIGHDR_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    /* Setup signal header and leave room for l2cap hdr */
    sighdr = (l2cap_sig_hdr_t *)(((uint8_t *)sig->p->payload)+L2CAP_HDR_LEN);

    /* Chain data to signal and set length of signal data */
    if(data == NULL)
    {
        sighdr->len = 0;
    }
    else
    {
        bt_pbuf_chain(sig->p, data);
        bt_pbuf_free(data);
        sighdr->len = data->tot_len;
    }

    sighdr->code = code;

    if(sighdr->code % 2)   /* If odd this is a resp/rej signal */
    {
        sig->sigid = ursp_id; /* Get id */

        BT_L2CAP_TRACE_DEBUG("_l2cap_signal_send: Sending response/reject signal with id = %d code = %d\n",
                             sig->sigid, sighdr->code);
    }
    else
    {
        sig->sigid = _l2cap_next_sigid(); /* Alloc id */
        sig->rtx = L2CAP_RTX; /* Set Response Timeout Expired timer (in seconds)
			     should be at least as large as the BB flush timeout */
        sig->nrtx = L2CAP_MAXRTX; /* Set max number of retransmissions */

        BT_L2CAP_TRACE_DEBUG("_l2cap_signal_send: Sending request signal with id = %d code = %d\n",
                             sig->sigid, sighdr->code);
    }
    sighdr->id = sig->sigid; /* Set id */

    /* Set up L2CAP hdr */
    hdr = sig->p->payload;
    hdr->len = sig->p->tot_len - L2CAP_HDR_LEN;
    hdr->cid = L2CAP_SIG_CID; /* 0x0001 */

    ret = l2cap_write(remote_bdaddr, sig->p, sig->p->tot_len); /* Send peer L2CAP signal */

    /* Put signal on unresponded list if it's a request signal, else deallocate it */
    if(ret == BT_ERR_OK && (sighdr->code % 2) == 0)
    {
        BT_L2CAP_TRACE_DEBUG("_l2cap_signal_send: Registering sent request signal with id = %d code = %d\n",
                             sig->sigid, sighdr->code);
        L2CAP_SIG_REG(&(pcb->unrsp_sigs), sig);
    }
    else
    {
        BT_L2CAP_TRACE_DEBUG("_l2cap_signal_send: Deallocating sent response/reject signal with id = %d code = %d\n",
                             sig->sigid, sighdr->code);
        bt_pbuf_free(sig->p);
        sig->p = NULL;
        bt_memp_free(MEMP_L2CAP_SIG, sig);
    }

    return ret;
}

static err_t l2cap_rexmit_signal(l2cap_pcb_t *pcb,l2cap_sig_t *sig)
{
    err_t ret;

    /* Set up L2CAP hdr */
    ret = l2cap_write(&(pcb->remote_bdaddr), sig->p, sig->p->tot_len); /* Send peer L2CAP signal */

    return ret;
}

err_t l2cap_connect_req(l2cap_pcb_t *pcb, struct bd_addr_t *bdaddr, uint16_t psm,
                        uint8_t role_switch, l2ca_connect_cfm_cb l2ca_connect_cfm)
{
    err_t ret;
    struct bt_pbuf_t *data;

    if(bdaddr != NULL)
    {
        bd_addr_set(&(pcb->remote_bdaddr),bdaddr);
    }
    else
    {
        return BT_ERR_VAL;
    }


    pcb->psm = psm;
    pcb->l2ca_connect_cfm = l2ca_connect_cfm;
    pcb->scid = _l2cap_cid_alloc();
    BT_ASSERT("l2cap_connect_req: out of CIDs\n", pcb->scid != 0);

    pcb->cfg.l2capcfg |= L2CAP_CFG_IR; /* We are the initiator of this connection */

    if(!hci_is_connected(bdaddr))
    {
        ret = hci_connect_req(bdaddr, role_switch); /* Create ACL link w pcb state == CLOSED */
    }
    else
    {
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_REQ_SIZE, BT_PBUF_RAM)) == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM;
        }
        ((uint16_t *)data->payload)[0] = psm;
        ((uint16_t *)data->payload)[1] = pcb->scid;
        ret = _l2cap_signal_send(pcb, L2CAP_CONN_REQ, 0, &(pcb->remote_bdaddr), data); /* Send l2cap_conn_req signal */

        pcb->state = W4_L2CAP_CONNECT_RSP;
    }

    L2CAP_REG(&l2cap_active_pcbs, pcb);

    return ret;
}

err_t l2cap_ertm_connect_req(l2cap_pcb_t *pcb, struct bd_addr_t *bdaddr, uint16_t psm, uint8_t role_switch,
                             l2ca_connect_cfm_cb l2ca_connect_cfm)
{
    err_t ret;
    struct bt_pbuf_t *data;

    if(bdaddr != NULL)
    {
        bd_addr_set(&(pcb->remote_bdaddr),bdaddr);
    }
    else
    {
        return BT_ERR_VAL;
    }

    pcb->psm = psm;
    pcb->l2ca_connect_cfm = l2ca_connect_cfm;
    pcb->scid = _l2cap_cid_alloc();
    BT_ASSERT("l2cap_connect_req: out of CIDs\n", pcb->scid != 0);

    pcb->cfg.l2capcfg |= L2CAP_CFG_IR; /* We are the initiator of this connection */

    if(!hci_is_connected(bdaddr))
    {
        ret = hci_connect_req(bdaddr, role_switch); /* Create ACL link w pcb state == CLOSED */
    }
    else
    {
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_REQ_SIZE, BT_PBUF_RAM)) == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM;
        }
        ((uint16_t *)data->payload)[0] = psm;
        ((uint16_t *)data->payload)[1] = pcb->scid;
        ret = _l2cap_signal_send(pcb, L2CAP_CONN_REQ, 0, &(pcb->remote_bdaddr), data); /* Send l2cap_conn_req signal */

        pcb->state = W4_L2CAP_CONNECT_RSP;
    }

    L2CAP_REG(&l2cap_active_pcbs, pcb);

    return ret;
}


err_t l2cap_config_req(l2cap_pcb_t *pcb)
{
    struct bt_pbuf_t *p, *q;
    l2cap_cfgopt_hdr_t *opthdr;
    err_t ret;

    switch(pcb->state)
    {
    case L2CAP_OPEN:
        BT_L2CAP_TRACE_DEBUG("l2cap_config_req: state = L2CAP_OPEN. Suspend transmission\n");
        /* Note: Application should have suspended data transmission, otherwise outgoing data will be
        dropped */
        pcb->state = L2CAP_CONFIG;
    /* Fallthrough */
    case L2CAP_CONFIG:
        BT_L2CAP_TRACE_DEBUG("l2cap_config_req: state = L2CAP_CONFIG\n");

        if((p = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFG_REQ_SIZE, BT_PBUF_RAM)) == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM;
        }

        /* Assemble config request packet. Only options that has to be changed will be
           sent */
        ((uint16_t *)p->payload)[0] = pcb->dcid;
        /* In this implementation we do not send multiple cmds in one
           signal packet. Therefore we will never send a config_req packet
           that will cause the signal to be larger than the minimum L2CAP MTU
           48 bytes. Hence, this flag will always be cleared */
        ((uint16_t *)p->payload)[1] = 0;

        /* Add MTU and out flush timeout to cfg packet if not default value. QoS (Best effort) is always
           set to default and can be skipped */
        if(pcb->cfg.inmtu != L2CAP_CFG_DEFAULT_INMTU)
        {
            if((q = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFGOPTHDR_LEN + L2CAP_MTU_LEN, BT_PBUF_RAM)) == NULL)
            {
                bt_pbuf_free(p);
                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                return BT_ERR_MEM;
            }
            opthdr = q->payload;
            opthdr->type = L2CAP_CFG_MTU;
            opthdr->len = L2CAP_MTU_LEN;
            ((uint16_t *)q->payload)[1] = pcb->cfg.inmtu;
            bt_pbuf_chain(p, q);
            bt_pbuf_free(q);
        }

        if(L2CAP_OUT_FLUSHTO != L2CAP_CFG_DEFAULT_OUTFLUSHTO)
        {
            if((q = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CFGOPTHDR_LEN + L2CAP_FLUSHTO_LEN, BT_PBUF_RAM)) == NULL)
            {
                bt_pbuf_free(p);
                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
                return BT_ERR_MEM;
            }
            opthdr = q->payload;
            opthdr->type = L2CAP_FLUSHTO;
            opthdr->len = L2CAP_FLUSHTO_LEN;
            ((uint16_t *)q->payload)[1] = pcb->cfg.outflushto = L2CAP_OUT_FLUSHTO;
            bt_pbuf_chain(p, q);
            bt_pbuf_free(q);
        }

        /* Send config request signal */
        ret = _l2cap_signal_send(pcb, L2CAP_CFG_REQ, 0, &(pcb->remote_bdaddr), p);
        break;
    default:
        BT_L2CAP_TRACE_DEBUG("l2cap_config_req: state = L2CAP_?. Invalid state\n");
        return BT_ERR_CONN; /* Invalid state. Connection is not in OPEN or CONFIG state */
    }
    return ret;
}

err_t l2cap_disconnect_req(l2cap_pcb_t *pcb, l2ca_disconnect_cfm_cb l2ca_disconnect_cfm)
{
    struct bt_pbuf_t *data;
    err_t ret;

    if(pcb->state == L2CAP_OPEN || pcb->state == L2CAP_CONFIG)
    {
        if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_DISCONN_REQ_SIZE, BT_PBUF_RAM)) == NULL)
        {
            BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
            return BT_ERR_MEM;
        }
        pcb->l2ca_disconnect_cfm = l2ca_disconnect_cfm;

        ((uint16_t *)data->payload)[0] = pcb->dcid;
        ((uint16_t *)data->payload)[1] = pcb->scid;

        ret = _l2cap_signal_send(pcb, L2CAP_DISCONN_REQ, 0, &(pcb->remote_bdaddr), data);

        if(ret == BT_ERR_OK)
        {
            pcb->state = W4_L2CAP_DISCONNECT_RSP;
        }
    }
    else
    {
        return BT_ERR_CONN; /* Signal not supported in this state */
    }

    return ret;
}

err_t l2cap_datawrite(l2cap_pcb_t *pcb, struct bt_pbuf_t *p)
{
    err_t ret;
    l2cap_hdr_t *l2caphdr;
    struct bt_pbuf_t *q;

    if(pcb->state != L2CAP_OPEN)
    {
        BT_L2CAP_TRACE_DEBUG("l2cap_datawrite: State != L2CAP_OPEN. Dropping data\n");
        return BT_ERR_CONN;
    }

    /* Build L2CAP header */
    if((q = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_HDR_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    bt_pbuf_chain(q, p);

    l2caphdr = q->payload;
    l2caphdr->cid = pcb->dcid;

    /* If length of the data exceeds the OutMTU then only the first OutMTU bytes are sent */
    if(p->tot_len > pcb->cfg.outmtu)
    {
        /* Send peer L2CAP data */
        l2caphdr->len = pcb->cfg.outmtu;
        if((ret = l2cap_write(&(pcb->remote_bdaddr), q, pcb->cfg.outmtu + L2CAP_HDR_LEN)) == BT_ERR_OK)
        {
            BT_L2CAP_TRACE_DEBUG("l2cap_datawrite: Length of data exceeds the OutMTU p->tot_len = %d\n", p->tot_len);
            ret = BT_ERR_BUF; /* Length of data exceeds the OutMTU */
        }
    }
    else
    {
        /* Send peer L2CAP data */
        l2caphdr->len = p->tot_len;
        BT_L2CAP_TRACE_DEBUG("l2cap_datawrite: q->tot_len = %d\n", q->tot_len);
        ret = l2cap_write(&(pcb->remote_bdaddr), q, q->tot_len);
    }

    /* Free L2CAP header. Higher layers will handle rest of packet */
    p = bt_pbuf_dechain(q);
    bt_pbuf_free(q);

    return ret;
}

err_t l2cap_fixed_channel_datawrite(l2cap_pcb_t *pcb, struct bt_pbuf_t *p,uint16_t cid)
{
    err_t ret;
    l2cap_hdr_t *l2caphdr;
    struct bt_pbuf_t *q;


    /* Build L2CAP header */
    if((q = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_HDR_LEN, BT_PBUF_RAM)) == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    bt_pbuf_chain(q, p);

    l2caphdr = q->payload;
    l2caphdr->cid = cid;


    l2caphdr->len = p->tot_len;
    BT_L2CAP_TRACE_DEBUG("l2cap_fixed_channel_datawrite: q->tot_len = %d\n", q->tot_len);
    ret = l2cap_write(&(pcb->remote_bdaddr), q, q->tot_len);

    /* Free L2CAP header. Higher layers will handle rest of packet */
    p = bt_pbuf_dechain(q);
    bt_pbuf_free(q);

    return ret;
}


err_t l2cap_ping(struct bd_addr_t *bdaddr, l2cap_pcb_t *tpcb,l2ca_ping_cb l2ca_ping)
{
    err_t ret;

    if(!hci_is_connected(bdaddr))
    {
        return BT_ERR_CONN;
    }

    bd_addr_set(&(tpcb->remote_bdaddr), bdaddr);
    tpcb->l2ca_ping = l2ca_ping;

    L2CAP_REG(&l2cap_active_pcbs, tpcb);

    ret = _l2cap_signal_send(tpcb, L2CAP_ECHO_REQ, 0, &(tpcb->remote_bdaddr), NULL); /* Send l2cap_echo_req signal */

    return ret;
}

void lp_connect_cfm(struct bd_addr_t *bdaddr, uint8_t encrypt_mode, err_t err)
{
    l2cap_pcb_t *pcb;
    struct bt_pbuf_t *data;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_bdaddr), bdaddr))
        {
            break;
        }
    }
    if(pcb == NULL)
    {
        /* Silently discard */
        BT_L2CAP_TRACE_DEBUG("lp_connect_cfm: Silently discard\n");
    }
    else
    {
        if(err == BT_ERR_OK)
        {
            pcb->encrypt = encrypt_mode;
            /* Send l2cap_conn_req signal if no error */
            if((data = bt_pbuf_alloc(BT_PBUF_RAW, L2CAP_CONN_REQ_SIZE, BT_PBUF_RAM)) != NULL)
            {
                ((uint16_t *)data->payload)[0] = pcb->psm;
                ((uint16_t *)data->payload)[1] = pcb->scid;
                if((ret = _l2cap_signal_send(pcb, L2CAP_CONN_REQ, 0, &(pcb->remote_bdaddr), data)) == BT_ERR_OK)
                {
                    pcb->state = W4_L2CAP_CONNECT_RSP;
                }
                else
                {
                    L2CA_ACTION_CONN_CFM(pcb,L2CAP_CONN_REF_RES,0x0000); /* No resources available? */
                }
                BT_L2CAP_TRACE_DEBUG("lp_connect_cfm: l2cap_conn_req signal sent. err = %d\nPSM = 0x%x\nscid = 0x%x\nencrypt mode = 0x%x\n", err, pcb->psm, pcb->scid, pcb->encrypt);
            }
            else
            {
                BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

                L2CA_ACTION_CONN_CFM(pcb,L2CAP_CONN_REF_RES,0x0000); /* No resources available */
            }
        }
        else
        {
            BT_L2CAP_TRACE_DEBUG("lp_connect_cfm: Connection falied\n");
            L2CA_ACTION_CONN_CFM(pcb,L2CAP_CONN_REF_RES,0x0000); /* No resources available */
        }
    }
}

void lp_connect_ind(struct bd_addr_t *bdaddr)
{
    BT_L2CAP_TRACE_DEBUG("lp_connect_ind\n");

	
}

void lp_disconnect_ind(struct bd_addr_t *bdaddr)
{
    l2cap_pcb_t *pcb, *tpcb;
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    for(pcb = l2cap_active_pcbs; pcb != NULL;)
    {
        tpcb = pcb->next;
        BT_L2CAP_TRACE_DEBUG("lp_disconnect_ind: Find a pcb with a matching Bluetooth address\n");
        /* All PCBs with matching Bluetooth address have been disconnected */
        if(bd_addr_cmp(&(pcb->remote_bdaddr), bdaddr))  // && pcb->state != L2CAP_CLOSED) {
        {
            pcb->state = L2CAP_CLOSED;
            BT_L2CAP_TRACE_DEBUG("lp_disconnect_ind: Notify application\n");
            L2CA_ACTION_DISCONN_IND(pcb,BT_ERR_OK);
        }
        pcb = tpcb;
    }
}

void le_connect_handler(struct bd_addr_t *bdaddr,uint8_t conn_role)
{
	l2cap_pcb_t *pcb;
	for(pcb = l2cap_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(pcb->fixed_cid == L2CAP_ATT_CID)
        {
        	pcb->state = L2CAP_OPEN;
			pcb->conn_role = conn_role;
			bd_addr_set(&(pcb->remote_bdaddr),bdaddr);
			L2CA_ACTION_CONN_IND(pcb, BT_ERR_OK);
        }

		if(pcb->fixed_cid == L2CAP_SM_CID)
        {
        	pcb->state = L2CAP_OPEN;
			pcb->conn_role = conn_role;
			bd_addr_set(&(pcb->remote_bdaddr),bdaddr);
			L2CA_ACTION_CONN_IND(pcb, BT_ERR_OK);
        }
    }
}


static uint8_t _l2cap_next_sigid(void)
{
    ++sigid_nxt;
    if(sigid_nxt == 0)
    {
        sigid_nxt = 1;
    }
    return sigid_nxt;
}



err_t l2cap_register_connect_ind(uint8_t psm,l2ca_connect_ind_cb l2ca_connect_ind)
{
    l2cap_pcb_listen_t *lpcb;

    lpcb = bt_memp_malloc(MEMP_L2CAP_PCB_LISTEN);
    if(lpcb == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }
    lpcb->psm = psm;
    lpcb->l2ca_connect_ind = l2ca_connect_ind;
    lpcb->state = L2CAP_LISTEN;
    L2CAP_REG(&l2cap_listen_pcbs, lpcb);
    return BT_ERR_OK;
}

void l2cap_register_disconnect_ind(l2cap_pcb_t *pcb,l2ca_disconnect_ind_cb l2ca_disconnect_ind)
{
    pcb->l2ca_disconnect_ind = l2ca_disconnect_ind;
}

void l2cap_register_timeout_ind(l2cap_pcb_t *pcb,l2ca_timeout_ind_cb l2ca_timeout_ind)
{
    pcb->l2ca_timeout_ind = l2ca_timeout_ind;
}

void l2cap_register_recv(l2cap_pcb_t *pcb,l2ca_recv_cb l2ca_recv)
{
    pcb->l2ca_recv = l2ca_recv;
}

err_t l2cap_fixed_channel_register_recv(uint16_t cid,
                                        l2ca_connect_ind_cb l2ca_connect_ind,
                                        l2ca_disconnect_ind_cb l2ca_disconnect_ind,
                                        l2ca_recv_cb l2ca_recv)
{
    l2cap_pcb_t *l2cappcb;

    if((l2cappcb = l2cap_new()) == NULL)
    {
        BT_L2CAP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] l2cap_new fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    l2cappcb->fixed_cid = cid;
    l2cappcb->l2ca_connect_ind = l2ca_connect_ind;
    l2cappcb->l2ca_disconnect_ind = l2ca_disconnect_ind;
    l2cappcb->l2ca_recv = l2ca_recv;
    l2cappcb->state = L2CAP_CLOSED;
    L2CAP_REG(&l2cap_active_pcbs, l2cappcb);

    return BT_ERR_OK;
}



