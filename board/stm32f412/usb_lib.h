/******************************************************************************
  * @file    usb_lib.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-26
  * @brief   led header file
******************************************************************************/
#ifndef USB_LIB_H_H_H
#define USB_LIB_H_H_H

#include "hw_misc.h"
#include "usb_device.h"

inline void hw_usb_init(){MX_USB_DEVICE_Init();};

#endif
