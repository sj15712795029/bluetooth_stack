#include "bt_wrapper.h"


extern struct phybusif_cb uart_if;

struct link_key_record link_key_instance = {0};
uint8_t eir_data[240]= {0};
bt_app_cb_t *bt_wrapper_cb = NULL;
uint16_t bt_profile_mask = 0;
uint8_t bt_sco_connected = 0;
uint8_t bt_call_active = 0;

smp_device_info_t le_device_instance = {0};


static err_t bt_inquiry_complete(uint16_t result);
static err_t bt_inquiry_result(hci_inq_res_t *inqres);
#if BT_BLE_ENABLE > 0
static err_t bt_le_inquiry_complete(uint16_t result);
static err_t bt_le_inquiry_result(hci_le_inq_res_t *le_inqres);
static uint8_t bt_le_adv_get_type(bt_le_adv_parse_t *bt_adv_le_parse);
static uint8_t bt_le_adv_get_size(bt_le_adv_parse_t *bt_adv_le_parse);
static uint8_t *bt_le_adv_get_data(bt_le_adv_parse_t *bt_adv_le_parse);
#endif




#if PROFILE_HID_ENABLE > 0
#if HID_DEVICE_MOUSE_ENABLE
static uint8_t hid_device_descriptor[] =
{

    0x05,0x01,	/*USAGE_PAGE (Generic Desktop)*/
    0x09,0x02,	/*USAGE (Mouse)*/
    0xa1,0x01,	/*COLLECTION (Application)*/
    0x09,0x01,	 /*USAGE (Pointer)*/
    0xa1,0x00,	 /*COLLECTION (Physical)*/

    0x05,0x09,  /*USAGE_PAGE (Button)*/
    0x19,0x01,  /*USAGE_MINIMUM (Button 1)*/
    0x29,0x03,  /*USAGE_MAXIMUM (Button 3)*/
    0x15,0x00,  /*LOGICAL_MINIMUM (0)*/
    0x25,0x01,  /*LOGICAL_MAXIMUM (1)*/
    0x95,0x03,  /*REPORT_COUNT (3)*/
    0x75,0x01,  /*REPORT_SIZE (1)*/
    0x81,0x02,  /*INPUT (Data,Var,Abs)*/
    0x95,0x01,  /*REPORT_COUNT (1)*/
    0x75,0x05,  /*REPORT_SIZE (5)*/
    0x81,0x03,  /*INPUT (Cnst,Var,Abs)*/

    0x05, 0x01, /*USAGE_PAGE (Generic Desktop)*/
    0x09, 0x30, /*USAGE (X)*/
    0x09, 0x31, /*USAGE (Y)*/
    0x15, 0x81, /*LOGICAL_MINIMUM (-127)*/
    0x25, 0x7f, /*LOGICAL_MAXIMUM (127)*/
    0x75, 0x08, /*REPORT_SIZE (8)*/
    0x95, 0x02, /*REPORT_COUNT (2)*/
    0x81, 0x06, /*INPUT (Data,Var,Rel)*/

    0xc0,       /*END_COLLECTION*/
    0xc0,        /*END_COLLECTION*/
};
#endif

#if HID_DEVICE_KEYBAORD_ENABLE
#define KEY_UNSUPPORT 0xff
#define KEY_RETURN     '\n'
#define KEY_ESCAPE      ' '
#define KEY_DELETE   0x7f
#define KEY_TAB         '\t'

static uint8_t keyboard_key_map [] =
{
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /*   0-3 */
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',                   /*  4-13 */
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',                   /* 14-23 */
    'u', 'v', 'w', 'x', 'y', 'z',                                       /* 24-29 */
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',                   /* 30-39 */
    KEY_RETURN, KEY_ESCAPE, KEY_DELETE, KEY_TAB, ' ',            /* 40-44 */
    '-', '=', '[', ']', '\\', KEY_UNSUPPORT, ';', '\'', 0x60, ',',       /* 45-54 */
    '.', '/', KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,   /* 55-60 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 61-64 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 65-68 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 69-72 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 73-76 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 77-80 */
    KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT, KEY_UNSUPPORT,             /* 81-84 */
    '*', '-', '+', '\n', '1', '2', '3', '4', '5',                       /* 85-97 */
    '6', '7', '8', '9', '0', '.', 0xa7,                                 /* 97-100 */
};

static uint8_t hid_device_descriptor[] =
{

    0x05, 0x01,              /* Usage Page (Generic Desktop) */
    0x09, 0x06,              /* Usage (Keyboard) */
    0xa1, 0x01,              /* Collection (Application) */

    /* Modifier byte */
    0x75, 0x01,              /*   Report Size (1) */
    0x95, 0x08,              /*   Report Count (8) */
    0x05, 0x07,              /*   Usage Page (Key codes)*/
    0x19, 0xe0,              /*   Usage Minimum (Keyboard LeftControl) */
    0x29, 0xe7,              /*   Usage Maxium (Keyboard Right GUI) */
    0x15, 0x00,              /*   Logical Minimum (0) */
    0x25, 0x01,              /*   Logical Maximum (1) */
    0x81, 0x02,              /*   Input (Data, Variable, Absolute) */

    /* Reserved byte */
    0x75, 0x01,              /*   Report Size (1) */
    0x95, 0x08,              /*   Report Count (8) */
    0x81, 0x03,              /*   Input (Constant, Variable, Absolute) */

    /* LED report + padding */
    0x95, 0x05,              /*   Report Count (5) */
    0x75, 0x01,              /*   Report Size (1) */
    0x05, 0x08,              /*   Usage Page (LEDs) */
    0x19, 0x01,              /*   Usage Minimum (Num Lock) */
    0x29, 0x05,              /*   Usage Maxium (Kana) */
    0x91, 0x02,              /*   Output (Data, Variable, Absolute) */
    0x95, 0x01,              /*   Report Count (1) */
    0x75, 0x03,              /*   Report Size (3) */
    0x91, 0x03,              /*   Output (Constant, Variable, Absolute) */

    /* Keycodes */
    0x95, 0x06,              /*   Report Count (6) */
    0x75, 0x08,              /*   Report Size (8) */
    0x15, 0x00,              /*   Logical Minimum (0) */
    0x25, 0xff,              /*   Logical Maximum (1) */
    0x05, 0x07,              /*   Usage Page (Key codes) */
    0x19, 0x00,              /*   Usage Minimum (Reserved (no event indicated)) */
    0x29, 0xff,              /*   Usage Maxium (Reserved) */
    0x81, 0x00,              /*   Input (Data, Array) */

    0xc0,                    /* End collection   */
};
#endif
#endif

