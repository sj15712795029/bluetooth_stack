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
#if PROFILE_HFP_ENABLE > 0
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

typedef struct
{
	void (*bt_init_result)(uint8_t status,uint16_t profile_mask);
	void (*bt_inquiry_status)(uint8_t status);
	void (*bt_inquiry_result)(struct bd_addr_t *address,uint8_t dev_type,uint8_t *name);

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

}bt_app_hfp_cb_t;


typedef struct
{
	bt_app_common_cb_t *app_common_cb;
	bt_app_spp_cb_t *app_spp_cb;
	bt_app_hfp_cb_t * app_hfp_cb;
	
}bt_app_cb_t;

uint8_t bt_start(bt_app_cb_t *app_cb);
uint8_t bt_stop(void);
uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_stop_inquiry(void);
uint8_t hci_start_periodic_inquiry(uint16_t min_length,uint16_t max_length,uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_stop_periodic_inquiry(void);
uint8_t bt_get_remote_name(struct bd_addr_t *bdaddr);
uint8_t bt_cancel_get_remote_name(struct bd_addr_t *bdaddr);
uint8_t bt_le_inquiry(uint8_t enable);
#endif
