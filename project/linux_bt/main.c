#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "bt_wrapper.h"

uint32_t sys_time = 0;
uint32_t last_sys_time = 0;

#define CONF_BSP_TICKS_PER_SEC 100


#define HW_DEBUG	printf

#define hw_memset 	memset
#define hw_memcpy	memcpy
#define hw_memcmp	memcmp
#define hw_strlen	strlen
#define hw_strcmp strcmp
#define hw_strncmp strncmp
#define hw_sprintf sprintf

#define HW_ERR_OK 0
#define HW_ERR_SHELL_NO_CMD 1

#define BT_HAVE_FUN "\
 \t\t								 \n  \
 \t\t                                \n  \
 \t\t             ^_^  Wireless Link BT  ^_^\n \
 \t\t                                \n  \
 \t\t                                \n   \
 \t\t                        _oo0oo_  \n   \
 \t\t                       o8888888o  \n   \
 \t\t                       88\" . \"88 \n   \
 \t\t                       (| -_- |)    \n   \
 \t\t                       0\\  =  /0     \n   \
 \t\t                     ___/`---'\\__\\_   \n   \
 \t\t                   .' \\\\|     |// '.   \n   \
 \t\t                  / \\\\|||  :  |||// \\   \n   \
 \t\t                 / _||||| -:- |||||- \\   \n   \
 \t\t                |   | \\\\\\  - /// |   |    \n   \
 \t\t                | \\_|  ''\\---/''  |_/ |    \n   \
 \t\t                \\  .-\\__  '-'  ___/-. /     \n   \
 \t\t              ___'. .'  /--.--\\  `. .'___    \n   \
 \t\t           ."" '<  `.___\\_<|>_/___.' >' "".   \n   \
 \t\t          | | :  `- \\`.;`\\ _ /`;.`/ - ` : | |  \n   \
 \t\t          \\  \\ `_.   \\_ __\\ /__ _/   .-` /  /   \n   \
 \t\t      =====`-.____`.___ \\_____/___.-`___.-'===== \n   \
 \t\t                        `=---='                   \n"


#define BT_SPLIT_NAME "---------------------------------------------------------------------------------------------------"

#define APP_LOG_COLOR_YELLOW "\033[40;33m"
#define APP_LOG_COLOR_RED "\033[40;31m"
#define APP_LOG_COLOR_BLUE "\033[40;36m"
#define APP_LOG_COLOR_RESET "\033[0m"


#define BT_START_CMD "BT_START"
#define BT_START_DES "Start bluetooth stack"
#define BT_STOP_CMD "BT_STOP"
#define BT_STOP_DES "Stop blueooth stack"
#define BT_WRITE_IAC_CMD "BT_W_IAC"
#define BT_WRITE_IAC_DES "Write current iac lap"
#define BT_INQUIRY_CMD "BT_INQUIRY"
#define BT_INQUIRY_DES "Inquiry device"
#define BT_CANCEL_INQUIRY_CMD "BT_CANCEL_INQUIRY"
#define BT_CANCEL_INQUIRY_DES "Cancel inquiry device"
#define BT_PERIOID_INQUIRY_CMD "BT_PERIOID_INQUIRY"
#define BT_PERIOID_INQUIRY_DES "Perioid inquiry device"
#define BT_CANCEL_PERIOID_INQUIRY_CMD "BT_CANCEL_PERIOID_INQUIRY"
#define BT_CANCEL_PERIOID_INQUIRY_DES "Cancel perioid inquiry device"
#define BT_LE_INQUIRY_CMD "BT_LE_INQUIRY"
#define BT_LE_INQUIRY_DES "BLE Inquiry device"
#define BT_LE_INQUIRY_CANCEL_CMD "BT_LE_STOP_INQUIRY"
#define BT_LE_INQUIRY_CANCEL_DES "BLE cancel Inquiry device"
#define BT_LE_ADV_ENABLE_CMD "BT_LE_ADV_ENABLE"
#define BT_LE_ADV_ENABLE_DES "Ble start advertising"
#define BT_LE_ADV_DISABLE_CMD "BT_LE_ADV_DISABLE"
#define BT_LE_ADV_DISABLE_DES "Ble stop advertising"
#define BT_SPP_CON_CMD "SPP_CON"
#define BT_SPP_CON_DES "Connect spp profile"
#define BT_SPP_SEND_CMD "SPP_SEND"
#define BT_SPP_SEND_DES "Spp sned data"
#define BT_SPP_DISCON_CMD "SPP_DISCON"
#define BT_SPP_DISCON_DES "Disconnect spp profile"
#define BT_HFP_CON_CMD "HFP_CON"
#define BT_HFP_CON_DES "Connect hfp profile"
#define BT_HFP_DISCON_CMD "HFP_DISCON"
#define BT_HFP_DISCON_DES "Disconnect hfp profile"
#define BT_HFP_AUDIO_TRANSFER_CMD "BT_AUDIO_TRANSFER"
#define BT_HFP_AUDIO_TRANSFER_DES "sco audio transfer"
#define BT_HFP_ANSWER_CMD "HFP_ANSWER"
#define BT_HFP_ANSWER_DES "Answer the incoming call"
#define BT_HFP_END_CALL_CMD "HFP_CALLEND"
#define BT_HFP_END_CALL_DES "End call"
#define BT_HFP_CALLOUT_PN_CMD "HFP_CALLOUT_PN"
#define BT_HFP_CALLOUT_PN_DES "Call out phone number(10086)"
#define BT_HFP_CALLOUT_MEM_CMD "HFP_CALLOUT_MEM"
#define BT_HFP_CALLOUT_MEM_DES "Call out phone number with memory 1"
#define BT_HFP_CALLOUT_LN_CMD "HFP_CALLOUT_LC"
#define BT_HFP_CALLOUT_LN_DES "Call out last number"
#define BT_HFP_LOCAL_PN_CMD "HFP_LPN"
#define BT_HFP_LOCAL_PN_DES "Get local phone number"
#define BT_HFP_CALL_LIST_CMD "HFP_CLCC"
#define BT_HFP_CALL_LIST_DES "Get call list information"
#define BT_HFP_DISABLE_ECNR_CMD "HFP_NRECD"
#define BT_HFP_DISABLE_ECNR_DES "Disable AG ECNR"
#define BT_HFP_VGS_CMD "HFP_VGS"
#define BT_HFP_VGS_DES "Set HFP speaker volume"
#define BT_HFP_VGM_CMD "HFP_VGM"
#define BT_HFP_VGM_DES "Set HFP mic volume"
#define BT_HFP_DTMF_CMD "HFP_DTMF"
#define BT_HFP_DTMF_DES "Transport hfp dtmf"
#define BT_HFP_VOICE_RECOG_ENABLE_CMD "HFP_VGE"
#define BT_HFP_VOICE_RECOG_ENABLE_DES "HFP voice recogntion enable"
#define BT_HFP_VOICE_RECOG_DISABLE_CMD "HFP_VGD"
#define BT_HFP_VOICE_RECOG_DISABLE_DES "HFP voice recogntion disable"
#define BT_HFP_GET_OP_NAME_CMD "HFP_GOPN"
#define BT_HFP_GET_OP_NAME_DES "HFP get operator name"
#define BT_HFP_GET_MANU_ID_CMD "HFP_CGMI"
#define BT_HFP_GET_MANU_ID_DES "HFP get manu name"
#define BT_HFP_GET_MODULE_ID_CMD "HFP_CGMM"
#define BT_HFP_GET_MODULE_ID_DES "HFP get module name"
#define BT_HFP_SET_BATT_LEVEL_CMD "HFP_BATT"
#define BT_HFP_SET_BATT_LEVEL_DES "HFP Set batt level"

