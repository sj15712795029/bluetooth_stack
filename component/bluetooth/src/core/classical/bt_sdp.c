/******************************************************************************
  * @file           bt_sdo.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt sdp source file
******************************************************************************/

#include "bt_sdp.h"
#include "bt_common.h"
#include "bt_pbuf.h"

/* Next service record handle to be used */
uint32_t rhdl_next;

/* Next transaction id to be used */
uint16_t tid_next;

/* The SDP PCB lists */
sdp_pcb_t *sdp_pcbs;
sdp_pcb_t *sdp_tmp_pcb;

/* List of all active service records in the SDP server */
sdp_record_t *sdp_server_records;
sdp_record_t *sdp_tmp_record; /* Only used for temp storage */

/* Bluetooth Base UUID: 00000000-0000-1000-8000- 00805F9B34FB */
const uint8_t sdp_base_uuid[] = { 0x00, 0x00, 0x00, 0x00, /* - */ 0x00, 0x00, /* - */ 0x10, 0x00, /* - */
                               0x80, 0x00, /* - */ 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
                             };

#define SDP_ACTION_SERVICE_SEARCHED(pcb,tot_src,curr_src,rhdls) if((pcb)->service_searched != NULL) ((pcb)->service_searched((pcb)->callback_arg,(pcb),(tot_src),(curr_src),(rhdls)))
#define SDP_ACTION_ATTRIB_RECV(pcb,attribl_bc,p) if((pcb)->attributes_recv != NULL) ((pcb)->attributes_recv((pcb)->callback_arg,(pcb),(attribl_bc),(p)))
#define SDP_ACTION_ATTRIB_SEARCHED(pcb,attribl_bc,p) if((pcb)-> attributes_searched != NULL) ((pcb)->attributes_searched((pcb)->callback_arg,(pcb),(attribl_bc),(p)))


#define SDP_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define SDP_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(sdp_tmp_pcb = *pcbs; sdp_tmp_pcb != NULL; sdp_tmp_pcb = sdp_tmp_pcb->next) { \
                               if(sdp_tmp_pcb->next != NULL && sdp_tmp_pcb->next == npcb) { \
                                  sdp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)
#define SDP_RECORD_REG(records, record) do { \
                                        record->next = *records; \
                                        *records = record; \
                                        } while(0)
#define SDP_RECORD_RMV(records, record) do { \
                            if(*records == record) { \
                               *records = (*records)->next; \
                            } else for(sdp_tmp_record = *records; sdp_tmp_record != NULL; sdp_tmp_record = sdp_tmp_record->next) { \
                               if(sdp_tmp_record->next != NULL && sdp_tmp_record->next == record) { \
                                  sdp_tmp_record->next = record->next; \
                                  break; \
                               } \
                            } \
                            record->next = NULL; \
                            } while(0)


static err_t sdp_connect_ind(void *arg, l2cap_pcb_t *pcb, err_t err);
static err_t sdp_disconnect_ind(void *arg, l2cap_pcb_t *pcb, err_t err);

static err_t sdp_connect_ind(void *arg, l2cap_pcb_t *pcb, err_t err)
{
    BT_SDP_TRACE_DEBUG("sdp_connect_ind psm 0x%x\n",pcb->psm);


    l2cap_register_recv(pcb, sdp_recv);
    l2cap_register_disconnect_ind(pcb, sdp_disconnect_ind);
    return BT_ERR_OK;
}

static err_t sdp_disconnect_ind(void *arg, l2cap_pcb_t *pcb, err_t err)
{
    err_t ret = BT_ERR_OK;
    BT_UNUSED_ARG(ret);
    BT_SDP_TRACE_DEBUG("sdp_disconnect_ind\n");

    if(pcb->psm == SDP_PSM)
    {
        sdp_lp_disconnected(pcb);
    }

    l2cap_close(pcb);
    return BT_ERR_OK;
}

static void sdp_uuid16_to_uuid128(uint8_t *uuid, uint32_t shortUUID)
{
    memcpy(uuid, sdp_base_uuid, 16);
    bt_be_store_32(uuid, 0, shortUUID);
}

void sdp_init(void)
{
    /* Clear globals */
    sdp_server_records = NULL;
    sdp_tmp_record = NULL;

    /* Inialize service record handles */
    rhdl_next = 0x0000FFFF;

    /* Initialize transaction ids */
    tid_next = 0x0000;

    l2cap_register_connect_ind(SDP_PSM,sdp_connect_ind);
}

