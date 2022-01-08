#ifndef BT_WRAPPER_H_H_H
#define BT_WRAPPER_H_H_H

#include "bt_common.h"
#include "bt_pbuf.h"
#include "bt_memp.h"
#include "bt_mem.h"
#include "bt_timer.h"
#include "bt_config.h"
#include "bt_hci.h"
#include "bt_l2cap.h"
#include "bt_sdp.h"
#include "bt_rfcomm.h"
#include "bt_phybusif_h4.h"
#if BT_ENABLE_SNOOP > 0
#include "bt_snoop.h"
#endif
#if PROFILE_HFP_HF_ENABLE > 0
#include "bt_hfp_hf.h"
#endif
#if PROFILE_SPP_ENABLE > 0
#include "bt_spp.h"
#endif
#if PROFILE_DID_ENABLE > 0
#include "bt_did.h"
#endif
#if PROFILE_PBAP_ENABLE > 0
#include "bt_pbap_client.h"
#endif
#if PROFILE_A2DP_ENABLE > 0
#include "bt_a2dp_sink.h"
#endif
#if PROFILE_AVRCP_ENABLE > 0
#include "bt_avrcp_controller.h"
#endif
#if PROFILE_HID_ENABLE > 0
#include "bt_hid_device.h"
#endif

#if BT_BLE_ENABLE > 0
#include "bt_gatt.h"
#include "bt_smp.h"
#if PROFILE_BAS_ENABLE > 0
#include "bt_bas.h"
#endif
#endif






#define LINK_KEY_MAX 10
struct link_key_record
{
	struct bd_addr_t remote_addr;
	uint8_t link_key[16];
	uint8_t link_key_type;
};

#define BT_INIT_SUCCESS 0
#define BT_INIT_FAIL 1
#define BT_INQUIRY_START 0
#define BT_INQUIRY_COMPLETE 1
#define BT_LE_INQUIRY_START 0
#define BT_LE_INQUIRY_COMPLETE 1

#define BT_INQUIRY_GIAC 0x9e8b33
#define BT_INQUIRY_LIAC 0x9e8b00


#define BT_PROFILE_HFP_HF_MASK (1<<0)
#define BT_PROFILE_HFP_AG_MASK (1<<1)
#define BT_PROFILE_A2DP_SINK_MASK (1<<2)
#define BT_PROFILE_A2DP_SOURCE_MASK (1<<3)
#define BT_PROFILE_AVRCP_CONTROL_MASK (1<<4)
#define BT_PROFILE_AVRCP_TARGET_MASK (1<<5)
#define BT_PROFILE_PBAP_PCE_MASK (1<<6)
#define BT_PROFILE_PBAP_PSE_MASK (1<<7)
#define BT_PROFILE_DID_PSE_MASK (1<<8)
#define BT_PROFILE_SPP_MASK (1<<9)
#define BT_PROFILE_HID_DEVICE_MASK (1<<10)

#define HFP_VOLUME_MIN 0
#define HFP_VOLUME_MAX 15

#define AVRCP_CONTROL_ID_PLAY 0x44
#define AVRCP_CONTROL_ID_PAUSE 0x46
#define AVRCP_CONTROL_ID_FAST_BACKWARD 0x48
#define AVRCP_CONTROL_ID_FAST_FORWARD 0x49
#define AVRCP_CONTROL_ID_FORWARD 0x4b
#define AVRCP_CONTROL_ID_BACKWARD 0x4c



typedef struct
{
	void (*bt_init_result)(uint8_t status,uint16_t profile_mask);
	void (*bt_inquiry_status)(uint8_t status);
	void (*bt_inquiry_result)(struct bd_addr_t *address,uint8_t dev_type,uint8_t *name);
	void (*bt_hardware_error)(uint8_t reason);
#if BT_BLE_ENABLE > 0
	void (*bt_le_inquiry_status)(uint8_t status);
	void (*bt_le_inquiry_result)(struct bd_addr_t *address,int8_t rssi,uint8_t adv_type,uint8_t adv_size,uint8_t *adv_data);
#endif
}bt_app_common_cb_t;

typedef struct
{
	void (*bt_spp_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_spp_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_spp_recv_data)(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len);
}bt_app_spp_cb_t;

typedef struct
{
	void (*bt_hfp_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hfp_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hfp_sco_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hfp_sco_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hfp_signal_strength_ind)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_hfp_roam_status_ind)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_hfp_batt_level_ind)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_hfp_operator)(struct bd_addr_t *remote_addr,uint8_t *operator);
	void (*bt_hfp_call_status)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_hfp_call_setup)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_hfp_local_pn)(struct bd_addr_t *remote_addr,uint8_t *local_pn);
	void (*bt_hfp_call_pn)(struct bd_addr_t *remote_addr,uint8_t *phone_number);
	void (*bt_hfp_manu_id)(struct bd_addr_t *remote_addr,uint8_t *manu_id);
	void (*bt_hfp_module_id)(struct bd_addr_t *remote_addr,uint8_t *module_id);
}bt_app_hfp_cb_t;