static err_t bt_ass_eir_data(void)
{
    uint8_t data_pos =0;
    uint8_t len = 0;

#if 1
    /* local name */
    len = strlen(BT_LOCAL_NAME);
    eir_data[data_pos++] = len + 1;
    eir_data[data_pos++] = BT_DT_COMPLETE_LOCAL_NAME;
    memcpy(eir_data+data_pos,BT_LOCAL_NAME,strlen(BT_LOCAL_NAME));
    data_pos += strlen(BT_LOCAL_NAME);
#endif

    /* 16 bit UUID */
    len = 1;
#if PROFILE_DID_ENABLE
    len += 2;
#endif
#if PROFILE_HFP_HF_ENABLE
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
#if PROFILE_HID_ENABLE
    len += 2;
#endif

    eir_data[data_pos++] = len;
    eir_data[data_pos++] = BT_DT_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS;
#if PROFILE_DID_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_PNP_INFORMATION & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_PNP_INFORMATION>>8) & 0xff;
#endif
#if PROFILE_HFP_HF_ENABLE
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
#if PROFILE_HID_ENABLE
    eir_data[data_pos++] = BT_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE & 0xff;
    eir_data[data_pos++] = (BT_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE>>8) & 0xff;
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

static err_t bt_inquiry_complete(uint16_t result);
static err_t bt_inquiry_result(hci_inq_res_t *inqres);
static err_t bt_get_remote_name_complete(struct bd_addr_t *bdaddr,uint8_t * name);


static err_t  link_key_req(void *arg,struct bd_addr_t *bdaddr)
{
    printf("link key request,address is :");
    bt_addr_dump(bdaddr->addr);
    if(bd_addr_cmp(&(link_key_instance.remote_addr),bdaddr))
    {
        hci_link_key_request_reply(bdaddr,(uint8_t *)&(link_key_instance.link_key));
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
    bt_addr_dump(bdaddr->addr);
    printf("link key notification,linkey is :");
    bt_hex_dump(key,16);
    printf("link key notification,key type is :%d\n",key_type);

    bd_addr_set(&(link_key_instance.remote_addr),bdaddr);
    memcpy(&(link_key_instance.link_key),key,16);
    link_key_instance.link_key_type = key_type;
    return 0;
}

err_t bt_hardware_error(uint8_t reson)
{
    printf("bt_hardware_error\r\n");

    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_hardware_error)
    {
        bt_wrapper_cb->app_common_cb->bt_hardware_error(reson);
    }

    return 0;
}


#if PROFILE_SPP_ENABLE
void spp_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:spp_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_spp_cb && bt_wrapper_cb->app_spp_cb->bt_spp_connect)
    {
        bt_wrapper_cb->app_spp_cb->bt_spp_connect(remote_addr,status);
    }
}
void spp_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:spp_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_spp_cb && bt_wrapper_cb->app_spp_cb->bt_spp_disconnect)
    {
        bt_wrapper_cb->app_spp_cb->bt_spp_disconnect(remote_addr,status);
    }
}
void spp_data_ind(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len)
{
    printf("WRAPPER << PROFILE:spp_data_ind,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("data len %d,data is:\n",data_len);
    bt_hex_dump(data,data_len);

    if(bt_wrapper_cb && bt_wrapper_cb->app_spp_cb && bt_wrapper_cb->app_spp_cb->bt_spp_recv_data)
    {
        bt_wrapper_cb->app_spp_cb->bt_spp_recv_data(remote_addr,data,data_len);
    }
}

static spp_cbs_t spp_wrapper_cb =
{
    spp_connect_set_up,
    spp_connect_realease,
    spp_data_ind,
};

#endif

#if PROFILE_HFP_HF_ENABLE
void hfp_hf_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_connect)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_connect(remote_addr,status);
    }
}
void hfp_hf_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_disconnect)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_disconnect(remote_addr,status);
    }
}
void hfp_hf_sco_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_sco_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    bt_sco_connected = 1;
    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_sco_connect)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_sco_connect(remote_addr,status);
    }
}
void hfp_hf_sco_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hfp_hf_sco_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    bt_sco_connected = 0;
    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_sco_disconnect)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_sco_disconnect(remote_addr,status);
    }
}
void hfp_hf_call_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_status,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
    switch(value)
    {
    case HFP_CALL_NO_INPORCESS:
        printf("NO CALL IN PROCESS\n");
        bt_call_active = 0;
        break;
    case HFP_CALL_INPORCESS:
        printf("CALL IN PROCESS\n");
        bt_call_active = 1;
        break;
    default:
        break;
    }

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_call_status)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_call_status(remote_addr,value);
    }
}
void hfp_hf_call_setup_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_setup_status,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
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

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_call_setup)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_call_setup(remote_addr,value);
    }

    if((value == HFP_CALL_NO_CALL) && (bt_call_active == 0))
    {
        if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_call_status)
        {
            bt_wrapper_cb->app_hfp_cb->bt_hfp_call_status(remote_addr,HFP_CALL_NO_INPORCESS);
        }
    }
}
void hfp_hf_call_held_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_held_status,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
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
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_signal_strength_ind)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_signal_strength_ind(remote_addr,value);
    }
}
void hfp_hf_battchg_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_battchg_status,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_batt_level_ind)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_batt_level_ind(remote_addr,value);
    }
}
void hfp_hf_server_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_server_status:%d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
}
void hfp_hf_roam_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_roam_status, value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_roam_status_ind)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_roam_status_ind(remote_addr,value);
    }
}
void hfp_hf_network(struct bd_addr_t *remote_addr,uint8_t mode,uint8_t format,uint8_t *operator,uint8_t operator_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_network,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("mode %d,format %d,operator %s,operator_len %d\n",mode,format,operator,operator_len);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_operator)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_operator(remote_addr,operator);
    }
}
void hfp_hf_ring(struct bd_addr_t *remote_addr)
{
    printf("WRAPPER << PROFILE:hfp_hf_ring,address is :\n");
    bt_addr_dump(remote_addr->addr);
}

