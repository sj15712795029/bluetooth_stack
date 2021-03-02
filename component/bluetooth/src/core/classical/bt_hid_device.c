

#include "bt_hid_device.h"

#if PROFILE_HID_ENABLE


uint8_t hid_service_record[512] = {0};
static  uint8_t hid_service_record_base[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x0, /* Service record handle attribute */
    SDP_UINT32, 0x00, 0x01, 0x00, 0x01, /*dummy vals, filled in on xmit*/

    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x1, /* Service class ID list attribute */
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x11,0x24,

    SDP_DES_SIZE8, 0x12,
    SDP_UINT16, 0x0, 0x4, /* Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0xd,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_UINT16, 0x0, 0x11,/* HID control */
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x0, 0x11, /* HID UUID */

    SDP_DES_SIZE8, 0x14,
    SDP_UINT16, 0x0, 0xd, /* Additional Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0xf,
    SDP_DES_SIZE8, 0xd,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_UINT16, 0x0, 0x13,/* HID interrupt */
    SDP_DES_SIZE8, 0x3,
    SDP_UUID16, 0x0, 0x11, /* HID UUID */

    SDP_DES_SIZE8, 0x0e,
    SDP_UINT16, 0x0, 0x6, /* LanguageBaseAttributeIDList */
    SDP_DES_SIZE8, 0x9,
    SDP_UINT16, 0x65,0x6e,
    SDP_UINT16, 0x00,0x6a,
    SDP_UINT16, 0x01,0x00,


    SDP_DES_SIZE8, 0x0d,
    SDP_UINT16, 0x0, 0x9, /* BluetoothProfile DescriptorList */
    SDP_DES_SIZE8, 0x8,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x11, 0x24, /* HID UUID */
    SDP_UINT16, 0x01, 0x1, /* version */



    SDP_DES_SIZE8, 0x06,
    SDP_UINT16, 0x2, 0x01, /* HIDParserVersion */
    SDP_UINT16, 0x1, 0x11, /* v1.1.1 */

    SDP_DES_SIZE8, 0x05,
    SDP_UINT16, 0x2, 0x02, /* HIDDeviceSubclass */
    SDP_UINT8, 0x40, /* pointing device & keyboard combo */

    SDP_DES_SIZE8, 0x05,
    SDP_UINT16, 0x2, 0x03, /* HIDCountryCode */
    SDP_UINT8, 0x21,

    SDP_DES_SIZE8, 0x05,
    SDP_UINT16, 0x2, 0x04, /* HIDVirtualCable */
    SDP_UINT8, 0x00,

    SDP_DES_SIZE8, 0x05,
    SDP_UINT16, 0x2, 0x05, /* HIDReconnectInitiate */
    SDP_UINT8, 0x01,

    SDP_DES_SIZE8, 0x05,
    SDP_UINT16, 0x2, 0x0e, /* HIDBootDevice */
    SDP_UINT8, 0x00,

#if 0
    SDP_DES_SIZE8, 0x3d,
    SDP_UINT16, 0x2, 0x06, /* HIDDescriptorList */
    SDP_DES_SIZE8, 0x38,
    SDP_DES_SIZE8, 0x36,
    SDP_DE_TYPE_UINT, 0x22,
    SDP_STR_SIZE8,0x32,
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)

    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)

    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)

    0xc0,                          //   END_COLLECTION
    0xc0                           // END_COLLECTION
#endif

};

hid_cbs_t *hid_cbs;
struct hid_device_pcb_t *hid_device_active_pcbs;  /* List of all active HID device PCBs */
struct hid_device_pcb_t *hid_device_tmp_pcb;


#define HID_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define HID_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(hid_device_tmp_pcb = *pcbs; hid_device_tmp_pcb != NULL; hid_device_tmp_pcb = hid_device_tmp_pcb->next) { \
                               if(hid_device_tmp_pcb->next != NULL && hid_device_tmp_pcb->next == npcb) { \
                                  hid_device_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)



struct hid_device_pcb_t *hid_new(void)
{
    struct hid_device_pcb_t *hid_pcb;

    hid_pcb = bt_memp_malloc(MEMP_HID_PCB);
    if(hid_pcb != NULL)
    {
        memset(hid_pcb, 0, sizeof(struct hid_device_pcb_t));
        return hid_pcb;
    }

    BT_HID_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);
    return NULL;
}

void hid_close(struct hid_device_pcb_t *hid_pcb)
{
    if(hid_pcb != NULL)
    {
        bt_memp_free(MEMP_HID_PCB,hid_pcb);
        hid_pcb = NULL;
    }
}


struct hid_device_pcb_t *hid_get_active_pcb( struct bd_addr_t *bdaddr)
{
    struct hid_device_pcb_t *hid_pcb;
    for(hid_pcb = hid_device_active_pcbs; hid_pcb != NULL; hid_pcb = hid_pcb->next)
    {
        if(bd_addr_cmp(&(hid_pcb->remote_bdaddr),bdaddr))
        {
            break;
        }
    }
    return hid_pcb;
}