typedef struct
{
	void (*bt_a2dp_signal_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_a2dp_signal_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_a2dp_stream_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_a2dp_stream_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_a2dp_start)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_a2dp_relase)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_a2dp_suspend)(struct bd_addr_t *remote_addr,uint8_t value);
	void (*bt_a2dp_abort)(struct bd_addr_t *remote_addr,uint8_t value);
}bt_app_a2dp_cb_t;

typedef struct
{
	void (*bt_avrcp_ctl_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_avrcp_ctl_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_avrcp_br_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_avrcp_br_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_avrcp_support_capabilities)(struct bd_addr_t *remote_addr,uint16_t support_cap_mask);
	void (*bt_avrcp_app_setting_attr)(struct bd_addr_t *remote_addr,uint16_t setting_attr_mask);
	void (*bt_avrcp_play_status_update)(struct bd_addr_t *remote_addr,uint8_t play_status);
	void (*bt_avrcp_track_change_update)(struct bd_addr_t *remote_addr);
	void (*bt_avrcp_playpos_change_update)(struct bd_addr_t *remote_addr,uint32_t millisecond);
	void (*bt_avrcp_battary_change_update)(struct bd_addr_t *remote_addr,uint32_t battary_status);
	void (*bt_avrcp_volume_change_update)(struct bd_addr_t *remote_addr,uint8_t volume);
	void (*bt_avrcp_element_attr_update)(struct bd_addr_t *remote_addr,uint8_t *title,uint8_t *artist,uint8_t *album,uint32_t current_index,uint32_t totol_count,uint32_t total_milliseconds);
}bt_app_avrcp_cb_t;

typedef struct
{
	void (*bt_hid_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hid_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_hid_interrupt_recv_data)(struct bd_addr_t *remote_addr,uint8_t *data,uint16_t data_len);
}bt_app_hid_cb_t;

typedef struct
{
	void (*bt_pbap_connect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_pbap_disconnect)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_pbap_query_repositories_size)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint16_t size);
	void (*bt_pbap_download_pb_status)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status);
	void (*bt_pbap_download_vcardlist_status)(struct bd_addr_t *remote_addr,uint8_t repositories,uint8_t type,uint8_t status);
}bt_app_pbap_cb_t;


