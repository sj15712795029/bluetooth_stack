/******************************************************************************
  * @file           bt_spp.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt spp source file
******************************************************************************/


#include "bt_spp.h"

#if PROFILE_SPP_ENABLE


/* TODO:断开连接不上 */
#if 0
static const uint8_t spp_service_record[] =
{
    SDP_DES_SIZE8, 22,
    SDP_UINT16, 0x0, 0x1, /* Service class ID list attribute */
    SDP_DES_SIZE8, 17,
    SDP_UUID128, 0x00,0x00,0x11,0x01,
    0x00,0x00,0x10,0x00,
    0x80,0x00,
    0x00,0x80,0x5f,0x9b,0x34,0xfb,
    SDP_DES_SIZE8, 0x11,
    SDP_UINT16, 0x0, 0x4, /* Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0xc,
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_DES_SIZE8, 0x5,
    SDP_UUID16, 0x0, 0x3, /*RFCOMM*/
    SDP_UINT8, RFCOMM_SPP_SERVER_CHNL, /*RFCOMM channel*/
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x5, /*Browse group list */
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x10, 0x02, /*PublicBrowseGroup*/
};
#endif

static const uint8_t spp_service_record[] =
{
	/* 1.ServiceClassIDList */
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST>>8&0xff, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST&0xff, /* Service class ID list attribute */
            SDP_DES_SIZE8, 3,
            SDP_UUID16, BT_SERVICE_CLASS_SERIAL_PORT>>8&0xff, BT_SERVICE_CLASS_SERIAL_PORT&0xff,

	/* 2.ProtocolDescriptorList */
            SDP_DES_SIZE8, 0x11,
            SDP_UINT16, BT_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST>>8&0xff, BT_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST&0xff,/* Protocol descriptor list attribute */
            SDP_DES_SIZE8, 0xc,
            SDP_DES_SIZE8, 0x3,
            SDP_UUID16, BT_PROTOCOL_L2CAP>>8&0xff, BT_PROTOCOL_L2CAP&0xff, /*L2CAP*/
            SDP_DES_SIZE8, 0x5,
            SDP_UUID16, BT_PROTOCOL_RFCOMM>>8&0xff, BT_PROTOCOL_RFCOMM&0xff, /*RFCOMM*/
            SDP_UINT8, RFCOMM_SPP_SERVER_CHNL, /*RFCOMM channel*/

	/* BluetoothProfileDescriptorList */
            SDP_DES_SIZE8, 0xd,
            SDP_UINT16, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST>>8&0xff, BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST&0xff, /* profile descriptor List */
            SDP_DES_SIZE8, 0x8,
            SDP_DES_SIZE8,0x06,
            SDP_UUID16,BT_SERVICE_CLASS_SERIAL_PORT>>8&0xff, BT_SERVICE_CLASS_SERIAL_PORT&0xff,
            SDP_UINT16,0x01,0x02,		/* V1.2 */
};



spp_cbs_t *spp_cbs;
struct spp_pcb_t *spp_active_pcbs;  /* List of all active SPP PCBs */
struct spp_pcb_t *spp_tmp_pcb;

