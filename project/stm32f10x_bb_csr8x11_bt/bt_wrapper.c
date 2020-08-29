#include "bt_wrapper.h"
#include "board_wrapper.h"
#include "bt_pbuf.h"
#include "bt_memp.h"
#include "bt_hci.h"
#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"
#include "bt_phybusif_h4.h"
#include "bt_spp.h"
#include "bt_hfp_hf.h"
#include "bt_did.h"
#include "bt_pbap_client.h"
#include "bt_mem.h"
#include "bt_common.h"
#include "bt_config.h"

extern struct phybusif_cb uart_if;

struct link_key_record link_key_instance = {0};
uint8_t eir_data[240]= {0};
bt_cb_t *bt_cb = NULL;
uint16_t bt_profile_mask = 0;

static err_t bt_inquiry_complete(struct hci_pcb_t *pcb,uint16_t result);
static err_t bt_inquiry_result(struct hci_pcb_t *pcb,struct hci_inq_res_t *inqres);
static err_t bt_ass_eir_data()
{
    uint8_t data_pos =0;
    uint8_t len = 0;

    /* 16 bit UUID */
    len = 1;
#if PROFILE_DID_ENABLE
    len += 2;
#endif
#if PROFILE_HFP_ENABLE
    len += 2;
#endif
#if PROFILE_SPP_ENABLE
    len += 2;
#endif
#if PROFILE_A2DP_ENABLE
    len += 2;
#endif
#if PROFILE_AVRCP_ENABLE
    len += 2;
#endif
    eir_data[data_pos++] = len;
    eir_data[data_pos++] = BT_DT_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS;
#if PROFILE_DID_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_PNP_INFORMATION & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_PNP_INFORMATION>>8) & 0xff;
#endif
#if PROFILE_HFP_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_HANDSFREE & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_HANDSFREE>>8) & 0xff;
#endif
#if PROFILE_SPP_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_SERIAL_PORT & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_SERIAL_PORT>>8) & 0xff;
#endif
#if PROFILE_A2DP_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_AUDIO_SINK & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_AUDIO_SINK>>8) & 0xff;
#endif
#if PROFILE_AVRCP_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_AV_REMOTE_CONTROL & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_AV_REMOTE_CONTROL>>8) & 0xff;
#endif

    /* Device ID */
#if PROFILE_DID_ENABLE
    eir_data[data_pos++] = 9;
    eir_data[data_pos++] = BT_DT_DEVICE_ID;
    eir_data[data_pos++] = DID_VENDOR_ID_SOURCE_VALUE & 0xff;
    eir_data[data_pos++] = (DID_VENDOR_ID_SOURCE_VALUE>>8) & 0xff;
    eir_data[data_pos++] = DID_VENDOR_ID_VALUE & 0xff;
    eir_data[data_pos++] = (DID_VENDOR_ID_VALUE>>8) & 0xff;
    eir_data[data_pos++] = DID_PRODUCT_ID_VALUE & 0xff;
    eir_data[data_pos++] = (DID_PRODUCT_ID_VALUE>>8) & 0xff;
    eir_data[data_pos++] = DID_VERSION_ID_VALUE & 0xff;
    eir_data[data_pos++] = (DID_VERSION_ID_VALUE>>8) & 0xff;
#endif

    return 0;
}



static err_t  link_key_req(void *arg,struct bd_addr_t *bdaddr)
{
    printf("link key request,address is :");
    bt_hex_dump(bdaddr->addr,6);
    if(bd_addr_cmp(&(link_key_instance.remote_addr),bdaddr))
    {
        hci_link_key_request_reply(bdaddr,&(link_key_instance.link_key));
    }
    else
    {
        hci_link_key_request_negative_reply(bdaddr);
    }
    return 0;
}