uint32_t sdp_next_rhdl(void)
{
    ++rhdl_next;
    if(rhdl_next == 0)
    {
        rhdl_next = 0x0000FFFF;
    }
    return rhdl_next;
}

sdp_record_t *sdp_record_new(uint8_t *record_de_list, uint8_t rlen,uint32_t hdl)
{
    sdp_record_t *record;

    record = bt_memp_malloc(MEMP_SDP_RECORD);
    if(record != NULL)
    {
        record->hdl = hdl;
        record->record_de_list = record_de_list;
        record->len = rlen;
        return record;
    }

	BT_SDP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void sdp_record_free(sdp_record_t *record)
{
    bt_memp_free(MEMP_SDP_RECORD, record);
}


err_t sdp_register_service(sdp_record_t *record)
{
    if(record == NULL)
    {
        return BT_ERR_ARG;
    }
    SDP_RECORD_REG(&sdp_server_records, record);
    return BT_ERR_OK;
}
void sdp_unregister_service(sdp_record_t *record)
{
    SDP_RECORD_RMV(&sdp_server_records, record);
}

uint16_t sdp_next_transid(void)
{
    ++tid_next;
    return tid_next;
}

uint8_t sdp_pattern_search(sdp_record_t *record, uint8_t size, struct bt_pbuf_t *p)
{
    uint8_t i, j;
    uint8_t *payload = (uint8_t *)p->payload;

    BT_SDP_TRACE_DEBUG("sdp_pattern_search size %d\n",size);
    bt_hex_dump(payload,size);
    BT_SDP_TRACE_DEBUG("record de list size %d\n",record->len);
    bt_hex_dump(record->record_de_list,record->len);
    for(i = 0; i < size; ++i)
    {
        if(SDP_DE_TYPE(payload[i]) == SDP_DE_TYPE_UUID)
        {
            switch(SDP_DE_SIZE(payload[i]))
            {
            case SDP_DE_SIZE_16:
                for(j = 0; j < record->len; ++j)
                {
                    if(SDP_DE_TYPE(record->record_de_list[j]) == SDP_DE_TYPE_UUID)
                    {
                        if(*((uint16_t *)(payload + i + 1)) == *((uint16_t *)(record->record_de_list + j + 1)))
                        {
                            return 1; /* Found a matching UUID in record */
                        }
                        ++j;
                    }
                }
                i += 2;
                break;
            case SDP_DE_SIZE_32:
                i += 4;
                break;
            case SDP_DE_SIZE_128:
                for(j = 0; j < record->len; ++j)
                {
                    if(SDP_DE_TYPE(record->record_de_list[j]) == SDP_DE_TYPE_UUID)
                    {
                        /* Handle uuid16 to uuid128 */
                        if(SDP_DE_SIZE(record->record_de_list[j]) == SDP_DE_SIZE_16)
                        {
                            uint8_t uuid128[16] = {0};
                            uint16_t uuid16 = bt_be_read_16(record->record_de_list,j+1);
                            sdp_uuid16_to_uuid128(uuid128,uuid16);
                            if(!memcmp(payload + i + 1,uuid128,16))
                                return 1; /* Found a matching UUID in record */
                        }
                        else if(SDP_DE_SIZE(record->record_de_list[j]) == SDP_DE_SIZE_128)
                        {
                            if(!memcmp(payload + i + 1,record->record_de_list + j + 1,16))
                                return 1; /* Found a matching UUID in record */
                        }
                        ++j;
                    }
                }
                i += 16;

                break;
            default:
                break;
            }
        }
    }
    return 0;
}

struct bt_pbuf_t *sdp_attribute_search(uint16_t max_attribl_bc, struct bt_pbuf_t *p, sdp_record_t *record)
{
    struct bt_pbuf_t *q = NULL;
    struct bt_pbuf_t *r;
    struct bt_pbuf_t *s = NULL;
    uint8_t *payload = (uint8_t *)p->payload;
    uint8_t size;
    uint8_t i = 0, j;
    uint16_t attr_id = 0, attr_id2 = 0;

    uint16_t attribl_bc = 0; /* Byte count of the sevice attributes */
    uint32_t hdl = bt_htonl(record->hdl);

    uint8_t attr_len = 0;
    uint16_t attr = 0;

    if(SDP_DE_TYPE(payload[0]) == SDP_DE_TYPE_DES  &&
            SDP_DE_SIZE(payload[0]) == SDP_DE_SIZE_N1)
    {
        /* Get size of attribute ID list */
        size = payload[1]; //TODO: correct to assume only one size byte in remote request? probably

        while(i < size)
        {
            /* Check if this is an attribute ID or a range of attribute IDs */
            if(payload[2+i] == (SDP_DE_TYPE_UINT  | SDP_DE_SIZE_16))
            {
            	attr_id = *(payload + 3 + i) << 8 | *(payload + 4 + i);
                attr_id2 = attr_id; /* For the range to cover this attribute ID only */
                i += 3;
            }
            else if(payload[2+i] == (SDP_DE_TYPE_UINT | SDP_DE_SIZE_32))
            {
            	/* attr range is big-endian */
				attr_id = *(payload + 3 + i) << 8 | *(payload + 4 + i);
                attr_id2 = *(payload + 5 + i) << 8 | *(payload + 6 + i);
                i += 5;
            }
            else
            {
                /* ERROR: Invalid req syntax */
                //TODO
            }

            for(j = 0; j < record->len; ++j)
            {
                if(record->record_de_list[j] == SDP_DES_SIZE8)
                {
                    if(record->record_de_list[j + 2] == SDP_UINT16)
                    {
                    	attr_len = record->record_de_list[j + 1]; /* attr len  */
                    	/* attr is big-endian */
                    	attr = *(record->record_de_list + j + 3) << 8 | *(record->record_de_list + j + 4);

                        if(attr >= attr_id && attr <= attr_id2)
                        {
                            if(attribl_bc +  attr_len + 2 > max_attribl_bc)
                            {
                                /* Abort attribute search since attribute list byte count must not
                                   exceed max attribute byte count in req */
                                break;
                            }
                            /* Allocate a bt_pbuf_t for the service attribute */
                            r = bt_pbuf_alloc(BT_PBUF_RAW, attr_len, BT_PBUF_RAM);
                            memcpy((uint8_t *)r->payload, record->record_de_list + j + 2, r->len);
                            attribl_bc += r->len;

                            /* If request included a service record handle attribute id, add the correct id to the
                            response */
                            if(attr == 0)
                            {
                                memcpy(((uint8_t *)r->payload) + 4, &hdl, 4);
                            }

                            /* Add the attribute to the service attribute list */
                            if(s == NULL)
                            {
                                s = r;
                            }
                            else
                            {
                                bt_pbuf_chain(s, r);
                                bt_pbuf_free(r);
                            }
                        }

                        j += attr_len + 1; /*  offset to next attribute block */
                    }
                }
            } /* for */
        } /* while */
    }
    else
    {
        /* ERROR: Invalid req syntax */
        BT_SDP_TRACE_DEBUG("sdp_attribute_search: Invalid req syntax");
    }
    /* Return service attribute list */
    if(s != NULL)
    {
        q = bt_pbuf_alloc(BT_PBUF_RAW, 2, BT_PBUF_RAM);
        ((uint8_t *)q->payload)[0] = SDP_DE_TYPE_DES | SDP_DE_SIZE_N1;
        ((uint8_t *)q->payload)[1] = s->tot_len;
        bt_pbuf_chain(q, s);
        bt_pbuf_free(s);
    }

    return q;
}

sdp_pcb_t *sdp_new(l2cap_pcb_t *l2cappcb)
{
    sdp_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_SDP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(sdp_pcb_t));
        pcb->l2cappcb = l2cappcb;
        return pcb;
    }

	BT_SDP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void sdp_free(sdp_pcb_t *pcb)
{
    bt_memp_free(MEMP_SDP_PCB, pcb);
    pcb = NULL;
}

