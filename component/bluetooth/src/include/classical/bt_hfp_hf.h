/******************************************************************************
  * @file           bt_hfp_hf.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt hfp hf role header file
******************************************************************************/

#ifndef BT_HFP_HF_H_H_H
#define BT_HFP_HF_H_H_H

#include "bt_common.h"


#if PROFILE_HFP_HF_ENABLE

#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"


#define HFP_HF_MAX_INDICATOR_NUM 20
/* Reference HFP V1.7 Spec */
/* HF Supported Features:
0: EC and/or NR function
1: Three-way calling
2: CLI presentation capability
3: Voice recognition activation
4: Remote volume control
5: Enhanced call status
6: Enhanced call control
7: Codec negotiation
8: HF Indicators
9: eSCO S4 (and T2) Settings Supported
10-31: Reserved for future definition
*/
#define HFP_HFSF_EC_NR_FUNCTION              (1<<0)
#define HFP_HFSF_THREE_WAY_CALLING           (1<<1)
#define HFP_HFSF_CLI_PRESENTATION_CAPABILITY (1<<2)
#define HFP_HFSF_VOICE_RECOGNITION_FUNCTION  (1<<3)
#define HFP_HFSF_REMOTE_VOLUME_CONTROL       (1<<4)
#define HFP_HFSF_ENHANCED_CALL_STATUS        (1<<5)
#define HFP_HFSF_ENHANCED_CALL_CONTROL       (1<<6)
#define HFP_HFSF_CODEC_NEGOTIATION           (1<<7)
#define HFP_HFSF_HF_INDICATORS               (1<<8)
#define HFP_HFSF_ESCO_S4                     (1<<9)

#define HFP_HF_SDP_UNSUPPORT_WBS 0
#define HFP_HF_SDP_SUPPORT_WBS 1
#define HFP_HF_SDP_SF_WBS (1<<5)
#define HFP_HF_SDP_SF_MASK 0x1f

/* AG Supported Features:
0: Three-way calling
1: EC and/or NR function
2: Voice recognition function
3: In-band ring tone capability
4: Attach a number to a voice tag
5: Ability to reject a call
6: Enhanced call status
7: Enhanced call control
8: Extended Error Result Codes
9: Codec negotiation
10: HF Indicators
11: eSCO S4 (and T2) Settings Supported
12-31: Reserved for future definition
*/
#define HFP_AGSF_THREE_WAY_CALLING              (1<<0)
#define HFP_AGSF_EC_NR_FUNCTION                 (1<<1)
#define HFP_AGSF_VOICE_RECOGNITION_FUNCTION     (1<<2)
#define HFP_AGSF_IN_BAND_RING_TONE              (1<<3)
#define HFP_AGSF_ATTACH_A_NUMBER_TO_A_VOICE_TAG (1<<4)
#define HFP_AGSF_ABILITY_TO_REJECT_A_CALL       (1<<5)
#define HFP_AGSF_ENHANCED_CALL_STATUS           (1<<6)
#define HFP_AGSF_ENHANCED_CALL_CONTROL          (1<<7)
#define HFP_AGSF_EXTENDED_ERROR_RESULT_CODES    (1<<8)
#define HFP_AGSF_CODEC_NEGOTIATION              (1<<9)
#define HFP_AGSF_HF_INDICATORS                 (1<<10)
#define HFP_AGSF_ESCO_S4                       (1<<11)

#define THREE_CALL_CMD_USER_BUSY 0x0
#define THREE_CALL_CMD_REL_ACTIVE_CALL 0x1
#define THREE_CALL_CMD_SWAP_CALL 0x2
#define THREE_CALL_CMD_JOHN_HELD_CALL 0x3
#define THREE_CALL_CMD_CONNECT_CALL 0x4

#define THREE_CALL_SERVICE_0		0x01
#define THREE_CALL_SERVICE_1		0x02
#define THREE_CALL_SERVICE_2		0x04
#define THREE_CALL_SERVICE_3		0x08
#define THREE_CALL_SERVICE_4		0x10
#define THREE_CALL_SERVICE_1x		0x20
#define THREE_CALL_SERVICE_2x		0x40



