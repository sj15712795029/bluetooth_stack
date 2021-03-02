/******************************************************************************
  * @file           bt_avrcp_controller.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-13
  * @brief          bt avrcp controller header file
******************************************************************************/

#ifndef BT_AVRCP_CONTROLLER_H_H_H
#define BT_AVRCP_CONTROLLER_H_H_H

#include "bt_common.h"

#if PROFILE_AVRCP_ENABLE


#include "bt_sdp.h"
#include "bt_avctp.h"


#define AVRCP_VENDOR_DEPENDENT_HDR_SIZE 10
#define AVRCP_PASS_THROUGH_SIZE 5

#define BT_SIG_COMPANY_ID 0x001958

typedef enum {
    AVRCP_MEDIA_ATTR_ALL = 0,
    AVRCP_MEDIA_ATTR_TITLE,
    AVRCP_MEDIA_ATTR_ARTIST,
    AVRCP_MEDIA_ATTR_ALBUM,
    AVRCP_MEDIA_ATTR_TRACK,
    AVRCP_MEDIA_ATTR_TOTAL_NUM_ITEMS,
    AVRCP_MEDIA_ATTR_GENRE,
    AVRCP_MEDIA_ATTR_SONG_LENGTH_MS,
    AVRCP_MEDIA_ATTR_DEFAULT_COVER_ART,
} avrcp_media_attribute_id_e;

typedef enum {
    AVRCP_PDU_ID_GET_CAPABILITIES = 0x10,
	AVRCP_PDU_ID_LIST_APP_SETTING_ARRT = 0x11,
    AVRCP_PDU_ID_GET_CURRENT_APP_SETTING_VALUE = 0x13,
    AVRCP_PDU_ID_SET_APP_SETTING_VALUE = 0x14,
    AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES = 0x20,
    AVRCP_PDU_ID_GET_PLAY_STATUS = 0x30,
    AVRCP_PDU_ID_REGISTER_NOTIFICATION = 0x31,
    AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE = 0x40,
    AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE = 0x41,
    AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME = 0x50,
    AVRCP_PDU_ID_SET_ADDRESSED_PLAYER = 0x60,
    AVRCP_PDU_ID_SET_BROWSED_PLAYER = 0x70,
    AVRCP_PDU_ID_GET_FOLDER_ITEMS = 0x71,
    AVRCP_PDU_ID_CHANGE_PATH = 0x72,
    AVRCP_PDU_ID_GET_ITEM_ATTRIBUTES = 0x73,
    AVRCP_PDU_ID_PLAY_ITEM = 0x74,
    AVRCP_PDU_ID_GET_TOTAL_NUMBER_OF_ITEMS = 0x75,
    AVRCP_PDU_ID_SEARCH = 0x80,
    AVRCP_PDU_ID_ADD_TO_NOW_PLAYING = 0x90,
    AVRCP_PDU_ID_GENERAL_REJECT = 0xA0,
    
    AVRCP_PDU_ID_UNDEFINED = 0xFF
} avrcp_pdu_id_e;

typedef enum {
    AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED = 0x01,            
	AVRCP_NOTIFY_EVT_TRACK_CHANGED = 0x02,                      
	AVRCP_NOTIFY_EVT_TRACK_REACHED_END = 0x03,                  
	AVRCP_NOTIFY_EVT_TRACK_REACHED_START = 0x04,                
	AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED = 0x05,               
	AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED = 0x06,                
	AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED = 0x07,              
	AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08, 
	AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED = 0x09,        
	AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED = 0x0a,          
	AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED = 0x0b,           
	AVRCP_NOTIFY_EVT_UIDS_CHANGED = 0x0c,                       
	AVRCP_NOTIFY_EVT_VOLUME_CHANGED = 0x0d     
} avrcp_notification_event_id_e;