void sdp_deinit(void)
{
    sdp_pcb_t *pcb, *tpcb;
    sdp_record_t *record, *trecord;

    for(pcb = sdp_pcbs; pcb != NULL;)
    {
        tpcb = pcb->next;
        SDP_RMV(&sdp_pcbs, pcb);
        sdp_free(pcb);
        pcb = tpcb;
    }

    for(record = sdp_server_records; record != NULL;)
    {
        trecord = record->next;
        sdp_unregister_service(record);
        sdp_record_free(record);
        record = trecord;
    }

}

void sdp_arg(sdp_pcb_t *pcb, void *arg)
{
    pcb->callback_arg = arg;
}

void sdp_lp_disconnected(l2cap_pcb_t *l2cappcb)
{
    sdp_pcb_t *pcb, *tpcb;

    pcb = sdp_pcbs;
    while(pcb != NULL)
    {
        tpcb = pcb->next;
        if(bd_addr_cmp(&(l2cappcb->remote_bdaddr), &(pcb->l2cappcb->remote_bdaddr)))
        {
            /* We do not need to notify upper layer, free PCB */
            sdp_free(pcb);
        }
        pcb = tpcb;
    }
}

err_t sdp_service_search_req(sdp_pcb_t *pcb, uint8_t *ssp, uint8_t ssplen, uint16_t max_src,sdp_service_searched_cb service_searched)
{
    struct bt_pbuf_t *p;
    sdp_hdr_t *sdphdr;

    /* Update PCB */
    pcb->tid = sdp_next_transid(); /* Set transaction id */

    /* Allocate packet for PDU hdr + service search pattern + max service record count +
       continuation state */
    p = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN+ssplen+2+1, BT_PBUF_RAM);
    sdphdr = p->payload;
    /* Add PDU header to packet */
    sdphdr->pdu = SDP_SS_PDU;
    sdphdr->id = bt_htons(pcb->tid);
    sdphdr->len = bt_htons(ssplen + 3); /* Seq descr + ServiceSearchPattern + MaxServiceRecCount + ContState */

    /* Add service search pattern to packet */
    memcpy(((uint8_t *)p->payload) + SDP_PDUHDR_LEN, ssp, ssplen);

    /* Add maximum service record count to packet */
    *((uint16_t *)(((uint8_t *)p->payload) + ssplen + SDP_PDUHDR_LEN)) = bt_htons(max_src);

    ((uint8_t *)p->payload)[SDP_PDUHDR_LEN+ssplen+2] = 0; /* No continuation */

    /* Update PCB */
    pcb->service_searched = service_searched; /* Set callback */
    SDP_REG(&sdp_pcbs, pcb); /* Register request */

    return l2cap_datawrite(pcb->l2cappcb, p);
}

