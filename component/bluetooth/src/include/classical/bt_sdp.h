/******************************************************************************
  * @file           bt_sdo.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-16
  * @brief          bt sdp header file
******************************************************************************/

#ifndef BT_SDP_H_H_H
#define BT_SDP_H_H_H

#include "bt_l2cap.h"


/* Type is constructed by ORing a type and size bitmask.
   Size is ignored for String, URL and sequence types.
   For String, URL types, the given value must be a char*,
   from which the size is calculated.
   For a sequence type the size is calculated directly from the
   list of elements added into the sequence.
   For integer types greater than 32 bit, and for 128 bit UUID
   types, the value is given as a byte array.
*/

#define SDP_DE_TYPE_NIL 0x00 /* Nil, the null type */
#define SDP_DE_TYPE_UINT 0x08 /* Unsigned Integer */
#define SDP_DE_TYPE_STCI 0x10 /* Signed, twos-complement integer */
#define SDP_DE_TYPE_UUID 0x18 /* UUID, a universally unique identifier */
#define SDP_DE_TYPE_STR 0x20 /* Text string */
#define SDP_DE_TYPE_BOOL 0x28 /* Boolean */
#define SDP_DE_TYPE_DES 0x30 /* Data Element Sequence */
#define SDP_DE_TYPE_DEA 0x38 /* Data Element Alternative */
#define SDP_DE_TYPE_URL 0x40 /* URL, a uniform resource locator */

#define SDP_DE_SIZE_8 0x0 /* 8 bit integer value */
#define SDP_DE_SIZE_16 0x1 /* 16 bit integer value */
#define SDP_DE_SIZE_32 0x2 /* 32 bit integer value */
#define SDP_DE_SIZE_64 0x3 /* 64 bit integer value */
#define SDP_DE_SIZE_128 0x4 /* 128 bit integer value */
#define SDP_DE_SIZE_N1 0x5 /* Data size is in next 1 byte */
#define SDP_DE_SIZE_N2 0x6 /* Data size is in next 2 bytes */
#define SDP_DE_SIZE_N4 0x7 /* Data size is in next 4 bytes */


#define SDP_DES_SIZE8	(SDP_DE_TYPE_DES  | SDP_DE_SIZE_N1)
#define SDP_DES_SIZE16	(SDP_DE_TYPE_DES  | SDP_DE_SIZE_N2)
#define SDP_UINT8		(SDP_DE_TYPE_UINT | SDP_DE_SIZE_8)
#define SDP_UINT16		(SDP_DE_TYPE_UINT | SDP_DE_SIZE_16)
#define SDP_UINT32		(SDP_DE_TYPE_UINT | SDP_DE_SIZE_32)
#define SDP_UUID16		(SDP_DE_TYPE_UUID | SDP_DE_SIZE_16)
#define SDP_UUID128		(SDP_DE_TYPE_UUID | SDP_DE_SIZE_128)

/* PDU identifiers */
#define SDP_ERR_PDU 0x01
#define SDP_SS_PDU 0x02
#define SDP_SSR_PDU 0x03
#define SDP_SA_PDU 0x04
#define SDP_SAR_PDU 0x05
#define SDP_SSA_PDU 0x06
#define SDP_SSAR_PDU 0x07

/* Response lengths and sizes */
#define SDP_PDUHDR_LEN 5
#define SDP_ATTRIBIDHDR_LEN 3
#define SDP_SSR_LEN 4
#define SDP_SRHDL_SIZE 4 /* Size of a service record handle */

/**/
#define SDP_MAX_SRHDLS 12


struct sdp_hdr_t {
  uint8_t pdu;
  uint16_t id;
  uint16_t len;
}BT_PACK_END;

struct sdp_record_t {
  struct sdp_record_t *next; /* For the linked list */
  
  uint32_t hdl; /* Service Record Handle */
  uint8_t *record_de_list;
  uint8_t len;
};

/* The SDP protocol control block */
struct sdp_pcb_t {
  struct sdp_pcb_t *next; /* For the linked list */

  struct l2cap_pcb_t *l2cappcb; /* The L2CAP connection */

  uint16_t tid; /* Transaction ID */
  
  void *callback_arg;