typedef enum {
    AVRCP_NOTIFY_EVT_PLAYBACK_STATUS_CHANGED_MASK = 0x01,            
	AVRCP_NOTIFY_EVT_TRACK_CHANGED_MASK = 0x02,                      
	AVRCP_NOTIFY_EVT_TRACK_REACHED_END_MASK = 0x04,                  
	AVRCP_NOTIFY_EVT_TRACK_REACHED_START_MASK = 0x08,                
	AVRCP_NOTIFY_EVT_PLAYBACK_POS_CHANGED_MASK = 0x10,               
	AVRCP_NOTIFY_EVT_BATT_STATUS_CHANGED_MASK = 0x20,                
	AVRCP_NOTIFY_EVT_SYSTEM_STATUS_CHANGED_MASK = 0x40,              
	AVRCP_NOTIFY_EVT_PLAYER_APPLICATION_SETTING_CHANGED_MASK = 0x80, 
	AVRCP_NOTIFY_EVT_NOW_PLAYING_CONTENT_CHANGED_MASK = 0x100,        
	AVRCP_NOTIFY_EVT_AVAILABLE_PLAYERS_CHANGED_MASK = 0x200,          
	AVRCP_NOTIFY_EVT_ADDRESSED_PLAYER_CHANGED_MASK = 0x400,           
	AVRCP_NOTIFY_EVT_UIDS_CHANGED_MASK = 0x800,                       
	AVRCP_NOTIFY_EVT_VOLUME_CHANGED_MASK = 0x1000    
} avrcp_notification_event_id_mask_e;



typedef enum {
    AVRCP_CAPABILITY_ID_COMPANY = 0x02,
    AVRCP_CAPABILITY_ID_EVENT = 0x03
} avrcp_capability_id_e;

typedef enum {
    AVRCP_CTYPE_CONTROL = 0,
    AVRCP_CTYPE_STATUS,
    AVRCP_CTYPE_SPECIFIC_INQUIRY,
    AVRCP_CTYPE_NOTIFY,
    AVRCP_CTYPE_GENERAL_INQUIRY,
    AVRCP_CTYPE_RESERVED5,
    AVRCP_CTYPE_RESERVED6,
    AVRCP_CTYPE_RESERVED7,
    AVRCP_CTYPE_RESPONSE_NOT_IMPLEMENTED = 8,
    AVRCP_CTYPE_RESPONSE_ACCEPTED,
    AVRCP_CTYPE_RESPONSE_REJECTED,
    AVRCP_CTYPE_RESPONSE_IN_TRANSITION,
    AVRCP_CTYPE_RESPONSE_IMPLEMENTED_STABLE = 0x0c,
    AVRCP_CTYPE_RESPONSE_CHANGED_STABLE,
    AVRCP_CTYPE_RESPONSE_RESERVED,
    AVRCP_CTYPE_RESPONSE_INTERIM
} avrcp_command_type_e;

typedef enum {
    AVRCP_SUBUNIT_TYPE_MONITOR = 0,
    AVRCP_SUBUNIT_TYPE_AUDIO = 1,
    AVRCP_SUBUNIT_TYPE_PRINTER,
    AVRCP_SUBUNIT_TYPE_DISC,
    AVRCP_SUBUNIT_TYPE_TAPE_RECORDER_PLAYER,
    AVRCP_SUBUNIT_TYPE_TUNER,
    AVRCP_SUBUNIT_TYPE_CA,
    AVRCP_SUBUNIT_TYPE_CAMERA,
    AVRCP_SUBUNIT_TYPE_RESERVED,
    AVRCP_SUBUNIT_TYPE_PANEL = 9,
    AVRCP_SUBUNIT_TYPE_BULLETIN_BOARD,
    AVRCP_SUBUNIT_TYPE_CAMERA_STORAGE,
    AVRCP_SUBUNIT_TYPE_VENDOR_UNIQUE = 0x1C,
    AVRCP_SUBUNIT_TYPE_RESERVED_FOR_ALL_SUBUNIT_TYPES,
    AVRCP_SUBUNIT_TYPE_EXTENDED_TO_NEXT_BYTE,
    AVRCP_SUBUNIT_TYPE_UNIT = 0x1F
} avrcp_subunit_type_e;