err_t sdp_service_attrib_req(sdp_pcb_t *pcb, uint32_t srhdl, uint16_t max_abc, uint8_t *attrids, uint8_t attrlen,sdp_attributes_recv_cb attributes_recv)
{
    sdp_hdr_t *sdphdr;
    uint8_t *payload;
    struct bt_pbuf_t *p;

    /* Allocate packet for PDU hdr + service rec hdl + max attribute byte count +
       attribute id data element sequense lenght  + continuation state */
    p = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN + attrlen + 7, BT_PBUF_RAM);

    /* Update PCB */
    pcb->tid = sdp_next_transid(); /* Set transaction id */

    /* Add PDU header to packet */
    sdphdr = p->payload;
    sdphdr->pdu = SDP_SA_PDU;
    sdphdr->id = bt_htons(pcb->tid);
    sdphdr->len = bt_htons((attrlen + 7)); /* Service rec hdl + Max attrib B count + Seq descr + Attribute sequence + ContState */

    payload = p->payload;

    /* Add service record handle to packet */
    *((uint32_t *)(payload + SDP_PDUHDR_LEN)) = bt_htonl(srhdl);

    /* Add maximum attribute count to packet */
    *((uint16_t *)(payload + SDP_PDUHDR_LEN + 4)) = bt_htons(max_abc);

    /* Add attribute id data element sequence to packet */
    memcpy(payload + SDP_PDUHDR_LEN + 6, attrids, attrlen);

    payload[SDP_PDUHDR_LEN + 6 + attrlen] = 0x00; /* No continuation */

    /* Update PCB */
    pcb->attributes_recv = attributes_recv; /* Set callback */
    SDP_REG(&sdp_pcbs, pcb); /* Register request */

    return l2cap_datawrite(pcb->l2cappcb, p);
}