void hfp_hf_clip(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type)
{
    printf("WRAPPER << PROFILE:hfp_hf_clip, type %d,address is :\n",type);
    bt_addr_dump(remote_addr->addr);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_call_waiting(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type)
{
    printf("WRAPPER << PROFILE:hfp_hf_call_waiting, type %d,address is :\n",type);
    bt_addr_dump(remote_addr->addr);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);
}

void hfp_hf_voice_recognition(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_voice_recognition, value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
}

void hfp_hf_spk_volume(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_spk_volume,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
}

void hfp_hf_mic_volume(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_mic_volume,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
}

void hfp_hf_hold_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:hfp_hf_hold_status,value %d,address is :\n",value);
    bt_addr_dump(remote_addr->addr);
}

void hfp_hf_local_number(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type,uint8_t service)
{
    uint8_t lpn_buffer[16] = {0};
    printf("WRAPPER << PROFILE:hfp_hf_local_number, type %d,service %d address is :\n",type,service);
    bt_addr_dump(remote_addr->addr);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);

    memcpy(lpn_buffer,number,number_len);
    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_local_pn)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_local_pn(remote_addr,lpn_buffer);
    }
}

void hfp_hf_call_list(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t index,uint8_t dir,uint8_t status,uint8_t mode,uint8_t mpty,uint8_t type)
{
    uint8_t pn_buffer[16] = {0};
    printf("WRAPPER << PROFILE:hfp_hf_call_list, index %d,dir %d status %d mode %d mpty %d type %d address is :\n",index,dir,status,mode,mpty,type);
    bt_addr_dump(remote_addr->addr);
    printf("number len %d,number is:\n",number_len);
    bt_hex_dump(number,number_len);

    memcpy(pn_buffer,number,number_len);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_call_pn)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_call_pn(remote_addr,pn_buffer);
    }
}

void hfp_hf_manufacturer_id(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len)
{
    uint8_t mid_buffer[32] = {0};
    printf("WRAPPER << PROFILE:hfp_hf_manufacturer_id, address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("mid len %d,mid is:\n",mid_len);
    bt_hex_dump(mid,mid_len);

    memcpy(mid_buffer,mid,mid_len);
    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_manu_id)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_manu_id(remote_addr,mid_buffer);
    }
}

void hfp_hf_model_id(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len)
{
    uint8_t mid_buffer[32] = {0};
    printf("WRAPPER << PROFILE:hfp_hf_model_id, address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("mid len %d,mid is:\n",mid_len);
    bt_hex_dump(mid,mid_len);

    memcpy(mid_buffer,mid,mid_len);
    if(bt_wrapper_cb && bt_wrapper_cb->app_hfp_cb && bt_wrapper_cb->app_hfp_cb->bt_hfp_module_id)
    {
        bt_wrapper_cb->app_hfp_cb->bt_hfp_module_id(remote_addr,mid_buffer);
    }
}

void hfp_hf_revision_id(struct bd_addr_t *remote_addr,uint8_t *rid,uint8_t rid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_revision_id, address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("rid len %d,mid is:\n",rid_len);
    bt_hex_dump(rid,rid_len);
}

void hfp_hf_product_id(struct bd_addr_t *remote_addr,uint8_t *pid,uint8_t pid_len)
{
    printf("WRAPPER << PROFILE:hfp_hf_product_id, address is :\n");
    bt_addr_dump(remote_addr->addr);
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


#if PROFILE_A2DP_ENABLE
void a2dp_sink_signal_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:a2dp_sink_signal_connect_set_up, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_signal_connect)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_signal_connect(remote_addr,status);
    }
}
void a2dp_sink_signal_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:a2dp_sink_signal_connect_realease, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_signal_disconnect)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_signal_disconnect(remote_addr,status);
    }
}
void a2dp_sink_stream_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_connect_set_up, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_stream_connect)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_stream_connect(remote_addr,status);
    }
}
void a2dp_sink_stream_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_connect_realease, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_stream_disconnect)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_stream_disconnect(remote_addr,status);
    }
}
void a2dp_sink_stream_start(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_start, address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_start)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_start(remote_addr,value);
    }

}
void a2dp_sink_stream_realease(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_realease, address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_relase)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_relase(remote_addr,value);
    }

}
void a2dp_sink_stream_suspend(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_suspend, address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_suspend)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_suspend(remote_addr,value);
    }
}
void a2dp_sink_stream_abort(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf("WRAPPER << PROFILE:a2dp_sink_stream_abort, address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_a2dp_cb && bt_wrapper_cb->app_a2dp_cb->bt_a2dp_abort)
    {
        bt_wrapper_cb->app_a2dp_cb->bt_a2dp_abort(remote_addr,value);
    }

}


static a2dp_sink_cbs_t a2dp_sink_wrapper_cb =
{
    a2dp_sink_signal_connect_set_up,
    a2dp_sink_signal_connect_realease,
    a2dp_sink_stream_connect_set_up,
    a2dp_sink_stream_connect_realease,
    a2dp_sink_stream_start,
    a2dp_sink_stream_realease,
    a2dp_sink_stream_suspend,
    a2dp_sink_stream_abort,
};
#endif



#if PROFILE_AVRCP_ENABLE
void avrcp_av_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:avrcp_av_connect_set_up, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_ctl_connect)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_ctl_connect(remote_addr,status);
    }
}
void avrcp_av_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:avrcp_av_connect_realease, address is :\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_ctl_disconnect)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_ctl_disconnect(remote_addr,status);
    }
}

void avrcp_support_capabilities(struct bd_addr_t *remote_addr,uint16_t support_cap_mask)
{
    printf("WRAPPER << PROFILE:avrcp_support_capabilities mask(0x%x), address is :\n",support_cap_mask);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_support_capabilities)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_support_capabilities(remote_addr,support_cap_mask);
    }
}

void avrcp_app_setting_attr(struct bd_addr_t *remote_addr,uint16_t setting_attr_mask)
{
    printf("WRAPPER << PROFILE:avrcp_app_setting_attr mask(0x%x), address is :\n",setting_attr_mask);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_app_setting_attr)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_app_setting_attr(remote_addr,setting_attr_mask);
    }
}