typedef enum {
    AVRCP_SUBUNIT_ID = 0,
    AVRCP_SUBUNIT_ID_IGNORE = 7
} avrcp_subunit_id_e;

typedef enum {
    AVRCP_CMD_OPCODE_VENDOR_DEPENDENT = 0x00,
    AVRCP_CMD_OPCODE_UNIT_INFO = 0x30,
    AVRCP_CMD_OPCODE_SUBUNIT_INFO = 0x31,
    AVRCP_CMD_OPCODE_PASS_THROUGH = 0x7C,
    AVRCP_CMD_OPCODE_UNDEFINED = 0xFF
} avrcp_command_opcode_e;

typedef enum {
    
    AVRCP_OPERATION_ID_SELECT = 0x00,
    AVRCP_OPERATION_ID_UP = 0x01,
    AVRCP_OPERATION_ID_DOWN = 0x02,
    AVRCP_OPERATION_ID_LEFT = 0x03,
    AVRCP_OPERATION_ID_RIGHT = 0x04,
    AVRCP_OPERATION_ID_ROOT_MENU = 0x09,

	AVRCP_OPERATION_ID_CHANNEL_UP = 0x30,
    AVRCP_OPERATION_ID_CHANNEL_DOWN = 0x31,
    AVRCP_OPERATION_ID_SKIP = 0x3C,
    AVRCP_OPERATION_ID_VOLUME_UP = 0x41,
    AVRCP_OPERATION_ID_VOLUME_DOWN = 0x42,
    AVRCP_OPERATION_ID_MUTE = 0x43,
    
    AVRCP_OPERATION_ID_PLAY = 0x44,
    AVRCP_OPERATION_ID_STOP = 0x45,
    AVRCP_OPERATION_ID_PAUSE = 0x46,
    AVRCP_OPERATION_ID_REWIND = 0x48,
    AVRCP_OPERATION_ID_FAST_FORWARD = 0x49,
    AVRCP_OPERATION_ID_FORWARD = 0x4B,
    AVRCP_OPERATION_ID_BACKWARD = 0x4C,
    AVRCP_OPERATION_ID_UNDEFINED = 0xFF
} avrcp_operation_id_e;

typedef enum {
    AVRCP_APP_SETTING_ATTR_ILLEGAL = 0x00,
	AVRCP_APP_SETTING_ATTR_EQUALIZER = 0x01,
	AVRCP_APP_SETTING_ATTR_REPEAT = 0x02,
	AVRCP_APP_SETTING_ATTR_SHUFFLE = 0x03,
	AVRCP_APP_SETTING_ATTR_SCAN = 0x04,
} avrcp_app_setting_attr_e;

typedef enum {
	AVRCP_APP_SETTING_ATTR_EQUALIZER_MASK = 0x01,
	AVRCP_APP_SETTING_ATTR_REPEAT_MASK = 0x02,
	AVRCP_APP_SETTING_ATTR_SHUFFLE_MASK = 0x04,
	AVRCP_APP_SETTING_ATTR_SCAN_MASK = 0x08,
} avrcp_app_setting_attr_mask_e;

typedef enum{
	AVRCP_APP_SETTING_ATTR_EQUALIZER_OFF = 0x01,
	AVRCP_APP_SETTING_ATTR_EQUALIZER_ON = 0x01,
}avrcp_app_setting_attr_equalizer_status_e;

typedef enum{
	AVRCP_APP_SETTING_ATTR_REPEAT_OFF = 0x01,
	AVRCP_APP_SETTING_ATTR_REPEAT_SINGLE = 0x02,
	AVRCP_APP_SETTING_ATTR_REPEAT_ALL = 0x03,
	AVRCP_APP_SETTING_ATTR_REPEAT_GROUP = 0x04,
}avrcp_app_setting_attr_repeat_status_e;

typedef enum{
	AVRCP_APP_SETTING_ATTR_SHUFFLE_OFF = 0x01,
	AVRCP_APP_SETTING_ATTR_SHUFFLE_ALL = 0x02,
	AVRCP_APP_SETTING_ATTR_SHUFFLE_GROUP = 0x03,
}avrcp_app_setting_attr_shuffle_status_e;

