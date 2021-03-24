/******************************************************************************
  * @file           bt_avrcp_controller.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt avrcp controller source file
******************************************************************************/

#include "bt_avrcp_controller.h"

#if PROFILE_AVRCP_ENABLE


struct avctp_pcb_t *temp_avcrp;
uint8_t wait_press_resp;

static  uint8_t avrcp_controller_service_record[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, 0x0, 0x0, /* Service record handle attribute */
    SDP_UINT32, 0x00, 0x01, 0x00, 0x01, /*dummy vals, filled in on xmit*/

    SDP_DES_SIZE8, 0xb,
    SDP_UINT16, 0x0, 0x1, /* Service class ID list attribute */
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x11,0x0e,
    SDP_UUID16, 0x11,0x0f,

    SDP_DES_SIZE8, 0x15,
    SDP_UINT16, 0x0, 0x4, /* Protocol descriptor list attribute */
    SDP_DES_SIZE8, 0x10,
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x1, 0x0, /*L2CAP*/
    SDP_UINT16, 0x0, 0x17,/* AVCTP */
    SDP_DES_SIZE8, 0x6,
    SDP_UUID16, 0x0, 0x17, /* AVCTP UUID */
    SDP_UINT16, 0x1, 0x04,/* AVCTP version */

    SDP_DES_SIZE8, 0xd,
    SDP_UINT16, 0x0, 0x9, /* profile descriptor List */
    SDP_DES_SIZE8, 0x8,
    SDP_DES_SIZE8,0x06,
    SDP_UUID16,0x11,0x0e,
    SDP_UINT16,0x01,0x04,

    SDP_DES_SIZE8, 0x6,
    SDP_UINT16, 0x3, 0x11, /* support feature 1:speaker */
    SDP_UINT16,0x00,0x01,

};

struct avrcp_pcb_t *avrcp_controller_active_pcbs;  /* List of all active A2DP PCBs */
struct avrcp_pcb_t *avrcp_controller_tmp_pcb;
#define AVRCP_PCB_REG(pcbs, npcb) do { \
                            npcb->next = *pcbs; \
                            *pcbs = npcb; \
                            } while(0)
#define AVRCP_PCB_RMV(pcbs, npcb) do { \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(avrcp_controller_tmp_pcb = *pcbs; avrcp_controller_tmp_pcb != NULL; avrcp_controller_tmp_pcb = avrcp_controller_tmp_pcb->next) { \
                               if(avrcp_controller_tmp_pcb->next != NULL && avrcp_controller_tmp_pcb->next == npcb) { \
                                  avrcp_controller_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            } while(0)

avrcp_controller_cbs_t *avrcp_controller_cbs;

static err_t _avrcp_controller_register_all_notification(struct avctp_pcb_t *avctp_pcb);


static struct avrcp_pcb_t *avrcp_new(struct avctp_pcb_t *avctp_pcb)
{
    struct avrcp_pcb_t *avrcp_pcb;

    avrcp_pcb = bt_memp_malloc(MEMP_A2DP_PCB);
    if(avrcp_pcb != NULL)
    {
        memset(avrcp_pcb, 0, sizeof(struct avrcp_pcb_t));
        avrcp_pcb->avctppcb = avctp_pcb;
        return avrcp_pcb;
    }
    BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_memp_malloc fail\n",__FILE__,__FUNCTION__,__LINE__);

    return NULL;
}

static struct avrcp_pcb_t *avrcp_get_active_pcb(struct bd_addr_t *bdaddr)
{
    struct avrcp_pcb_t *avrcp_pcb = NULL;
    for(avrcp_pcb = avrcp_controller_active_pcbs; avrcp_pcb != NULL; avrcp_pcb = avrcp_pcb->next)
    {
        if(bd_addr_cmp(&(avrcp_pcb->remote_addr),bdaddr))
        {
            break;
        }
    }
    return avrcp_pcb;
}

static void avrcp_close(struct avrcp_pcb_t *avrcp_pcb)
{
    if(avrcp_pcb != NULL)
    {
        bt_memp_free(MEMP_A2DP_PCB, avrcp_pcb);
        avrcp_pcb = NULL;
    }
}