void avrcp_play_status_update(struct bd_addr_t *remote_addr,uint8_t play_status)
{
    printf("WRAPPER << PROFILE:avrcp_play_status_update play status(%d), address is :\n",play_status);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_play_status_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_play_status_update(remote_addr,play_status);
    }
}

void avrcp_track_change_update(struct bd_addr_t *remote_addr)
{
    printf("WRAPPER << PROFILE:avrcp_track_change_update\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_track_change_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_track_change_update(remote_addr);
    }
}

void avrcp_playpos_change_update(struct bd_addr_t *remote_addr,uint32_t millisecond)
{
    printf("WRAPPER << PROFILE:avrcp_playpos_change_update ms(%d)\n",millisecond);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_playpos_change_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_playpos_change_update(remote_addr,millisecond);
    }
}

void avrcp_battary_change_update(struct bd_addr_t *remote_addr,uint32_t battary_status)
{
    printf("WRAPPER << PROFILE:avrcp_battary_change_update battary_status(%d)\n",battary_status);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_battary_change_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_battary_change_update(remote_addr,battary_status);
    }
}

void avrcp_volume_change_update(struct bd_addr_t *remote_addr,uint8_t volume)
{
    printf("WRAPPER << PROFILE:avrcp_volume_change_update volume(%d)\n",volume);
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_volume_change_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_volume_change_update(remote_addr,volume);
    }
}

void avrcp_element_attr_update(struct bd_addr_t *remote_addr,now_playing_info_t* now_playing_info)
{
    printf("WRAPPER << PROFILE:avrcp_element_attr_update\n");
    bt_addr_dump(remote_addr->addr);
    if(bt_wrapper_cb && bt_wrapper_cb->app_avrcp_cb && bt_wrapper_cb->app_avrcp_cb->bt_avrcp_element_attr_update)
    {
        bt_wrapper_cb->app_avrcp_cb->bt_avrcp_element_attr_update(remote_addr,now_playing_info->now_playing_title,now_playing_info->now_playing_artist,
                now_playing_info->now_playing_album,now_playing_info->current_index,now_playing_info->totol_count,now_playing_info->totol_milliseconds);
    }
}


static avrcp_controller_cbs_t avrcp_controller_wrapper_cb =
{
    avrcp_av_connect_set_up,
    avrcp_av_connect_realease,
    NULL,
    NULL,
    avrcp_support_capabilities,
    avrcp_app_setting_attr,
    avrcp_play_status_update,
    avrcp_track_change_update,
    avrcp_playpos_change_update,
    avrcp_battary_change_update,
    avrcp_volume_change_update,
    avrcp_element_attr_update,
};


#endif



#if PROFILE_PBAP_ENABLE
void pbap_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_pbap_cb && bt_wrapper_cb->app_pbap_cb->bt_pbap_connect)
    {
        bt_wrapper_cb->app_pbap_cb->bt_pbap_connect(remote_addr,status);
    }
}
void pbap_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_pbap_cb && bt_wrapper_cb->app_pbap_cb->bt_pbap_disconnect)
    {
        bt_wrapper_cb->app_pbap_cb->bt_pbap_disconnect(remote_addr,status);
    }
}

void pbap_query_repositories_size(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t size)
{
    printf("WRAPPER << PROFILE:pbap_query_repositories_size,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) size(%d)\n",repositories,type,size);

    if(bt_wrapper_cb && bt_wrapper_cb->app_pbap_cb && bt_wrapper_cb->app_pbap_cb->bt_pbap_query_repositories_size)
    {
        bt_wrapper_cb->app_pbap_cb->bt_pbap_query_repositories_size(remote_addr,repositories,type,size);
    }
}

void pbap_download_phonebook_status(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_download_phonebook_status,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) status(%d)\n",repositories,type,status);

    if(bt_wrapper_cb && bt_wrapper_cb->app_pbap_cb && bt_wrapper_cb->app_pbap_cb->bt_pbap_download_pb_status)
    {
        bt_wrapper_cb->app_pbap_cb->bt_pbap_download_pb_status(remote_addr,repositories,type,status);
    }
}

void pbap_download_phonebook_data(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t *data,uint16_t data_len)
{
    printf("WRAPPER << PROFILE:pbap_download_phonebook_data,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) data_len(%d)\n",repositories,type,data_len);

    //bt_hex_dump(data,data_len);
}

void pbap_download_vcardlist_status(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status)
{
    printf("WRAPPER << PROFILE:pbap_download_vcardlist_status,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) status(%d)\n",repositories,type,status);

    if(bt_wrapper_cb && bt_wrapper_cb->app_pbap_cb && bt_wrapper_cb->app_pbap_cb->bt_pbap_download_vcardlist_status)
    {
        bt_wrapper_cb->app_pbap_cb->bt_pbap_download_vcardlist_status(remote_addr,repositories,type,status);
    }
}

void pbap_download_vcardlist_data(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t *data,uint16_t data_len)
{
    printf("WRAPPER << PROFILE:pbap_download_vcardlist_data,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) data_len(%d)\n",repositories,type,data_len);

    bt_hex_dump(data,data_len);
}

void pbap_download_vcardentry_data(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t entry_number,uint8_t *data,uint16_t data_len)
{
    printf("WRAPPER << PROFILE:pbap_download_vcardentry_data,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) entry_number(%d) data_len(%d)\n",repositories,type,entry_number,data_len);

    bt_hex_dump(data,data_len);
}

static pbap_client_cbs_t pbap_client_wrapper_cb =
{
    pbap_connect_set_up,
    pbap_connect_realease,
    pbap_query_repositories_size,
    pbap_download_phonebook_status,
    pbap_download_phonebook_data,
    pbap_download_vcardlist_status,
    pbap_download_vcardlist_data,
    pbap_download_vcardentry_data,
};

#endif