  void (* service_searched)(void *arg, struct sdp_pcb_t *pcb, uint16_t tot_src, uint16_t curr_src, uint32_t *rhdls);
  void (* attributes_recv)(void *arg, struct sdp_pcb_t *pcb, uint16_t attribl_bc,struct bt_pbuf_t *p);
  void (* attributes_searched)(void *arg, struct sdp_pcb_t *pcb, uint16_t attribl_bc, struct bt_pbuf_t *p);
};

#define SDP_DE_TYPE(type_size) ((type_size) & 0xF8)
#define SDP_DE_SIZE(type_size) ((type_size) & 0x07)



/**
 * Protocol Identifiers
 */
#define BT_PROTOCOL_SDP                                                     0x0001 // Bluetooth Core Specification
#define BT_PROTOCOL_UDP                                                     0x0002 // [NO USE BY PROFILES]
#define BT_PROTOCOL_RFCOMM                                                  0x0003 // RFCOMM with TS 07.10
#define BT_PROTOCOL_TCP                                                     0x0004 // [NO USE BY PROFILES]
#define BT_PROTOCOL_TCS_BIN                                                 0x0005 // Telephony Control Specification / TCS Binary [DEPRECATED]
#define BT_PROTOCOL_TCS_AT                                                  0x0006 // [NO USE BY PROFILES]
#define BT_PROTOCOL_ATT                                                     0x0007 // Attribute Protocol
#define BT_PROTOCOL_OBEX                                                    0x0008 // IrDA Interoperability
#define BT_PROTOCOL_IP                                                      0x0009 // [NO USE BY PROFILES]
#define BT_PROTOCOL_FTP                                                     0x000A // [NO USE BY PROFILES]
#define BT_PROTOCOL_HTTP                                                    0x000C // [NO USE BY PROFILES]
#define BT_PROTOCOL_WSP                                                     0x000E // [NO USE BY PROFILES]
#define BT_PROTOCOL_BNEP                                                    0x000F // Bluetooth Network Encapsulation Protocol (BNEP)
#define BT_PROTOCOL_UPNP                                                    0x0010 // Extended Service Discovery Profile (ESDP) [DEPRECATED]
#define BT_PROTOCOL_HIDP                                                    0x0011 // Human Interface Device Profile (HID)
#define BT_PROTOCOL_HARDCOPY_CONTROL_CHANNEL                                0x0012 // Hardcopy Cable Replacement Profile (HCRP)
#define BT_PROTOCOL_HARDCOPY_DATA_CHANNEL                                   0x0014 // See Hardcopy Cable Replacement Profile (HCRP)
#define BT_PROTOCOL_HARDCOPY_NOTIFICATION                                   0x0016 // Hardcopy Cable Replacement Profile (HCRP)
#define BT_PROTOCOL_AVCTP                                                   0x0017 // Audio/Video Control Transport Protocol (AVCTP)
#define BT_PROTOCOL_AVDTP                                                   0x0019 // Audio/Video Distribution Transport Protocol (AVDTP)
#define BT_PROTOCOL_CMTP                                                    0x001B // Common ISDN Access Profile (CIP) [DEPRECATED]
#define BT_PROTOCOL_MCAP_CONTROL_CHANNEL                                    0x001E // Multi-Channel Adaptation Protocol (MCAP)
#define BT_PROTOCOL_MCAP_DATA_CHANNEL                                       0x001F // Multi-Channel Adaptation Protocol (MCAP)
#define BT_PROTOCOL_L2CAP                                                   0x0100 // Bluetooth Core Specification

/**
 * Service Classes
 */