static err_t hid_device_send_handshake(struct hid_device_pcb_t *hid_pcb,uint8_t result_code)
{
	struct bt_pbuf_t *p;

    if(!hid_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, 1, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_HID_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

	((uint8_t *)p->payload)[0] = (HID_MSG_TYPE_HANDSHAKE<<4) | result_code;

	l2cap_datawrite(hid_pcb->hid_device_control_l2cappcb, p);
	
	return BT_ERR_OK;
}

static err_t hid_device_disconnect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
	struct hid_device_pcb_t *hid_pcb = hid_get_active_pcb(&l2cap_pcb->remote_bdaddr);
    BT_HID_TRACE_DEBUG("hid_device_disconnect_ind psm 0x%x\n",l2cap_psm(l2cap_pcb));

	if(l2cap_psm(l2cap_pcb) == HID_CONTROL_PSM)
	{
		if(hid_cbs && hid_cbs->hid_connect_realease)
            hid_cbs->hid_connect_realease(&hid_pcb->remote_bdaddr,BT_ERR_OK);
		HID_PCB_RMV(&hid_device_active_pcbs,hid_pcb);   
		hid_close(hid_pcb);
	}
    l2cap_close(l2cap_pcb);
    return BT_ERR_OK;
}

err_t hid_device_input(void *arg, l2cap_pcb_t *l2cap_pcb, struct bt_pbuf_t *p, err_t err)
{
	struct hid_device_pcb_t *hid_pcb = hid_get_active_pcb(&l2cap_pcb->remote_bdaddr);
	uint8_t message_type = (((uint8_t *)p->payload)[0]) >> 4;
	
	switch(message_type)
	{
		case HID_MSG_TYPE_HANDSHAKE:
			break;
		default:
		{
			hid_device_send_handshake(hid_pcb,HID_HANDSHAKE_PARAM_TYPE_ERR_UNSUPPORTED_REQUEST);
			break;
		}
	}

	return BT_ERR_OK;
}


static err_t hid_device_connect_ind(void *arg, l2cap_pcb_t *l2cap_pcb, err_t err)
{
	struct hid_device_pcb_t *hid_pcb;
    BT_HID_TRACE_DEBUG("hid_connect_ind psm 0x%x\n",l2cap_psm(l2cap_pcb));

	if((hid_pcb = hid_get_active_pcb(&l2cap_pcb->remote_bdaddr)) == NULL)
	{
		hid_pcb = (struct hid_device_pcb_t *)hid_new();
        hid_pcb->hid_device_control_l2cappcb = l2cap_pcb;
		memcpy(&hid_pcb->remote_bdaddr,&l2cap_pcb->remote_bdaddr,BD_ADDR_LEN);
        HID_PCB_REG(&hid_device_active_pcbs,hid_pcb);
	}
	else
	{
		hid_pcb->hid_device_interrupt_l2cappcb = l2cap_pcb;
		if(hid_cbs && hid_cbs->hid_connect_set_up)
            hid_cbs->hid_connect_set_up(&hid_pcb->remote_bdaddr,BT_ERR_OK);
	}

    l2cap_register_disconnect_ind(l2cap_pcb, hid_device_disconnect_ind);
    l2cap_register_recv(l2cap_pcb, hid_device_input);
    return BT_ERR_OK;
}


err_t hid_device_init(hid_cbs_t *cb)
{
    l2cap_register_connect_ind(HID_CONTROL_PSM, hid_device_connect_ind);
    l2cap_register_connect_ind(HID_INTERRUPT_PSM, hid_device_connect_ind);

	hid_cbs = cb;

    return BT_ERR_OK;
}

err_t hid_device_sdp_init(uint8_t *descriptor,uint16_t descriptor_len)
{
    uint16_t hid_sdp_len = 0;
    sdp_record_t *record;
    uint16_t hid_sdp_base_size = sizeof(hid_service_record_base);
    uint32_t hid_record_hdl = sdp_next_rhdl();

    memcpy(hid_service_record,hid_service_record_base,hid_sdp_base_size);
    hid_sdp_len += hid_sdp_base_size;
    hid_service_record[hid_sdp_len++] = SDP_DES_SIZE8;
    hid_service_record[hid_sdp_len++] = descriptor_len+11;
    hid_service_record[hid_sdp_len++] = SDP_UINT16;
    hid_service_record[hid_sdp_len++] = 0x02;
    hid_service_record[hid_sdp_len++] = 0x06;
    hid_service_record[hid_sdp_len++] = SDP_DES_SIZE8;
    hid_service_record[hid_sdp_len++] = descriptor_len+6;
    hid_service_record[hid_sdp_len++] = SDP_DES_SIZE8;
    hid_service_record[hid_sdp_len++] = descriptor_len+4;
    hid_service_record[hid_sdp_len++] = SDP_DE_TYPE_UINT;
    hid_service_record[hid_sdp_len++] = 0x22;
    hid_service_record[hid_sdp_len++] = SDP_STR_SIZE8;
    hid_service_record[hid_sdp_len++] = descriptor_len;
    memcpy(hid_service_record+hid_sdp_len,descriptor,descriptor_len);
    descriptor_len += descriptor_len;


    if((record = sdp_record_new((uint8_t *)hid_service_record,hid_sdp_len,hid_record_hdl)) == NULL)
    {
        BT_HID_TRACE_DEBUG("did_init: Could not alloc SDP record\n");
        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)hid_service_record,sizeof(hid_service_record));

	return BT_ERR_OK;
	
}

err_t hid_device_interupt_report(struct bd_addr_t *remote_addr,uint8_t *report,uint8_t report_size)
{
	struct bt_pbuf_t *p;
    struct hid_device_pcb_t *hid_pcb = hid_get_active_pcb(remote_addr);

    if(!hid_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, report_size+1, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_HID_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

	((uint8_t *)p->payload)[0] = (HID_MSG_TYPE_DATA<<4) | HID_DATA_TYPE_INPUT;
    memcpy(((uint8_t *)p->payload) + 1, report, report_size);

	l2cap_datawrite(hid_pcb->hid_device_interrupt_l2cappcb, p);
	
	return BT_ERR_OK;

}

#endif

