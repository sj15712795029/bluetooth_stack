#ifndef CSR8x11_H_H_H
#define CSR8x11_H_H_H
#include "bt_vendor_manager.h"


#define CSR_MANUFACTURER_NAME 0xa
#define CSR8x11_HCI_VERSION 0x6
#define CSR8x11_HCI_REVERSION 0x2031
#define CSR8x11_LMP_VERSION 0x6
#define CSR8x11_LMP_SUBVERSION 0x2031

#define PS_COMAND 0x7003
#define WARM_RESET 0x4002

#define VENDOR_CONTINUE 1
#define VENDOR_END 0

chip_mgr_t * csr8x11_instance(void);
#endif