#define BT_SERVICE_CLASS_SERVICE_DISCOVERY_SERVER                           0x1000 // Bluetooth Core Specification
#define BT_SERVICE_CLASS_BROWSE_GROUP_DESCRIPTOR                            0x1001 // Bluetooth Core Specification
#define BT_SERVICE_CLASS_SERIAL_PORT                                        0x1101 // Serial Port Profile (SPP) NOTE: The example SDP record in SPP v1.0 does not include a BluetoothProfileDescriptorList attribute, but some implementations may also use this UUID for the Profile Identifier.
#define BT_SERVICE_CLASS_LAN_ACCESS_USING_PPP                               0x1102 // LAN Access Profile [DEPRECATED] NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_DIALUP_NETWORKING                                  0x1103 // Dial-up Networking Profile (DUN) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_IR_MC_SYNC                                         0x1104 // Synchronization Profile (SYNC) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_OBEX_OBJECT_PUSH                                   0x1105 // Object Push Profile (OPP) NOTE: Used as both Service Class Identifier and Profile.
#define BT_SERVICE_CLASS_OBEX_FILE_TRANSFER                                 0x1106 // File Transfer Profile (FTP) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_IR_MC_SYNC_COMMAND                                 0x1107 // Synchronization Profile (SYNC)
#define BT_SERVICE_CLASS_HEADSET                                            0x1108 // Headset Profile (HSP) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_CORDLESS_TELEPHONY                                 0x1109 // Cordless Telephony Profile (CTP) NOTE: Used as both Service Class Identifier and Profile Identifier. [DEPRECATED]
#define BT_SERVICE_CLASS_AUDIO_SOURCE                                       0x110A // Advanced Audio Distribution Profile (A2DP)
#define BT_SERVICE_CLASS_AUDIO_SINK                                         0x110B // Advanced Audio Distribution Profile (A2DP)
#define BT_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET                           0x110C // Audio/Video Remote Control Profile (AVRCP)
#define BT_SERVICE_CLASS_ADVANCED_AUDIO_DISTRIBUTION                        0x110D // Advanced Audio Distribution Profile (A2DP)
#define BT_SERVICE_CLASS_AV_REMOTE_CONTROL                                  0x110E // Audio/Video Remote Control Profile (AVRCP) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_AV_REMOTE_CONTROL_CONTROLLER                       0x110F // Audio/Video Remote Control Profile (AVRCP) NOTE: The AVRCP specification v1.3 and later require that 0x110E also be included in the ServiceClassIDList before 0x110F for backwards compatibility.
#define BT_SERVICE_CLASS_INTERCOM                                           0x1110 // Intercom Profile (ICP) NOTE: Used as both Service Class Identifier and Profile Identifier. [DEPRECATED]
#define BT_SERVICE_CLASS_FAX                                                0x1111 // Fax Profile (FAX) NOTE: Used as both Service Class Identifier and Profile Identifier. [DEPRECATED]
#define BT_SERVICE_CLASS_HEADSET_AUDIO_GATEWAY_AG                           0x1112 // Headset Profile (HSP)
#define BT_SERVICE_CLASS_WAP                                                0x1113 // Interoperability Requirements for Bluetooth technology as a WAP, Bluetooth SIG [DEPRECATED]
#define BT_SERVICE_CLASS_WAP_CLIENT                                         0x1114 // Interoperability Requirements for Bluetooth technology as a WAP, Bluetooth SIG [DEPRECATED]
#define BT_SERVICE_CLASS_PANU                                               0x1115 // Personal Area Networking Profile (PAN) NOTE: Used as both Service Class Identifier and Profile Identifier for PANU role.
#define BT_SERVICE_CLASS_NAP                                                0x1116 // Personal Area Networking Profile (PAN) NOTE: Used as both Service Class Identifier and Profile Identifier for NAP role.
#define BT_SERVICE_CLASS_GN                                                 0x1117 // Personal Area Networking Profile (PAN) NOTE: Used as both Service Class Identifier and Profile Identifier for GN role.
#define BT_SERVICE_CLASS_DIRECT_PRINTING                                    0x1118 // Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_REFERENCE_PRINTING                                 0x1119 // See Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_BASIC_IMAGING_PROFILE                              0x111A // Basic Imaging Profile (BIP)
#define BT_SERVICE_CLASS_IMAGING_RESPONDER                                  0x111B // Basic Imaging Profile (BIP)
#define BT_SERVICE_CLASS_IMAGING_AUTOMATIC_ARCHIVE                          0x111C // Basic Imaging Profile (BIP)
#define BT_SERVICE_CLASS_IMAGING_REFERENCED_OBJECTS                         0x111D // Basic Imaging Profile (BIP)
#define BT_SERVICE_CLASS_HANDSFREE                                          0x111E // Hands-Free Profile (HFP) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_HANDSFREE_AUDIO_GATEWAY                            0x111F // Hands-free Profile (HFP)
#define BT_SERVICE_CLASS_DIRECT_PRINTING_REFERENCE_OBJECTS_SERVICE          0x1120 // Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_REFLECTED_UI                                       0x1121 // Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_BASIC_PRINTING                                     0x1122 // Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_PRINTING_STATUS                                    0x1123 // Basic Printing Profile (BPP)
#define BT_SERVICE_CLASS_HUMAN_INTERFACE_DEVICE_SERVICE                     0x1124 // Human Interface Device (HID) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_HARDCOPY_CABLE_REPLACEMENT                         0x1125 // Hardcopy Cable Replacement Profile (HCRP)
#define BT_SERVICE_CLASS_HCR_PRINT                                          0x1126 // Hardcopy Cable Replacement Profile (HCRP)
#define BT_SERVICE_CLASS_HCR_SCAN                                           0x1127 // Hardcopy Cable Replacement Profile (HCRP)
#define BT_SERVICE_CLASS_COMMON_ISDN_ACCESS                                 0x1128 // Common ISDN Access Profile (CIP) NOTE: Used as both Service Class Identifier and Profile Identifier. [DEPRECATED]
#define BT_SERVICE_CLASS_SIM_ACCESS                                         0x112D // SIM Access Profile (SAP) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_PHONEBOOK_ACCESS_PCE                               0x112E // Phonebook Access Profile (PBAP)
#define BT_SERVICE_CLASS_PHONEBOOK_ACCESS_PSE                               0x112F // Phonebook Access Profile (PBAP)
#define BT_SERVICE_CLASS_PHONEBOOK_ACCESS                                   0x1130 // Phonebook Access Profile (PBAP)
#define BT_SERVICE_CLASS_HEADSET_HS                                         0x1131 // Headset Profile (HSP) NOTE: See erratum #3507. 0x1108 and 0x1203 should also be included in the ServiceClassIDList before 0x1131 for backwards compatibility.
#define BT_SERVICE_CLASS_MESSAGE_ACCESS_SERVER                              0x1132 // Message Access Profile (MAP)
#define BT_SERVICE_CLASS_MESSAGE_NOTIFICATION_SERVER                        0x1133 // Message Access Profile (MAP)
#define BT_SERVICE_CLASS_MESSAGE_ACCESS_PROFILE                             0x1134 // Message Access Profile (MAP)
#define BT_SERVICE_CLASS_GNSS                                               0x1135 // Global Navigation Satellite System Profile (GNSS)
#define BT_SERVICE_CLASS_GNSS_SERVER                                        0x1136 // Global Navigation Satellite System Profile (GNSS)
#define BT_SERVICE_CLASS_3D_DISPLAY                                         0x1137 // 3D Synchronization Profile (3DSP)
#define BT_SERVICE_CLASS_3D_GLASSES                                         0x1138 // 3D Synchronization Profile (3DSP)
#define BT_SERVICE_CLASS_3D_SYNCHRONIZATION                                 0x1139 // 3D Synchronization Profile (3DSP)
#define BT_SERVICE_CLASS_MPS_PROFILE_UUID                                   0x113A // Multi-Profile Specification (MPS)
#define BT_SERVICE_CLASS_MPS_SC_UUID                                        0x113B // Multi-Profile Specification (MPS)
#define BT_SERVICE_CLASS_CTN_ACCESS_SERVICE                                 0x113C // Calendar, Task, andNotes (CTN)Profile
#define BT_SERVICE_CLASS_CTN_NOTIFICATION_SERVICE                           0x113D // CalendarTasksandNotes (CTN)Profile
#define BT_SERVICE_CLASS_CTN_PROFILE                                        0x113E // CalendarTasksandNotes (CTN)Profile
#define BT_SERVICE_CLASS_PNP_INFORMATION                                    0x1200 // Device Identification (DID) NOTE: Used as both Service Class Identifier and Profile Identifier.
#define BT_SERVICE_CLASS_GENERIC_NETWORKING                                 0x1201 // N/A
#define BT_SERVICE_CLASS_GENERIC_FILE_TRANSFER                              0x1202 // N/A
#define BT_SERVICE_CLASS_GENERIC_AUDIO                                      0x1203 // N/A
#define BT_SERVICE_CLASS_GENERIC_TELEPHONY                                  0x1204 // N/A
#define BT_SERVICE_CLASS_UPNP_SERVICE                                       0x1205 // Enhanced Service Discovery Profile (ESDP) [DEPRECATED]
#define BT_SERVICE_CLASS_UPNP_IP_SERVICE                                    0x1206 // Enhanced Service Discovery Profile (ESDP) [DEPRECATED]
#define BT_SERVICE_CLASS_ESDP_UPNP_IP_PAN                                   0x1300 // Enhanced Service Discovery Profile (ESDP) [DEPRECATED]
#define BT_SERVICE_CLASS_ESDP_UPNP_IP_LAP                                   0x1301 // Enhanced Service Discovery Profile (ESDP)[DEPRECATED]
#define BT_SERVICE_CLASS_ESDP_UPNP_L2CAP                                    0x1302 // Enhanced Service Discovery Profile (ESDP)[DEPRECATED]
#define BT_SERVICE_CLASS_VIDEO_SOURCE                                       0x1303 // Video Distribution Profile (VDP)
#define BT_SERVICE_CLASS_VIDEO_SINK                                         0x1304 // Video Distribution Profile (VDP)
#define BT_SERVICE_CLASS_VIDEO_DISTRIBUTION                                 0x1305 // Video Distribution Profile (VDP)
#define BT_SERVICE_CLASS_HDP                                                0x1400 // Health Device Profile
#define BT_SERVICE_CLASS_HDP_SOURCE                                         0x1401 // Health Device Profile (HDP)
#define BT_SERVICE_CLASS_HDP_SINK                                           0x1402 // Health Device Profile (HDP)

