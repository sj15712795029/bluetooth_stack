#ifndef BT_WRAPPER_H_H_H
#define BT_WRAPPER_H_H_H

#include "bt_hfp_hf.h"
#include "bt_pbap_client.h"
#include "bt_avrcp_controller.h"
#include "bt_spp.h"
#include "bt_common.h"

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

}bt_cb_t;

uint8_t bt_start(bt_cb_t *cb);
uint8_t bt_stop(void);
uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_le_inquiry(uint8_t enable);
#endif
