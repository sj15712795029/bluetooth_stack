/******************************************************************************
  * @file           bt_did.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt did header file
******************************************************************************/

#ifndef BT_DID_H_H_H
#define BT_DID_H_H_H

#include "bt_sdp.h"
#include "bt_common.h"

#define DID_VENDOR_ID_SOURCE_BLUETOOTH 0x0001
#define DIDS_VENDOR_ID_SOURCE_USB       0x0002

/* DID Attribute ID */
#define DID_SPEC_ID 0x0200
#define DID_VENDOR_ID 0x0201
#define DID_PRODUCT_ID 0x0202
#define DID_VERSION_ID 0x0203
#define DID_PRIMARY_RECORD 0x0204
#define DID_VENDOR_ID_SOURCE 0x0205
#define DID_CLIENT_EXECUTABLE_URL 0x000b
#define DID_SERVICE_DESCRIPTION 0x0001
#define DID_DOCU_URL 0x000a

/* DID Attribute value */
#define DID_SPEC_ID_VALUE 0x0103 /* DID SPEC V1.3 */
/*
	vendor ID value 0xffff :This value has special meaning depending on the context in which it used.
	Link Manager Protocol (LMP): This value may be used in the internal and
	interoperability tests before a Company ID has been assigned.
	This value shall not be used in shipping end products.
	Device ID Profile: This value is reserved as the default vendor ID
	when no Device ID service record is present in a remote device. 
*/
#define DID_VENDOR_ID_VALUE 0xffff
#define DID_PRODUCT_ID_VALUE 0xffff
#define DID_VERSION_ID_VALUE 0x0100
#define DID_PRIMARY_RECORD_VALUE 0x01
#define DID_VENDOR_ID_SOURCE_VALUE DID_VENDOR_ID_SOURCE_BLUETOOTH

err_t did_init(void);

#endif
