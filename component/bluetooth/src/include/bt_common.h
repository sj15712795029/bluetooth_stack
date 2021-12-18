/******************************************************************************
  * @file           bt_common.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2020-4-14
  * @brief          bt common header file
******************************************************************************/
 
#ifndef COMMON_H_H_H
#define COMMON_H_H_H
#include "bt_config.h"
#include "bt_debug.h"
#include "bt_mem.h"
#include "bt_memp.h"

#ifndef err_t
typedef int8_t err_t;
#endif

/** Definitions for error constants. */
typedef enum {
/** No error, everything OK. */
  BT_ERR_OK         = 0,
/** Out of memory error.     */
  BT_ERR_MEM        = -1,
/** Buffer error.            */
  BT_ERR_BUF        = -2,
/** Timeout.                 */
  BT_SERR_TIMEOUT    = -3,
/** Routing problem.         */
  BT_ERR_RTE        = -4,
/** Operation in progress    */
  BT_ERR_INPROGRESS = -5,
/** Illegal value.           */
  BT_ERR_VAL        = -6,
/** Operation would block.   */
  BT_ERR_WOULDBLOCK = -7,
/** Address in use.          */
  BT_ERR_USE        = -8,
/** Already connecting.      */
  BT_ERR_ALREADY    = -9,
/** Conn already established.*/
  BT_ERR_ISCONN     = -10,
/** Not connected.           */
  BT_ERR_CONN       = -11,
/** Low-level netif error    */
  BT_ERR_IF         = -12,
/** Connection aborted.      */
  BT_ERR_ABRT       = -13,
/** Connection reset.        */
  BT_ERR_RST        = -14,
/** Connection closed.       */
  BT_ERR_CLSD       = -15,
/** Illegal argument.        */
  BT_ERR_ARG        = -16,
} bt_err_enum_t;


#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#define MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#define BT_CONST_CAST(target_type, val) ((target_type)((ptrdiff_t)val))
#define BT_UNUSED_ARG(x) (void)(x=x)

#define BT_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define BT_MIN(x , y)  (((x) < (y)) ? (x) : (y))

#define bt_htons(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define bt_ntohs(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define bt_htonl(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))
#define bt_ntohl(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))

#define bt_htonll(x) ((((uint64_t)bt_htonl(x)) << 32) + bt_htonl((x) >> 32))

#define SYS_ARCH_INC(var, val) do { \
                                var += val; \
                              } while(0)

#define SYS_ARCH_DEC(var, val) do { \
                                var -= val; \
                              } while(0)

#define SYS_ARCH_GET(var, ret) do { \
                                ret = var; \
                              } while(0)

#define SYS_ARCH_SET(var, val) do { \
                                var = val; \
                              } while(0)

#define BT_LE_U16_TO_ARRAY(u16) (u16&0xff),(u16>>8&0xff)


#define BD_ADDR_LEN 6

struct bd_addr_t {
  uint8_t addr[BD_ADDR_LEN];
};


#define MAX_UUID_SIZE              16
typedef struct {
#define LEN_UUID_16     2
#define LEN_UUID_32     4
#define LEN_UUID_128    16
    uint16_t len;
    union {
        uint16_t      uuid16;
        uint32_t      uuid32;
        uint8_t       uuid128[MAX_UUID_SIZE];
    } uu;

} bt_uuid_t;



#define BD_ADDR(bdaddr, a, b, c, d, e, f) do{ \
                                        bdaddr->addr[0] = a; \
				        bdaddr ->addr[1] = b; \
				        bdaddr->addr[2] = c; \
				        bdaddr->addr[3] = d; \
				        bdaddr->addr[4] = e; \
				       bdaddr->addr[5] = f; }while(0)

#define bd_addr_cmp(addr1, addr2) (((addr1)->addr[0] == (addr2)->addr[0]) && \
				   ((addr1)->addr[1] == (addr2)->addr[1]) && \
				   ((addr1)->addr[2] == (addr2)->addr[2]) && \
				   ((addr1)->addr[3] == (addr2)->addr[3]) && \
				   ((addr1)->addr[4] == (addr2)->addr[4]) && \
				   ((addr1)->addr[5] == (addr2)->addr[5]))

#define bd_addr_set(addr1, addr2) do { \
                                   (addr1)->addr[0] = (addr2)->addr[0]; \
				   (addr1)->addr[1] = (addr2)->addr[1]; \
				   (addr1)->addr[2] = (addr2)->addr[2]; \
				   (addr1)->addr[3] = (addr2)->addr[3]; \
				   (addr1)->addr[4] = (addr2)->addr[4]; \
				   (addr1)->addr[5] = (addr2)->addr[5]; }while(0)