static err_t avrcp_ass_vendor_dependent_hdr(uint8_t *buffer,uint8_t subunit_type,uint8_t pdu_id,uint8_t ctype,uint8_t para_len)
{
    uint8_t cmd_pos = 0;
    buffer[cmd_pos++] = ctype;
    buffer[cmd_pos++] = (subunit_type << 3) | AVRCP_SUBUNIT_ID;
    buffer[cmd_pos++] = AVRCP_CMD_OPCODE_VENDOR_DEPENDENT;
    bt_be_store_24(buffer, cmd_pos, BT_SIG_COMPANY_ID);
    cmd_pos += 3;
    buffer[cmd_pos++] = pdu_id;
    buffer[cmd_pos++] = 0;
    bt_be_store_16(buffer, cmd_pos, para_len);

	return BT_ERR_OK;
}

static err_t avrcp_ass_pass_through_hdr(uint8_t *buffer,uint8_t subunit_type,uint8_t control_id,uint8_t is_press,uint8_t ctype,uint8_t para_len)
{
    uint8_t cmd_pos = 0;
    buffer[cmd_pos++] = ctype;
    buffer[cmd_pos++] = (subunit_type << 3) | AVRCP_SUBUNIT_ID;
	buffer[cmd_pos++] = AVRCP_CMD_OPCODE_PASS_THROUGH;
    buffer[cmd_pos++] = control_id | (is_press?0x0:0x80);
    buffer[cmd_pos++] = para_len;

	return BT_ERR_OK;
}



static err_t avrcp_controller_parse_get_capabilities_rsp(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint16_t para_len = bt_be_read_16(buffer, 8);
    uint8_t capabilities_id = buffer[10];
    uint8_t capabilities_count = buffer[11];

    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_get_capabilities_rsp: para_len(%d) capabilities_id(%d) capabilities_count(%d)\n",para_len,capabilities_id,capabilities_count);
    if(capabilities_id == AVRCP_CAPABILITY_ID_COMPANY)
    {

    }
    else if(capabilities_id == AVRCP_CAPABILITY_ID_EVENT)
    {
        uint8_t index = 0;
        uint8_t *event_id = buffer+12;
        for(index = 0; index < capabilities_count; index++)
        {
            switch(event_id[index])
            {
            case AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_TRACK_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_TRACK_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_TRACK_REACHED_END:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_TRACK_REACHED_END_MASK;
                break;
            case AVRCP_NOTIFY_EVT_TRACK_REACHED_START:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_TRACK_REACHED_START_MASK;
                break;
            case AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_UIDS_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_UIDS_CHANGED_MASK;
                break;
            case AVRCP_NOTIFY_EVT_VOLUME_CHANGED:
                avrcp_pcb->remote_notify_mask |= AVRCP_NOTIFY_EVT_VOLUME_CHANGED_MASK;
                break;
            default:
                BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avrcp event_id(0x%x)\n",__FILE__,__FUNCTION__,__LINE__,event_id[index]);
                break;
            }
        }

        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_get_capabilities_rsp: avrcp_pcb->remote_notify_mask(0x%x)\n",avrcp_pcb->remote_notify_mask);

        if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_support_capabilities)
            avrcp_controller_cbs->avrcp_support_capabilities(&avrcp_pcb->remote_addr,avrcp_pcb->remote_notify_mask);
    }

    return BT_ERR_OK;
}


