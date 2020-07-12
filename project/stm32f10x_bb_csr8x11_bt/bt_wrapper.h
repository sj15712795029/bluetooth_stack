#ifndef BT_WRAPPER_H_H_H
#define BT_WRAPPER_H_H_H

#include "bt_hfp_hf.h"
#include "bt_pbap_client.h"
#include "bt_avrcp_controller.h"
#include "bt_common.h"

#define LINK_KEY_MAX 10
struct link_key_record
{
	struct bd_addr_t remote_addr;
	uint8_t link_key[16];
	uint8_t link_key_type;
};




uint8_t bt_start(void);
uint8_t bt_stop(void);
uint8_t bt_start_inquiry(uint8_t inquiry_len,uint8_t max_dev);
uint8_t bt_le_inquiry(uint8_t enable);
#endif
