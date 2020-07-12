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

#include "bt_sdp.h"
#include "bt_common.h"
#include "bt_avctp.h"


#define AVRCP_VENDOR_DEPENDENT_HDR_SIZE 10
#define AVRCP_PASS_THROUGH_SIZE 5

#define BT_SIG_COMPANY_ID 0x001958

typedef enum {
    AVRCP_MEDIA_ATTR_ALL = 0x0000,
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
    AVRCP_PDU_ID_GetCurrentPlayerApplicationSettingValue = 0x13,
    AVRCP_PDU_ID_SetPlayerApplicationSettingValue = 0x14,
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
    AVRCP_NOTIFICATION_EVENT_PLAYBACK_STATUS_CHANGED = 0x01,            // Change in playback status of the current track.
    AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED = 0x02,                      // Change of current track
    AVRCP_NOTIFICATION_EVENT_TRACK_REACHED_END = 0x03,                  // Reached end of a track
    AVRCP_NOTIFICATION_EVENT_TRACK_REACHED_START = 0x04,                // Reached start of a track
    AVRCP_NOTIFICATION_EVENT_PLAYBACK_POS_CHANGED = 0x05,               // Change in playback position. Returned after the specified playback notification change notification interval
    AVRCP_NOTIFICATION_EVENT_BATT_STATUS_CHANGED = 0x06,                // Change in battery status
    AVRCP_NOTIFICATION_EVENT_SYSTEM_STATUS_CHANGED = 0x07,              // Change in system status
    AVRCP_NOTIFICATION_EVENT_PLAYER_APPLICATION_SETTING_CHANGED = 0x08, // Change in player application setting
    AVRCP_NOTIFICATION_EVENT_NOW_PLAYING_CONTENT_CHANGED = 0x09,        // The content of the Now Playing list has changed, see 6.9.5.
    AVRCP_NOTIFICATION_EVENT_AVAILABLE_PLAYERS_CHANGED = 0x0a,          // The available players have changed, see 6.9.4.
    AVRCP_NOTIFICATION_EVENT_ADDRESSED_PLAYER_CHANGED = 0x0b,           // The Addressed Player has been changed, see 6.9.2.
    AVRCP_NOTIFICATION_EVENT_UIDS_CHANGED = 0x0c,                       // The UIDs have changed, see 6.10.3.3.
    AVRCP_NOTIFICATION_EVENT_VOLUME_CHANGED = 0x0d                      // The volume has been changed locally on the TG, see 6.13.3.
} avrcp_notification_event_id_e;


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
    AVRCP_CTYPE_RESPONSE_IN_TRANSITION, // target state is in transition. A subsequent STATUS command, may result in the return of a STABLE status
    AVRCP_CTYPE_RESPONSE_IMPLEMENTED_STABLE,
    AVRCP_CTYPE_RESPONSE_CHANGED_STABLE,
    AVRCP_CTYPE_RESPONSE_RESERVED,
    AVRCP_CTYPE_RESPONSE_INTERIM            // target is unable to respond with either ACCEPTED or REJECTED within 100 millisecond
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
    AVRCP_SUBUNIT_TYPE_EXTENDED_TO_NEXT_BYTE, // The unit_type field may take value 1E16, which means that the field is extended to the following byte. In that case, an additional byte for extended_unit_type will be added immediately following operand[1].
                                              // Further extension is possible when the value of extended_unit_type is FF16, in which case another byte will be added.
    AVRCP_SUBUNIT_TYPE_UNIT = 0x1F
} avrcp_subunit_type_e;

typedef enum {
    AVRCP_SUBUNIT_ID = 0,
    AVRCP_SUBUNIT_ID_IGNORE = 7
} avrcp_subunit_id_e;

typedef enum {
    AVRCP_CMD_OPCODE_VENDOR_DEPENDENT = 0x00,
    // AVRCP_CMD_OPCODE_RESERVE = 0x01,
    AVRCP_CMD_OPCODE_UNIT_INFO = 0x30,
    AVRCP_CMD_OPCODE_SUBUNIT_INFO = 0x31,
    AVRCP_CMD_OPCODE_PASS_THROUGH = 0x7C,
    // AVRCP_CMD_OPCODE_VERSION = 0xB0,
    // AVRCP_CMD_OPCODE_POWER = 0xB2,
    AVRCP_CMD_OPCODE_UNDEFINED = 0xFF
} avrcp_command_opcode_e;

/* reference AV/C subunit spec */
typedef enum {
    AVRCP_OPERATION_ID_CHANNEL_UP = 0x30,
    AVRCP_OPERATION_ID_CHANNEL_DOWN = 0x31,
    AVRCP_OPERATION_ID_SELECT = 0x00,
    AVRCP_OPERATION_ID_UP = 0x01,
    AVRCP_OPERATION_ID_DOWN = 0x02,
    AVRCP_OPERATION_ID_LEFT = 0x03,
    AVRCP_OPERATION_ID_RIGHT = 0x04,
    AVRCP_OPERATION_ID_ROOT_MENU = 0x09,

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

err_t avrcp_controller_init(void);
#endif