static err_t avrcp_controller_parse_list_app_setting_rsp(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint16_t para_len = bt_be_read_16(buffer, 8);
    uint8_t app_setting_attr_num = buffer[10];

    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_list_app_setting_rsp: para_len(%d) app_setting_attr_num(%d)\n",para_len,app_setting_attr_num);
    if(app_setting_attr_num > 0)
    {
        uint8_t index = 0;
        uint8_t *setting_attr = buffer+11;
        for(index = 0; index < app_setting_attr_num; index++)
        {
            switch(setting_attr[index])
            {
            case AVRCP_APP_SETTING_ATTR_EQUALIZER:
                avrcp_pcb->remote_app_setting_attr_mask |= AVRCP_APP_SETTING_ATTR_EQUALIZER_MASK;
                break;
            case AVRCP_APP_SETTING_ATTR_REPEAT:
                avrcp_pcb->remote_app_setting_attr_mask |= AVRCP_APP_SETTING_ATTR_REPEAT_MASK;
                break;
            case AVRCP_APP_SETTING_ATTR_SHUFFLE:
                avrcp_pcb->remote_app_setting_attr_mask |= AVRCP_APP_SETTING_ATTR_SHUFFLE_MASK;
                break;
            case AVRCP_APP_SETTING_ATTR_SCAN:
                avrcp_pcb->remote_app_setting_attr_mask |= AVRCP_APP_SETTING_ATTR_SCAN_MASK;
                break;
            default:
                BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avrcp setting_attr(0x%x)\n",__FILE__,__FUNCTION__,__LINE__,setting_attr[index]);
                break;
            }
        }

        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_list_app_setting_rsp: avrcp_pcb->remote_notify_mask(0x%x)\n",avrcp_pcb->remote_notify_mask);

        if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_app_setting_attr)
            avrcp_controller_cbs->avrcp_app_setting_attr(&avrcp_pcb->remote_addr,avrcp_pcb->remote_app_setting_attr_mask);
    }
    else
    {
        if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_app_setting_attr)
            avrcp_controller_cbs->avrcp_app_setting_attr(&avrcp_pcb->remote_addr,0);
    }

    return BT_ERR_OK;
}


static err_t avrcp_controller_parse_get_element_attr_rsp(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint8_t index = 0;
    uint16_t para_len = bt_be_read_16(buffer, 8);
    uint8_t element_attr_num = buffer[10];
    uint8_t *para_palyload = buffer + 11;

    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_get_element_attr_rsp: para_len(%d) element_attr_num(%d)\n",para_len,element_attr_num);

	memset(&avrcp_pcb->now_playing_info,0,sizeof(now_playing_info_t));
    for(index = 0; index < element_attr_num; index++)
    {
        uint32_t attr_id = bt_be_read_32(para_palyload, 0);
        uint16_t attr_length = bt_be_read_16(para_palyload+6, 0);

        switch(attr_id)
        {
        case AVRCP_MEDIA_ATTR_TITLE:
            if (attr_length > AVRCP_ID3_TITIL_MAX_SIZE )
                attr_length = AVRCP_ID3_TITIL_MAX_SIZE;
			memcpy(avrcp_pcb->now_playing_info.now_playing_title,para_palyload+8,attr_length);
            break;
        case AVRCP_MEDIA_ATTR_ARTIST:
            if (attr_length > AVRCP_ID3_ARTIST_MAX_SIZE)
                attr_length = AVRCP_ID3_ARTIST_MAX_SIZE;
			memcpy(avrcp_pcb->now_playing_info.now_playing_artist,para_palyload+8,attr_length);
            break;
        case AVRCP_MEDIA_ATTR_ALBUM:
            if (attr_length > AVRCP_ID3_ALBUM_MAX_SIZE)
                attr_length = AVRCP_ID3_ALBUM_MAX_SIZE;
			memcpy(avrcp_pcb->now_playing_info.now_playing_album,para_palyload+8,attr_length);
            break;
        case AVRCP_MEDIA_ATTR_TRACK:
			avrcp_pcb->now_playing_info.current_index= bt_atoi_spec_size(para_palyload+8,attr_length);
            break;
        case AVRCP_MEDIA_ATTR_TOTAL_NUM_ITEMS:
			avrcp_pcb->now_playing_info.totol_count= bt_atoi_spec_size(para_palyload+8,attr_length);
            break;
        case AVRCP_MEDIA_ATTR_GENRE:
            break;
        case AVRCP_MEDIA_ATTR_SONG_LENGTH_MS:
			avrcp_pcb->now_playing_info.totol_milliseconds= bt_atoi_spec_size(para_palyload+8,attr_length);
            break;
        default:	
			BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow attr_id(%d)\n",__FILE__,__FUNCTION__,__LINE__,attr_id);
            break;
        }

        para_palyload += 8+attr_length;
    }

	BT_AVRCP_TRACE_DEBUG("now_playing_title(%s)\n",avrcp_pcb->now_playing_info.now_playing_title);
	BT_AVRCP_TRACE_DEBUG("now_playing_artist(%s)\n",avrcp_pcb->now_playing_info.now_playing_artist);
	BT_AVRCP_TRACE_DEBUG("now_playing_album(%s)\n",avrcp_pcb->now_playing_info.now_playing_album);
	BT_AVRCP_TRACE_DEBUG("now_playing_index(%d/%d)\n",avrcp_pcb->now_playing_info.current_index,avrcp_pcb->now_playing_info.totol_count);
	BT_AVRCP_TRACE_DEBUG("now_playing_totol_ms(%d)\n",avrcp_pcb->now_playing_info.totol_milliseconds);

	if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_element_attr_update)
            avrcp_controller_cbs->avrcp_element_attr_update(&avrcp_pcb->remote_addr,&avrcp_pcb->now_playing_info);
    return BT_ERR_OK;
}

