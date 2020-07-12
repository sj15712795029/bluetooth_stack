//#include "hw_misc.h"
//#include "board_wrapper.h"
#include "bt_wrapper.h"
//#include "bt_hfp_hf.h"

static const uint8_t usage[]=
{
    "usage:\n"
    "CMD\t\t\tdescription\n"
    "BT_START\t\tStart bluetooth stack\n"
    "BT_STOP\t\t\tStop blueooth stack\n"
    "BT_INQUIRY\t\tInquiry device\n"

};


#define BT_START_CMD "BT_START"
#define BT_START_DES "Start bluetooth stack"
#define BT_STOP_CMD "BT_STOP"
#define BT_START_DES "Stop blueooth stack"
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
#define BT_LE_INQUIRY_CANCEL_CMD "BT_LE_INQUIRY_STOP"
#define BT_LE_INQUIRY_CANCEL_DES "BLE cancel Inquiry device"
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
#define BT_HFP_SCO_CON_CMD "HFP_SCO_CON"
#define BT_HFP_SCO_CON_DES "Connect hfp sco"
#define BT_HFP_SCO_DISCON_CMD "HFP_SCO_DISCON"
#define BT_HFP_SCO_DISCON_DES "Disconnect hfp sco"
#define BT_HFP_ANSWER_CMD "HFP_ANSWER"
#define BT_HFP_ANSWER_DES "Answer the incoming call"
#define BT_HFP_CALLOUT_PN_CMD "HFP_CALLOUT_PN"
#define BT_HFP_CALLOUT_PN_DES "Call out phone number(10086)"
#define BT_HFP_CALLOUT_MEM_CMD "HFP_CALLOUT_MEM"
#define BT_HFP_CALLOUT_MEM_DES "Call out phone number with memory 1"
#define BT_HFP_CALLOUT_LN_CMD "HFP_CALLOUT_LC"
#define BT_HFP_CALLOUT_LN_DES "Call out last number"




typedef struct
{
    uint8_t *cmd;
    uint8_t *description;
} cmd_desctiption_t;

cmd_desctiption_t cmd_usage[] =
{
    {BT_START_CMD,BT_START_DES},
    {BT_STOP_CMD,BT_START_DES},
    {BT_INQUIRY_CMD,BT_INQUIRY_CMD},
    	{BT_CANCEL_INQUIRY_CMD,BT_CANCEL_INQUIRY_DES},
    	{BT_PERIOID_INQUIRY_CMD,BT_PERIOID_INQUIRY_DES},
    	{BT_CANCEL_PERIOID_INQUIRY_CMD,BT_CANCEL_PERIOID_INQUIRY_DES},
    {BT_LE_INQUIRY_CMD,BT_LE_INQUIRY_DES},
    {BT_LE_INQUIRY_CANCEL_CMD,BT_LE_INQUIRY_CANCEL_DES},
    {BT_SPP_CON_CMD,BT_SPP_CON_DES},
    {BT_SPP_SEND_CMD,BT_SPP_SEND_DES},
    {BT_SPP_DISCON_CMD,BT_SPP_DISCON_DES},
    {BT_HFP_CON_CMD,BT_HFP_CON_DES},
    {BT_HFP_DISCON_CMD,BT_HFP_DISCON_DES},
    {BT_HFP_SCO_CON_CMD,BT_HFP_SCO_CON_DES},
    {BT_HFP_SCO_DISCON_CMD,BT_HFP_SCO_DISCON_DES},
    {BT_HFP_ANSWER_CMD,BT_HFP_ANSWER_DES},
    {BT_HFP_CALLOUT_PN_CMD,BT_HFP_CALLOUT_PN_DES},
    {BT_HFP_CALLOUT_MEM_CMD,BT_HFP_CALLOUT_MEM_DES},
    {BT_HFP_CALLOUT_LN_CMD,BT_HFP_CALLOUT_LN_DES},
};