#define BT_AVRCP_LIST_APP_ATTR_CMD "AVRCP_LIST_APP_ATTR"
#define BT_AVRCP_LIST_APP_ATTR_DES "List application seeting attribute"
#define BT_AVRCP_GET_SONG_INFO_CMD "AVRCP_GET_ID3"
#define BT_AVRCP_GET_SONG_INFO_DES "Get nowplaying song infomation"
#define BT_AVRCP_CONTROL_PLAY_CMD "AVRCP_PLAY"
#define BT_AVRCP_CONTROL_PLAY_DES "AVRCP control:play"
#define BT_AVRCP_CONTROL_PAUSE_CMD "AVRCP_PAUSE"
#define BT_AVRCP_CONTROL_PAUSE_DES "AVRCP control:pause"
#define BT_AVRCP_CONTROL_AVRCP_PREV_CMD "AVRCP_PREV"
#define BT_AVRCP_CONTROL_AVRCP_PREV_DES "AVRCP control:prev song"
#define BT_AVRCP_CONTROL_AVRCP_NEXT_CMD "AVRCP_NEXT"
#define BT_AVRCP_CONTROL_AVRCP_NEXT_DES "AVRCP control:next song"
#define BT_AVRCP_CONTROL_AVRCP_FF_CMD "AVRCP_FAST_FORWARD"
#define BT_AVRCP_CONTROL_AVRCP_FF_DES "AVRCP control:fast forward"
#define BT_AVRCP_CONTROL_AVRCP_FB_CMD "AVRCP_FAST_BACKWARD"
#define BT_AVRCP_CONTROL_AVRCP_FB_DES "AVRCP control:fast baward"
#define BT_PBAP_CONNECT_CMD "PBAP_CON"
#define BT_PBAP_CONNECT_DES "Connect pbap profile"

#define BT_SMP_SEC_REQ_CMD "SMP_SEC_REQ"
#define BT_SMP_SEC_REQ_DES "Smp security request"
#define BT_GATTC_MTU_REQ_CMD "GATTC_MTU"
#define BT_GATTC_MTU_REQ_DES "Gatt client exchange MTU"
#define BT_GATTC_PRIMARY_DISCOVERY_REQ_CMD "GATTC_DIS_P"
#define BT_GATTC_PRIMARY_DISCOVERY_REQ_DES "Gatt client discovery all primary services"
#define BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_CMD "GATTC_DIS_UUID"
#define BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_DES "Gatt client discovery primary services by UUID"
#define BT_GATTC_RELATIONSHIP_DISCOVERY_REQ_CMD "GATTC_DIS_FINCLUDE"
#define BT_GATTC_RELATIONSHIP_UUID_DISCOVERY_REQ_DES "Gatt client discovery relationship"
#define BT_GATTC_CHAR_DISCOVERY_REQ_CMD "GATTC_DIS_CHAR"
#define BT_GATTC_CHAR_DISCOVERY_REQ_DES "Gatt client discovery all characteristics"
#define BT_GATTC_CHARD_DISCOVERY_REQ_CMD "GATTC_DIS_CDES"
#define BT_GATTC_CHARD_DISCOVERY_REQ_DES "Gatt client discovery all characteristics descriptors"
#define BT_GATTC_CHAR_READ_VALUE_CMD "GATTC_RCHAR"
#define BT_GATTC_CHAR_READ_VALUE_DES "Gatt client read characteristics value"


typedef struct
{
    uint8_t *cmd;
    uint8_t *description;
} cmd_desctiption_t;