typedef enum
{
    HFP_IDLE = 0,
    HFP_W2_SDP_CONNECTED,
    HFP_W2_SDP_QUERY_RFCOMM_CHANNEL,
    HFP_W4_SDP_QUERY_COMPLETE,
    HFP_W2_SDP_DISCONNECTD,
    HFP_W4_SDP_DISCONNECTD,
    HFP_W2_RFCOMM_CONNECTED,
    HFP_W4_RFCOMM_CONNECTED,

    HFP_W2_SERVER_CN_CONNECTED,
    HFP_W4_SERVER_CN_CONNECTED,
    HFP_EXCHANGE_SUPPORTED_FEATURES,
    HFP_W4_EXCHANGE_SUPPORTED_FEATURES,

    HFP_NOTIFY_ON_CODECS,
    HFP_W4_NOTIFY_ON_CODECS,

    HFP_RETRIEVE_INDICATORS,
    HFP_W4_RETRIEVE_INDICATORS,

    HFP_RETRIEVE_INDICATORS_STATUS,
    HFP_W4_RETRIEVE_INDICATORS_STATUS,

    HFP_ENABLE_INDICATORS_STATUS_UPDATE,
    HFP_W4_ENABLE_INDICATORS_STATUS_UPDATE,

    HFP_RETRIEVE_CAN_HOLD_CALL,
    HFP_W4_RETRIEVE_CAN_HOLD_CALL,

    HFP_LIST_GENERIC_STATUS_INDICATORS,
    HFP_W4_LIST_GENERIC_STATUS_INDICATORS,

    HFP_RETRIEVE_GENERIC_STATUS_INDICATORS,
    HFP_W4_RETRIEVE_GENERIC_STATUS_INDICATORS,

    HFP_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS,
    HFP_W4_RETRIEVE_INITITAL_STATE_GENERIC_STATUS_INDICATORS,

    HFP_SERVICE_LEVEL_CONNECTION_ESTABLISHED,

    HFP_W2_ACCPET_CONNECT_SCO,
    HFP_W2_CONNECT_SCO,
    HFP_W4_SCO_CONNECTED,

    HFP_AUDIO_CONNECTION_ESTABLISHED,

    HFP_W2_DISCONNECT_SCO,
    HFP_W4_SCO_DISCONNECTED,
    HFP_SCO_DISCONNECTED,

    HFP_W2_DISCONNECT_RFCOMM,
    HFP_W4_RFCOMM_DISCONNECTED,
    HFP_W4_RFCOMM_DISCONNECTED_AND_RESTART,
    HFP_W4_CONNECTION_ESTABLISHED_TO_SHUTDOWN
} hfp_state_e;

typedef enum
{
    HFP_SERVICE_UNAVAILABLE,
    HFP_SERVICE_AVAILABLE,
} hfp_service_indictor_e;

typedef enum
{
    HFP_CALL_NO_INPORCESS,
    HFP_CALL_INPORCESS,
} hfp_call_indictor_e;

typedef enum
{
    HFP_CALL_NO_CALL,
    HFP_CALL_INCOMING_CALL,
    HFP_CALL_OUTGOING_CALL,
    HFP_CALL_RALERT_OUTGOING_CALL,
} hfp_callsetup_indictor_e;

typedef enum
{
    HFP_CALL_NO_CALL_HELD,
    HFP_CALL_CALL_ACTIVE_HELD,
    HFP_CALL_CALL_NO_ACTIVE_HELD,
} hfp_callheld_indictor_e;

typedef enum
{
    HFP_ROAM_DEACTIVE,
    HFP_ROAM_ACTIVE,
} hfp_roam_indictor_e;

typedef enum
{
    HFP_PUT_HOLDCALL,
    HFP_ACCEPT_HOLD_CALL,
    HFP_REJECT_HOLD_CALL,
} hfp_hold_status_e;