static struct spp_pcb_t *spp_new(void);
static struct spp_pcb_t *spp_get_active_pcb(struct bd_addr_t *bdaddr);
static void spp_close(struct spp_pcb_t *pcb);
static err_t l2cap_connect_cfm(void *arg, l2cap_pcb_t *l2cappcb, uint16_t result, uint16_t status);
static err_t l2cap_disconnect_cfm(void *arg, l2cap_pcb_t *pcb);
static err_t spp_connect_cfm(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t spp_connect_ind(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t spp_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err);
static err_t spp_recv_data(void *arg, rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err);
static void spp_sdp_attributes_recv(void *arg,sdp_pcb_t *sdppcb, uint16_t attribl_bc, struct bt_pbuf_t *p);
static uint8_t spp_get_rfcomm_cn(uint16_t attribl_bc, struct bt_pbuf_t *attribute_list);
static err_t spp_run(struct spp_pcb_t *pcb);


err_t spp_init(spp_cbs_t *cb)
{
    sdp_record_t *record;
    uint16_t spp_de_size = 0;
    uint32_t spp_record_hdl = sdp_next_rhdl();

    BT_SPP_TRACE_DEBUG("spp_init: Allocate RFCOMM PCB for CN %d\n", RFCOMM_SPP_SERVER_CHNL);

    rfcomm_listen(RFCOMM_SPP_SERVER_CHNL, spp_connect_ind);

    BT_SPP_TRACE_DEBUG("spp_init: spp_de_size %d\n",spp_de_size);

    bt_hex_dump((uint8_t *)spp_service_record,sizeof(spp_service_record));
    if((record = sdp_record_new((uint8_t *)spp_service_record, sizeof(spp_service_record),spp_record_hdl)) == NULL)
    {
        BT_SPP_TRACE_DEBUG("spp_init: Could not alloc SDP record\n");

        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    spp_cbs = cb;
    return BT_ERR_OK;
}

err_t spp_connect(struct bd_addr_t *addr)
{
    struct spp_pcb_t *spppcb;

    if((spppcb = spp_new()) == NULL)
    {
        BT_SPP_TRACE_DEBUG("spp_connect: Could not alloc rfcomm pcb\n");

        return BT_ERR_MEM;
    }

    spppcb->state = SPP_W2_SDP_CONNECTED;

    bd_addr_set(&(spppcb->remote_addr),addr );
    spp_run(spppcb);

    return BT_ERR_OK;
}

err_t spp_disconnect(struct bd_addr_t *addr)
{
    struct spp_pcb_t *spppcb = spp_get_active_pcb(addr);
    if(!spppcb)
        return BT_ERR_CONN;

    spppcb->state = SPP_W2_DISCONNECT_RFCOMM;
    spp_run(spppcb);

    return BT_ERR_OK;
}

err_t spp_send_data(struct bd_addr_t *addr,uint8_t *data,uint16_t data_len)
{
    struct bt_pbuf_t *p = NULL;
    struct spp_pcb_t *spppcb = spp_get_active_pcb(addr);
    if(!spppcb)
        return BT_ERR_CONN;

    p = bt_pbuf_alloc(BT_PBUF_RAW, data_len, BT_PBUF_RAM);
	if(!p)
	{
		BT_SPP_TRACE_ERROR("spp_send_data: Could not alloc spp pbuf\n");
		return BT_ERR_MEM;
	}
    memcpy((uint8_t *)p->payload, data, data_len);
	
    rfcomm_uih(spppcb->rfcommpcb, rfcomm_cn(spppcb->rfcommpcb), p);

    bt_pbuf_free(p);

    return BT_ERR_OK;
}

static struct spp_pcb_t *spp_new(void)
{
    struct spp_pcb_t *pcb;

    pcb = bt_memp_malloc(MEMP_SPP_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(struct spp_pcb_t));
        return pcb;
    }

    BT_SPP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

    return NULL;
}

static struct spp_pcb_t *spp_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct spp_pcb_t *pcb = NULL;
    for(pcb = spp_active_pcbs; pcb != NULL; pcb = pcb->next)
    {
        if(bd_addr_cmp(&(pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return pcb;
}

static void spp_close(struct spp_pcb_t *pcb)
{
    if(pcb != NULL)
    {
        bt_memp_free(MEMP_SPP_PCB, pcb);
        pcb = NULL;
    }
}

static err_t spp_recv_data(void *arg, rfcomm_pcb_t *pcb, struct bt_pbuf_t *p, err_t err)
{
    struct spp_pcb_t *spppcb = spp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(!spppcb)
        return BT_ERR_CONN;

    BT_SPP_TRACE_DEBUG("spp_recv_data: p->len == %d p->tot_len == %d\n", p->len, p->tot_len);

    bt_hex_dump(p->payload,p->len);

    if(spp_cbs && spp_cbs->spp_data_ind)
        spp_cbs->spp_data_ind(&spppcb->remote_addr,p->payload,p->len);
    return BT_ERR_OK;
}

static err_t spp_disconnected(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    struct spp_pcb_t *spppcb;
    BT_SPP_TRACE_DEBUG("spp_disconnected: CN = %d\n", rfcomm_cn(pcb));

    rfcomm_close(pcb);

    spppcb = spp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));
    if(spppcb)
    {
        BT_SPP_TRACE_DEBUG("spp_disconnected: close spp active pcb\n");

        spppcb->state = SPP_W4_RFCOMM_DISCONNECTED_AND_RESTART;
        spp_run(spppcb);

    }
    return BT_ERR_OK;
}

static err_t spp_connect_cfm(void *arg, rfcomm_pcb_t *pcb, err_t err)
{

    struct spp_pcb_t *spppcb = spp_get_active_pcb(&(pcb->l2cappcb->remote_bdaddr));

    if(err == BT_ERR_OK)
    {
        BT_SPP_TRACE_DEBUG("spp_connect_cfm. CN = %d\n", rfcomm_cn(pcb));

        rfcomm_register_disc(pcb, spp_disconnected);
        rfcomm_register_recv(pcb, spp_recv_data);
        spppcb->rfcommpcb = pcb;
        spppcb->state = SPP_SERVER_CN_CONNECTED;
        spp_run(spppcb);
    }
    else
    {
        BT_SPP_TRACE_DEBUG("spp_connect_cfm. Connection attempt failed CN = %d\n", rfcomm_cn(pcb));

        l2cap_close(pcb->l2cappcb);
        rfcomm_close(pcb);
    }
    return BT_ERR_OK;
}

static err_t spp_connect_ind(void *arg, rfcomm_pcb_t *pcb, err_t err)
{
    struct spp_pcb_t *spppcb;
    BT_SPP_TRACE_DEBUG("spp_connect_ind: CN = %d\n", rfcomm_cn(pcb));

    if((spppcb = spp_new()) == NULL)
    {
        BT_SPP_TRACE_DEBUG("spp_connect_ind: Could not alloc rfcomm pcb\n");

        return BT_ERR_MEM;
    }
    bd_addr_set(&(spppcb->remote_addr),&(pcb->l2cappcb->remote_bdaddr));
    SPP_PCB_REG(&spp_active_pcbs, spppcb);
    rfcomm_register_disc(pcb, spp_disconnected);
    if(pcb->cn == RFCOMM_SPP_SERVER_CHNL)
    {
        rfcomm_register_recv(pcb, spp_recv_data);
    }
    spppcb->rfcommpcb = pcb;
    spppcb->state = SPP_SERVER_CN_CONNECTED;
    spp_run(spppcb);
    return BT_ERR_OK;
}


static err_t l2cap_connect_cfm(void *arg, l2cap_pcb_t *l2cappcb, uint16_t result, uint16_t status)
{
    struct spp_pcb_t *spppcb = spp_get_active_pcb(&(l2cappcb->remote_bdaddr));

    if(spppcb)
    {

        if(result == L2CAP_CONN_SUCCESS)
        {
            BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP connected pcb->state = %d,psm %d\n", l2cappcb->state,l2cap_psm(l2cappcb));

            /* Tell L2CAP that we wish to be informed of a disconnection request */
            switch(l2cap_psm(l2cappcb))
            {
            case SDP_PSM:
                BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: SDP L2CAP configured. Result = %d\n", result);

                if((spppcb->sdppcb= sdp_new(l2cappcb)) == NULL)
                {
                    BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a SDP PCB\n");
                    return BT_ERR_MEM;
                }
                spppcb->state = SPP_W2_SDP_QUERY_RFCOMM_CHANNEL;
                spppcb->sdppcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, sdp_recv);
                spp_run(spppcb);
                return BT_ERR_OK;
            case RFCOMM_PSM:
                if((spppcb->rfcommpcb = rfcomm_new(l2cappcb)) == NULL)
                {
                    BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: Failed to create a RFCOMM PCB\n");

                    return BT_ERR_MEM;
                }
                spppcb->rfcommpcb->l2cappcb = l2cappcb;
                l2cap_register_recv(l2cappcb, rfcomm_input);
                spppcb->state = SPP_W2_SERVER_CN_CONNECTED;
                spp_run(spppcb);
                break;
            default:
                return BT_ERR_VAL;
            }
        }
        else
        {
            BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: L2CAP not connected. Redo inquiry\n");
            l2cap_close(l2cappcb);
        }

        return BT_ERR_OK;
    }
    else
    {
        BT_SPP_TRACE_DEBUG("l2cap_connect_cfm: connect fail\n");

        SPP_PCB_RMV(&spp_active_pcbs, spppcb);
        spp_close(spppcb);
        return BT_ERR_CONN;
    }

}

static err_t l2cap_disconnect_cfm(void *arg, l2cap_pcb_t *pcb)
{
    struct spp_pcb_t *spppcb = spp_get_active_pcb(&(pcb->remote_bdaddr));

    if(!spppcb)
        return BT_ERR_CONN;

    BT_SPP_TRACE_DEBUG("l2cap_disconnected_cfm psm 0x%02x\n",l2cap_psm(pcb));


    switch(l2cap_psm(pcb))
    {
    case SDP_PSM:
        spppcb->state = SPP_W2_RFCOMM_CONNECTED;
        spp_run(spppcb);
        break;
    case RFCOMM_PSM:
        SPP_PCB_RMV(&spp_active_pcbs, spppcb);
        rfcomm_close(spppcb->rfcommpcb);
        spp_close(spppcb);
        break;
    default:
        break;
    }
    l2cap_close(pcb);
    return BT_ERR_OK;
}

static void spp_sdp_attributes_recv(void *arg, sdp_pcb_t *sdppcb, uint16_t attribl_bc, struct bt_pbuf_t *p)
{
    struct spp_pcb_t *spppcb = spp_get_active_pcb(&(sdppcb->l2cappcb->remote_bdaddr));

    if(!spppcb)
        return;

    /* Get the RFCOMM channel identifier from the protocol descriptor list */
    if((spppcb->remote_cn = spp_get_rfcomm_cn(attribl_bc, p)) != 0)
    {
        BT_SPP_TRACE_DEBUG("spp_sdp_attributes_recv:remote cn %d\n",spppcb->remote_cn);

    }

    spppcb->state = SPP_W2_SDP_DISCONNECTD;
    spp_run(spppcb);
}

static uint8_t spp_get_rfcomm_cn(uint16_t attribl_bc, struct bt_pbuf_t *attribute_list)
{
    uint8_t i;
    for(i = 0; i < attribl_bc; i++)
    {
        if(((uint8_t *)attribute_list->payload)[i] == (SDP_DE_TYPE_UUID | SDP_DE_SIZE_16))
        {
            if(bt_ntohs(*((uint16_t *)(((uint8_t *)attribute_list->payload)+i+1))) == 0x0003)
            {
                return *(((uint8_t *)attribute_list->payload)+i+4);
            }
        }
    }
    return 0;
}

static err_t spp_run(struct spp_pcb_t *pcb)
{
    BT_SPP_TRACE_DEBUG("spp_run: state %d\n", pcb->state);
    switch(pcb->state)
    {
    case SPP_W2_SDP_CONNECTED:
        SPP_PCB_REG(&spp_active_pcbs, pcb);
        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_SPP_TRACE_DEBUG("spp_run: Could not alloc L2CAP pcb\n");

            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), SDP_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case SPP_W2_SDP_QUERY_RFCOMM_CHANNEL:
    {
        uint8_t spp[] = {0x35, 0x03, 0x19, 0x11, 0x01}; /* Service search pattern with SPP UUID is default */

        uint8_t attrids[] = {0x35, 0x03, 0x09, 0x00, 0x04}; /* Attribute IDs to search for in data element
										sequence form */

        sdp_service_search_attrib_req(pcb->sdppcb, 0xFFFF, spp, sizeof(spp),
                                      attrids, sizeof(attrids), spp_sdp_attributes_recv);
        break;
    }
    case SPP_W2_SDP_DISCONNECTD:
        l2cap_disconnect_req(pcb->sdppcb->l2cappcb, l2cap_disconnect_cfm);
        sdp_free(pcb->sdppcb);
        break;
    case SPP_W2_RFCOMM_CONNECTED:

        if((pcb->l2cappcb = l2cap_new()) == NULL)
        {
            BT_SPP_TRACE_DEBUG("hfp_hf_run: Could not alloc L2CAP pcb\n");
            return BT_ERR_MEM;
        }
        l2cap_connect_req(pcb->l2cappcb, &(pcb->remote_addr), RFCOMM_PSM, HCI_ALLOW_ROLE_SWITCH, l2cap_connect_cfm);
        break;
    case SPP_W2_SERVER_CN_CONNECTED:
        rfcomm_connect(pcb->rfcommpcb, pcb->remote_cn, spp_connect_cfm);
        break;
    case SPP_SERVER_CN_CONNECTED:
        if(spp_cbs && spp_cbs->spp_connect_set_up)
            spp_cbs->spp_connect_set_up(&pcb->remote_addr,BT_ERR_OK);
        break;
    case SPP_W2_DISCONNECT_RFCOMM:
        rfcomm_disconnect(pcb->rfcommpcb);
        //l2cap_disconnect_req(pcb->rfcommpcb->l2cappcb, l2cap_disconnect_cfm);
        break;
    case SPP_W4_RFCOMM_DISCONNECTED_AND_RESTART:
        if(spp_cbs && spp_cbs->spp_connect_realease)
            spp_cbs->spp_connect_realease(&pcb->remote_addr,BT_ERR_OK);
        pcb->state = SPP_IDLE;
        SPP_PCB_RMV(&spp_active_pcbs, pcb);
        spp_close(pcb);
        break;
    default:
        break;
    }

    return BT_ERR_OK;
}

#endif