static err_t avrcp_controller_parse_get_play_status_rsp(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint16_t para_len = bt_be_read_16(buffer, 8);
	uint32_t song_lenght = bt_be_read_32(buffer, 10);
	uint32_t song_pos = bt_be_read_32(buffer, 14);
	uint8_t play_status = buffer[18];

    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

	BT_AVRCP_TRACE_DEBUG("song_lenght(%d)\n",song_lenght);
	BT_AVRCP_TRACE_DEBUG("song_pos(%d)\n",song_pos);
	BT_AVRCP_TRACE_DEBUG("play_status(%d)\n",play_status);


	if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_playpos_change_update)
            avrcp_controller_cbs->avrcp_playpos_change_update(&avrcp_pcb->remote_addr,song_pos);
    return BT_ERR_OK;
}



static err_t _avrcp_controller_play_status_change_handle(struct avrcp_pcb_t *avrcp_pcb,uint8_t play_status)
{
    BT_AVRCP_TRACE_DEBUG("avrcp_controller_play_status_handle: status %d\n",play_status);

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_play_status_update)
        avrcp_controller_cbs->avrcp_play_status_update(&avrcp_pcb->remote_addr,play_status);

    return BT_ERR_OK;
}


static err_t _avrcp_controller_track_change_handle(struct avrcp_pcb_t *avrcp_pcb)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_track_change_handle\n");

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_track_change_update)
        avrcp_controller_cbs->avrcp_track_change_update(&avrcp_pcb->remote_addr);

    return BT_ERR_OK;
}

static err_t _avrcp_controller_track_reached_end_handle(struct avrcp_pcb_t *avrcp_pcb)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_track_reached_end_handle\n");

    return BT_ERR_OK;
}


static err_t _avrcp_controller_track_reached_start_handle(struct avrcp_pcb_t *avrcp_pcb)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_track_reached_start_handle\n");

    return BT_ERR_OK;
}


static err_t _avrcp_controller_playpos_change_handle(struct avrcp_pcb_t *avrcp_pcb,uint32_t millisecond)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_playpos_change_handle pos %d\n",millisecond);

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_playpos_change_update)
        avrcp_controller_cbs->avrcp_playpos_change_update(&avrcp_pcb->remote_addr,millisecond);

    return BT_ERR_OK;
}


static err_t _avrcp_controller_battary_status_change_handle(struct avrcp_pcb_t *avrcp_pcb,uint8_t battary_status)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_battary_status_change_handle: battary_status(%d)\n",battary_status);

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_battary_change_update)
        avrcp_controller_cbs->avrcp_battary_change_update(&avrcp_pcb->remote_addr,battary_status);

    return BT_ERR_OK;
}