typedef enum
{
	HFP_EN_SAFE_IND = 1,
	HFP_BATT_LEVEL_IND =2,
}hfp_hf_indicator_e;

typedef enum
{
    HFP_CME_ERROR_AG_FAILURE = 0,
    HFP_CME_ERROR_NO_CONNECTION_TO_PHONE,
    HFP_CME_ERROR_2,
    HFP_CME_ERROR_OPERATION_NOT_ALLOWED,
    HFP_CME_ERROR_OPERATION_NOT_SUPPORTED,
    HFP_CME_ERROR_PH_SIM_PIN_REQUIRED,
    HFP_CME_ERROR_6,
    HFP_CME_ERROR_7,
    HFP_CME_ERROR_8,
    HFP_CME_ERROR_9,
    HFP_CME_ERROR_SIM_NOT_INSERTED,
    HFP_CME_ERROR_SIM_PIN_REQUIRED,
    HFP_CME_ERROR_SIM_PUK_REQUIRED,
    HFP_CME_ERROR_SIM_FAILURE,
    HFP_CME_ERROR_SIM_BUSY,
    HFP_CME_ERROR_15,
    HFP_CME_ERROR_INCORRECT_PASSWORD,
    HFP_CME_ERROR_SIM_PIN2_REQUIRED,
    HFP_CME_ERROR_SIM_PUK2_REQUIRED,
    HFP_CME_ERROR_19,
    HFP_CME_ERROR_MEMORY_FULL,
    HFP_CME_ERROR_INVALID_INDEX,
    HFP_CME_ERROR_22,
    HFP_CME_ERROR_MEMORY_FAILURE,
    HFP_CME_ERROR_TEXT_STRING_TOO_LONG,
    HFP_CME_ERROR_INVALID_CHARACTERS_IN_TEXT_STRING,
    HFP_CME_ERROR_DIAL_STRING_TOO_LONG,
    HFP_CME_ERROR_INVALID_CHARACTERS_IN_DIAL_STRING,
    HFP_CME_ERROR_28,
    HFP_CME_ERROR_29,
    HFP_CME_ERROR_NO_NETWORK_SERVICE,
    HFP_CME_ERROR_NETWORK_TIMEOUT,
    HFP_CME_ERROR_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY
} hfp_cme_error_e;

#define HFP_HF_INDICATOR_ENHANDCED_SAFETY 0x01	/* 0:disable 1:enable */
#define HFP_HF_INDICATOR_BATTERY_LEVEL 0x02			/* range 0~100 */

#define HFP_CODEC_CVSD 0x01
#define HFP_CODEC_MSBC 0x02

typedef struct
{
    void (*hfp_hf_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*hfp_hf_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*hfp_hf_sco_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*hfp_hf_sco_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*hfp_hf_call_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_call_setup_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_call_held_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_signal_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_battchg_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_server_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_roam_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_network)(struct bd_addr_t *remote_addr,uint8_t mode,uint8_t format,uint8_t *operator,uint8_t operator_len);
    void (*hfp_hf_ring)(struct bd_addr_t *remote_addr);
    void (*hfp_hf_clip)(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type);
    void (*hfp_hf_call_waiting)(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type);
    void (*hfp_hf_voice_recognition)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_spk_vol)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_mic_vol)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_hold_status)(struct bd_addr_t *remote_addr,uint8_t value);
    void (*hfp_hf_local_number)(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t type,uint8_t service);
    void (*hfp_hf_call_list)(struct bd_addr_t *remote_addr,uint8_t *number,uint8_t number_len,uint8_t index,uint8_t dir,uint8_t status,uint8_t mode,uint8_t mpty,uint8_t type);
    void (*hfp_hf_manufacturer_id)(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len);
    void (*hfp_hf_model_id)(struct bd_addr_t *remote_addr,uint8_t *mid,uint8_t mid_len);
    void (*hfp_hf_revision_id)(struct bd_addr_t *remote_addr,uint8_t *rid,uint8_t rid_len);
    void (*hfp_hf_product_id)(struct bd_addr_t *remote_addr,uint8_t *pid,uint8_t pid_len);
} hfp_hf_cbs_t;



