#include "hw_misc.h"
#include "board_wrapper.h"
#include "bt_wrapper.h"
#include "pbap_client.h"

static const uint8_t usage[]=
{
    "usage:\n"
    "CMD\t\t\tdescription\n"
    "BT_START\t\tStart bluetooth stack\n"
    "BT_STOP\t\t\tStop blueooth stack\n"
    "BT_INQUIRY\t\tInquiry device\n"

};



uint8_t shell_parse(uint8_t *shell_string)
{
    struct bd_addr oppo_addr;
    oppo_addr.addr[5] = 0x9c;
    oppo_addr.addr[4] = 0x0c;
    oppo_addr.addr[3] = 0xdf;
    oppo_addr.addr[2] = 0x24;
    oppo_addr.addr[1] = 0x7f;
    oppo_addr.addr[0] = 0x0a;

    struct bd_addr iphone_addr;
    iphone_addr.addr[5] = 0x94;
    iphone_addr.addr[4] = 0xbf;
    iphone_addr.addr[3] = 0x2d;
    iphone_addr.addr[2] = 0x51;
    iphone_addr.addr[1] = 0xb2;
    iphone_addr.addr[0] = 0x0e;
    if(hw_strcmp("BT_START",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt start\n");
        bt_start();
        return HW_ERR_OK;
    }
    if(hw_strcmp("BT_STOP",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        bt_stop();
        return HW_ERR_OK;
    }
    if(hw_strcmp("BT_INQUIRY",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        bt_start_inquiry(0x30,1);
        return HW_ERR_OK;
    }
    if(hw_strcmp("SPP_SEND",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        spp_send_data(&oppo_addr,"111111",hw_strlen("111111"));
        return HW_ERR_OK;
    }

    if(hw_strcmp("SPP_CON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate spp CON\n");

        spp_connect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("SPP_DISCON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate spp DISCON\n");

        spp_disconnect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_CON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP CON\n");

        pbap_client_connect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_DISCON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP DISCON\n");

        pbap_client_disconnect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LP",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LI",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LO",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LM",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_download_phonebook(&oppo_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

	 if(hw_strcmp("PBAP_LPC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LIC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_INCOMING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LOC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_OUTGOING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LMC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_MISSING_BOOK_TYPE);
        return HW_ERR_OK;
    }

    if(hw_strcmp("PBAP_LCC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LI\n");

        pbap_client_query_phonebook_size(&oppo_addr,PB_LOCAL_REPOSITORY,PB_COMBINE_BOOK_TYPE);
        return HW_ERR_OK;
    }

	if(hw_strcmp("PBAP_SP",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_set_path(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

	if(hw_strcmp("PBAP_SPL",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_vcard_list(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE);
        return HW_ERR_OK;
    }

	if(hw_strcmp("PBAP_DVE",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate PBAP PBAP_LPB\n");

        pbap_client_download_vcard_entry(&oppo_addr,PB_LOCAL_REPOSITORY,PB_PHONEBOOK_TYPE,1);
        return HW_ERR_OK;
    }


    if(hw_strcmp("HFP_CON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP CON\n");

        hfp_hf_connect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_DISCON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP DISCON\n");

        hfp_hf_disconnect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_SCO_CON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP SCO CON\n");

        hfp_hf_audio_connect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_SCO_DISCON",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP SCO DISCON\n");

        hfp_hf_audio_disconnect(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_ANSWER",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP ANSWER INCOMING CALL\n");

        hfp_hf_answer_incoming_call(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLOUT_PN",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate call out number\n");
        hfp_hf_callout_with_phone_number(&oppo_addr,"10086");
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLOUT_MEM",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_callout_with_memory(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLOUT_LC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_callout_with_last_number(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_WN",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate call wait enable\n");
        hfp_hf_set_call_waiting_notification(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_WD",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate call wait disenable\n");
        hfp_hf_set_call_waiting_notification(&oppo_addr,0);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CLIE",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI enable\n");
        hfp_hf_set_call_line_identification_notification(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CLID",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI disable\n");
        hfp_hf_set_call_line_identification_notification(&oppo_addr,0);
        return HW_ERR_OK;
    }


    if(hw_strcmp("HFP_NRECD",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI disable\n");
        hfp_hf_disable_ag_nrec(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGE",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI enable\n");
        hfp_hf_set_voice_recognition(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGD",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI disable\n");
        hfp_hf_set_voice_recognition(&oppo_addr,0);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_GPN",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate get phone number via voice tag\n");
        hfp_hf_get_phone_number_via_voice_tag(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_DTMF",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate get phone number via voice tag\n");
        hfp_hf_transmit_dtmf(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGM",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate VGM\n");
        hfp_hf_set_mic_volume(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGS",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate VGM\n");
        hfp_hf_set_spk_volume(&oppo_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_LPN",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate local number\n");
        hfp_hf_get_local_phone_number(&oppo_addr);
        return HW_ERR_OK;
    }


    if(hw_strcmp("HFP_CLCC",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_query_call_list(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLEND",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_hangup(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_NET_F",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_set_format_network(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_NET_N",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_get_network(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_I",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_set_indicator_enable_value(&oppo_addr,"call",0);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CGMI",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_manufacturer_id(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CGMM",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_model_id(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CGMR",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_revision_id(&oppo_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_PID",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_pid(&oppo_addr);
        return HW_ERR_OK;
    }


    if(hw_strcmp("HFP_CON1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP CON\n");

        hfp_hf_connect(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CGMI1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_manufacturer_id(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_GPN1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate get phone number via voice tag\n");
        hfp_hf_get_phone_number_via_voice_tag(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CGMM1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_model_id(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_QB1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_query_hold_status(&iphone_addr);
        return HW_ERR_OK;
    }


    if(hw_strcmp("HFP_CGMR1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_revision_id(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGE1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI enable\n");
        hfp_hf_set_voice_recognition(&iphone_addr,1);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_VGD1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate CLI disable\n");
        hfp_hf_set_voice_recognition(&iphone_addr,0);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLOUT_PN1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_callout_with_phone_number(&iphone_addr,"10010");
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_PID1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_get_pid(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CLCC1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_query_call_list(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_CALLEND1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_hangup(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_NET_F1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_set_format_network(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_NET_N1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_get_network(&iphone_addr);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_TX_IND1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate bt stop\n");
        hfp_hf_transfer_hf_indicator_value(&iphone_addr,2,99);
        return HW_ERR_OK;
    }

    if(hw_strncmp("HFP_TC1:",(const char*)shell_string,hw_strlen("HFP_TC1:")) == 0)
    {
        uint8_t cmd = *(shell_string + hw_strlen("HFP_TC1:"))-0x30;
        uint8_t index = *(shell_string + hw_strlen("HFP_TC1:") + 1)-0x30;
        HW_DEBUG("SHELL:operate HFP_TC1,cmd %d,index %d\n",cmd,index);
        hfp_hf_three_call_control(&iphone_addr,cmd,index);
        return HW_ERR_OK;
    }

    if(hw_strcmp("HFP_I1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:operate HFP_I\n");
        hfp_hf_set_indicator_enable_value(&iphone_addr,"call",0);
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_GET_CAP1",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_GET_CAP1\n");
        avrcp_controller_get_supported_company_ids();
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_GET_CAP2",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_GET_CAP2\n");
        avrcp_controller_get_supported_events();
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_REG_TRACK_C",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_REG_TRACK_C\n");
        avrcp_controller_enable_notification(AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_GET_INFO",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:AVRCP_REG_TRACK_C\n");
        uint8_t media_info[7] = {1,2,3,4,5,6,7};
        avrcp_controller_get_element_attributes(media_info,sizeof(media_info));
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_PLAY",(const char*)shell_string) == 0)
    {
        HW_DEBUG("SHELL:AVRCP PLAY\n");
        avrcp_controller_play();
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_PAUSE",(const char*)shell_string) == 0)
    {
        //HW_DEBUG("SHELL:AVRCP pause\n");
        avrcp_controller_pause();
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_FORWARD",(const char*)shell_string) == 0)
    {
        //HW_DEBUG("SHELL:AVRCP forward\n");
        avrcp_controller_forward();
        return HW_ERR_OK;
    }

    if(hw_strcmp("AVRCP_BACKWARD",(const char*)shell_string) == 0)
    {
        //HW_DEBUG("SHELL:AVRCP backward\n");
        avrcp_controller_backward();
        return HW_ERR_OK;
    }

    //HW_DEBUG("%s\n",usage);
    return HW_ERR_SHELL_NO_CMD;
}