static err_t _avrcp_controller_system_status_change_handle(struct avrcp_pcb_t *avrcp_pcb,uint8_t system_status)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_system_status_change_handle system_status(%d)\n",system_status);

    return BT_ERR_OK;
}


static err_t _avrcp_controller_volume_change_handle(struct avrcp_pcb_t *avrcp_pcb,uint8_t volume)
{
    BT_AVRCP_TRACE_DEBUG("_avrcp_controller_volume_change_handle: volume(%d)\n",volume);

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_volume_change_update)
        avrcp_controller_cbs->avrcp_volume_change_update(&avrcp_pcb->remote_addr,volume);

    return BT_ERR_OK;
}


static err_t avrcp_controller_parse_notification_change_rsp(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint16_t para_len = bt_be_read_16(buffer, 8);
    uint8_t event_id = buffer[10];
    uint8_t *para_palyload = buffer + 11;

    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: para_len(%d) event_id(%d)\n",para_len,event_id);

    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,event_id);

    switch(event_id)
    {
    case AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED\n");
        _avrcp_controller_play_status_change_handle(avrcp_pcb,para_palyload[0]);
        break;
    case AVRCP_NOTIFY_EVT_TRACK_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_TRACK_CHANGED\n");
        _avrcp_controller_track_change_handle(avrcp_pcb);
        break;
    case AVRCP_NOTIFY_EVT_TRACK_REACHED_END:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_TRACK_REACHED_END\n");
        _avrcp_controller_track_reached_end_handle(avrcp_pcb);
        break;
    case AVRCP_NOTIFY_EVT_TRACK_REACHED_START:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_TRACK_REACHED_START\n");
        _avrcp_controller_track_reached_start_handle(avrcp_pcb);
        break;
    case AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED:
    {
        uint32_t millisecond = bt_be_read_32(para_palyload,0);
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED\n");
        _avrcp_controller_playpos_change_handle(avrcp_pcb,millisecond);
        break;
    }
    case AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED\n");
        _avrcp_controller_battary_status_change_handle(avrcp_pcb,para_palyload[0]);
        break;
    case AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED\n");
        _avrcp_controller_system_status_change_handle(avrcp_pcb,para_palyload[0]);
        break;
    case AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED\n");
        break;
    case AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED\n");
        break;
    case AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED\n");
        break;
    case AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED\n");
        break;
    case AVRCP_NOTIFY_EVT_UIDS_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_UIDS_CHANGED\n");
        break;
    case AVRCP_NOTIFY_EVT_VOLUME_CHANGED:
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_notification_change_rsp: AVRCP_NOTIFY_EVT_VOLUME_CHANGED\n");
        _avrcp_controller_volume_change_handle(avrcp_pcb,para_palyload[0]);
        break;
    default:
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avrcp event id(%d)\n",__FILE__,__FUNCTION__,__LINE__,event_id);
        break;


    }

    return BT_ERR_OK;
}


