/******************************************************************************
  * @file           bt_did.c
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt did source file
******************************************************************************/

#include "bt_did.h"

#if PROFILE_DID_ENABLE

/* ²âÊÔEIR£¬Ð´ÎÄµµ */

static  uint8_t did_service_record[] =
{
    SDP_DES_SIZE8, 0x8,
    SDP_UINT16, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST>>8&0xff, BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST&0xff,  /* Service class ID list attribute */
            SDP_DES_SIZE8, 3,
            SDP_UUID16, BT_SERVICE_CLASS_PNP_INFORMATION>>8&0xff,BT_SERVICE_CLASS_PNP_INFORMATION&0xff,	/* PnP */

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, DID_SPEC_ID>>8&0xff, DID_SPEC_ID&0xff, /* SpecificationID ID 0x200 */
            SDP_UINT16,DID_SPEC_ID_VALUE>>8&0xff,DID_SPEC_ID_VALUE&0xff,

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, DID_VENDOR_ID>>8&0xff, DID_VENDOR_ID&0xff, /* VendorID ID 0x201 */
            SDP_UINT16,DID_VENDOR_ID_VALUE>>8&0xff,DID_VENDOR_ID_VALUE&0xff,

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, DID_PRODUCT_ID>>8&0xff, DID_PRODUCT_ID&0xff, /* ProductID ID 0x202 */
            SDP_UINT16,DID_PRODUCT_ID_VALUE>>8&0xff,DID_PRODUCT_ID_VALUE&0xff,

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, DID_VERSION_ID>>8&0xff, DID_VERSION_ID&0xff, /* Version ID 0x203 */
            SDP_UINT16,DID_VERSION_ID_VALUE>>8&0xff,DID_VERSION_ID_VALUE&0xff,

            SDP_DES_SIZE8, 0x5,
            SDP_UINT16, DID_PRIMARY_RECORD>>8&0xff, DID_PRIMARY_RECORD&0xff,/* PrimaryRecord ID 0x204 */
            SDP_BOOL,DID_PRIMARY_RECORD_VALUE,

            SDP_DES_SIZE8, 0x6,
            SDP_UINT16, DID_VENDOR_ID_SOURCE>>8&0xff, DID_VENDOR_ID_SOURCE&0xff, /* VendorIDSource ID 0x205 */
            SDP_UINT16,DID_VENDOR_ID_SOURCE_VALUE>>8&0xff,DID_VENDOR_ID_SOURCE_VALUE&0xff,
};

err_t did_init(void)
{
    sdp_record_t *record;

    uint8_t did_sdp_size = sizeof(did_service_record);
    uint32_t did_record_hdl = sdp_next_rhdl();

    if((record = sdp_record_new((uint8_t *)did_service_record,did_sdp_size,did_record_hdl)) == NULL)
    {
    	BT_DID_TRACE_DEBUG("did_init: Could not alloc SDP record\n");
        return BT_ERR_MEM;
    }
    else
    {
        sdp_register_service(record);
    }
    bt_hex_dump((uint8_t *)did_service_record,sizeof(did_service_record));

	return BT_ERR_OK;
}

#endif

