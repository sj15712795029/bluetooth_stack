/******************************************************************************
  * @file           bt_hci.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-10-31
  * @brief          bt chip manager source file
******************************************************************************/

#include "bt_vendor_manager.h"

extern chip_mgr_t * csr8x11_instance(void);
extern chip_mgr_t * bcm43430a1_instance(void);
extern chip_mgr_t * bcm4345c5_instance(void);
extern chip_mgr_t * cyw54591_instance(void);
extern chip_mgr_t * cyw43438_instance(void);
extern chip_mgr_t * h4_no_fw_instance(void);

chip_mgr_t * bt_vendor_get_chip_mrg(const uint8_t *name)
{
	if(strcmp(VENDOR_CSR8X11_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_CSR8X11_SUPPORT > 0
		return csr8x11_instance();
#endif
	}

	if(strcmp(VENDOR_BCM43430A1_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_BCM43430A1_SUPPORT > 0
		return bcm43430a1_instance();
#endif
	}

	if(strcmp(VENDOR_BCM4345C5_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_BCM4345C5_SUPPORT > 0
		return bcm4345c5_instance();
#endif
	}

	if(strcmp(VENDOR_CYW54591_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_CYW54591_SUPPORT > 0
		return cyw54591_instance();
#endif
	}

	if(strcmp(VENDOR_CYW43438_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_CYW43438_SUPPORT > 0
		return cyw43438_instance();
#endif
	}

	if(strcmp(VENDOR_H4_NO_FW_NAME,(const char*)name) == 0)
	{
#if BT_VENDOR_COMMON_NO_FW_SUPPORT > 0
		return h4_no_fw_instance();
#endif
	}


	return NULL;
}