static err_t avrcp_controller_parse_vendor_dependent(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
    uint8_t avrcp_response = buffer[0] & 0x0f;
    uint8_t pdu_id = buffer[6];

    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: avrcp_response(0x%02x) pdu_id(0x%02x)\n",avrcp_response,pdu_id);

    if(avrcp_response == AVRCP_CTYPE_RESPONSE_IMPLEMENTED_STABLE) /* Response */
    {
        switch(pdu_id)
        {
        case AVRCP_PDU_ID_GET_CAPABILITIES:
        {
            BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: AVRCP_PDU_ID_GET_CAPABILITIES\n");
            avrcp_controller_parse_get_capabilities_rsp(avctp_pcb,buffer,buffer_len);
            _avrcp_controller_register_all_notification(avctp_pcb);

            break;
        }
        case AVRCP_PDU_ID_LIST_APP_SETTING_ARRT:
        {
            BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: AVRCP_PDU_ID_LIST_APP_SETTING_ARRT\n");
            avrcp_controller_parse_list_app_setting_rsp(avctp_pcb,buffer,buffer_len);
            break;
        }
        case AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES:
        {
            BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES\n");
            avrcp_controller_parse_get_element_attr_rsp(avctp_pcb,buffer,buffer_len);
            break;
        }
		case AVRCP_PDU_ID_GET_PLAY_STATUS:
		{
			BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: AVRCP_PDU_ID_GET_PLAY_STATUS\n");
			avrcp_controller_parse_get_play_status_rsp(avctp_pcb,buffer,buffer_len);
			break;
		}
        default:
        {
            BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avrcp pdu id(%d)\n",__FILE__,__FUNCTION__,__LINE__,pdu_id);
            break;
        }
        }
    }
    else if(avrcp_response == AVRCP_CTYPE_RESPONSE_CHANGED_STABLE)
    {
        switch(pdu_id)
        {
        case AVRCP_PDU_ID_REGISTER_NOTIFICATION:
        {
            BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_vendor_dependent: AVRCP_PDU_ID_REGISTER_NOTIFICATION\n");
            avrcp_controller_parse_notification_change_rsp(avctp_pcb,buffer,buffer_len);
            break;
        }
        default:
        {
            BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avrcp pdu id(%d)\n",__FILE__,__FUNCTION__,__LINE__,pdu_id);
            break;
        }
        }
    }

    return BT_ERR_OK;
}

static err_t avrcp_controller_parse_pass_through(struct avctp_pcb_t *avctp_pcb,uint8_t *buffer,uint16_t buffer_len)
{
  	struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_CONN;
    }
    BT_AVRCP_TRACE_DEBUG("avrcp_controller_parse_pass_through: control id(0x%02x) wait realease(%d)\n",buffer[3]&0x7f,avrcp_pcb->pass_though_wait_release);

    if(avrcp_pcb->pass_though_wait_release == 1)
    {
    	struct bt_pbuf_t *p;
    	avrcp_pcb->pass_though_wait_release = 0;
		
	    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_PASS_THROUGH_SIZE, BT_PBUF_RAM)) == NULL)
	    {
	        /* Could not allocate memory for bt_pbuf_t */
	        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
	        return BT_ERR_MEM;
	    }

	    avrcp_ass_pass_through_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,buffer[3]&0x7f,0,AVRCP_CTYPE_CONTROL,0);

	    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);

	    bt_pbuf_free(p);		
    }
	
    return BT_ERR_OK;
}


static err_t avrcp_controller_get_capabilities(struct avctp_pcb_t *avctp_pcb,uint8_t capability_id)
{
    uint8_t get_capabilities_para_len = 1;
    struct bt_pbuf_t *p;
    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+get_capabilities_para_len, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_vendor_dependent_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   AVRCP_PDU_ID_GET_CAPABILITIES,AVRCP_CTYPE_STATUS,get_capabilities_para_len);

    ((uint8_t *)p->payload)[AVRCP_VENDOR_DEPENDENT_HDR_SIZE] = capability_id;
    avctp_datawrite(avctp_pcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


static err_t avrcp_connect_ind(struct avctp_pcb_t *avctp_pcb)
{
    struct avrcp_pcb_t *avrcp_pcb;

    if((avrcp_pcb = avrcp_new(avctp_pcb)) == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not alloc avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }
    bd_addr_set(&(avrcp_pcb->remote_addr),&(avctp_pcb->remote_bdaddr));
    AVRCP_PCB_REG(&avrcp_controller_active_pcbs, avrcp_pcb);

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_ctl_connect_set_up)
        avrcp_controller_cbs->avrcp_ctl_connect_set_up(&avrcp_pcb->remote_addr,BT_ERR_OK);
    return BT_ERR_OK;
}

static err_t avrcp_signal_disconnect_ind(struct avctp_pcb_t *avctp_pcb)
{
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    if(avrcp_controller_cbs && avrcp_controller_cbs->avrcp_ctl_connect_realease)
        avrcp_controller_cbs->avrcp_ctl_connect_realease(&avrcp_pcb->remote_addr,BT_ERR_OK);

    AVRCP_PCB_RMV(&avrcp_controller_active_pcbs, avrcp_pcb);
    avrcp_close(avrcp_pcb);
    return BT_ERR_OK;
}