#if PROFILE_HID_ENABLE > 0
void hid_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hid_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hid_cb && bt_wrapper_cb->app_hid_cb->bt_hid_connect)
    {
        bt_wrapper_cb->app_hid_cb->bt_hid_connect(remote_addr,status);
    }
}
void hid_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:hid_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hid_cb && bt_wrapper_cb->app_hid_cb->bt_hid_disconnect)
    {
        bt_wrapper_cb->app_hid_cb->bt_hid_disconnect(remote_addr,status);
    }
}
void hid_interrupt_data_ind(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len)
{
    printf("WRAPPER << PROFILE:hid_interrupt_data_ind,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("data len %d,data is:\n",data_len);
    bt_hex_dump(data,data_len);

    if(bt_wrapper_cb && bt_wrapper_cb->app_hid_cb && bt_wrapper_cb->app_hid_cb->bt_hid_interrupt_recv_data)
    {
        bt_wrapper_cb->app_hid_cb->bt_hid_interrupt_recv_data(remote_addr,data,data_len);
    }
}

static hid_cbs_t hid_wrapper_cb =
{
    hid_connect_set_up,
    hid_connect_realease,
    hid_interrupt_data_ind,
};

#endif


#if BT_BLE_ENABLE > 0
void gatt_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:gattc_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->bt_gatt_connect_set_up)
    {
        bt_wrapper_cb->app_gatt_cb->bt_gatt_connect_set_up(remote_addr,status);
    }

}
void gatt_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:gattc_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->bt_gatt_connect_realease)
    {
        bt_wrapper_cb->app_gatt_cb->bt_gatt_connect_realease(remote_addr,status);
    }
}

void gattc_mtu_value(struct bd_addr_t *remote_addr,uint16_t mtu)
{
    printf("WRAPPER << PROFILE:gattc_mtu_value,mtu(%d) address is :\n",mtu);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gatt_mtu_value)
    {
        bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gatt_mtu_value(remote_addr,mtu);
    }
}

void gattc_discovery_primary_service(struct bd_addr_t *remote_addr,gatt_client_pri_service_t *pri_service,uint16_t count)
{
    printf("WRAPPER << PROFILE:gattc_discovery_primary_service,count(%d) address is :\n",count);
    bt_addr_dump(remote_addr->addr);


    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_primary_service)
    {
        bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_primary_service(remote_addr,pri_service,count);
    }
}


void gattc_discovery_uuid_primary_service(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
    printf("WRAPPER << PROFILE:gattc_discovery_uuid_primary_service,start_handle(%d) end_handle(%d) address is :\n",start_handle,end_handle);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_uuid_primary_service)
    {
        bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_uuid_primary_service(remote_addr,start_handle,end_handle);
    }
}


void gattc_discovery_char(struct bd_addr_t *remote_addr,uint16_t attribute_handle,uint16_t char_value_handle,uint8_t properties,uint16_t uuid16,uint8_t *uuid128)
{
    printf("WRAPPER << PROFILE:gattc_discovery_char,attribute_handle(%d) char_value_handle(%d) properties(0x%x) uuid16(0x%x) address is :\n",attribute_handle,char_value_handle,properties,uuid16);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs && bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_char)
    {
        bt_wrapper_cb->app_gatt_cb->gatt_client_cbs->bt_gattc_discovery_char(remote_addr,attribute_handle,char_value_handle,properties,uuid16,uuid128);
    }
}


gatt_client_cbs_t gattc_wrapper_cb =
{
    gattc_mtu_value,
    gattc_discovery_primary_service,
    gattc_discovery_uuid_primary_service,
    gattc_discovery_char,
};

void gatts_mtu_value(struct bd_addr_t *remote_addr,uint16_t mtu)
{
    printf("WRAPPER << PROFILE:gatts_mtu_value,mtu(%d) address is :\n",mtu);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_gatt_cb && bt_wrapper_cb->app_gatt_cb->gatt_server_cbs && bt_wrapper_cb->app_gatt_cb->gatt_server_cbs->bt_gatt_mtu_value)
    {
        bt_wrapper_cb->app_gatt_cb->gatt_server_cbs->bt_gatt_mtu_value(remote_addr,mtu);
    }
}


gatt_server_cbs_t gatts_wrapper_cb =
{
    gatts_mtu_value,
};



static gatt_cbs_t gatt_wrapper_cb =
{
    gatt_connect_set_up,
    gatt_connect_realease,
    &gattc_wrapper_cb,
    &gatts_wrapper_cb,
};


void sm_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:sm_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_smp_cb && bt_wrapper_cb->app_smp_cb->bt_smp_connect_set_up)
    {
        bt_wrapper_cb->app_smp_cb->bt_smp_connect_set_up(remote_addr,status);
    }

}
void sm_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf("WRAPPER << PROFILE:sm_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_smp_cb && bt_wrapper_cb->app_smp_cb->bt_smp_connect_realease)
    {
        bt_wrapper_cb->app_smp_cb->bt_smp_connect_realease(remote_addr,status);
    }

}

void smp_passkey_display(struct bd_addr_t *remote_addr,uint32_t passkey)
{
    printf("WRAPPER << PROFILE:smp_passkey_display passkey(%d),address is :\n",passkey);
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_smp_cb && bt_wrapper_cb->app_smp_cb->bt_smp_passkey_display)
    {
        bt_wrapper_cb->app_smp_cb->bt_smp_passkey_display(remote_addr,passkey);
    }
}

void smp_passkey_input(struct bd_addr_t *remote_addr,uint32_t *passkey)
{
    printf("WRAPPER << PROFILE:smp_passkey_input address is :\n");
    bt_addr_dump(remote_addr->addr);

    if(bt_wrapper_cb && bt_wrapper_cb->app_smp_cb && bt_wrapper_cb->app_smp_cb->bt_smp_passkey_input)
    {
        bt_wrapper_cb->app_smp_cb->bt_smp_passkey_input(remote_addr,passkey);
    }
}

void smp_ltk_generate(smp_device_info_t *device_info)
{
    printf("WRAPPER << PROFILE:smp_ltk_generate address is :\n");
    bt_addr_dump((uint8_t *)&device_info->remote_addr);
    printf("ltk:\n");
    bt_hex_dump(device_info->ltk,16);
	le_device_instance.is_sc = device_info->is_sc;
    bd_addr_set(&(le_device_instance.remote_addr),&device_info->remote_addr);
    memcpy(&(le_device_instance.ltk),device_info->ltk,16);
}

