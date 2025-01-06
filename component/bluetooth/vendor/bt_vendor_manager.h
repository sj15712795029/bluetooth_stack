#ifndef CHIP_MANAGER_H_H_H
#define CHIP_MANAGER_H_H_H

#include "bt_common.h"

#define VENDOR_CSR8X11_NAME "CSR8x11"
#define VENDOR_BCM43430A1_NAME "BCM43430A1"
#define VENDOR_BCM4345C5_NAME "BCM4345C5"
#define VENDOR_CYW54591_NAME "CYW54591"
#define VENDOR_CYW43438_NAME "CYW43438"
#define VENDOR_H4_NO_FW_NAME "H4_NO_FW"




typedef enum
{
    VENDOR_STATUS_SET_BAUD_RATE,
	VENDOR_STATUS_INITED,
} vendor_statue_e;


typedef void(*init_done_cb)(uint8_t vendor_status);
typedef struct
{

    const char * name;
    void  (*vendor_init)(init_done_cb cb,uint8_t ogf,uint8_t ocf);
    void (*vendor_deinit)(void);
	void (*vendor_set_baud_rate)(uint32_t baud_rate);
	void (*vendor_cmd_handle)(uint8_t ogf,uint8_t ocf,uint8_t *payload);
} chip_mgr_t;

chip_mgr_t *bt_vendor_get_chip_mrg(const uint8_t *name);



#endif