static err_t _avrcp_controller_register_all_notification(struct avctp_pcb_t *avctp_pcb)
{
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(&avctp_pcb->remote_bdaddr);

    if(avrcp_pcb == NULL)
    {
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] Could not find avrcp pcb\n",__FILE__,__FUNCTION__,__LINE__);

        return BT_ERR_MEM;
    }

    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_TRACK_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_TRACK_REACHED_END);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_TRACK_REACHED_START);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_UIDS_CHANGED);
    avrcp_controller_register_notification(&avrcp_pcb->remote_addr,AVRCP_NOTIFY_EVT_VOLUME_CHANGED);

    return BT_ERR_OK;
}


static err_t avrcp_control_event_handle(struct avctp_pcb_t *avctp_pcb,uint32_t msg_id,struct bt_pbuf_t *p)
{
    switch(msg_id)
    {
    case AVCTP_EVT_CONNECT_IND:
        BT_AVRCP_TRACE_DEBUG("avrcp_control_event_handle: AVCTP_EVT_CONNECT_IND\n");
        avrcp_connect_ind(avctp_pcb);
        avrcp_controller_get_capabilities(avctp_pcb,AVRCP_CAPABILITY_ID_EVENT);
        break;
    case AVCTP_EVT_CONNECT_CFM:
        BT_AVRCP_TRACE_DEBUG("avrcp_control_event_handle: AVCTP_EVT_CONNECT_CFM\n");
        avrcp_controller_get_capabilities(avctp_pcb,AVRCP_CAPABILITY_ID_EVENT);
        break;
    case AVCTP_EVT_DISCON_IND:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_control_event_handle: AVCTP_EVT_DISCON_IND\n");
        avrcp_signal_disconnect_ind(avctp_pcb);
        break;
    }
    case AVCTP_EVT_DISCON_CFM:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_control_event_handle: AVCTP_EVT_DISCON_CFM\n");
        break;
    }
    default:
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] unknow avctp signal id(%d)\n",__FILE__,__FUNCTION__,__LINE__,msg_id);
        break;
    }

	return BT_ERR_OK;
}


err_t avrcp_controller_data_handle(struct avctp_pcb_t *avctp_pcb,struct bt_pbuf_t *p)
{
    uint8_t *data = p->payload;
    uint8_t opcode = data[2];


    BT_AVRCP_TRACE_DEBUG("avrcp_controller_data_handle: p->len == %d\n", p->len);
    bt_hex_dump(p->payload,p->len);

    switch(opcode)
    {
    case AVRCP_CMD_OPCODE_VENDOR_DEPENDENT:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_data_handle: AVRCP_CMD_OPCODE_VENDOR_DEPENDENT\n");
        avrcp_controller_parse_vendor_dependent(avctp_pcb,p->payload,p->len);

        break;
    }
    case AVRCP_CMD_OPCODE_UNIT_INFO:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_data_handle: AVRCP_CMD_OPCODE_UNIT_INFO\n");
        break;
    }
    case AVRCP_CMD_OPCODE_SUBUNIT_INFO:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_data_handle: AVRCP_CMD_OPCODE_SUBUNIT_INFO\n");
        break;
    }
    case AVRCP_CMD_OPCODE_PASS_THROUGH:
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_data_handle: AVRCP_CMD_OPCODE_PASS_THROUGH\n");
		avrcp_controller_parse_pass_through(avctp_pcb,p->payload,p->len);
        break;
    }
    default:
        break;
    }
    return BT_ERR_OK;
}


err_t avrcp_controller_init(avrcp_controller_cbs_t *cb)
{
    sdp_record_t *record;

    uint32_t controller_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)avrcp_controller_service_record,sizeof(avrcp_controller_service_record),controller_record_hdl)) == NULL)
    {
        BT_AVRCP_TRACE_DEBUG("avrcp_controller_init: Could not alloc SDP record\n");
        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)avrcp_controller_service_record,sizeof(avrcp_controller_service_record));

    avrcp_controller_cbs = cb;
    avctp_init(avrcp_control_event_handle,avrcp_controller_data_handle);
    return BT_ERR_OK;
}