cmd_desctiption_t cmd_usage[] =
{
    {(uint8_t *)BT_START_CMD,(uint8_t *)BT_START_DES},
    {(uint8_t *)BT_STOP_CMD,(uint8_t *)BT_STOP_DES},
    {(uint8_t *)BT_WRITE_IAC_CMD,(uint8_t *)BT_WRITE_IAC_DES},
    {(uint8_t *)BT_INQUIRY_CMD,(uint8_t *)BT_INQUIRY_CMD},
    {(uint8_t *)BT_CANCEL_INQUIRY_CMD,(uint8_t *)BT_CANCEL_INQUIRY_DES},
    {(uint8_t *)BT_PERIOID_INQUIRY_CMD,(uint8_t *)BT_PERIOID_INQUIRY_DES},
    {(uint8_t *)BT_CANCEL_PERIOID_INQUIRY_CMD,(uint8_t *)BT_CANCEL_PERIOID_INQUIRY_DES},
    {(uint8_t *)BT_SPLIT_NAME,NULL},
#if BT_BLE_ENABLE > 0
    {(uint8_t *)BT_LE_INQUIRY_CMD,(uint8_t *)BT_LE_INQUIRY_DES},
    {(uint8_t *)BT_LE_INQUIRY_CANCEL_CMD,(uint8_t *)BT_LE_INQUIRY_CANCEL_DES},
    {(uint8_t *)BT_LE_ADV_ENABLE_CMD,(uint8_t *)BT_LE_ADV_ENABLE_DES},
    {(uint8_t *)BT_LE_ADV_DISABLE_CMD,(uint8_t *)BT_LE_ADV_DISABLE_CMD},
    {(uint8_t *)BT_SPLIT_NAME,NULL},
#endif
#if PROFILE_SPP_ENABLE > 0
    {(uint8_t *)BT_SPP_CON_CMD,(uint8_t *)BT_SPP_CON_DES},
    {(uint8_t *)BT_SPP_SEND_CMD,(uint8_t *)BT_SPP_SEND_DES},
    {(uint8_t *)BT_SPP_DISCON_CMD,(uint8_t *)BT_SPP_DISCON_DES},
    {(uint8_t *)BT_SPLIT_NAME,NULL},
#endif
#if PROFILE_HFP_HF_ENABLE > 0
    {(uint8_t *)BT_HFP_CON_CMD,(uint8_t *)BT_HFP_CON_DES},
    {(uint8_t *)BT_HFP_DISCON_CMD,(uint8_t *)BT_HFP_DISCON_DES},
    {(uint8_t *)BT_HFP_AUDIO_TRANSFER_CMD,(uint8_t *)BT_HFP_AUDIO_TRANSFER_DES},
    {(uint8_t *)BT_HFP_ANSWER_CMD,(uint8_t *)BT_HFP_ANSWER_DES},
    {(uint8_t *)BT_HFP_END_CALL_CMD,(uint8_t *)BT_HFP_END_CALL_DES},
    {(uint8_t *)BT_HFP_CALLOUT_PN_CMD,(uint8_t *)BT_HFP_CALLOUT_PN_DES},
    {(uint8_t *)BT_HFP_CALLOUT_MEM_CMD,(uint8_t *)BT_HFP_CALLOUT_MEM_DES},
    {(uint8_t *)BT_HFP_CALLOUT_LN_CMD,(uint8_t *)BT_HFP_CALLOUT_LN_DES},
    {(uint8_t *)BT_HFP_LOCAL_PN_CMD,(uint8_t *)BT_HFP_LOCAL_PN_DES},
    {(uint8_t *)BT_HFP_CALL_LIST_CMD,(uint8_t *)BT_HFP_CALL_LIST_DES},
    {(uint8_t *)BT_HFP_DISABLE_ECNR_CMD,(uint8_t *)BT_HFP_DISABLE_ECNR_DES},
    {(uint8_t *)BT_HFP_VGS_CMD,(uint8_t *)BT_HFP_VGS_DES},
    {(uint8_t *)BT_HFP_VGM_CMD,(uint8_t *)BT_HFP_VGM_DES},
    {(uint8_t *)BT_HFP_DTMF_CMD,(uint8_t *)BT_HFP_DTMF_DES},
    {(uint8_t *)BT_HFP_VOICE_RECOG_ENABLE_CMD,(uint8_t *)BT_HFP_VOICE_RECOG_ENABLE_DES},
    {(uint8_t *)BT_HFP_VOICE_RECOG_DISABLE_CMD,(uint8_t *)BT_HFP_VOICE_RECOG_DISABLE_DES},
    {(uint8_t *)BT_HFP_GET_MANU_ID_CMD,(uint8_t *)BT_HFP_GET_MANU_ID_DES},
    {(uint8_t *)BT_HFP_GET_MODULE_ID_CMD,(uint8_t *)BT_HFP_GET_MODULE_ID_DES},
    {(uint8_t *)BT_HFP_SET_BATT_LEVEL_CMD,(uint8_t *)BT_HFP_SET_BATT_LEVEL_DES},
    {(uint8_t *)BT_HFP_GET_OP_NAME_CMD,(uint8_t *)BT_HFP_GET_OP_NAME_DES},
    {(uint8_t *)BT_SPLIT_NAME,NULL},
#endif
#if PROFILE_AVRCP_ENABLE > 0
    {(uint8_t *)BT_AVRCP_LIST_APP_ATTR_CMD,(uint8_t *)BT_AVRCP_LIST_APP_ATTR_DES},
    {(uint8_t *)BT_AVRCP_GET_SONG_INFO_CMD,(uint8_t *)BT_AVRCP_GET_SONG_INFO_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_PLAY_CMD,(uint8_t *)BT_AVRCP_CONTROL_PLAY_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_PAUSE_CMD,(uint8_t *)BT_AVRCP_CONTROL_PAUSE_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_AVRCP_PREV_CMD,(uint8_t *)BT_AVRCP_CONTROL_AVRCP_PREV_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_AVRCP_NEXT_CMD,(uint8_t *)BT_AVRCP_CONTROL_AVRCP_NEXT_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_AVRCP_FF_CMD,(uint8_t *)BT_AVRCP_CONTROL_AVRCP_FF_DES},
    {(uint8_t *)BT_AVRCP_CONTROL_AVRCP_FB_CMD,(uint8_t *)BT_AVRCP_CONTROL_AVRCP_FB_DES},
    {(uint8_t *)BT_SPLIT_NAME,NULL},
#endif
#if PROFILE_PBAP_ENABLE > 0
    {(uint8_t *)BT_PBAP_CONNECT_CMD,(uint8_t *)BT_PBAP_CONNECT_DES},
#endif

#if BT_BLE_ENABLE > 0
	{(uint8_t *)BT_SMP_SEC_REQ_CMD,(uint8_t *)BT_SMP_SEC_REQ_DES},
    {(uint8_t *)BT_GATTC_MTU_REQ_CMD,(uint8_t *)BT_GATTC_MTU_REQ_DES},
    {(uint8_t *)BT_GATTC_PRIMARY_DISCOVERY_REQ_CMD,(uint8_t *)BT_GATTC_PRIMARY_DISCOVERY_REQ_DES},
    {(uint8_t *)BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_CMD,(uint8_t *)BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_DES},
    {(uint8_t *)BT_GATTC_RELATIONSHIP_DISCOVERY_REQ_CMD,(uint8_t *)BT_GATTC_RELATIONSHIP_UUID_DISCOVERY_REQ_DES},
    {(uint8_t *)BT_GATTC_CHAR_DISCOVERY_REQ_CMD,(uint8_t *)BT_GATTC_CHAR_DISCOVERY_REQ_DES},
    {(uint8_t *)BT_GATTC_CHARD_DISCOVERY_REQ_CMD,(uint8_t *)BT_GATTC_CHARD_DISCOVERY_REQ_DES},
    {(uint8_t *)BT_GATTC_CHAR_READ_VALUE_CMD,(uint8_t *)BT_GATTC_CHAR_READ_VALUE_DES},
#endif


};




void show_usage()
{
    uint32_t index = 0;

    printf(APP_LOG_COLOR_YELLOW"%s\n"APP_LOG_COLOR_RESET,BT_HAVE_FUN);
    printf(APP_LOG_COLOR_YELLOW"----------------------------------------------------------------------------------------------------|"APP_LOG_COLOR_RESET"\n");
    for(index = 0; index < sizeof(cmd_usage)/sizeof(cmd_desctiption_t); index++)
    {
        if(cmd_usage[index].cmd[0] == '-')
            HW_DEBUG(APP_LOG_COLOR_YELLOW"|%s|" APP_LOG_COLOR_RESET "\n",cmd_usage[index].cmd);
        else
        {
            if(hw_strlen((char *)cmd_usage[index].cmd) < 8)
                HW_DEBUG("|\t"APP_LOG_COLOR_RED"%s"APP_LOG_COLOR_RESET"\t\t\t\t ->    "APP_LOG_COLOR_BLUE"%s"APP_LOG_COLOR_RESET"\n",cmd_usage[index].cmd,cmd_usage[index].description);
            else if((hw_strlen((char *)cmd_usage[index].cmd) >= 8) && (hw_strlen((char *)cmd_usage[index].cmd) < 16))
                HW_DEBUG("|\t"APP_LOG_COLOR_RED"%s"APP_LOG_COLOR_RESET"\t\t\t ->    "APP_LOG_COLOR_BLUE"%s"APP_LOG_COLOR_RESET"\n",cmd_usage[index].cmd,cmd_usage[index].description);
            else if((hw_strlen((char *)cmd_usage[index].cmd) >= 16) && (hw_strlen((char *)cmd_usage[index].cmd) < 24))
                HW_DEBUG("|\t"APP_LOG_COLOR_RED"%s"APP_LOG_COLOR_RESET"\t\t ->    "APP_LOG_COLOR_BLUE"%s"APP_LOG_COLOR_RESET"\n",cmd_usage[index].cmd,cmd_usage[index].description);
            else
                HW_DEBUG("|\t"APP_LOG_COLOR_RED"%s"APP_LOG_COLOR_RESET"\t ->    "APP_LOG_COLOR_BLUE"%s"APP_LOG_COLOR_RESET"\n",cmd_usage[index].cmd,cmd_usage[index].description);
        }

    }
    printf(APP_LOG_COLOR_YELLOW"----------------------------------------------------------------------------------------------------|"APP_LOG_COLOR_RESET"\n");
}

struct bd_addr_t connect_addr;


void bt_app_init_result(uint8_t status,uint16_t profile_mask)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_init_result(%d) profile_mask(0x%x)\n",status,profile_mask);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_inquiry_status(uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_inquiry_status %d\n",status);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_inquiry_result(struct bd_addr_t *address,uint8_t dev_type,uint8_t *name)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("address:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",address->addr[0],address->addr[1],address->addr[2],\
           address->addr[3],address->addr[4],address->addr[5]);
    printf("type %d\n",dev_type);
    printf("name %s\n",name);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hardware_error(uint8_t reason)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hardware_error reason %d\n",reason);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}