/**
 * Attributes
 */
#define BT_ATTRIBUTE_PUBLIC_BROWSE_ROOT                                     0x1002 // Bluetooth Core Specification
#define BT_ATTRIBUTE_SUPPORTED_FEATURES                                     0x0311 // 
#define BT_ATTRIBUTE_GOEP_L2CAP_PSM                                         0x0200 // 
#define BT_ATTRIBUTE_SUPPORTED_CAPABILITIES                                 0x0310 // 
#define BT_ATTRIBUTE_SUPPORTED_FUNCTIONS                                    0x0312 // 
#define BT_ATTRIBUTE_TOTAL_IMAGING_DATA_CAPACITY                            0x0313 // 
#define BT_ATTRIBUTE_DOCUMENT_FORMATS_SUPPORTED                             0x0350 // 
#define BT_ATTRIBUTE_CHARACTER_REPERTOIRES_SUPPORTED                        0x0352 // 
#define BT_ATTRIBUTE_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED                    0x0354 // 
#define BT_ATTRIBUTE_COLOR_SUPPORTED                                        0x0356 // 
#define BT_ATTRIBUTE_1284_ID                                                0x0358 // 
#define BT_ATTRIBUTE_PRINTER_NAME                                           0x035A // 
#define BT_ATTRIBUTE_PRINTER_LOCATION                                       0x035C // 
#define BT_ATTRIBUTE_DUPLEX_SUPPORTED                                       0x035E // 
#define BT_ATTRIBUTE_MEDIA_TYPES_SUPPORTED                                  0x0360 // 
#define BT_ATTRIBUTE_MAX_MEDIA_WIDTH                                        0x0362 // 
#define BT_ATTRIBUTE_MAX_MEDIA_LENGTH                                       0x0364 // 
#define BT_ATTRIBUTE_ENHANCED_LAYOUT_SUPPORTED                              0x0366 // 
#define BT_ATTRIBUTE_RUI_FORMATS_SUPPORTED                                  0x0368 // 
#define BT_ATTRIBUTE_REFERENCE_PRINTING_RUI_SUPPORTED                       0x0370 // 
#define BT_ATTRIBUTE_DIRECT_PRINTING_RUI_SUPPORTED                          0x0372 // 
#define BT_ATTRIBUTE_REFERENCE_PRINTING_TOP_URL                             0x0374 // 
#define BT_ATTRIBUTE_DIRECT_PRINTING_TOP_URL                                0x0376 // 
#define BT_ATTRIBUTE_PRINTER_ADMIN_RUI_TOP_URL                              0x0378 // 
#define BT_ATTRIBUTE_DEVICE_NAME                                            0x037A // 
#define BT_ATTRIBUTE_SERVICE_RECORD_HANDLE                                  0x0000 // 
#define BT_ATTRIBUTE_SERVICE_CLASS_ID_LIST                                  0x0001 // 
#define BT_ATTRIBUTE_SERVICE_RECORD_STATE                                   0x0002 // 
#define BT_ATTRIBUTE_SERVICE_ID                                             0x0003 // 
#define BT_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST                               0x0004 // 
#define BT_ATTRIBUTE_BROWSE_GROUP_LIST                                      0x0005 // 
#define BT_ATTRIBUTE_LANGUAGE_BASE_ATTRIBUTE_ID_LIST                        0x0006 // 
#define BT_ATTRIBUTE_SERVICE_INFO_TIME_TO_LIVE                              0x0007 // 
#define BT_ATTRIBUTE_SERVICE_AVAILABILITY                                   0x0008 // 
#define BT_ATTRIBUTE_BLUETOOTH_PROFILE_DESCRIPTOR_LIST                      0x0009 // 
#define BT_ATTRIBUTE_DOCUMENTATION_URL                                      0x000A // 
#define BT_ATTRIBUTE_CLIENT_EXECUTABLE_URL                                  0x000B // 
#define BT_ATTRIBUTE_ICON_URL                                               0x000C // 
#define BT_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESCRIPTOR_LISTS                   0x000D // 
#define BT_ATTRIBUTE_VERSION_NUMBER_LIST                                    0x0200 // 
#define BT_ATTRIBUTE_SERVICE_DATABASE_STATE                                 0x0201 // 
#define BT_ATTRIBUTE_SPECIFICATION_ID                                       0x0200 // 
#define BT_ATTRIBUTE_VENDOR_ID                                              0x0201 // 
#define BT_ATTRIBUTE_PRODUCT_ID                                             0x0202 // 
#define BT_ATTRIBUTE_VERSION                                                0x0203 // 
#define BT_ATTRIBUTE_PRIMARY_RECORD                                         0x0204 // 
#define BT_ATTRIBUTE_VENDOR_ID_SOURCE                                       0x0205 // 
#define BT_ATTRIBUTE_NETWORK                                                0x0301 // 
#define BT_ATTRIBUTE_FRIENDLY_NAME                                          0x0304 // 
#define BT_ATTRIBUTE_DEVICE_LOCATION                                        0x0306 // 
#define BT_ATTRIBUTE_REMOTE_AUDIO_VOLUME_CONTROL                            0x0302 // 
#define BT_ATTRIBUTE_SUPPORT_FEATURES_LIST                                  0x0200 // 
#define BT_ATTRIBUTE_DATA_EXCHANGE_SPECIFICATION                            0x0301 // 
#define BT_ATTRIBUTE_MCAP_SUPPORTED_PROCEDURES                              0x0302 // 
#define BT_ATTRIBUTE_HID_DEVICE_RELEASE_NUMBER                              0x0200 // 
#define BT_ATTRIBUTE_HID_PARSER_VERSION                                     0x0201 // 
#define BT_ATTRIBUTE_HID_DEVICE_SUBCLASS                                    0x0202 // 
#define BT_ATTRIBUTE_HID_COUNTRY_CODE                                       0x0203 // 
#define BT_ATTRIBUTE_HID_VIRTUAL_CABLE                                      0x0204 // 
#define BT_ATTRIBUTE_HID_RECONNECT_INITIATE                                 0x0205 // 
#define BT_ATTRIBUTE_HID_DESCRIPTOR_LIST                                    0x0206 // 
#define BT_ATTRIBUTE_HIDLANGID_BASE_LIST                                    0x0207 // 
#define BT_ATTRIBUTE_HIDSDP_DISABLE                                         0x0208 // 
#define BT_ATTRIBUTE_HID_BATTERY_POWER                                      0x0209 // 
#define BT_ATTRIBUTE_HID_REMOTE_WAKE                                        0x020A // 
#define BT_ATTRIBUTE_HID_PROFILE_VERSION                                    0x020B // 
#define BT_ATTRIBUTE_HID_SUPERVISION_TIMEOUT                                0x020C // 
#define BT_ATTRIBUTE_HID_NORMALLY_CONNECTABLE                               0x020D // 
#define BT_ATTRIBUTE_HID_BOOT_DEVICE                                        0x020E // 
#define BT_ATTRIBUTE_HIDSSR_HOST_MAX_LATENCY                                0x020F // 
#define BT_ATTRIBUTE_HIDSSR_HOST_MIN_TIMEOUT                                0x0210 // 
#define BT_ATTRIBUTE_MAS_INSTANCE_ID                                        0x0315 // 
#define BT_ATTRIBUTE_SUPPORTED_MESSAGE_TYPES                                0x0316 // 
#define BT_ATTRIBUTE_MAP_SUPPORTED_FEATURES                                 0x0317 // 
#define BT_ATTRIBUTE_SERVICE_VERSION                                        0x0300 // 
#define BT_ATTRIBUTE_SUPPORTED_FORMATS_LIST                                 0x0303 // 
#define BT_ATTRIBUTE_IP_SUBNET                                              0x0200 // 
#define BT_ATTRIBUTE_SECURITY_DESCRIPTION                                   0x030A // 
#define BT_ATTRIBUTE_NET_ACCESS_TYPE                                        0x030B // 
#define BT_ATTRIBUTE_MAX_NET_ACCESSRATE                                     0x030C // 
#define BT_ATTRIBUTE_IPV4_SUBNET                                            0x030D // 
#define BT_ATTRIBUTE_IPV6_SUBNET                                            0x030E // 
#define BT_ATTRIBUTE_SUPPORTED_REPOSITORIES                                 0x0314 // 
#define BT_ATTRIBUTE_PBAP_SUPPORTED_FEATURES                                0x0317 // 
#define BT_ATTRIBUTE_SUPPORTED_DATA_STORES_LIST                             0x0301 // 
#define BT_ATTRIBUTE_MPSD_SCENARIOS                                         0x0200 // 
#define BT_ATTRIBUTE_MPMD_SCENARIOS                                         0x0201 // 
#define BT_ATTRIBUTE_SUPPORTED_PROFILES_AND_PROTOCOLS                       0x0202 // 
#define BT_ATTRIBUTE_CAS_INSTANCE_ID                                        0x0315 // 
#define B_ATTRIBUTE_CTN_SUPPORTED_FEATURES                                 0x0317 // 
#define BV_ATTRIBUTE_GNSS_SUPPORTED_FEATURES                                0x0200