#define bd_addr_debug_print(bdaddr) printf("0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n", \
		    (bdaddr)->addr[0], \
		    (bdaddr)->addr[1], \
		    (bdaddr)->addr[2], \
		    (bdaddr)->addr[3], \
		    (bdaddr)->addr[4], \
		    (bdaddr)->addr[5])


#define BT_DT_FLAGS	0x01
#define BT_DT_INCOMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS      0x02
#define BT_DT_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS        0x03
#define BT_DT_INCOMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS      0x04
#define BT_DT_COMPLETE_LIST_OF_32_BIT_SERVICE_CLASS_UUIDS        0x05
#define BT_DT_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS     0x06 
#define BT_DT_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS       0x07
#define BT_DT_SHORTENED_LOCAL_NAME                               0x08
#define BT_DT_COMPLETE_LOCAL_NAME                                0x09
#define BT_DT_TX_POWER_LEVEL                                     0x0A
#define BT_DT_CLASS_OF_DEVICE                                    0x0D
#define BT_DT_SIMPLE_PAIRING_HASH_C                              0x0E
#define BT_DT_SIMPLE_PAIRING_HASH_C_192                          0x0E
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R                        0x0F
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R_192                    0x0F
#define BT_DT_DEVICE_ID                                          0x10
#define BT_DT_SECURITY_MANAGER_TK_VALUE                          0x10
#define BT_DT_SECURITY_MANAGER_OUT_OF_BAND_FLAGS                 0x11
#define BT_DT_SLAVE_CONNECTION_INTERVAL_RANGE                    0x12
#define BT_DT_LIST_OF_16_BIT_SERVICE_SOLICITATION_UUIDS          0x14
#define BT_DT_LIST_OF_32_BIT_SERVICE_SOLICITATION_UUIDS          0x1F
#define BT_DT_LIST_OF_128_BIT_SERVICE_SOLICITATION_UUIDS         0x15
#define BT_DT_SERVICE_DATA                                       0x16
#define BT_DT_SERVICE_DATA_16_BIT_UUID                           0x16
#define BT_DT_SERVICE_DATA_32_BIT_UUID                           0x20
#define BT_DT_SERVICE_DATA_128_BIT_UUID                          0x21
#define BT_DT_LE_SECURE_CONNECTIONS_CONFIRMATION_VALUE           0x22
#define BT_DT_LE_SECURE_CONNECTIONS_RANDOM_VALUE                 0x23
#define BT_DT_URI                                                0x24
#define BT_DT_INDOOR_POSITIONING                                 0x25
#define BT_DT_TRANSPORT_DISCOVERY_DATA                           0x26
#define BT_DT_PUBLIC_TARGET_ADDRESS                              0x17
#define BT_DT_RANDOM_TARGET_ADDRESS                              0x18
#define BT_DT_APPEARANCE                                         0x19
#define BT_DT_ADVERTISING_INTERVAL                               0x1A
#define BT_DT_LE_BLUETOOTH_DEVICE_ADDRESS                        0x1B
#define BT_DT_LE_ROLE                                            0x1C
#define BT_DT_SIMPLE_PAIRING_HASH_C_256                          0x1D
#define BT_DT_SIMPLE_PAIRING_RANDOMIZER_R_256                    0x1E
#define BT_DT_3D_INFORMATION_DATA                                0x3D
#define BT_DT_MANUFACTURER_SPECIFIC_DATA                         0xFF


/*********************************
 *** Class of Device constants ***
 *********************************/
#define BT_FORMAT_TYPE_1 0x00

/****************************
 * minor device class field
 ****************************/

/* 0x00 is used as unclassified for all minor device classes */
#define BT_COD_MINOR_UNCLASSIFIED 0x00

/* minor device class field for Computer Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BT_COD_MINOR_DESKTOP_WORKSTATION 0x04
#define BT_COD_MINOR_SERVER_COMPUTER 0x08
#define BT_COD_MINOR_LAPTOP 0x0C
#define BT_COD_MINOR_HANDHELD_PC_PDA 0x10 /* clam shell */
#define BT_COD_MINOR_PALM_SIZE_PC_PDA 0x14
#define BT_COD_MINOR_WEARABLE_COMPUTER 0x18 /* watch sized */

/* minor device class field for Phone Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BT_COD_MINOR_CELLULAR 0x04
#define BT_COD_MINOR_CORDLESS 0x08
#define BT_COD_MINOR_SMART_PHONE 0x0C
/* wired modem or voice gatway */
#define BT_COD_MINOR_WIRED_MDM_V_GTWY 0x10
#define BT_COD_MINOR_ISDN_ACCESS 0x14