err_t sdp_service_search_attrib_req(sdp_pcb_t *pcb, uint16_t max_abc, uint8_t *ssp, uint8_t ssplen, uint8_t *attrids,uint8_t attrlen, sdp_attributes_searched_cb attributes_searched)
{
    sdp_hdr_t *sdphdr;

    struct bt_pbuf_t *p;
    uint8_t *payload;
    uint16_t pbuf_bc = 0;
    BT_UNUSED_ARG(pbuf_bc);
    /* Allocate packet for PDU hdr + service search pattern + max attribute byte count +
       attribute id list + continuation state */
    p = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN+ssplen+2+attrlen+1, BT_PBUF_RAM);

    /* Update PCB */
    pcb->tid = sdp_next_transid(); /* Set transaction id */

    /* Add PDU header to packet */
    sdphdr = p->payload;
    sdphdr->pdu = SDP_SSA_PDU;
    sdphdr->id = bt_htons(pcb->tid);
    sdphdr->len = bt_htons(ssplen + 2 + attrlen + 1);

    pbuf_bc = SDP_PDUHDR_LEN;
    payload = (uint8_t *)p->payload;

    /* Add service search pattern to packet */
    memcpy(((uint8_t *)p->payload) + SDP_PDUHDR_LEN, ssp, ssplen);

    /* Add maximum attribute count to packet */
    *((uint16_t *)(payload + SDP_PDUHDR_LEN + ssplen)) = bt_htons(max_abc);

    /* Add attribute id data element sequence to packet */
    memcpy(payload + SDP_PDUHDR_LEN + ssplen + 2, attrids, attrlen);

    payload[SDP_PDUHDR_LEN + ssplen + 2 + attrlen] = 0x00; /* No continuation */

    pcb->attributes_searched = attributes_searched; /* Set callback */
    SDP_REG(&sdp_pcbs, pcb); /* Register request */

    return l2cap_datawrite(pcb->l2cappcb, p);
}

err_t sdp_service_search_rsp(l2cap_pcb_t *pcb, struct bt_pbuf_t *p, sdp_hdr_t *reqhdr)
{
    sdp_record_t *record;
    sdp_hdr_t *rsphdr;

    struct bt_pbuf_t *q; /* response packet */
    struct bt_pbuf_t *r; /* tmp buffer */

    uint16_t max_src = 0;
    uint16_t curr_src = 0;
    uint16_t tot_src = 0;

    uint8_t size = 0;

    err_t ret;

    if(SDP_DE_TYPE(((uint8_t *)p->payload)[0]) == SDP_DE_TYPE_DES &&
            SDP_DE_SIZE(((uint8_t *)p->payload)[0]) ==  SDP_DE_SIZE_N1)
    {
        /* Size of the search pattern must be in the next byte since only
           12 UUIDs are allowed in one pattern */
        size = ((uint8_t *)p->payload)[1];

        /* Get maximum service record count that follows the service search pattern */
        max_src = bt_ntohs(*((uint16_t *)(((uint8_t *)p->payload)+(2+size))));

        bt_pbuf_header(p, -2);
    }
    else
    {
        //TODO: INVALID SYNTAX ERROR
    }

    /* Allocate header + Total service rec count + Current service rec count  */
    q  = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN+4, BT_PBUF_RAM);

    rsphdr = q->payload;
    rsphdr->pdu = SDP_SSR_PDU;
    rsphdr->id = reqhdr->id;

    for(record = sdp_server_records; record != NULL; record = record->next)
    {
        /* Check if service search pattern matches record */
        if(sdp_pattern_search(record, size, p))
        {
            if(max_src > 0)
            {
                /* Add service record handle to packet */
                r = bt_pbuf_alloc(BT_PBUF_RAW, 4, BT_PBUF_RAM);
                *((uint32_t *)r->payload) = bt_htonl(record->hdl);
                bt_pbuf_chain(q, r);
                bt_pbuf_free(r);
                --max_src;
                ++curr_src;
            }
            ++tot_src;
        }
    }

    /* Add continuation state to packet */
    r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM);
    ((uint8_t *)r->payload)[0] = 0x00;
    bt_pbuf_chain(q, r);
    bt_pbuf_free(r);

    /* Add paramenter length to header */
    rsphdr->len = bt_htons(q->tot_len - SDP_PDUHDR_LEN);

    /* Add total service record count to packet */
    *((uint16_t *)(((uint8_t *)q->payload) + SDP_PDUHDR_LEN)) = bt_htons(tot_src);

    /* Add current service record count to packet */
    *((uint16_t *)(((uint8_t *)q->payload) + SDP_PDUHDR_LEN + 2)) = bt_htons(curr_src);


    {
        uint16_t i;
        for(r = q; r != NULL; r = r->next)
        {
            for(i = 0; i < r->len; ++i)
            {
                BT_SDP_TRACE_DEBUG("sdp_service_search_rsp: 0x%x\n", ((uint8_t *)r->payload)[i]);
            }
            BT_SDP_TRACE_DEBUG("sdp_service_search_rsp: STOP\n");
        }
    }

    ret = l2cap_datawrite(pcb, q);
    bt_pbuf_free(q);
    return ret;
}


