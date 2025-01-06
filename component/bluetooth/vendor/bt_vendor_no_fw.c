#include "bt_vendor_cyw54591.h"
#include "bt_common.h"
#include "bt_pbuf.h"
#include "bt_hci.h"
#include "bt_phybusif_h4.h"

#if BT_VENDOR_COMMON_NO_FW_SUPPORT > 0

void h4_no_fw_vendor_init(init_done_cb cb,uint8_t ogf,uint8_t ocf)
{
	BT_VENDOR_TRACE_DEBUG("cyw54591_vendor_init ogf(0x%x) ocf(0x%x)\n",ogf,ocf);

	cb(VENDOR_STATUS_INITED);
}


void h4_no_fw_vendor_deinit(void)
{

}


chip_mgr_t h4_no_fw_mgr =
{
    /* const char * name */ "H4_NO_FW",
    /* void  (*vendor_init)(init_done_cb cb,uint8_t ogf,uint8_t ocf) */&h4_no_fw_vendor_init,
    /* void (*vendor_deinit)(void) */&h4_no_fw_vendor_deinit,
    /* void (*vendor_set_baud_rate)(uint32_t baud_rate);*/ NULL,
    /* void (*vendor_cmd_handle)(uint8_t ogf,uint8_t ocf,uint8_t *payload);*/NULL,
};
#endif


chip_mgr_t * h4_no_fw_instance(void)
{
#if BT_VENDOR_COMMON_NO_FW_SUPPORT > 0
    return &h4_no_fw_mgr;
#else
    return NULL;
#endif
}