/* minor device class field for LAN Access Point Major Class */
/* Load Factor Field bit 5-7 */
#define BT_COD_MINOR_FULLY_AVAILABLE 0x00
#define BT_COD_MINOR_1_17_UTILIZED 0x20
#define BT_COD_MINOR_17_33_UTILIZED 0x40
#define BT_COD_MINOR_33_50_UTILIZED 0x60
#define BT_COD_MINOR_50_67_UTILIZED 0x80
#define BT_COD_MINOR_67_83_UTILIZED 0xA0
#define BT_COD_MINOR_83_99_UTILIZED 0xC0
#define BT_COD_MINOR_NO_SERVICE_AVAILABLE 0xE0
/* sub-Field bit 2-4 */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */

/* minor device class field for Audio/Video Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BT_COD_MINOR_CONFM_HEADSET 0x04
#define BT_COD_MINOR_CONFM_HANDSFREE 0x08
#define BT_COD_MINOR_MICROPHONE 0x10
#define BT_COD_MINOR_LOUDSPEAKER 0x14
#define BT_COD_MINOR_HEADPHONES 0x18
#define BT_COD_MINOR_PORTABLE_AUDIO 0x1C
#define BT_COD_MINOR_CAR_AUDIO 0x20
#define BT_COD_MINOR_SET_TOP_BOX 0x24
#define BT_COD_MINOR_HIFI_AUDIO 0x28
#define BT_COD_MINOR_VCR 0x2C
#define BT_COD_MINOR_VIDEO_CAMERA 0x30
#define BT_COD_MINOR_CAMCORDER 0x34
#define BT_COD_MINOR_VIDEO_MONITOR 0x38
#define BT_COD_MINOR_VIDDISP_LDSPKR 0x3C
#define BT_COD_MINOR_VIDEO_CONFERENCING 0x40
#define BT_COD_MINOR_GAMING_TOY 0x48

/* minor device class field for Peripheral Major Class */
/* Bits 6-7 independently specify mouse, keyboard, or combo mouse/keyboard */
#define BT_COD_MINOR_KEYBOARD 0x40
#define BT_COD_MINOR_POINTING 0x80
#define BT_COD_MINOR_COMBO 0xC0
/* Bits 2-5 OR'd with selection from bits 6-7 */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BT_COD_MINOR_JOYSTICK 0x04
#define BT_COD_MINOR_GAMEPAD 0x08
#define BT_COD_MINOR_REMOTE_CONTROL 0x0C
#define BT_COD_MINOR_SENSING_DEVICE 0x10
#define BT_COD_MINOR_DIGITIZING_TABLET 0x14
#define BT_COD_MINOR_CARD_READER 0x18 /* e.g. SIM card reader */
#define BT_COD_MINOR_DIGITAL_PAN 0x1C
#define BT_COD_MINOR_HAND_SCANNER 0x20
#define BT_COD_MINOR_HAND_GESTURAL_INPUT 0x24

/* minor device class field for Imaging Major Class */
/* Bits 5-7 independently specify display, camera, scanner, or printer */
#define BT_COD_MINOR_DISPLAY 0x10
#define BT_COD_MINOR_CAMERA 0x20
#define BT_COD_MINOR_SCANNER 0x40
#define BT_COD_MINOR_PRINTER 0x80
/* Bits 2-3 Reserved */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */

/* minor device class field for Wearable Major Class */
/* Bits 2-7 meaningful    */
#define BT_COD_MINOR_WRIST_WATCH 0x04
#define BT_COD_MINOR_PAGER 0x08
#define BT_COD_MINOR_JACKET 0x0C
#define BT_COD_MINOR_HELMET 0x10
#define BT_COD_MINOR_GLASSES 0x14

/* minor device class field for Toy Major Class */
/* Bits 2-7 meaningful    */
#define BT_COD_MINOR_ROBOT 0x04
#define BT_COD_MINOR_VEHICLE 0x08
#define BT_COD_MINOR_DOLL_ACTION_FIGURE 0x0C
#define BT_COD_MINOR_CONTROLLER 0x10
#define BT_COD_MINOR_GAME 0x14

/* minor device class field for Health Major Class */
/* Bits 2-7 meaningful    */
#define BT_COD_MINOR_BLOOD_MONITOR 0x04
#define BT_COD_MINOR_THERMOMETER 0x08
#define BT_COD_MINOR_WEIGHING_SCALE 0x0C
#define BT_COD_MINOR_GLUCOSE_METER 0x10
#define BT_COD_MINOR_PULSE_OXIMETER 0x14
#define BT_COD_MINOR_HEART_PULSE_MONITOR 0x18
#define BT_COD_MINOR_HEALTH_DATA_DISPLAY 0x1C
#define BT_COD_MINOR_STEP_COUNTER 0x20
#define BT_COD_MINOR_BODY_COM_ANALYZER 0x24
#define BT_COD_MINOR_PEAK_FLOW_MONITOR 0x28
#define BT_COD_MINOR_MEDICATION_MONITOR 0x2C
#define BT_COD_MINOR_KNEE_PROSTHESIS 0x30
#define BT_COD_MINOR_ANKLE_PROSTHESIS 0x34