void smp_ltk_request(smp_device_info_t *device_info,uint8_t *has_device)
{
    printf("WRAPPER << PROFILE:smp_ltk_request address is :\n");
    bt_addr_dump((uint8_t *)&device_info->remote_addr);

	if(bd_addr_cmp(&(le_device_instance.remote_addr),&device_info->remote_addr))
    {
    	device_info->is_sc = le_device_instance.is_sc;
		memcpy(device_info->ltk,le_device_instance.ltk,16);
    	*has_device = 1;
    }
    else
    {
        *has_device = 0;
    }

}


static smp_cbs_t smp_wrapper_cb =
{
    sm_connect_set_up,
    sm_connect_realease,
    smp_passkey_display,
    smp_passkey_input,
    smp_ltk_generate,
    smp_ltk_request,
};
#endif



err_t bt_stack_worked(void *arg)
{
    printf("bt_stack_worked\r\n");

#if PROFILE_HFP_HF_ENABLE
    uint16_t hf_feature = HFP_HFSF_EC_NR_FUNCTION |  HFP_HFSF_THREE_WAY_CALLING|
                          HFP_HFSF_CLI_PRESENTATION_CAPABILITY | HFP_HFSF_VOICE_RECOGNITION_FUNCTION |
                          HFP_HFSF_REMOTE_VOLUME_CONTROL |HFP_HFSF_ENHANCED_CALL_STATUS |
                          HFP_HFSF_ENHANCED_CALL_CONTROL | HFP_HFSF_CODEC_NEGOTIATION|
                          HFP_HFSF_HF_INDICATORS |HFP_HFSF_ESCO_S4;
#endif

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
#if PROFILE_HFP_HF_ENABLE
    hfp_hf_init(hf_feature,HFP_HF_SDP_SUPPORT_WBS,&hfp_hf_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_HFP_HF_MASK;
#endif
#if PROFILE_PBAP_ENABLE
    pbap_client_init(&pbap_client_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_PBAP_PCE_MASK;
#endif
#if PROFILE_A2DP_ENABLE
    a2dp_sink_init(&a2dp_sink_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_A2DP_SINK_MASK;
#endif
#if PROFILE_AVRCP_ENABLE
    avrcp_controller_init(&avrcp_controller_wrapper_cb);
    bt_profile_mask |= BT_PROFILE_AVRCP_CONTROL_MASK;
#endif

#if PROFILE_HID_ENABLE > 0
    hid_device_init(&hid_wrapper_cb);
    hid_device_sdp_init(hid_device_descriptor,sizeof(hid_device_descriptor));
    bt_profile_mask |= BT_PROFILE_HID_DEVICE_MASK;
#endif

#if BT_BLE_ENABLE > 0
    gatt_init(&gatt_wrapper_cb);
    smp_init(&smp_wrapper_cb);
    gatt_server_init();
#if PROFILE_BAS_ENABLE > 0
    bas_init(100);
#endif
#endif

    if(hci_get_version() >= HCI_PROTO_VERSION_4_2)
        hci_le_read_p256_public_key();


    bt_ass_eir_data();
    hci_write_eir(eir_data);

    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_init_result)
    {
        bt_wrapper_cb->app_common_cb->bt_init_result(BT_INIT_SUCCESS,bt_profile_mask);
    }


    return 0;
}


uint8_t bt_start(bt_app_cb_t *app_cb)
{

    bt_wrapper_cb = app_cb;
    bt_mem_init();
    bt_memp_init();
    phybusif_open(BT_BAUDRATE_1);
#if BT_ENABLE_SNOOP > 0
    bt_snoop_init();
#endif
    /* blueooth stack init */
    hci_init();
    hci_register_link_key_req(link_key_req);
    hci_register_link_key_not(link_key_not);
    hci_register_bt_working(bt_stack_worked);
    hci_register_hardware_error(bt_hardware_error);

    phybusif_reset(&uart_if);
    hci_reset();

    return 0;
}

uint8_t bt_stop(void)
{
    bt_profile_mask = 0;
    bt_wrapper_cb = NULL;
    hci_deinit();
    l2cap_deinit();
    sdp_deinit();
    rfcomm_deinit();
    return 0;
}

uint8_t bt_write_current_iac(uint8_t iac_num,uint32_t *iac)
{
	hci_write_current_iac_lap(iac_num,iac);

	return 0;
}


uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev)
{
    uint32_t lap =  INQUIRY_GIAC;

    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_inquiry_status)
    {
        bt_wrapper_cb->app_common_cb->bt_inquiry_status(BT_INQUIRY_START);
    }

    hci_inquiry(lap,inquiry_len,max_dev,bt_inquiry_result,bt_inquiry_complete);
    return 0;
}


uint8_t bt_stop_inquiry(void)
{
    hci_cancel_inquiry();
    return 0;
}

uint8_t bt_start_periodic_inquiry(uint16_t min_length,uint16_t max_length,uint8_t inquiry_len,uint8_t max_dev)
{
    uint32_t lap =  0x9E8B33;    /* GIAC */
    hci_periodic_inquiry(min_length,max_length,lap,inquiry_len,max_dev,bt_inquiry_result,bt_inquiry_complete);

    return 0;
}
uint8_t bt_stop_periodic_inquiry(void)
{
    hci_cancel_periodic_inquiry();
    return 0;
}

uint8_t bt_get_remote_name(struct bd_addr_t *bdaddr)
{
    hci_get_remote_name(bdaddr,bt_get_remote_name_complete);
    return 0;
}


uint8_t bt_cancel_get_remote_name(struct bd_addr_t *bdaddr)
{
    hci_cancel_get_remote_name(bdaddr);
    return 0;
}

#if BT_BLE_ENABLE > 0
uint8_t bt_le_start_inquiry(void)
{
    hci_le_set_scan_param(1,0x30,0x30,0,0);
    hci_le_inquiry(0,bt_le_inquiry_result,bt_le_inquiry_complete);

    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_le_inquiry_status)
    {
        bt_wrapper_cb->app_common_cb->bt_le_inquiry_status(BT_LE_INQUIRY_START);
    }
    return 0;
}

uint8_t bt_le_stop_inquiry(void)
{
    hci_le_cancel_inquiry();

    return 0;
}