typedef struct
{
    void (*bt_gatt_mtu_value)(struct bd_addr_t *remote_addr,uint16_t mtu);
	void (*bt_gattc_discovery_primary_service)(struct bd_addr_t *remote_addr,gatt_client_pri_service_t *pri_service,uint16_t count);
	void (*bt_gattc_discovery_uuid_primary_service)(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
	void (*bt_gattc_discovery_char)(struct bd_addr_t *remote_addr,uint16_t attribute_handle,uint16_t char_value_handle,uint8_t properties,uint16_t uuid16,uint8_t *uuid128);
} bt_gatt_client_cbs_t;

typedef struct
{
    void (*bt_gatt_mtu_value)(struct bd_addr_t *remote_addr,uint16_t mtu);
} bt_gatt_server_cbs_t;

typedef struct
{
	void (*bt_gatt_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*bt_gatt_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	bt_gatt_client_cbs_t *gatt_client_cbs;
	bt_gatt_server_cbs_t *gatt_server_cbs;
}bt_gatt_cbs_t;

typedef struct
{
	void (*bt_smp_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*bt_smp_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	void (*bt_smp_passkey_display)(struct bd_addr_t *remote_addr,uint32_t passkey);
	void (*bt_smp_passkey_input)(struct bd_addr_t *remote_addr,uint32_t *passkey);
}bt_smp_cbs_t;


typedef struct
{
	bt_app_common_cb_t *app_common_cb;
	bt_app_spp_cb_t *app_spp_cb;
	bt_app_hfp_cb_t * app_hfp_cb;
	bt_app_a2dp_cb_t *app_a2dp_cb;
	bt_app_avrcp_cb_t *app_avrcp_cb;
	bt_app_hid_cb_t *app_hid_cb;
	bt_app_pbap_cb_t *app_pbap_cb;
	bt_gatt_cbs_t *app_gatt_cb;
	bt_smp_cbs_t *app_smp_cb;
}bt_app_cb_t;


typedef struct
{
	uint8_t *adv_data;
	uint8_t adv_len;
	uint8_t adv_item_len;
	uint8_t adv_offset;
}bt_le_adv_parse_t;

/*********************** COMMON API ***********************/
uint8_t bt_start(bt_app_cb_t *app_cb);
uint8_t bt_stop(void);
uint8_t bt_write_current_iac(uint8_t iac_num,uint32_t *iac);
uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_stop_inquiry(void);
uint8_t bt_start_periodic_inquiry(uint16_t min_length,uint16_t max_length,uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_stop_periodic_inquiry(void);
uint8_t bt_get_remote_name(struct bd_addr_t *bdaddr);
uint8_t bt_cancel_get_remote_name(struct bd_addr_t *bdaddr);
#if BT_BLE_ENABLE > 0
uint8_t bt_le_start_inquiry(void);
uint8_t bt_le_stop_inquiry(void);
uint8_t bt_le_adv_parse_init(bt_le_adv_parse_t *bt_adv_le_parse,uint8_t adv_size,uint8_t *adv_data);
uint8_t bt_le_adv_has_more(bt_le_adv_parse_t *bt_adv_le_parse);
uint8_t bt_le_adv_data_parse(bt_le_adv_parse_t *bt_adv_le_parse,uint8_t *adv_item_type,uint8_t *adv_item_data_len,uint8_t **adv_item_data);
uint8_t bt_le_set_adv_enable(uint8_t adv_data_len,uint8_t *adv_data);
uint8_t bt_le_set_adv_disable(void);

#endif


#if PROFILE_HFP_HF_ENABLE
/************************* HFP hf API ***********************/
uint8_t bt_hfp_hf_get_operator(struct bd_addr_t *bdaddr);
uint8_t bt_hfp_hf_audio_transfer(struct bd_addr_t *bdaddr);
uint8_t bt_hfp_hf_accept_incoming_call(struct bd_addr_t *bdaddr);
uint8_t bt_hfp_hf_end_call(struct bd_addr_t *bdaddr);
uint8_t bt_hfp_hf_callout_by_number(struct bd_addr_t *addr,uint8_t *number);
uint8_t bt_hfp_hf_callout_by_memory(struct bd_addr_t *addr,uint8_t memory_id);
uint8_t bt_hfp_hf_callout_by_last(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_get_local_phone_number(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_get_call_list(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_disable_ecnr(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_set_mic_volume(struct bd_addr_t *addr,uint8_t value);
uint8_t bt_hfp_hf_set_spk_volume(struct bd_addr_t *addr,uint8_t value);
uint8_t bt_hfp_hf_transmit_dtmf(struct bd_addr_t *addr,uint8_t value);
uint8_t bt_hfp_hf_set_voice_recognition(struct bd_addr_t *addr,uint8_t enable);
uint8_t bt_hfp_hf_send_batt_level(struct bd_addr_t *addr,uint8_t batt_level);
uint8_t bt_hfp_hf_get_manufacturer_id(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_get_model_id(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_get_revision_id(struct bd_addr_t *addr);
uint8_t bt_hfp_hf_get_pid(struct bd_addr_t *addr);
#endif

#if PROFILE_AVRCP_ENABLE
/************************* AVRCP controller API ***********************/
uint8_t bt_avrcp_controller_get_play_status(struct bd_addr_t *remote_addr);
uint8_t bt_avrcp_controller_list_app_setting_attr(struct bd_addr_t *remote_addr);
uint8_t bt_avrcp_controller_get_element_attributes(struct bd_addr_t *remote_addr);
uint8_t bt_avrcp_controller_control(struct bd_addr_t *remote_addr,uint8_t control_id);

#endif

#if PROFILE_HID_ENABLE
/************************* HID device API ***********************/
uint8_t bt_hid_interupt_report(struct bd_addr_t *remote_addr,uint8_t *report,uint8_t report_size);
uint8_t bt_hid_find_keycode(uint8_t *keycode,uint8_t find_char);
#endif

#if PROFILE_PBAP_ENABLE
/************************* PBAP client API ***********************/
uint8_t bt_pbap_client_connect(struct bd_addr_t *remote_addr);
uint8_t bt_pbap_client_disconnect(struct bd_addr_t *remote_addr);
uint8_t bt_pbap_client_query_phonebook_size(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
uint8_t bt_pbap_client_download_phonebook(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
uint8_t bt_pbap_client_set_path(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
uint8_t bt_pbap_client_download_vcard_list(struct bd_addr_t *addr,uint8_t repositories,uint8_t type);
uint8_t bt_pbap_client_download_vcard_entry(struct bd_addr_t *addr,uint8_t repositories,uint8_t type,uint16_t entry_number);
uint8_t bt_pbap_client_download_abort(struct bd_addr_t *addr);
#endif

#if BT_BLE_ENABLE
uint8_t bt_smp_security_request(struct bd_addr_t *remote_addr);
uint8_t bt_gatt_client_exchange_mtu(struct bd_addr_t *remote_addr,uint16_t mtu);
uint8_t bt_gatt_client_discovery_pri_service(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
uint8_t bt_gatt_client_discovery_pri_service_uuid(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle,uint16_t uuid16,uint8_t *uuid128);
uint8_t bt_gatt_client_find_include(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
uint8_t bt_gatt_client_discovery_characteristics(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
uint8_t bt_gatt_client_discovery_char_des(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
uint8_t bt_gatt_client_read_char_value(struct bd_addr_t *remote_addr,uint16_t handle);


#endif


#endif