static err_t  link_key_not(void *arg, struct bd_addr_t *bdaddr, uint8_t *key,uint8_t key_type)
{
    printf("link key notification,address is :");
    bt_hex_dump(bdaddr->addr,6);
    printf("link key notification,linkey is :");
    bt_hex_dump(key,16);
    printf("link key notification,key type is :%d\n",key_type);

    bd_addr_set(&(link_key_instance.remote_addr),bdaddr);
    memcpy(&(link_key_instance.link_key),key,16);
    link_key_instance.link_key_type = key_type;
    return 0;
}

err_t bt_stack_worked(void *arg)
{
    printf("bt_stack_worked\r\n");
    bt_ass_eir_data();
    hci_write_eir(eir_data);

    if(bt_cb && bt_cb->bt_init_result)
        bt_cb->bt_init_result(BT_INIT_SUCCESS,bt_profile_mask);

    return 0;
}

#if PROFILE_SPP_ENABLE
void spp_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:spp_connect_set_up,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void spp_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:spp_connect_realease,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void spp_data_ind(struct bd_addr_t *remote_addr,uint8_t *data,uint8_t data_len)
{
    printf("WRAPPER << PROFILE:spp_data_ind,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("data len %d,data is:\n",data_len);
    bt_hex_dump(data,data_len);
}

static spp_cbs_t spp_wrapper_cb =
{
    spp_connect_set_up,
    spp_connect_realease,
    spp_data_ind,
};

#endif

#if PROFILE_HFP_ENABLE
void hfp_hf_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_connect_set_up,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_connect_realease,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_sco_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_sco_connect_set_up,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_sco_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_sco_connect_realease,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_call_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
    switch(value)
    {
    case HFP_CALL_NO_INPORCESS:
        printf("NO CALL IN PROCESS\n");
        break;
    case HFP_CALL_INPORCESS:
        printf("CALL IN PROCESS\n");
        break;
    default:
        break;
    }
}
void hfp_hf_call_setup_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_setup_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
    switch(value)
    {
    case HFP_CALL_NO_CALL:
        printf("HFP_CALL_NO_CALL\n");
        break;
    case HFP_CALL_INCOMING_CALL:
        printf("HFP_CALL_INCOMING_CALL\n");
        break;
    case HFP_CALL_OUTGOING_CALL:
        printf("HFP_CALL_OUTGOING_CALL\n");
        break;
    case HFP_CALL_RALERT_OUTGOING_CALL:
        printf("HFP_CALL_RALERT_OUTGOING_CALL\n");
        break;
    default:
        break;
    }
}
void hfp_hf_call_held_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_held_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
    switch(value)
    {
    case HFP_CALL_NO_CALL_HELD:
        printf("HFP_CALL_NO_CALL_HELD\n");
        break;
    case HFP_CALL_CALL_ACTIVE_HELD:
        printf("HFP_CALL_CALL_ACTIVE_HELD\n");
        break;
    case HFP_CALL_CALL_NO_ACTIVE_HELD:
        printf("HFP_CALL_CALL_NO_ACTIVE_HELD\n");
        break;
    default:
        break;
    }
}
void hfp_hf_signal_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_signal_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_battchg_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_battchg_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_server_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_server_status,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_roam_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_roam_status, value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}
void hfp_hf_network(struct bd_addr_t *remote_addr,uint8_t mode,uint8_t format,uint8_t *operator,uint8_t operator_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_network,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("mode %d,format %d,operator %s,operator_len %d\n",mode,format,operator,operator_len);
}
void hfp_hf_ring(struct bd_addr_t *remote_addr)
{
    printf("WRAPPER << PROFILE:hfp_hf_ring,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}

void hfp_hf_clip(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type)
{
    printf("WRAPPER << PROFILE:hfp_hf_clip, type %d,address is :\n",type);
    bt_hex_dump(remote_addr->addr,6);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_call_waiting(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_waiting, type %d,address is :\n",type);
    bt_hex_dump(remote_addr->addr,6);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_voice_recognition(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_voice_recognition, value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}

void hfp_hf_spk_volume(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_spk_volume,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}

void hfp_hf_mic_volume(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_mic_volume,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}

void hfp_hf_hold_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_hold_status,value %d,address is :\n",value);
    bt_hex_dump(remote_addr->addr,6);
}

void hfp_hf_local_number(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type,uint8_t service)
{
    printf("WRAPPER << PROFILE:hfp_hf_local_number, type %d,service %d address is :\n",type,service);
    bt_hex_dump(remote_addr->addr,6);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_call_list(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t index,uint8_t dir,uint8_t status,uint8_t mode,uint8_t mpty,uint8_t type)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_list, index %d,dir %d status %d mode %d mpty %d type %d address is :\n",index,dir,status,mode,mpty,type);
    bt_hex_dump(remote_addr->addr,6);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_manufacturer_id(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_manufacturer_id, address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("mid len %d,mid is:\n",mid_len);
    bt_hex_dump(mid,mid_len);
}

void hfp_hf_model_id(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_model_id, address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("mid len %d,mid is:\n",mid_len);
    bt_hex_dump(mid,mid_len);
}

void hfp_hf_revision_id(struct bd_addr_t *remote_addr,uint8_t *rid,uint8_t rid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_revision_id, address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("rid len %d,mid is:\n",rid_len);
    bt_hex_dump(rid,rid_len);
}

void hfp_hf_product_id(struct bd_addr_t *remote_addr,uint8_t *pid,uint8_t pid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_product_id, address is :\n");
    bt_hex_dump(remote_addr->addr,6);
    printf("pid len %d,mid is:\n",pid_len);
    bt_hex_dump(pid,pid_len);
}

static hfp_hf_cbs_t hfp_hf_wrapper_cb =
{
    hfp_hf_connect_set_up,
    hfp_hf_connect_realease,
    hfp_hf_sco_connect_set_up,
    hfp_hf_sco_connect_realease,
    hfp_hf_call_status,
    hfp_hf_call_setup_status,
    hfp_hf_call_held_status,
    hfp_hf_signal_status,
    hfp_hf_battchg_status,
    hfp_hf_server_status,
    hfp_hf_roam_status,
    hfp_hf_network,
    hfp_hf_ring,
    hfp_hf_clip,
    hfp_hf_call_waiting,
    hfp_hf_voice_recognition,
    hfp_hf_spk_volume,
    hfp_hf_mic_volume,
    hfp_hf_hold_status,
    hfp_hf_local_number,
    hfp_hf_call_list,
    hfp_hf_manufacturer_id,
    hfp_hf_model_id,
    hfp_hf_revision_id,
    hfp_hf_product_id,
};

#endif

#if PROFILE_PBAP_ENABLE
void pbap_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_connect_set_up,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}
void pbap_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_connect_realease,address is :\n");
    bt_hex_dump(remote_addr->addr,6);
}

static pbap_client_cbs_t pbap_client_wrapper_cb =
{
    pbap_connect_set_up,
    pbap_connect_realease,
};

#endif

uint8_t bt_start(bt_cb_t *cb)
{
#if PROFILE_HFP_ENABLE
    uint16_t hf_feature = HFP_HFSF_EC_NR_FUNCTION |  HFP_HFSF_THREE_WAY_CALLING|
                          HFP_HFSF_CLI_PRESENTATION_CAPABILITY | HFP_HFSF_VOICE_RECOGNITION_FUNCTION |
                          HFP_HFSF_REMOTE_VOLUME_CONTROL |HFP_HFSF_ENHANCED_CALL_STATUS |
                          HFP_HFSF_ENHANCED_CALL_CONTROL | HFP_HFSF_CODEC_NEGOTIATION|
                          HFP_HFSF_HF_INDICATORS |HFP_HFSF_ESCO_S4;
#endif

    bt_cb = cb;
    bt_mem_init();
    bt_memp_init();
    phybusif_open(115200,0);
    /* blueooth stack init */
    hci_init();
    hci_link_key_req(link_key_req);
    hci_link_key_not(link_key_not);
    hci_bt_working(bt_stack_worked);

    l2cap_init();
    sdp_init();
    rfcomm_init();
    /* bluetooth classical profile init */
#if PROFILE_DID_ENABLE
    did_init();
    bt_profile_mask |= BT_PROFILE_DID_PSE_MASK;
#endif
#if PROFILE_SPP_ENABLE
    spp_init(&spp_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_SPP_MASK;
#endif
#if PROFILE_HFP_ENABLE
    hfp_hf_init(hf_feature,HFP_HF_SDP_UNSUPPORT_WBS,&hfp_hf_wrapper_cb);
    //hfp_hf_init(hf_feature,HFP_HF_SDP_SUPPORT_WBS,&hfp_hf_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_HFP_HF_MASK;
#endif
#if PROFILE_PBAP_ENABLE
    pbap_client_init(&pbap_client_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_PBAP_PCE_MASK;
#endif
#if PROFILE_A2DP_ENABLE
    a2dp_sink_init();
    bt_profile_mask |= BT_PROFILE_A2DP_SINK_MASK;
#endif
#if PROFILE_AVRCP_ENABLE
    avrcp_controller_init();
    bt_profile_mask |= BT_PROFILE_AVRCP_CONTROL_MASK;
#endif

    hci_reset();

    phybusif_reset(&uart_if);
    return 0;
}

uint8_t bt_stop(void)
{
    bt_profile_mask = 0;
    bt_cb = NULL;
    hci_reset_all();
    l2cap_reset_all();
    sdp_reset_all();
    rfcomm_reset_all();
    return 0;
}

uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev)
{
    uint32_t lap =  0x9E8B33;    /* GIAC */

    if(bt_cb && bt_cb->bt_inquiry_status)
        bt_cb->bt_inquiry_status(BT_INQUIRY_START);
    hci_inquiry(lap,inquiry_len,max_dev,bt_inquiry_result,bt_inquiry_complete);
    return 0;
}


uint8_t bt_le_inquiry(uint8_t enable)
{
    hci_set_le_scan_enable(enable,0);
    return 0;
}


static err_t bt_inquiry_result(struct hci_pcb_t *pcb,struct hci_inq_res_t *inqres)
{
    if(inqres != NULL)
    {
    	uint8_t dev_type = BT_COD_TYPE_UNKNOW;
        uint16_t cod_dev_service;
        uint16_t cod_dev_major;
        uint16_t cod_dev_minor;
        printf("ires->psrm %d\nires->psm %d\nires->co %d\n", inqres->psrm, inqres->psm, inqres->co);
        printf("ires->bdaddr %02x:%02x:%02x:%02x:%02x:%02x\n",
               inqres->bdaddr.addr[5], inqres->bdaddr.addr[4], inqres->bdaddr.addr[3],
               inqres->bdaddr.addr[2], inqres->bdaddr.addr[1], inqres->bdaddr.addr[0]);
        printf("inqres->rssi %d\n",inqres->rssi);
        printf("inqres->remote_name %s\n",inqres->remote_name);
	printf("cod[0]=(0x%x) cod[1]=(0x%x) cod[2]=(0x%x)\n",inqres->cod[0],inqres->cod[1],inqres->cod[2]);
	
        dev_type = bt_parse_cod(inqres->cod,&cod_dev_service,&cod_dev_major,&cod_dev_minor);
        printf("cod_dev_service(0x%x) cod_dev_major(0x%x) cod_dev_minor(0x%x)\n",cod_dev_service,cod_dev_major,cod_dev_minor);
        if(bt_cb && bt_cb->bt_inquiry_result)
            bt_cb->bt_inquiry_result(&inqres->bdaddr,dev_type,inqres->remote_name);
    }

    return BT_ERR_OK;
}

static err_t bt_inquiry_complete(struct hci_pcb_t *pcb,uint16_t result)
{
    if(bt_cb && bt_cb->bt_inquiry_status)
        bt_cb->bt_inquiry_status(BT_INQUIRY_COMPLETE);
    return BT_ERR_OK;
}