#if BT_BLE_ENABLE > 0
void bt_app_le_inquiry_result(struct bd_addr_t *address,int8_t rssi,uint8_t adv_type,uint8_t adv_size,uint8_t *adv_data)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    bt_le_adv_parse_t bt_le_adv_parse = {0};
    printf("address:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",address->addr[0],address->addr[1],address->addr[2],\
           address->addr[3],address->addr[4],address->addr[5]);
    printf("adv type %d\n",adv_type);
    printf("adv size %d\n",adv_size);
    printf("adv rssi %d\n",rssi);
    printf("adv data:");
    bt_hex_dump(adv_data,adv_size);
    for(bt_le_adv_parse_init(&bt_le_adv_parse,adv_size,adv_data); bt_le_adv_has_more(&bt_le_adv_parse);)
    {
        uint8_t adv_item_size;
        uint8_t adv_item_type;
        uint8_t *adv_item_data;
        bt_le_adv_data_parse(&bt_le_adv_parse,&adv_item_type,&adv_item_size,&adv_item_data);
        printf("adv_item_size(%d)\n",adv_item_size);
        printf("adv_item_type(%d)\n",adv_item_type);
        printf("adv_item_data:\n");
        bt_hex_dump(adv_item_data,adv_item_size);
        switch(adv_item_type)
        {
        case BT_DT_FLAGS:
            break;
        case BT_DT_COMPLETE_LOCAL_NAME:
        {
            uint8_t index = 0;
            printf("LE ADV NAME:");
            for(index = 0; index < adv_item_size; index++)
                printf("%c",adv_item_data[index]);
            printf("\n");
            break;
        }
        case BT_DT_TX_POWER_LEVEL:
            printf("%d dBm\n", *(int8_t*)adv_item_data);
            break;
        default:
            break;
        }
    }
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}


void bt_app_le_inquiry_status(uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_le_inquiry_status %d\n",status);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

uint8_t adv_data[] =
{
    0x08, BT_DT_COMPLETE_LOCAL_NAME, 'B', 'T', '_', 'D', 'E', 'M', 'O',
};
uint8_t adv_data_len = sizeof(adv_data);

#endif


static bt_app_common_cb_t bt_app_common_cb =
{
    bt_app_init_result,
    bt_app_inquiry_status,
    bt_app_inquiry_result,
    bt_app_hardware_error,
#if BT_BLE_ENABLE > 0
    bt_app_le_inquiry_status,
    bt_app_le_inquiry_result,
#endif
};


#if PROFILE_HFP_HF_ENABLE > 0
void bt_app_hfp_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_connect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hfp_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_disconnect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_sco_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_sco_connect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_sco_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_sco_disconnect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}


void bt_app_hfp_signal_strength_ind(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_signal_strength_ind value %d address:\n",value);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_roam_status_ind(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_hfp_roam_status_ind value %d address:\n",value);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_batt_level_ind(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_hfp_batt_level_ind value %d address:\n",value);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_operator(struct bd_addr_t *remote_addr,uint8_t *operator)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_operator operator %s address:\n",operator);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_call_status(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_call_status value %d address:\n",value);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hfp_call_setup(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_call_setup value %d address:\n",value);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_local_pn(struct bd_addr_t *remote_addr,uint8_t *local_pn)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_local_pn %s address:\n",local_pn);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_call_pn(struct bd_addr_t *remote_addr,uint8_t *phone_number)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_call_pn %s address:\n",phone_number);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hfp_manu_id(struct bd_addr_t *remote_addr,uint8_t *mid)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_manu_id %s address:\n",mid);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_hfp_module_id(struct bd_addr_t *remote_addr,uint8_t *mid)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hfp_module_id %s address:\n",mid);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}


static bt_app_hfp_cb_t bt_app_hfp_cb =
{
    bt_app_hfp_connect,
    bt_app_hfp_disconnect,
    bt_app_hfp_sco_connect,
    bt_app_hfp_sco_disconnect,
    bt_app_hfp_signal_strength_ind,
    bt_app_hfp_roam_status_ind,
    bt_app_hfp_batt_level_ind,
    bt_app_hfp_operator,
    bt_app_hfp_call_status,
    bt_app_hfp_call_setup,
    bt_app_hfp_local_pn,
    bt_app_hfp_call_pn,
    bt_app_hfp_manu_id,
    bt_app_hfp_module_id,
};
#endif

#if PROFILE_A2DP_ENABLE > 0
void bt_app_a2dp_signal_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_signal_connect:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}
void bt_app_a2dp_signal_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_signal_disconnect:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_stream_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_stream_connect:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_stream_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_stream_disconnect:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_start(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_start address:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_relase(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_relase:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_suspend(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_suspend:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_a2dp_abort(struct bd_addr_t *remote_addr,uint8_t value)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_a2dp_abort:\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

static bt_app_a2dp_cb_t bt_app_a2dp_cb =
{
    bt_app_a2dp_signal_connect,
    bt_app_a2dp_signal_disconnect,
    bt_app_a2dp_stream_connect,
    bt_app_a2dp_stream_disconnect,
    bt_app_a2dp_start,
    bt_app_a2dp_relase,
    bt_app_a2dp_suspend,
    bt_app_a2dp_abort,
};
#endif

#if PROFILE_AVRCP_ENABLE > 0
void bt_app_avrcp_ctl_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_av_connect:\n");
    bt_addr_dump(remote_addr->addr);

    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_avrcp_ctl_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_av_disconnect:\n");
    bt_addr_dump(remote_addr->addr);

    memset(&connect_addr,0,sizeof(connect_addr));
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

void bt_app_avrcp_support_capabilities(struct bd_addr_t *remote_addr,uint16_t support_cap_mask)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_support_capabilities: mask(0x%x)\n",support_cap_mask);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_app_setting_attr(struct bd_addr_t *remote_addr,uint16_t setting_attr_mask)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_app_setting_attr: mask(0x%x)\n",setting_attr_mask);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_play_status_update(struct bd_addr_t *remote_addr,uint8_t play_status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_play_status_update: play status update(%d)\n",play_status);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_track_change_update(struct bd_addr_t *remote_addr)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_track_change_update\n");
    bt_addr_dump(remote_addr->addr);
    bt_avrcp_controller_get_element_attributes(remote_addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_playpos_change_update(struct bd_addr_t *remote_addr,uint32_t millisecond)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_playpos_change_update ms(%d)\n",millisecond);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_battary_change_update(struct bd_addr_t *remote_addr,uint32_t battary_status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_battary_change_update battary_status(%d)\n",battary_status);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_volume_change_update(struct bd_addr_t *remote_addr,uint32_t volume)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_volume_change_update volume(%d)\n",volume);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_avrcp_element_attr_update(struct bd_addr_t *remote_addr,uint8_t *title,uint8_t *artist,uint8_t *album,uint32_t current_index,uint32_t totol_count,uint32_t total_milliseconds)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_avrcp_element_attr_update\n");
    bt_addr_dump(remote_addr->addr);

    printf("title(%s)\n",title);
    printf("artist(%s)\n",artist);
    printf("album(%s)\n",album);
    printf("current_index(%d/%d)\n",current_index,totol_count);
    printf("total_milliseconds(%d)\n",total_milliseconds);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}