typedef enum{
	AVRCP_APP_SETTING_ATTR_SCAN_OFF = 0x01,
	AVRCP_APP_SETTING_ATTR_SCAN_ALL = 0x02,
	AVRCP_APP_SETTING_ATTR_SCAN_GROUP = 0x03,
}avrcp_app_setting_attr_scan_status_e;


typedef enum{
    AVRCP_PLAYBACK_STATUS_STOPPED = 0x00,
    AVRCP_PLAYBACK_STATUS_PLAYING,
    AVRCP_PLAYBACK_STATUS_PAUSED,
    AVRCP_PLAYBACK_STATUS_FWD_SEEK,
    AVRCP_PLAYBACK_STATUS_REV_SEEK,
    AVRCP_PLAYBACK_STATUS_ERROR = 0xFF
} avrcp_playback_status_e;

typedef enum{
    AVRCP_BATTERY_STATUS_NORMAL = 0x00,
    AVRCP_BATTERY_STATUS_WARNING,
    AVRCP_BATTERY_STATUS_CRITICAL,
    AVRCP_BATTERY_STATUS_EXTERNAL,
    AVRCP_BATTERY_STATUS_FULL_CHARGE
} avrcp_battery_status_e;

typedef enum{
    AVRCP_SYSTEM_STATUS_POWER_ON = 0x00,
	AVRCP_SYSTEM_STATUS_POWER_OFF,
	AVRCP_SYSTEM_STATUS_UNPLUGGED,
} avrcp_system_status_e;


typedef struct
{
	uint8_t now_playing_title[AVRCP_ID3_TITIL_MAX_SIZE];
	uint8_t now_playing_artist[AVRCP_ID3_ARTIST_MAX_SIZE];
	uint8_t now_playing_album[AVRCP_ID3_ALBUM_MAX_SIZE];
	uint32_t current_index;
	uint32_t totol_count;
	uint32_t totol_milliseconds;
}now_playing_info_t;

struct avrcp_pcb_t
{
    struct avrcp_pcb_t *next; /* For the linked list */
	struct avctp_pcb_t *avctppcb;
	struct bd_addr_t remote_addr;
	uint16_t remote_notify_mask;
	uint16_t remote_app_setting_attr_mask;
	now_playing_info_t now_playing_info;
	uint8_t pass_though_wait_release;
};

typedef struct
{
    void (*avrcp_ctl_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*avrcp_ctl_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*avrcp_br_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*avrcp_br_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*avrcp_support_capabilities)(struct bd_addr_t *remote_addr,uint16_t support_cap_mask);
	void (*avrcp_app_setting_attr)(struct bd_addr_t *remote_addr,uint16_t setting_attr_mask);
	void (*avrcp_play_status_update)(struct bd_addr_t *remote_addr,uint8_t play_status);
	void (*avrcp_track_change_update)(struct bd_addr_t *remote_addr);
	void (*avrcp_playpos_change_update)(struct bd_addr_t *remote_addr,uint32_t millisecond);
	void (*avrcp_battary_change_update)(struct bd_addr_t *remote_addr,uint32_t battary_status);
	void (*avrcp_volume_change_update)(struct bd_addr_t *remote_addr,uint8_t volume);
	void (*avrcp_element_attr_update)(struct bd_addr_t *remote_addr,now_playing_info_t* now_playing_info);
} avrcp_controller_cbs_t;



err_t avrcp_controller_init(avrcp_controller_cbs_t *cb);
err_t avrcp_controller_list_app_setting_attr(struct bd_addr_t *remote_addr);
err_t avrcp_controller_get_play_status(struct bd_addr_t *remote_addr);
err_t avrcp_controller_register_notification(struct bd_addr_t *remote_addr,uint8_t notificaion_id);
err_t avrcp_controller_get_element_attributes(struct bd_addr_t *remote_addr);
err_t avrcp_controller_control(struct bd_addr_t *remote_addr,uint8_t control_id);

#endif

#endif