uint8_t bt_le_adv_parse_init(bt_le_adv_parse_t *bt_adv_le_parse,uint8_t adv_size,uint8_t *adv_data)
{
    bt_adv_le_parse->adv_data = adv_data;
    bt_adv_le_parse->adv_len = adv_size;
    bt_adv_le_parse->adv_offset = 0;
    bt_adv_le_parse->adv_item_len = 0;

    return 0;
}
uint8_t bt_le_adv_has_more(bt_le_adv_parse_t *bt_adv_le_parse)
{
    if(bt_adv_le_parse->adv_offset >= bt_adv_le_parse->adv_len)
        return 0;
    else
        return 1;
}


uint8_t bt_le_adv_data_parse(bt_le_adv_parse_t *bt_adv_le_parse,uint8_t *adv_item_type,uint8_t *adv_item_data_len,uint8_t **adv_item_data)
{
    *adv_item_data_len = bt_le_adv_get_size(bt_adv_le_parse);
    *adv_item_type = bt_le_adv_get_type(bt_adv_le_parse);
    *adv_item_data = bt_le_adv_get_data(bt_adv_le_parse);

    return 0;

}

uint8_t bt_le_set_adv_enable(uint8_t adv_data_len,uint8_t *adv_data)

{
    struct bd_addr_t addr = {0};
    hci_le_set_adv_param(0x30,0x30,0,0,0,&addr,0x07,0);
    hci_le_set_adv_data(adv_data_len,adv_data);
    hci_le_set_adv_enable(1);

    return 0;
}
uint8_t bt_le_set_adv_disable(void)

{
    hci_le_set_adv_enable(0);

    return 0;
}


#endif

#if PROFILE_HFP_HF_ENABLE
/************************* HFP API ***********************/
uint8_t bt_hfp_hf_get_operator(struct bd_addr_t *bdaddr)
{
    hfp_hf_get_network(bdaddr);
    return 0;
}

uint8_t bt_hfp_hf_audio_transfer(struct bd_addr_t *bdaddr)
{
    printf("APP >> WRAPPER: bt_hfp_hf_audio_transfer bt_sco_connected(%d)\n",bt_sco_connected);
    if(bt_sco_connected == 0)
    {
        hfp_hf_audio_connect(bdaddr);
    }
    else
    {
        hfp_hf_audio_disconnect(bdaddr);
    }

    return 0;
}

uint8_t bt_hfp_hf_accept_incoming_call(struct bd_addr_t *bdaddr)
{
    hfp_hf_answer_incoming_call(bdaddr);

    return 0;
}

uint8_t bt_hfp_hf_end_call(struct bd_addr_t *bdaddr)
{
    hfp_hf_hangup(bdaddr);

    return 0;
}

uint8_t bt_hfp_hf_callout_by_number(struct bd_addr_t *addr,uint8_t *number)
{
    hfp_hf_callout_with_phone_number(addr,number);

    return 0;
}

uint8_t bt_hfp_hf_callout_by_memory(struct bd_addr_t *addr,uint8_t memory_id)
{
    hfp_hf_callout_with_memory(addr,memory_id);

    return 0;
}

uint8_t bt_hfp_hf_callout_by_last(struct bd_addr_t *addr)
{
    hfp_hf_callout_with_last_number(addr);

    return 0;
}

uint8_t bt_hfp_hf_get_local_phone_number(struct bd_addr_t *addr)
{
    hfp_hf_get_local_phone_number(addr);

    return 0;
}

uint8_t bt_hfp_hf_get_call_list(struct bd_addr_t *addr)
{
    hfp_hf_query_call_list(addr);

    return 0;
}

uint8_t bt_hfp_hf_disable_ecnr(struct bd_addr_t *addr)
{
    hfp_hf_disable_ag_nrec(addr);

    return 0;
}


uint8_t bt_hfp_hf_set_mic_volume(struct bd_addr_t *addr,uint8_t value)
{
    uint8_t mic_volume = value>=HFP_VOLUME_MAX?HFP_VOLUME_MAX:value;
    hfp_hf_set_mic_volume(addr,mic_volume);

    return 0;
}

uint8_t bt_hfp_hf_set_spk_volume(struct bd_addr_t *addr,uint8_t value)
{
    uint8_t spk_volume = (value>=HFP_VOLUME_MAX)?HFP_VOLUME_MAX:value;
    hfp_hf_set_spk_volume(addr,spk_volume);

    return 0;
}

uint8_t bt_hfp_hf_transmit_dtmf(struct bd_addr_t *addr,uint8_t value)
{
    hfp_hf_transmit_dtmf(addr,value);

    return 0;
}

uint8_t bt_hfp_hf_set_voice_recognition(struct bd_addr_t *addr,uint8_t enable)
{
    hfp_hf_set_voice_recognition(addr,enable);

    return 0;
}

uint8_t bt_hfp_hf_send_batt_level(struct bd_addr_t *addr,uint8_t batt_level)
{
    hfp_hf_transfer_hf_indicator_value(addr,HFP_BATT_LEVEL_IND,batt_level);
    return 0;
}


uint8_t bt_hfp_hf_get_manufacturer_id(struct bd_addr_t *addr)
{
    hfp_hf_get_manufacturer_id(addr);
    return 0;
}

uint8_t bt_hfp_hf_get_model_id(struct bd_addr_t *addr)
{
    hfp_hf_get_model_id(addr);

    return 0;
}

uint8_t bt_hfp_hf_get_revision_id(struct bd_addr_t *addr)
{
    hfp_hf_get_revision_id(addr);
    return 0;
}

uint8_t bt_hfp_hf_get_pid(struct bd_addr_t *addr)
{
    hfp_hf_get_pid(addr);
    return 0;
}
#endif


#if PROFILE_AVRCP_ENABLE

uint8_t bt_avrcp_controller_get_play_status(struct bd_addr_t *remote_addr)
{
    avrcp_controller_get_play_status(remote_addr);
    return 0;
}

uint8_t bt_avrcp_controller_list_app_setting_attr(struct bd_addr_t *remote_addr)
{
    avrcp_controller_list_app_setting_attr(remote_addr);
    return 0;
}

uint8_t bt_avrcp_controller_get_element_attributes(struct bd_addr_t *remote_addr)
{
    avrcp_controller_get_element_attributes(remote_addr);
    return 0;
}

uint8_t bt_avrcp_controller_control(struct bd_addr_t *remote_addr,uint8_t control_id)
{
    avrcp_controller_control(remote_addr,control_id);

    return 0;
}