err_t sdp_service_attrib_rsp(l2cap_pcb_t *pcb, struct bt_pbuf_t *p, sdp_hdr_t *reqhdr)
{
    sdp_record_t *record;
    sdp_hdr_t *rsphdr;

    struct bt_pbuf_t *q;
    struct bt_pbuf_t *r;

    uint16_t max_attribl_bc = 0; /* Maximum attribute list byte count */

    err_t ret;

    /* Find record */
    for(record = sdp_server_records; record != NULL; record = record->next)
    {
        if(record->hdl == bt_ntohl(*((uint32_t *)p->payload)))
        {
            break;
        }
    }
    if(record != NULL)
    {
        /* Get maximum attribute byte count */
        max_attribl_bc = bt_ntohs(((uint16_t *)p->payload)[2]);

        /* Allocate rsp packet header + Attribute list count */
        q  = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN+2, BT_PBUF_RAM);
        rsphdr = q->payload;
        rsphdr->pdu = SDP_SAR_PDU;
        rsphdr->id = reqhdr->id;

        /* Search for attributes and add them to a bt_pbuf_t */
        bt_pbuf_header(p, -6);
        r = sdp_attribute_search(max_attribl_bc, p, record);

        if(r != NULL)
        {
            /* Add attribute list byte count length to header */
            *((uint16_t *)(((uint8_t *)q->payload) + SDP_PDUHDR_LEN)) = bt_htons(r->tot_len);
            bt_pbuf_chain(q, r); /* Chain attribute id list for service to response packet */
            bt_pbuf_free(r);
        }
        else
        {
            *((uint16_t *)(((uint8_t *)q->payload) + SDP_PDUHDR_LEN)) = 0;
        }

        /* Add continuation state to packet */
        r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM);
        ((uint8_t *)r->payload)[0] = 0x00; //TODO: Is this correct?
        bt_pbuf_chain(q, r);
        bt_pbuf_free(r);

        /* Add paramenter length to header */
        rsphdr->len = bt_htons(q->tot_len - SDP_PDUHDR_LEN);

        {
            uint16_t i;
            for(r = q; r != NULL; r = r->next)
            {
                for(i = 0; i < r->len; ++i)
                {
                    BT_SDP_TRACE_DEBUG("sdp_service_attrib_rsp: 0x%x\n", ((uint8_t *)r->payload)[i]);
                }
                BT_SDP_TRACE_DEBUG("sdp_service_attrib_rsp: STOP\n");
            }
        }

        ret = l2cap_datawrite(pcb, q);
        bt_pbuf_free(q);

        return ret;
    }
    //TODO: ERROR NO SERVICE RECORD MATCHING HANDLE FOUND
    return BT_ERR_OK;
}