void show_usage()
{
    uint32_t index = 0;
    for(index = 0; index < sizeof(cmd_usage)/sizeof(cmd_desctiption_t); index++)
    {
        printf("CMD(%s) -> DESCRIPTION(%s)\n",cmd_usage[index].cmd,cmd_usage[index].description);
    }
}
uint8_t shell_parse(uint8_t *shell_string)
{
    struct bd_addr_t oppo_addr;
    oppo_addr.addr[5] = 0x9c;
    oppo_addr.addr[4] = 0x0c;
    oppo_addr.addr[3] = 0xdf;
    oppo_addr.addr[2] = 0x24;
    oppo_addr.addr[1] = 0x7f;
    oppo_addr.addr[0] = 0x0a;

    struct bd_addr_t iphone_addr;
    iphone_addr.addr[5] = 0x94;
    iphone_addr.addr[4] = 0xbf;
    iphone_addr.addr[3] = 0x2d;
    iphone_addr.addr[2] = 0x51;
    iphone_addr.addr[1] = 0xb2;
    iphone_addr.addr[0] = 0x0e;

	struct bd_addr_t samsungs7_addr;
    samsungs7_addr.addr[5] = 0xe4;
    samsungs7_addr.addr[4] = 0x58;
    samsungs7_addr.addr[3] = 0xb8;
    samsungs7_addr.addr[2] = 0x42;
    samsungs7_addr.addr[1] = 0x65;
    samsungs7_addr.addr[0] = 0xa3;
    if(strncmp("BT_START",(const char*)shell_string,strlen(BT_START_CMD)) == 0)
    {
        printf("SHELL:operate bt start\n");
        bt_start();
        return 0;
    }
    if(strncmp("BT_STOP",(const char*)shell_string,strlen(BT_STOP_CMD)) == 0)
    {
        printf("SHELL:operate bt stop\n");
        bt_stop();
        return 0;
    }
    if(strncmp(BT_INQUIRY_CMD,(const char*)shell_string,strlen(BT_INQUIRY_CMD)) == 0)
    {
        printf("SHELL:operate bt inquiry\n");
        bt_start_inquiry(0x30,0);
        return 0;
    }

	if(strncmp(BT_CANCEL_INQUIRY_CMD,(const char*)shell_string,strlen(BT_CANCEL_INQUIRY_CMD)) == 0)
    {
        printf("SHELL:operate bt cancel inquiry\n");
        bt_stop_inquiry();
        return 0;
    }

	if(strncmp(BT_PERIOID_INQUIRY_CMD,(const char*)shell_string,strlen(BT_PERIOID_INQUIRY_CMD)) == 0)
    {
        printf("SHELL:operate bt perioid inquiry\n");
        hci_start_periodic_inquiry(0x02,0x10,0x30,0);
        return 0;
    }

	if(strncmp(BT_CANCEL_PERIOID_INQUIRY_CMD,(const char*)shell_string,strlen(BT_CANCEL_PERIOID_INQUIRY_CMD)) == 0)
    {
        printf("SHELL:operate bt cancel perioid inquiry\n");
        bt_stop_periodic_inquiry();
        return 0;
    }

	
    if(strncmp("BT_LE_INQUIRY",(const char*)shell_string,strlen(BT_LE_INQUIRY_CMD)) == 0)
    {
        printf("SHELL:operate BLE inquiry\n");
        bt_le_inquiry(1);
        return 0;
    }
    if(strncmp("BT_LE_INQUIRY_STOP",(const char*)shell_string,strlen(BT_LE_INQUIRY_CANCEL_CMD)) == 0)
    {
        printf("SHELL:operate cancel BLE inquriy\n");
        bt_le_inquiry(0);
        return 0;
    }

    if(strncmp("SPP_CON",(const char*)shell_string,strlen(BT_SPP_CON_CMD)) == 0)
    {
        printf("SHELL:operate spp CON\n");

        spp_connect(&oppo_addr);
        return 0;
    }

    if(strncmp("SPP_SEND",(const char*)shell_string,strlen(BT_SPP_SEND_CMD)) == 0)
    {
        printf("SHELL:operate spp send\n");
        spp_send_data(&oppo_addr,"Hello SPP",strlen("Hello SPP"));
        return 0;
    }



    if(strncmp("SPP_DISCON",(const char*)shell_string,strlen(BT_SPP_DISCON_CMD)) == 0)
    {
        printf("SHELL:operate spp DISCON\n");

        spp_disconnect(&oppo_addr);
        return 0;
    }

    if(strncmp("HFP_CON",(const char*)shell_string,strlen(BT_HFP_CON_CMD)) == 0)
    {
        printf("SHELL:operate HFP CON\n");

        hfp_hf_connect(&oppo_addr);
        //hfp_hf_connect(&samsungs7_addr);
        
        return 0;
    }

    if(strncmp("HFP_DISCON",(const char*)shell_string,strlen(BT_HFP_DISCON_CMD)) == 0)
    {
        printf("SHELL:operate HFP DISCON\n");

        hfp_hf_disconnect(&oppo_addr);
        return 0;
    }

    if(strncmp("HFP_SCO_CON",(const char*)shell_string,strlen(BT_HFP_SCO_CON_CMD)) == 0)
    {
        printf("SHELL:operate HFP SCO CON\n");

        hfp_hf_audio_connect(&oppo_addr);
        return 0;
    }

    if(strncmp("HFP_SCO_DISCON",(const char*)shell_string,strlen(BT_HFP_SCO_DISCON_CMD)) == 0)
    {
        printf("SHELL:operate HFP SCO DISCON\n");

        hfp_hf_audio_disconnect(&oppo_addr);
        return 0;
    }

    if(strncmp("HFP_ANSWER",(const char*)shell_string,strlen(BT_HFP_ANSWER_CMD)) == 0)
    {
        printf("SHELL:operate HFP ANSWER INCOMING CALL\n");

        hfp_hf_answer_incoming_call(&oppo_addr);
        return 0;
    }

    if(strncmp("HFP_CALLOUT_PN",(const char*)shell_string,strlen(BT_HFP_CALLOUT_PN_CMD)) == 0)
    {
        printf("SHELL:operate call out number 10086\n");
        hfp_hf_callout_with_phone_number(&oppo_addr,"10086");
        return 0;
    }

    if(strncmp("HFP_CALLOUT_MEM",(const char*)shell_string,strlen(BT_HFP_CALLOUT_MEM_CMD)) == 0)
    {
        printf("SHELL:operatecall out number with memory\n");
        hfp_hf_callout_with_memory(&oppo_addr,1);
        return 0;
    }

    if(strncmp("HFP_CALLOUT_LC",(const char*)shell_string,strlen(BT_HFP_CALLOUT_LN_CMD)) == 0)
    {
        printf("SHELL:operate call out with last number\n");
        hfp_hf_callout_with_last_number(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_WN",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate call wait enable\n");
        hfp_hf_set_call_waiting_notification(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_WD",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate call wait disenable\n");
        hfp_hf_set_call_waiting_notification(&oppo_addr,0);
        return 0;
    }

    if(strcmp("HFP_CLIE",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI enable\n");
        hfp_hf_set_call_line_identification_notification(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_CLID",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI disable\n");
        hfp_hf_set_call_line_identification_notification(&oppo_addr,0);
        return 0;
    }


    if(strcmp("HFP_NRECD",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI disable\n");
        hfp_hf_disable_ag_nrec(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_VGE",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI enable\n");
        hfp_hf_set_voice_recognition(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_VGD",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI disable\n");
        hfp_hf_set_voice_recognition(&oppo_addr,0);
        return 0;
    }

    if(strcmp("HFP_GPN",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate get phone number via voice tag\n");
        hfp_hf_get_phone_number_via_voice_tag(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_DTMF",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate get phone number via voice tag\n");
        hfp_hf_transmit_dtmf(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_VGM",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate VGM\n");
        hfp_hf_set_mic_volume(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_VGS",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate VGM\n");
        hfp_hf_set_spk_volume(&oppo_addr,1);
        return 0;
    }

    if(strcmp("HFP_LPN",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate local number\n");
        hfp_hf_get_local_phone_number(&oppo_addr);
        return 0;
    }


    if(strcmp("HFP_CLCC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_query_call_list(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_CALLEND",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_hangup(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_NET_F",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_set_format_network(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_NET_N",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_get_network(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_I",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_set_indicator_enable_value(&oppo_addr,"call",0);
        return 0;
    }

    if(strcmp("HFP_CGMI",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_manufacturer_id(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_CGMM",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_model_id(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_CGMR",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_revision_id(&oppo_addr);
        return 0;
    }

    if(strcmp("HFP_PID",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_pid(&oppo_addr);
        return 0;
    }


    if(strcmp("HFP_CON1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP CON\n");

        hfp_hf_connect(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_CGMI1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_manufacturer_id(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_GPN1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate get phone number via voice tag\n");
        hfp_hf_get_phone_number_via_voice_tag(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_CGMM1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_model_id(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_QB1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_query_hold_status(&iphone_addr);
        return 0;
    }


    if(strcmp("HFP_CGMR1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_revision_id(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_VGE1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI enable\n");
        hfp_hf_set_voice_recognition(&iphone_addr,1);
        return 0;
    }

    if(strcmp("HFP_VGD1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate CLI disable\n");
        hfp_hf_set_voice_recognition(&iphone_addr,0);
        return 0;
    }

    if(strcmp("HFP_CALLOUT_PN1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_callout_with_phone_number(&iphone_addr,"10010");
        return 0;
    }

    if(strcmp("HFP_PID1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_get_pid(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_CLCC1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_query_call_list(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_CALLEND1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_hangup(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_NET_F1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_set_format_network(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_NET_N1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_get_network(&iphone_addr);
        return 0;
    }

    if(strcmp("HFP_TX_IND1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate bt stop\n");
        hfp_hf_transfer_hf_indicator_value(&iphone_addr,2,99);
        return 0;
    }

    if(strncmp("HFP_TC1:",(const char*)shell_string,strlen("HFP_TC1:")) == 0)
    {
        uint8_t cmd = *(shell_string + strlen("HFP_TC1:"))-0x30;
        uint8_t index = *(shell_string + strlen("HFP_TC1:") + 1)-0x30;
        printf("SHELL:operate HFP_TC1,cmd %d,index %d\n",cmd,index);
        hfp_hf_three_call_control(&iphone_addr,cmd,index);
        return 0;
    }

    if(strcmp("HFP_I1",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate HFP_I\n");
        hfp_hf_set_indicator_enable_value(&iphone_addr,"call",0);
        return 0;
    }


    if(strcmp("PBAP_CON",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP CON\n");

        pbap_client_connect(&oppo_addr);
        return 0;
    }

    if(strcmp("PBAP_DISCON",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP DISCON\n");

        pbap_client_disconnect(&oppo_addr);
        return 0;
    }

    if(strcmp("PBAP_LP",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LI",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LO",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LM",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LPC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LIC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LOC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LMC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_LCC",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_SP",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_set_path(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_SPL",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_vcard_list(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return 0;
    }

    if(strcmp("PBAP_DVE",(const char*)shell_string) == 0)
    {
        printf("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_vcard_entry(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE,1);
        return 0;
    }




    if(strcmp("AVRCP_GET_CAP1",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP_GET_CAP1\n");
        avrcp_controller_get_supported_company_ids();
        return 0;
    }

    if(strcmp("AVRCP_GET_CAP2",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP_GET_CAP2\n");
        avrcp_controller_get_supported_events();
        return 0;
    }

    if(strcmp("AVRCP_REG_TRACK_C",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP_REG_TRACK_C\n");
        avrcp_controller_enable_notification(AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);
        return 0;
    }

    if(strcmp("AVRCP_GET_INFO",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP_REG_TRACK_C\n");
        uint8_t media_info[7] = {1,2,3,4,5,6,7};
        avrcp_controller_get_element_attributes(media_info,sizeof(media_info));
        return 0;
    }

    if(strcmp("AVRCP_PLAY",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP PLAY\n");
        avrcp_controller_play();
        return 0;
    }

    if(strcmp("AVRCP_PAUSE",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP pause\n");
        avrcp_controller_pause();
        return 0;
    }

    if(strcmp("AVRCP_FORWARD",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP forward\n");
        avrcp_controller_forward();
        return 0;
    }

    if(strcmp("AVRCP_BACKWARD",(const char*)shell_string) == 0)
    {
        printf("SHELL:AVRCP backward\n");
        avrcp_controller_backward();
        return 0;
    }

    show_usage();
    return 1;
}