/* Functions for interfacing with SDP: */
void sdp_init(void);
uint32_t sdp_next_rhdl(void);

/* Client API */
struct sdp_pcb_t *sdp_new(struct l2cap_pcb_t *l2cappcb);
void sdp_free(struct sdp_pcb_t *pcb);
void sdp_reset_all(void);
void sdp_arg(struct sdp_pcb_t *pcb, void *arg);
err_t sdp_service_search_req(struct sdp_pcb_t *pcb, uint8_t *ssp, uint8_t ssplen, uint16_t max_src,
		   void (* service_searched)(void *arg, struct sdp_pcb_t *pcb, uint16_t tot_src, uint16_t curr_src, uint32_t *rhdls));
err_t sdp_service_attrib_req(struct sdp_pcb_t *pcb, uint32_t srhdl, uint16_t max_abc, uint8_t *attrids, uint8_t attrlen,
			     void (* attributes_recv)(void *arg, struct sdp_pcb_t *pcb, uint16_t attribl_bc, struct bt_pbuf_t *p));
err_t sdp_service_search_attrib_req(struct sdp_pcb_t *pcb, uint16_t max_abc, uint8_t *ssp, uint8_t ssplen, uint8_t *attrids, 
				    uint8_t attrlen, void (* attributes_searched)(void *arg, struct sdp_pcb_t *pcb, uint16_t attribl_bc, struct bt_pbuf_t *p));

/* Server API */
/* Functions to be used when adding and removing service records to and from the SDDB */
struct sdp_record_t *sdp_record_new(uint8_t *record_de_list, uint8_t rlen,uint32_t hdl);
void sdp_record_free(struct sdp_record_t *record);
err_t sdp_register_service(struct sdp_record_t *record);
void sdp_unregister_service(struct sdp_record_t *record);

/* Lower layer API */
void sdp_lp_disconnected(struct l2cap_pcb_t *l2cappcb);
err_t sdp_recv(void *arg, struct l2cap_pcb_t *pcb, struct bt_pbuf_t *p, err_t err);



#endif