err_t sdp_service_search_attrib_rsp(l2cap_pcb_t *pcb, struct bt_pbuf_t *p, sdp_hdr_t *reqhdr)
{
    sdp_record_t *record;
    sdp_hdr_t *rsphdr;

    struct bt_pbuf_t *q; /* response packet */
    struct bt_pbuf_t *r = NULL; /* tmp buffer */
    struct bt_pbuf_t *s = NULL; /* tmp buffer */

    uint16_t max_attribl_bc = 0;
    uint8_t size = 0;

    /* Get size of service search pattern */
    if(SDP_DE_TYPE(((uint8_t *)p->payload)[0]) == SDP_DE_TYPE_DES &&
            SDP_DE_SIZE(((uint8_t *)p->payload)[0]) ==  SDP_DE_SIZE_N1)
    {
        /* Size of the search pattern must be in the next byte since only
           12 UUIDs are allowed in one pattern */
        size = ((uint8_t *)p->payload)[1];

        /* Get maximum attribute byte count that follows the service search pattern */
        max_attribl_bc = bt_ntohs(*((uint16_t *)(((uint8_t *)p->payload)+(2+size))));

        bt_pbuf_header(p, -2);
    }
    else
    {
        //TODO: INVALID SYNTAX ERROR
    }

    /* Allocate header + attribute list count */
    q  = bt_pbuf_alloc(BT_PBUF_RAW, SDP_PDUHDR_LEN + 2, BT_PBUF_RAM);

    rsphdr = q->payload;
    rsphdr->pdu = SDP_SSAR_PDU;
    rsphdr->id = reqhdr->id;

    for(record = sdp_server_records; record != NULL; record = record->next)
    {
        /* Check if service search pattern matches record */
        if(sdp_pattern_search(record, size, p))
        {
            /* Search for attributes and add them to a bt_pbuf_t */
            bt_pbuf_header(p, -(size + 2));
            r = sdp_attribute_search(max_attribl_bc, p, record);

            if(r != NULL)
            {
                if(q->next == NULL)
                {
                    s = bt_pbuf_alloc(BT_PBUF_RAW, 2, BT_PBUF_RAM);
                    bt_pbuf_chain(q, s); /* Chain attribute id list for service to response packet */
                    bt_pbuf_free(s);
                }
                max_attribl_bc -= r->tot_len; /* Calculate remaining number of bytes of attribute
						       data the server is to return in response to the
						       request */
                bt_pbuf_chain(q, r); /* Chain attribute id list for service to response packet */
                bt_pbuf_free(r);
            }
            bt_pbuf_header(p, size + 2);
        }
    }

    /* Add attribute list byte count length and length of all attribute lists in this PDU to packet */
    if(q->next != NULL )
    {
        *((uint16_t *)(((uint8_t *)q->payload) + SDP_PDUHDR_LEN)) = bt_htons(q->tot_len - SDP_PDUHDR_LEN - 2);

        ((uint8_t *)q->next->payload)[0] = 0x35;
        ((uint8_t *)q->next->payload)[1] = q->tot_len - SDP_PDUHDR_LEN - 4;
    }
    else
    {
        struct bt_pbuf_t *temp;
        bt_be_store_16((uint8_t *)q->payload,SDP_PDUHDR_LEN,2);
        temp  = bt_pbuf_alloc(BT_PBUF_RAW, 2, BT_PBUF_RAM);
        ((uint8_t *)temp->payload)[0] = SDP_DES_SIZE8;
        ((uint8_t *)temp->payload)[1] = 0;
        bt_pbuf_chain(q, temp);
        bt_pbuf_free(temp);

    }

    /* Add continuation state to packet */
    if((r = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        BT_SDP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    }
    else
    {
        ((uint8_t *)r->payload)[0] = 0x00; //TODO: Is this correct?
        bt_pbuf_chain(q, r);
        bt_pbuf_free(r);
    }

    /* Add paramenter length to header */
    rsphdr->len = bt_htons(q->tot_len - SDP_PDUHDR_LEN);

    for(r = q; r != NULL; r = r->next)
    {
        uint8_t i;
        for(i = 0; i < r->len; ++i)
        {
            BT_SDP_TRACE_DEBUG("sdp_service_search_attrib_rsp: 0x%x\n", ((uint8_t *)r->payload)[i]);
        }
        BT_SDP_TRACE_DEBUG("sdp_service_search_attrib_rsp: STOP\n");

    }

    return l2cap_datawrite(pcb, q);
}

err_t sdp_recv(void *arg, l2cap_pcb_t *pcb, struct bt_pbuf_t *s, err_t err)
{
    sdp_hdr_t *sdphdr;
    sdp_pcb_t *sdppcb;
    err_t ret = BT_ERR_OK;
    uint16_t i;
    struct bt_pbuf_t *p, *q, *r;

    BT_SDP_TRACE_DEBUG("sdp_recv: in........\n");
    if(s->len != s->tot_len)
    {
        BT_SDP_TRACE_DEBUG("sdp_recv: Fragmented packet received. Reassemble into one buffer\n");
        if((p = bt_pbuf_alloc(BT_PBUF_RAW, s->tot_len, BT_PBUF_RAM)) != NULL)
        {
            i = 0;
            for(r = s; r != NULL; r = r->next)
            {
                memcpy(((uint8_t *)p->payload) + i, r->payload, r->len);
                i += r->len;
            }
            bt_pbuf_free(s);
        }
        else
        {
            BT_SDP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);

            bt_pbuf_free(s);
            return BT_ERR_MEM;
        }
    }
    else
    {
        p = s;
    }

    for(r = p; r != NULL; r = r->next)
    {
        for(i = 0; i < r->len; ++i)
        {
            BT_SDP_TRACE_DEBUG("sdp_recv: 0x%x\n", ((uint8_t *)r->payload)[i]);
        }

        BT_SDP_TRACE_DEBUG("sdp_recv: STOP\n");
    }

    sdphdr = p->payload;
    bt_pbuf_header(p, -SDP_PDUHDR_LEN);

    switch(sdphdr->pdu)
    {
    case SDP_ERR_PDU:

        BT_SDP_TRACE_DEBUG("sdp_recv: Error response 0x%ld\n", bt_ntohs(*((uint16_t *)p->payload)));
        bt_pbuf_free(p);
        break;
    case SDP_SS_PDU: /* Client request */
        BT_SDP_TRACE_DEBUG("sdp_recv: Service search request\n");

        ret = sdp_service_search_rsp(pcb, p, sdphdr);
        bt_pbuf_free(p);
        break;
    case SDP_SSR_PDU: /* Server response */
        BT_SDP_TRACE_DEBUG("sdp_recv: Service search response\n");
        /* Find the original request */
        for(sdppcb = sdp_pcbs; sdppcb != NULL; sdppcb = sdppcb->next)
        {
            if(sdppcb->tid == bt_ntohs(sdphdr->id))
            {
                break; /* Found */
            } /* if */
        } /* for */
        if(sdppcb != NULL)
        {
            /* Unregister the request */
            SDP_RMV(&sdp_pcbs, sdppcb);
            /* Callback function for a service search response */
            SDP_ACTION_SERVICE_SEARCHED(sdppcb, bt_ntohs(((uint16_t *)p->payload)[0]), bt_ntohs(((uint16_t *)p->payload)[1]), ((uint32_t *)p->payload) + 1);
        }
        bt_pbuf_free(p);
        break;
    case SDP_SA_PDU:
        BT_SDP_TRACE_DEBUG("sdp_recv: Service attribute request\n");
        ret = sdp_service_attrib_rsp(pcb, p, sdphdr);
        bt_pbuf_free(p);
        break;
    case SDP_SAR_PDU:
        BT_SDP_TRACE_DEBUG("sdp_recv: Service attribute response\n");

        /* Find the original request */
        for(sdppcb = sdp_pcbs; sdppcb != NULL; sdppcb = sdppcb->next)
        {
            if(sdppcb->tid == bt_ntohs(sdphdr->id))
            {
                /* Unregister the request */
                SDP_RMV(&sdp_pcbs, sdppcb);
                /* If packet is divided into several pbufs we need to merge them */
                if(p->next != NULL)
                {
                    r = bt_pbuf_alloc(BT_PBUF_RAW, p->tot_len, BT_PBUF_RAM);
                    i = 0;
                    for(q = p; q != NULL; q = q->next)
                    {
                        memcpy(((uint8_t *)r->payload)+i, q->payload, q->len);
                        i += q->len;
                    }
                    bt_pbuf_free(p);
                    p = r;
                }
                i = *((uint16_t *)p->payload);
                bt_pbuf_header(p, -2);
                /* Callback function for a service attribute response */
                SDP_ACTION_ATTRIB_RECV(sdppcb, i, p);
            } /* if */
        } /* for */
        bt_pbuf_free(p);
        break;
    case SDP_SSA_PDU:
        BT_SDP_TRACE_DEBUG("sdp_recv: Service search attribute request\n");
        ret = sdp_service_search_attrib_rsp(pcb, p, sdphdr);
        bt_pbuf_free(p);
        break;
    case SDP_SSAR_PDU:
        BT_SDP_TRACE_DEBUG("sdp_recv: Service search attribute response\n");

        /* Find the original request */
        for(sdppcb = sdp_pcbs; sdppcb != NULL; sdppcb = sdppcb->next)
        {
            if(sdppcb->tid == bt_ntohs(sdphdr->id))
            {
                /* Unregister the request */
                SDP_RMV(&sdp_pcbs, sdppcb);
                /* If packet is divided into several pbufs we need to merge them */
                if(p->next != NULL)
                {
                    r = bt_pbuf_alloc(BT_PBUF_RAW, p->tot_len, BT_PBUF_RAM);
                    i = 0;
                    for(q = p; q != NULL; q = q->next)
                    {
                        memcpy(((uint8_t *)r->payload)+i, q->payload, q->len);
                        i += q->len;
                    }
                    bt_pbuf_free(p);
                    p = r;
                }
                i = *((uint16_t *)p->payload);
                bt_pbuf_header(p, -2);
                /* Callback function for a service search attribute response */
                SDP_ACTION_ATTRIB_SEARCHED(sdppcb, i, p);
                break; /* Abort request search */
            } /* if */
        } /* for */
        bt_pbuf_free(p);
        break;
    default:
        //TODO: INVALID SYNTAX ERROR
        break;
    }
    return ret;
}