err_t avrcp_controller_list_app_setting_attr(struct bd_addr_t *remote_addr)
{
    struct bt_pbuf_t *p;
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(remote_addr);

    if(!avrcp_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_vendor_dependent_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   AVRCP_PDU_ID_LIST_APP_SETTING_ARRT,AVRCP_CTYPE_STATUS,0);

    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_get_play_status(struct bd_addr_t *remote_addr)
{
	struct bt_pbuf_t *p;
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(remote_addr);

    if(!avrcp_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_vendor_dependent_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   AVRCP_PDU_ID_GET_PLAY_STATUS,AVRCP_CTYPE_STATUS,0);

    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}


err_t avrcp_controller_register_notification(struct bd_addr_t *remote_addr,uint8_t notificaion_id)
{
    struct bt_pbuf_t *p;
    uint8_t register_notification_para_len = 5;
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(remote_addr);

    if(!avrcp_pcb)
        return BT_ERR_CONN;

    if(notificaion_id > AVRCP_NOTIFY_EVT_VOLUME_CHANGED)
        return BT_ERR_ARG;

    if((avrcp_pcb->remote_notify_mask & (1<<(notificaion_id-1)))== 0)
        return BT_ERR_ARG;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+register_notification_para_len, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_vendor_dependent_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   AVRCP_PDU_ID_REGISTER_NOTIFICATION,AVRCP_CTYPE_NOTIFY,register_notification_para_len);
    ((uint8_t *)p->payload)[AVRCP_VENDOR_DEPENDENT_HDR_SIZE] = notificaion_id;
    bt_be_store_32(p->payload, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+1, 0);

    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);
    bt_pbuf_free(p);
    return BT_ERR_OK;
}

err_t avrcp_controller_get_element_attributes(struct bd_addr_t *remote_addr)
{
    struct bt_pbuf_t *p;
    uint8_t get_element_attributes_para_len = 9;
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(remote_addr);

    if(!avrcp_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_VENDOR_DEPENDENT_HDR_SIZE+get_element_attributes_para_len, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_vendor_dependent_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES,AVRCP_CTYPE_STATUS,get_element_attributes_para_len);

    memset((uint8_t *)p->payload+AVRCP_VENDOR_DEPENDENT_HDR_SIZE,0,8);
    ((uint8_t *)p->payload)[AVRCP_VENDOR_DEPENDENT_HDR_SIZE+8] = AVRCP_MEDIA_ATTR_ALL;

    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);

    bt_pbuf_free(p);

	return BT_ERR_OK;
}

err_t avrcp_controller_control(struct bd_addr_t *remote_addr,uint8_t control_id)
{
	struct bt_pbuf_t *p;
    struct avrcp_pcb_t *avrcp_pcb = avrcp_get_active_pcb(remote_addr);

    if(!avrcp_pcb)
        return BT_ERR_CONN;

    if((p = bt_pbuf_alloc(BT_PBUF_RAW, AVRCP_PASS_THROUGH_SIZE, BT_PBUF_RAM)) == NULL)
    {
        /* Could not allocate memory for bt_pbuf_t */
        BT_AVRCP_TRACE_ERROR("ERROR:file[%s],function[%s],line[%d] bt_pbuf_alloc fail\n",__FILE__,__FUNCTION__,__LINE__);
        return BT_ERR_MEM;
    }

    avrcp_ass_pass_through_hdr(p->payload,AVRCP_SUBUNIT_TYPE_PANEL,
                                   control_id,1,AVRCP_CTYPE_CONTROL,0);

	avrcp_pcb->pass_though_wait_release = 1;
    avctp_datawrite(avrcp_pcb->avctppcb, p,BT_SERVICE_CLASS_AV_REMOTE_CONTROL);

    bt_pbuf_free(p);

	return BT_ERR_OK;
}

#endif