#endif

#if PROFILE_HID_ENABLE

uint8_t bt_hid_interupt_report(struct bd_addr_t *remote_addr,uint8_t *report,uint8_t report_size)
{
    hid_device_interupt_report(remote_addr,report,report_size);

    return 0;
}

uint8_t bt_hid_find_keycode(uint8_t *keycode,uint8_t find_char)
{
    uint8_t index = 0;
    for(index = 0; index < sizeof(keyboard_key_map); index++)
    {
        if(keyboard_key_map[index] == find_char)
        {
            *keycode = index;
            break;
        }
    }

    return 0;
}


#endif

#if PROFILE_PBAP_ENABLE
/************************* PBAP client API ***********************/
uint8_t bt_pbap_client_connect(struct bd_addr_t *remote_addr)
{
    pbap_client_connect(remote_addr);
    return 0;
}
uint8_t bt_pbap_client_disconnect(struct bd_addr_t *remote_addr)
{
    pbap_client_disconnect(remote_addr);
    return 0;
}

uint8_t bt_pbap_client_query_phonebook_size(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    pbap_client_query_phonebook_size(addr,repositories,type);
    return 0;
}
uint8_t bt_pbap_client_download_phonebook(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    pbap_client_download_phonebook(addr,repositories,type);
    return 0;
}

uint8_t bt_pbap_client_set_path(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    pbap_client_set_path(addr,repositories,type);
    return 0;
}

uint8_t bt_pbap_client_download_vcard_list(struct bd_addr_t *addr,uint8_t repositories,uint8_t type)
{
    pbap_client_download_vcard_list(addr,repositories,type);
    return 0;
}

uint8_t bt_pbap_client_download_vcard_entry(struct bd_addr_t *addr,uint8_t repositories,uint8_t type,uint16_t entry_number)
{
    pbap_client_download_vcard_entry(addr,repositories,type,entry_number);
    return 0;
}

uint8_t bt_pbap_client_download_abort(struct bd_addr_t *addr)
{
    pbap_client_download_abort(addr);
    return 0;
}

#endif


#if BT_BLE_ENABLE

uint8_t bt_smp_security_request(struct bd_addr_t *remote_addr)
{
    smp_security_request(remote_addr);

    return 0;
}

uint8_t bt_gatt_client_exchange_mtu(struct bd_addr_t *remote_addr,uint16_t mtu)
{
    gatt_client_exchange_mtu(remote_addr,mtu);

    return 0;
}

uint8_t bt_gatt_client_discovery_pri_service(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
    gatt_client_discovery_pri_service(remote_addr,start_handle,end_handle);

    return 0;
}

uint8_t bt_gatt_client_discovery_pri_service_uuid(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle,uint16_t uuid16,uint8_t *uuid128)
{
    gatt_client_discovery_pri_service_uuid(remote_addr,start_handle,end_handle,uuid16,uuid128);

    return 0;
}

uint8_t bt_gatt_client_find_include(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
    gatt_client_find_include(remote_addr,start_handle,end_handle);

    return 0;
}

uint8_t bt_gatt_client_discovery_characteristics(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
    gatt_client_discovery_characteristics(remote_addr,start_handle,end_handle);

    return 0;
}

uint8_t bt_gatt_client_discovery_char_des(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
    gatt_client_discovery_char_des(remote_addr,start_handle,end_handle);

    return 0;
}

uint8_t bt_gatt_client_read_char_value(struct bd_addr_t *remote_addr,uint16_t handle)
{
    gatt_client_read_char_value(remote_addr,handle);

    return 0;
}


#endif




static err_t bt_inquiry_result(hci_inq_res_t *inqres)
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

        if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_inquiry_result)
        {
            bt_wrapper_cb->app_common_cb->bt_inquiry_result(&inqres->bdaddr,dev_type,inqres->remote_name);
        }

    }

    return BT_ERR_OK;
}

static err_t bt_inquiry_complete(uint16_t result)
{
    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_inquiry_status)
    {
        bt_wrapper_cb->app_common_cb->bt_inquiry_status(BT_INQUIRY_COMPLETE);
    }
    return BT_ERR_OK;
}

#if BT_BLE_ENABLE > 0
static err_t bt_le_inquiry_result(hci_le_inq_res_t *le_inqres)
{
    if(le_inqres != NULL)
    {

        if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_le_inquiry_result)
        {
            bt_wrapper_cb->app_common_cb->bt_le_inquiry_result(&le_inqres->bdaddr,le_inqres->rssi,
                    le_inqres->adv_type,le_inqres->adv_size,le_inqres->adv_data);
        }

    }

    return BT_ERR_OK;
}


static err_t bt_le_inquiry_complete(uint16_t result)
{
    if(bt_wrapper_cb && bt_wrapper_cb->app_common_cb && bt_wrapper_cb->app_common_cb->bt_le_inquiry_status)
    {
        bt_wrapper_cb->app_common_cb->bt_le_inquiry_status(BT_LE_INQUIRY_COMPLETE);
    }
    return BT_ERR_OK;
}

static uint8_t bt_le_adv_get_type(bt_le_adv_parse_t *bt_adv_le_parse)
{
    return bt_adv_le_parse->adv_data[bt_adv_le_parse->adv_offset++];
}
static uint8_t bt_le_adv_get_size(bt_le_adv_parse_t *bt_adv_le_parse)
{
    uint8_t adv_item_size = bt_adv_le_parse->adv_data[bt_adv_le_parse->adv_offset++] - 1;
    bt_adv_le_parse->adv_item_len = adv_item_size;
    return adv_item_size;
}
static uint8_t *bt_le_adv_get_data(bt_le_adv_parse_t *bt_adv_le_parse)
{
    uint8_t *adv_data = bt_adv_le_parse->adv_data + bt_adv_le_parse->adv_offset;
    bt_adv_le_parse->adv_offset += bt_adv_le_parse->adv_item_len;
    return adv_data;
}


#endif

static err_t bt_get_remote_name_complete(struct bd_addr_t *bdaddr,uint8_t * name)
{
    printf("---------bt_address:\n");
    bt_addr_dump(bdaddr->addr);
    printf("---------bt_name:\n");
    bt_hex_dump(name,248);
    return BT_ERR_OK;
}