static bt_app_avrcp_cb_t bt_app_avrcp_cb =
{
    bt_app_avrcp_ctl_connect,
    bt_app_avrcp_ctl_disconnect,
    NULL,
    NULL,
    bt_app_avrcp_support_capabilities,
    bt_app_avrcp_app_setting_attr,
    bt_app_avrcp_play_status_update,
    bt_app_avrcp_track_change_update,
    bt_app_avrcp_playpos_change_update,
    bt_app_avrcp_battary_change_update,
    bt_app_avrcp_volume_change_update,
    bt_app_avrcp_element_attr_update,
};
#endif


#if PROFILE_SPP_ENABLE > 0
void bt_app_spp_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_spp_connect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_spp_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);

    printf("bt_app_spp_disconnect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    memset(&connect_addr,0,sizeof(connect_addr));
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_spp_recv_data(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);

    printf("bt_app_spp_recv_data len %d address:\n",data_len);
    bt_addr_dump(remote_addr->addr);
    printf("data is :");
    bt_hex_dump(data,data_len);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

static bt_app_spp_cb_t bt_app_spp_cb =
{
    bt_app_spp_connect,
    bt_app_spp_disconnect,
    bt_app_spp_recv_data,
};
#endif

#if PROFILE_HID_ENABLE > 0
void bt_app_hid_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hid_connect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hid_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hid_disconnect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    memset(&connect_addr,0,sizeof(connect_addr));
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_hid_interrupt_recv_data(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_hid_interrupt_recv_data len %d address:\n",data_len);
    bt_addr_dump(remote_addr->addr);
    printf("data is :");
    bt_hex_dump(data,data_len);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

static bt_app_hid_cb_t bt_app_hid_cb =
{
    bt_app_hid_connect,
    bt_app_hid_disconnect,
    bt_app_hid_interrupt_recv_data,
};
#endif

#if PROFILE_PBAP_ENABLE > 0
void bt_app_pbap_connect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_pbap_connect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_pbap_disconnect(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_pbap_disconnect status %d address:\n",status);
    bt_addr_dump(remote_addr->addr);
    memset(&connect_addr,0,sizeof(connect_addr));
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_pbap_query_repositories_size(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t size)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_pbap_query_repositories_size,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) size(%d)\n",repositories,type,size);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_pbap_download_pb_status(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_pbap_download_pb_status,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) status(%d)\n",repositories,type,status);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_pbap_download_vcardlist_status(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_pbap_download_vcardlist_status,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("repositories(%d) type(%d) status(%d)\n",repositories,type,status);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}


static bt_app_pbap_cb_t bt_app_pbap_cb =
{
    bt_app_pbap_connect,
    bt_app_pbap_disconnect,
    bt_app_pbap_query_repositories_size,
    bt_app_pbap_download_pb_status,
    bt_app_pbap_download_vcardlist_status,
};

#endif


#if BT_BLE_ENABLE > 0
void bt_app_gatt_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gatt_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);

    connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_gatt_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gatt_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_gattc_mtu_value(struct bd_addr_t *remote_addr,uint16_t mtu)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gattc_mtu_value,mtu(%d) address is :\n",mtu);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_gattc_discovery_primary_service(struct bd_addr_t *remote_addr,gatt_client_pri_service_t *pri_service,uint16_t count)
{
	uint8_t index = 0;
	uint16_t start_handle;
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gattc_discovery_primary_service,count(0x%x) address is :\n",count);
    bt_addr_dump(remote_addr->addr);
	for(index = 0; index < count; index++)
	{
		start_handle = pri_service[index].end_handle+1;
		if(pri_service[index].uuid.len == LEN_UUID_16)
			printf("primary service[%d] start_handle(%d) end_handle(%d) uuid16(0x%x)\n",index,pri_service[index].start_handle,pri_service[index].end_handle,pri_service[index].uuid.uu.uuid16);
		else
		{
			printf("primary service[%d] start_handle(%d) end_handle(%d)\n",index,pri_service[index].start_handle,pri_service[index].end_handle);
			bt_uuid128_dump(pri_service[index].uuid.uu.uuid128);
		}
	}

	bt_gatt_client_discovery_pri_service(&connect_addr,start_handle,0xffff);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_gattc_discovery_uuid_primary_service(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle)
{
	printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gattc_discovery_uuid_primary_service,start_handle(%d) end_handle(%d) address is :\n",start_handle,end_handle);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_gattc_discovery_char(struct bd_addr_t *remote_addr,uint16_t attribute_handle,uint16_t char_value_handle,uint8_t properties,uint16_t uuid16,uint8_t *uuid128)
{
	printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gattc_discovery_char,attribute_handle(%d) char_value_handle(%d) properties(0x%x) uuid16(0x%x) address is :\n",attribute_handle,char_value_handle,properties,uuid16);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}


bt_gatt_client_cbs_t bt_app_gattc_cb =
{
    bt_app_gattc_mtu_value,
    bt_app_gattc_discovery_primary_service,
    bt_app_gattc_discovery_uuid_primary_service,
    bt_app_gattc_discovery_char,
};


void bt_app_gatts_mtu_value(struct bd_addr_t *remote_addr,uint16_t mtu)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_gatts_mtu_value,mtu(%d) address is :\n",mtu);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}


bt_gatt_server_cbs_t bt_app_gatts_cb =
{
    bt_app_gatts_mtu_value,
};



static bt_gatt_cbs_t bt_app_gatt_cb =
{
    bt_app_gatt_connect_set_up,
    bt_app_gatt_connect_realease,
    &bt_app_gattc_cb,
    &bt_app_gatts_cb,
};


void bt_app_smp_connect_set_up(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_smp_connect_set_up,address is :\n");
    bt_addr_dump(remote_addr->addr);
	connect_addr.addr[5] = remote_addr->addr[5];
    connect_addr.addr[4] = remote_addr->addr[4];
    connect_addr.addr[3] = remote_addr->addr[3];
    connect_addr.addr[2] = remote_addr->addr[2];
    connect_addr.addr[1] = remote_addr->addr[1];
    connect_addr.addr[0] = remote_addr->addr[0];
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);

}
void bt_app_smp_connect_realease(struct bd_addr_t *remote_addr,uint8_t status)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_smp_connect_realease,address is :\n");
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_smp_passkey_display(struct bd_addr_t *remote_addr,uint32_t passkey)
{
    printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_smp_passkey_display passkey(%d),address is :\n",passkey);
    bt_addr_dump(remote_addr->addr);
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}

void bt_app_smp_passkey_input(struct bd_addr_t *remote_addr,uint32_t *passkey)
{
	printf(APP_LOG_COLOR_BLUE"%s\n",BT_SPLIT_NAME);
    printf("bt_app_smp_passkey_input,address is :\n");
    bt_addr_dump(remote_addr->addr);

	printf("enter passkey\r\n");
    scanf("%d",passkey);
	
    printf("%s"APP_LOG_COLOR_RESET"\n",BT_SPLIT_NAME);
}


static bt_smp_cbs_t bt_app_smp_cb =
{
    bt_app_smp_connect_set_up,
    bt_app_smp_connect_realease,
    bt_app_smp_passkey_display,
    bt_app_smp_passkey_input
};


#endif


static bt_app_cb_t bt_app_cb =
{
    &bt_app_common_cb,

#if PROFILE_SPP_ENABLE > 0
    &bt_app_spp_cb,
#else
    NULL,
#endif

#if PROFILE_HFP_HF_ENABLE > 0
    &bt_app_hfp_cb,
#else
    NULL,
#endif

#if PROFILE_A2DP_ENABLE > 0
    &bt_app_a2dp_cb,
#else
    NULL,
#endif

#if PROFILE_AVRCP_ENABLE > 0
    &bt_app_avrcp_cb,
#else
    NULL,
#endif

#if PROFILE_HID_ENABLE > 0
    &bt_app_hid_cb,
#else
    NULL,
#endif

#if PROFILE_PBAP_ENABLE > 0
    &bt_app_pbap_cb,
#else
    NULL,
#endif

#if BT_BLE_ENABLE > 0
    &bt_app_gatt_cb,
    &bt_app_smp_cb,
#else
    NULL,
    NULL,
#endif

};



#define LINUX_SPP_SEND_STRING "Hello,I am SPP in Linux"
uint8_t spp_buf[2048];
uint8_t shell_parse(uint8_t *shell_string)
{


    if(hw_strcmp(BT_START_CMD,(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt start\n");
        bt_start(&bt_app_cb);
        return HW_ERR_OK;
    }

    if(hw_strcmp(BT_STOP_CMD,(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        bt_stop();
        return HW_ERR_OK;
    }

	if(hw_strncmp(BT_WRITE_IAC_CMD,(const char*)shell_string,hw_strlen(BT_WRITE_IAC_CMD)) == 0)
    {
    	uint32_t iac[2] = {BT_INQUIRY_LIAC,BT_INQUIRY_LIAC+1};
        HW_DEBUG("SHELL:operate bt write iac\n");
		
        bt_write_current_iac(sizeof(iac)/sizeof(uint32_t),iac);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_INQUIRY_CMD,(const char*)shell_string,hw_strlen(BT_INQUIRY_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt inquiry\n");
        bt_start_inquiry(0x30,HCI_INQUIRY_MAX_DEV);
        return HW_ERR_OK;
    }

    if(hw_strcmp(BT_CANCEL_INQUIRY_CMD,(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt cancel inquiry\n");
        bt_stop_inquiry();
        return 0;
    }

#if BT_BLE_ENABLE > 0
    if(hw_strncmp(BT_LE_INQUIRY_CMD,(const char*)shell_string,hw_strlen(BT_LE_INQUIRY_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate ble inquiry\n");
        bt_le_start_inquiry();
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_LE_INQUIRY_CANCEL_CMD,(const char*)shell_string,hw_strlen(BT_LE_INQUIRY_CANCEL_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate ble cancel inquiry\n");
        bt_le_stop_inquiry();
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_LE_ADV_ENABLE_CMD,(const char*)shell_string,hw_strlen(BT_LE_ADV_ENABLE_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate ble start advertising\n");
        bt_le_set_adv_enable(adv_data_len,adv_data);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_LE_ADV_DISABLE_CMD,(const char*)shell_string,hw_strlen(BT_LE_ADV_DISABLE_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate ble stop advertising\n");
        bt_le_set_adv_disable();
        return HW_ERR_OK;
    }
#endif

#if PROFILE_SPP_ENABLE > 0
    if(hw_strncmp(BT_SPP_SEND_CMD,(const char*)shell_string,hw_strlen(BT_SPP_SEND_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt spp send \n");
        spp_send_data(&connect_addr,(uint8_t *)LINUX_SPP_SEND_STRING,hw_strlen(LINUX_SPP_SEND_STRING));
        return HW_ERR_OK;
    }

    if(hw_strncmp("SPP_TEST1",(const char*)shell_string,hw_strlen("SPP_TEST1")) == 0)
    {
        uint16_t index = 0;
        for(index = 0; index < sizeof(spp_buf); index++)
            spp_buf[index] = index%0xff;
        HW_DEBUG("SHELL:operate bt spp send1 len\n");
        spp_send_data(&connect_addr,(uint8_t *)spp_buf,990);
        return HW_ERR_OK;
    }

    if(hw_strncmp("SPP_TEST2",(const char*)shell_string,hw_strlen("SPP_TEST2")) == 0)
    {
        uint16_t index = 0;
        for(index = 0; index < sizeof(spp_buf); index++)
            spp_buf[index] = index%0xff;
        HW_DEBUG("SHELL:operate bt spp send2 len\n");

        spp_send_data(&connect_addr,(uint8_t *)spp_buf+1,990);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_SPP_CON_CMD,(const char*)shell_string,hw_strlen(BT_SPP_CON_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate spp CON\n");

        spp_connect(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_SPP_DISCON_CMD,(const char*)shell_string,hw_strlen(BT_SPP_DISCON_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate spp DISCON\n");

        spp_disconnect(&connect_addr);
        return HW_ERR_OK;
    }
#endif

#if PROFILE_PBAP_ENABLE > 0
    if(hw_strncmp("PBAP_CON",(const char*)shell_string,hw_strlen("PBAP_CON")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP CON\n");

        bt_pbap_client_connect(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_DISCON",(const char*)shell_string,hw_strlen("PBAP_DISCON")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_DISCON\n");

        bt_pbap_client_disconnect(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_LP",(const char*)shell_string,hw_strlen("PBAP_LP")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LP\n");

        bt_pbap_client_download_phonebook(&connect_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_LI",(const char*)shell_string,hw_strlen("PBAP_LI")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        bt_pbap_client_download_phonebook(&connect_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_LO",(const char*)shell_string,hw_strlen("PBAP_LO")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LO\n");

        bt_pbap_client_download_phonebook(&connect_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_LM",(const char*)shell_string,hw_strlen("PBAP_LM")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LM\n");

        bt_pbap_client_download_phonebook(&connect_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_LC",(const char*)shell_string,hw_strlen("PBAP_LC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LC\n");

        bt_pbap_client_download_phonebook(&connect_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_QUEQY_LP",(const char*)shell_string,hw_strlen("PBAP_QUEQY_LP")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_QUEQY_LP\n");

        bt_pbap_client_query_phonebook_size(&connect_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_QUEQY_LIC",(const char*)shell_string,hw_strlen("PBAP_QUEQY_LIC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_QUEQY_LIC\n");

        bt_pbap_client_query_phonebook_size(&connect_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_QUEQY_LOC",(const char*)shell_string,hw_strlen("PBAP_QUEQY_LOC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_QUEQY_LOC\n");

        bt_pbap_client_query_phonebook_size(&connect_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_QUEQY_LMC",(const char*)shell_string,hw_strlen("PBAP_QUEQY_LMC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_QUEQY_LMC\n");

        bt_pbap_client_query_phonebook_size(&connect_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_QUEQY_LCC",(const char*)shell_string,hw_strlen("PBAP_QUEQY_LCC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_QUEQY_LCC\n");

        bt_pbap_client_query_phonebook_size(&connect_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_SPL",(const char*)shell_string,hw_strlen("PBAP_SPL")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_SPL\n");

        bt_pbap_client_set_path(&connect_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_SPC",(const char*)shell_string,hw_strlen("PBAP_SPC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_SPC\n");

        bt_pbap_client_set_path(&connect_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_DVL",(const char*)shell_string,hw_strlen("PBAP_DVL")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_DVL\n");

        bt_pbap_client_download_vcard_list(&connect_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_DVC",(const char*)shell_string,hw_strlen("PBAP_DVC")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_DVC\n");

        bt_pbap_client_download_vcard_list(&connect_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_DVE",(const char*)shell_string,hw_strlen("PBAP_DVE")) == 0)
    {
        static uint16_t dn_entry_number = 1;
        HW_DEBUG("SHELL:operate PBAP PBAP_DVE\n");

        bt_pbap_client_download_vcard_entry(&connect_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE,dn_entry_number++);
        return HW_ERR_OK;
    }

    if(hw_strncmp("PBAP_ABORT",(const char*)shell_string,hw_strlen("PBAP_ABORT")) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_ABORT\n");

        bt_pbap_client_download_abort(&connect_addr);
        return HW_ERR_OK;
    }

#endif


#if PROFILE_HFP_HF_ENABLE > 0
    if(hw_strncmp(BT_HFP_CON_CMD,(const char*)shell_string,hw_strlen(BT_HFP_CON_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate HFP CON\n");

        hfp_hf_connect(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_DISCON_CMD,(const char*)shell_string,hw_strlen(BT_HFP_DISCON_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate HFP DISCON\n");

        hfp_hf_disconnect(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_AUDIO_TRANSFER_CMD,(const char*)shell_string,hw_strlen(BT_HFP_AUDIO_TRANSFER_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate HFP AUDIO TRANSFER\n");

        bt_hfp_hf_audio_transfer(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_ANSWER_CMD,(const char*)shell_string,hw_strlen(BT_HFP_ANSWER_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate HFP ANSWER INCOMING CALL\n");

        bt_hfp_hf_accept_incoming_call(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_END_CALL_CMD,(const char*)shell_string,hw_strlen(BT_HFP_END_CALL_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt end call\n");
        bt_hfp_hf_end_call(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_CALLOUT_PN_CMD,(const char*)shell_string,hw_strlen(BT_HFP_CALLOUT_PN_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate call out number 10086\n");
        bt_hfp_hf_callout_by_number(&connect_addr,(uint8_t *)"10086");
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_CALLOUT_MEM_CMD,(const char*)shell_string,hw_strlen(BT_HFP_CALLOUT_MEM_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        bt_hfp_hf_callout_by_memory(&connect_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_CALLOUT_LN_CMD,(const char*)shell_string,hw_strlen(BT_HFP_CALLOUT_LN_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        bt_hfp_hf_callout_by_last(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_LOCAL_PN_CMD,(const char*)shell_string,hw_strlen(BT_HFP_LOCAL_PN_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate local number\n");
        bt_hfp_hf_get_local_phone_number(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_CALL_LIST_CMD,(const char*)shell_string,hw_strlen(BT_HFP_CALL_LIST_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate bt get call list\n");
        bt_hfp_hf_get_call_list(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_DISABLE_ECNR_CMD,(const char*)shell_string,hw_strlen(BT_HFP_DISABLE_ECNR_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate disable ag ecnr\n");
        bt_hfp_hf_disable_ecnr(&connect_addr);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_VOICE_RECOG_ENABLE_CMD,(const char*)shell_string,hw_strlen(BT_HFP_VOICE_RECOG_ENABLE_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate voice_recognition enable\n");
        bt_hfp_hf_set_voice_recognition(&connect_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_VOICE_RECOG_DISABLE_CMD,(const char*)shell_string,hw_strlen(BT_HFP_VOICE_RECOG_DISABLE_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate voice_recognition disable\n");
        bt_hfp_hf_set_voice_recognition(&connect_addr,0);
        return HW_ERR_OK;
    }


    if(hw_strncmp(BT_HFP_DTMF_CMD,(const char*)shell_string,hw_strlen(BT_HFP_DTMF_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate hfp active call dtmf\n");
        bt_hfp_hf_transmit_dtmf(&connect_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_VGM_CMD,(const char*)shell_string,hw_strlen(BT_HFP_VGM_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate VGM\n");
        bt_hfp_hf_set_mic_volume(&connect_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_VGS_CMD,(const char*)shell_string,hw_strlen(BT_HFP_VGS_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate VGM\n");
        bt_hfp_hf_set_spk_volume(&connect_addr,1);
        return HW_ERR_OK;
    }


    if(hw_strncmp(BT_HFP_GET_MANU_ID_CMD,(const char*)shell_string,hw_strlen(BT_HFP_GET_MANU_ID_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate get manufacturer name\n");
        bt_hfp_hf_get_manufacturer_id(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_GET_MODULE_ID_CMD,(const char*)shell_string,hw_strlen(BT_HFP_GET_MODULE_ID_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate get module id\n");
        bt_hfp_hf_get_model_id(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_HFP_SET_BATT_LEVEL_CMD,(const char*)shell_string,hw_strlen(BT_HFP_SET_BATT_LEVEL_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate set batt level\n");
        bt_hfp_hf_send_batt_level(&connect_addr,60);
        return HW_ERR_OK;
    }


    if(hw_strncmp(BT_HFP_GET_OP_NAME_CMD,(const char*)shell_string,hw_strlen(BT_HFP_GET_OP_NAME_CMD)) == 0)
    {
        HW_DEBUG("SHELL:operate get ag operator name\n");
        bt_hfp_hf_get_operator(&connect_addr);
        return HW_ERR_OK;
    }

#endif

#if PROFILE_AVRCP_ENABLE > 0
    if(hw_strncmp("AVRCP_LIST_APP_ATTR",(const char*)shell_string,hw_strlen("AVRCP_LIST_APP_ATTR")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_LIST_APP_ATTR\n");
        bt_avrcp_controller_list_app_setting_attr(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_PLAY_STATUS",(const char*)shell_string,hw_strlen("AVRCP_PLAY_STATUS")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_PLAY_STATUS\n");
        bt_avrcp_controller_get_play_status(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_GET_ID3",(const char*)shell_string,hw_strlen("AVRCP_GET_ID3")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_GET_ID3\n");
        bt_avrcp_controller_get_element_attributes(&connect_addr);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_PLAY",(const char*)shell_string,hw_strlen("AVRCP_PLAY")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_PLAY\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_PLAY);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_PAUSE",(const char*)shell_string,hw_strlen("AVRCP_PAUSE")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_PLAY\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_PAUSE);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_PREV",(const char*)shell_string,hw_strlen("AVRCP_PREV")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_PREV\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_BACKWARD);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_NEXT",(const char*)shell_string,hw_strlen("AVRCP_NEXT")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_NEXT\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_FORWARD);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_FAST_BACKWARD",(const char*)shell_string,hw_strlen("AVRCP_FAST_BACKWARD")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_FAST_BACKWARD\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_FAST_BACKWARD);
        return HW_ERR_OK;
    }

    if(hw_strncmp("AVRCP_FAST_FORWARD",(const char*)shell_string,hw_strlen("AVRCP_FAST_FORWARD")) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_FAST_FORWARD\n");
        bt_avrcp_controller_control(&connect_addr,AVRCP_CONTROL_ID_FAST_FORWARD);
        return HW_ERR_OK;
    }

#endif

#if PROFILE_HID_ENABLE > 0
    if(hw_strncmp("HID_MOUSE_L",(const char*)shell_string,hw_strlen("HID_MOUSE_L")) == 0)
    {
        uint8_t report[3] = {0,-20,0};
        HW_DEBUG("SHELL:HID_MOUSE_L\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_MOUSE_R",(const char*)shell_string,hw_strlen("HID_MOUSE_R")) == 0)
    {
        uint8_t report[3] = {0,20,0};
        HW_DEBUG("SHELL:HID_MOUSE_R\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_MOUSE_U",(const char*)shell_string,hw_strlen("HID_MOUSE_U")) == 0)
    {
        uint8_t report[3] = {0,0,-20};
        HW_DEBUG("SHELL:HID_MOUSE_U\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_MOUSE_D",(const char*)shell_string,hw_strlen("HID_MOUSE_D")) == 0)
    {
        uint8_t report[3] = {0,0,20};
        HW_DEBUG("SHELL:HID_MOUSE_D\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_MOUSE_L_CLICK",(const char*)shell_string,hw_strlen("HID_MOUSE_L_CLICK")) == 0)
    {
        uint8_t report[3] = {1,0,0};
        HW_DEBUG("SHELL:HID_MOUSE_L_CLICK\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_MOUSE_R_CLICK",(const char*)shell_string,hw_strlen("HID_MOUSE_R_CLICK")) == 0)
    {
        uint8_t report[3] = {2,0,0};
        HW_DEBUG("SHELL:HID_MOUSE_R_CLICK\n");

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));
        return HW_ERR_OK;
    }

    if(hw_strncmp("HID_KEYBOARD_INPUT",(const char*)shell_string,hw_strlen("HID_KEYBOARD_INPUT")) == 0)
    {
        uint8_t keycode = 0xff;
        uint8_t report[9] = {0};

        HW_DEBUG("SHELL:HID_KEYBOARD_INPUT\n");
        bt_hid_find_keycode(&keycode,'a');
        HW_DEBUG("KEY INDEX %d\n",keycode);
        report[3] = keycode;

        bt_hid_interupt_report(&connect_addr,report,sizeof(report));

        return HW_ERR_OK;
    }
#endif

#if BT_BLE_ENABLE > 0
	if(hw_strncmp(BT_SMP_SEC_REQ_CMD,(const char*)shell_string,hw_strlen(BT_SMP_SEC_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_SMP_SEC_REQ_CMD\n");
        bt_smp_security_request(&connect_addr);

        return HW_ERR_OK;
    }
	
    if(hw_strncmp(BT_GATTC_MTU_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_MTU_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_MTU_REQ_CMD\n");
        bt_gatt_client_exchange_mtu(&connect_addr,GATT_BLE_MTU_SIZE);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_GATTC_PRIMARY_DISCOVERY_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_PRIMARY_DISCOVERY_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_PRIMARY_DISCOVERY_REQ_CMD\n");
        bt_gatt_client_discovery_pri_service(&connect_addr,1,0xffff);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_PRIMARY_UUID_DISCOVERY_REQ_CMD\n");
        bt_gatt_client_discovery_pri_service_uuid(&connect_addr,1,0xffff,BT_UUID_SERVCLASS_GAP_SERVER,NULL);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_GATTC_RELATIONSHIP_DISCOVERY_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_RELATIONSHIP_DISCOVERY_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_RELATIONSHIP_DISCOVERY_REQ_CMD\n");
        bt_gatt_client_find_include(&connect_addr,1,0xffff);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_GATTC_CHAR_DISCOVERY_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_CHAR_DISCOVERY_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_CHAR_DISCOVERY_REQ_CMD\n");
        bt_gatt_client_discovery_characteristics(&connect_addr,1,0xffff);

        return HW_ERR_OK;
    }

	if(hw_strncmp(BT_GATTC_CHARD_DISCOVERY_REQ_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_CHARD_DISCOVERY_REQ_CMD)) == 0)
    {

        HW_DEBUG("SHELL:BT_GATTC_CHAR_DISCOVERY_REQ_CMD\n");
        bt_gatt_client_discovery_char_des(&connect_addr,1,0xffff);

        return HW_ERR_OK;
    }

    if(hw_strncmp(BT_GATTC_CHAR_READ_VALUE_CMD,(const char*)shell_string,hw_strlen(BT_GATTC_CHAR_READ_VALUE_CMD)) == 0)
    {
		uint16_t handle;
        printf("Input GATT handle\n");
        scanf("%hd",(uint16_t *)&handle);
		
        HW_DEBUG("SHELL:BT_GATTC_CHAR_READ_VALUE_CMD\n");	
        bt_gatt_client_read_char_value(&connect_addr,handle);

        return HW_ERR_OK;
    }


#if PROFILE_BAS_ENABLE > 0
    if(hw_strncmp("BAS_LEVEL_UPDATE",(const char*)shell_string,hw_strlen("BAS_LEVEL_UPDATE")) == 0)
    {
        static uint8_t bat_level = 100;

        HW_DEBUG("SHELL:BAS_LEVEL_UPDATE\n");
        bas_batt_level_notification(bat_level==0?bat_level=100:bat_level--);

        return HW_ERR_OK;
    }
#endif


#endif



    show_usage();
    return HW_ERR_SHELL_NO_CMD;
}


#define UART1_MAX_REV	1024
uint8_t uart_rev_buffer[UART1_MAX_REV];
uint8_t uart_rev_len;

void *timer_thread(void *data)
{
    printf("timer_thread runing...\n");
    while(1)
    {
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1000/CONF_BSP_TICKS_PER_SEC * 1000;

        select(0, NULL, NULL, NULL, &tv);
        sys_time += 1000/CONF_BSP_TICKS_PER_SEC;

        utimer_polling();
    }
}

#define STDIN_BUF_SIZE 128
uint8_t stdin_buf[STDIN_BUF_SIZE] = {0};
uint8_t stdin_recv_len = 0;

void *stdin_process_thread(void *data)
{

    while(1)
    {
        fd_set read_fd;
        int result = 0;

        /* monitor uart rx */
        FD_ZERO (&read_fd);
        FD_SET (0, &read_fd);

        if ((result = select (1, &read_fd, NULL, NULL, NULL)) == -1)
        {
            printf("ERROR:file[%s],function[%s],line[%d] select fail\n",__FILE__,__FUNCTION__,__LINE__);
        }

        if(result > 0 && FD_ISSET ( 0, &read_fd))
        {

            memset(stdin_buf,0,STDIN_BUF_SIZE);
            int read_result = read(0,stdin_buf,STDIN_BUF_SIZE);
            stdin_recv_len = read_result;
            //printf("--------1---------len %d\n",read_result);
            //bt_hex_dump(stdin_buf,read_result);
            //printf("--------2----------\n");

        }
    }
}


void stdin_process_init()
{

    pthread_t thread_stdin_id;
    pthread_create(&thread_stdin_id, NULL, stdin_process_thread, NULL);
}
void stdin_process_run()
{
    if(stdin_recv_len != 0)
    {
        shell_parse(stdin_buf);
        stdin_recv_len = 0;
    }
}



void board_init()
{
    pthread_t thread_timer_id;

    last_sys_time = sys_time;
    utimer_init();

    bt_start(&bt_app_cb);
    pthread_create(&thread_timer_id, NULL, timer_thread, NULL);


}



extern struct phybusif_cb uart_if;
int main()
{
    stdin_process_init();
    board_init();
    while(1)
    {

        stdin_process_run();
        if(phybusif_input(&uart_if) != BT_ERR_OK)
            usleep(1000);

        if(sys_time - last_sys_time > 1000)
        {
            //printf("bt stack running\n");
            last_sys_time = sys_time;
            l2cap_tmr();
            rfcomm_tmr();
        }


    }
}