struct hfp_pcb_t
{
    struct hfp_pcb_t *next; /* For the linked list */
    rfcomm_pcb_t *rfcommpcb;
    sdp_pcb_t *sdppcb;
    l2cap_pcb_t*l2cappcb;

    struct bd_addr_t remote_addr;
    uint16_t hfp_hf_local_support_feature;
    uint16_t hfp_hf_remote_support_feature;
    uint8_t hfp_hf_remote_three_call_support_bitmap;
    uint8_t hfp_hf_remote_network_name[32];

    uint8_t remote_cn;
    hfp_state_e state;
    uint8_t hfp_hf_slc_done;

    uint8_t call_index;
    uint8_t call_setup_index;
    uint8_t call_held_index;
    uint8_t service_index;
    uint8_t signal_index;
    uint8_t roam_index;
    uint8_t battchg_index;
    uint8_t number_of_indicator;
    uint8_t indicator_enable_map[HFP_HF_MAX_INDICATOR_NUM];

    uint8_t ag_support_enhandced_safety_indicator;
    uint8_t ag_enhandced_safety_indicator_enable;
    uint8_t ag_support_battery_level_indicator;
    uint8_t ag_battery_level_indicator_enable;

    uint8_t ag_support_inband_ring;
    uint8_t audio_codec_select;
};


err_t hfp_hf_init(uint16_t hf_support_feature,uint8_t support_wbs,hfp_hf_cbs_t *cb);
err_t hfp_hf_connect(struct bd_addr_t *addr);
err_t hfp_hf_disconnect(struct bd_addr_t *addr);
err_t hfp_hf_set_format_network(struct bd_addr_t *addr);
err_t hfp_hf_get_network(struct bd_addr_t *addr);
err_t hfp_hf_audio_connect(struct bd_addr_t *addr);
err_t hfp_hf_audio_disconnect(struct bd_addr_t *addr);
err_t hfp_hf_set_ag_extended_error(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_answer_incoming_call(struct bd_addr_t *addr);
err_t hfp_hf_hangup(struct bd_addr_t *addr);
err_t hfp_hf_callout_with_phone_number(struct bd_addr_t *addr,uint8_t *number);
err_t hfp_hf_callout_with_memory(struct bd_addr_t *addr,uint8_t memory_id);
err_t hfp_hf_callout_with_last_number(struct bd_addr_t *addr);
err_t hfp_hf_set_call_waiting_notification(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_three_call_control(struct bd_addr_t *addr,uint8_t cmd,uint8_t index);
err_t hfp_hf_set_call_line_identification_notification(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_disable_ag_nrec(struct bd_addr_t *addr);
err_t hfp_hf_set_voice_recognition(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_get_phone_number_via_voice_tag(struct bd_addr_t *addr);
err_t hfp_hf_transmit_dtmf(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_set_mic_volume(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_set_spk_volume(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_query_hold_status(struct bd_addr_t *addr);
err_t hfp_hf_control_call_hold(struct bd_addr_t *addr,uint8_t value);
err_t hfp_hf_get_local_phone_number(struct bd_addr_t *addr);
err_t hfp_hf_query_call_list(struct bd_addr_t *addr);
err_t hfp_hf_transfer_hf_indicator_value(struct bd_addr_t *addr,uint8_t indicator,uint8_t value);
err_t hfp_hf_set_indicator_enable_value(struct bd_addr_t *addr,uint8_t *indicator_name,uint8_t value);
err_t hfp_hf_get_manufacturer_id(struct bd_addr_t *addr);
err_t hfp_hf_get_model_id(struct bd_addr_t *addr);
err_t hfp_hf_get_revision_id(struct bd_addr_t *addr);
err_t hfp_hf_get_pid(struct bd_addr_t *addr);

#endif

#endif