/***************************
 * major device class field
 ***************************/
#define BT_COD_MAJOR_MISCELLANEOUS 0x00
#define BT_COD_MAJOR_COMPUTER 0x01
#define BT_COD_MAJOR_PHONE 0x02
#define BT_COD_MAJOR_LAN_ACCESS_PT 0x03
#define BT_COD_MAJOR_AUDIO 0x04
#define BT_COD_MAJOR_PERIPHERAL 0x05
#define BT_COD_MAJOR_IMAGING 0x06
#define BT_COD_MAJOR_WEARABLE 0x07
#define BT_COD_MAJOR_TOY 0x08
#define BT_COD_MAJOR_HEALTH 0x09
#define BT_COD_MAJOR_UNCLASSIFIED 0x1F

/***************************
 * service class fields
 ***************************/
#define BT_COD_SERVICE_LMTD_DISCOVER 0x0020
#define BT_COD_SERVICE_POSITIONING 0x0100
#define BT_COD_SERVICE_NETWORKING 0x0200
#define BT_COD_SERVICE_RENDERING 0x0400
#define BT_COD_SERVICE_CAPTURING 0x0800
#define BT_COD_SERVICE_OBJ_TRANSFER 0x1000
#define BT_COD_SERVICE_AUDIO 0x2000
#define BT_COD_SERVICE_TELEPHONY 0x4000
#define BT_COD_SERVICE_INFORMATION 0x8000

/* class of device field macros */
#define BT_COD_FORMAT_TYPE(u8, pd) \
  { (u8) = (pd)[2] & 0x03; }
#define BT_COD_MINOR_CLASS(u8, pd) \
  { (u8) = (pd)[2] & 0xFC; }
#define BT_COD_MAJOR_CLASS(u8, pd) \
  { (u8) = (pd)[1] & 0x1F; }
#define BT_COD_SERVICE_CLASS(u16, pd) \
  {                                    \
    (u16) = (pd)[0];                   \
    (u16) <<= 8;                       \
    (u16) += (pd)[1] & 0xE0;           \
  }

/* to set the fields (assumes that format type is always 0) */
#define FIELDS_TO_COD(pd, mn, mj, sv)                   \
  {                                                     \
    (pd)[2] = mn;                                       \
    (pd)[1] = (mj) + ((sv)&BTM_COD_SERVICE_CLASS_LO_B); \
    (pd)[0] = (sv) >> 8;                                \
  }

/* the COD masks */
#define BT_COD_FORMAT_TYPE_MASK 0x03
#define BT_COD_MINOR_CLASS_MASK 0xFC
#define BT_COD_MAJOR_CLASS_MASK 0x1F
#define BT_COD_SERVICE_CLASS_LO_B 0x00E0
#define BT_COD_SERVICE_CLASS_MASK 0xFFE0

#define BT_COD_TYPE_UNKNOW 0x00
#define BT_COD_TYPE_HEADSET 0x01

#ifndef CASE_RETURN_STR
#define CASE_RETURN_STR(const) case const: return #const;
#endif


uint16_t bt_le_read_16(const uint8_t * buffer, int pos);
uint32_t bt_le_read_24(const uint8_t * buffer, int pos);
uint32_t bt_le_read_32(const uint8_t * buffer, int pos);
void bt_le_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void bt_le_store_24(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_le_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
uint32_t bt_be_read_16( const uint8_t * buffer, int pos);
uint32_t bt_be_read_24( const uint8_t * buffer, int pos);
uint32_t bt_be_read_32( const uint8_t * buffer, int pos);
void bt_be_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
void bt_be_store_24(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_be_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
void bt_be_store_64(uint8_t *buffer,uint16_t pos,uint64_t value);
uint32_t bt_atoi_spec_size(const uint8_t *str,uint8_t size);
uint32_t bt_atoi(const uint8_t *str);
void bt_hex_dump(uint8_t *data,uint32_t len);
uint8_t* bt_hex_string(const void* buf, uint16_t len);
void bt_addr_dump(uint8_t *addr);
void bt_uuid128_dump(uint8_t *uuid128);
uint8_t bt_parse_cod(uint8_t cod[3],uint16_t *device_service,uint16_t * device_major,uint16_t *device_minor);
void bt_memcpy_swap(void *dst, const void *src, uint16_t length);
void bt_mem_swap(void *buf, uint16_t length);

#endif

