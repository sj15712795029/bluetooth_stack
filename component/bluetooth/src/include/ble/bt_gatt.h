/******************************************************************************
  * @file           bt_gatt.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2021-1-13
  * @brief          bt ble att server role header file
******************************************************************************/

#ifndef BT_GATT_H_H_H
#define BT_GATT_H_H_H


#include "bt_common.h"
#include "bt_att.h"

/* Attribute permissions
*/
#define GATT_PERM_READ (1 << 0)              /* bit 0 */
#define GATT_PERM_READ_ENCRYPTED (1 << 1)    /* bit 1 */
#define GATT_PERM_READ_ENC_MITM (1 << 2)     /* bit 2 */
#define GATT_PERM_WRITE (1 << 4)             /* bit 4 */
#define GATT_PERM_WRITE_ENCRYPTED (1 << 5)   /* bit 5 */
#define GATT_PERM_WRITE_ENC_MITM (1 << 6)    /* bit 6 */
#define GATT_PERM_WRITE_SIGNED (1 << 7)      /* bit 7 */
#define GATT_PERM_WRITE_SIGNED_MITM (1 << 8) /* bit 8 */


#define BT_UUID_SERVCLASS_GAP_SERVER 0x1800
#define BT_UUID_SERVCLASS_GATT_SERVER 0x1801
#define BT_UUID_SERVCLASS_IMMEDIATE_ALERT 0x1802 /* immediate alert */
#define BT_UUID_SERVCLASS_LINKLOSS 0x1803        /* Link Loss Alert */
#define BT_UUID_SERVCLASS_TX_POWER 0x1804        /* TX power */
#define BT_UUID_SERVCLASS_CURRENT_TIME 0x1805    /* Link Loss Alert */
#define BT_UUID_SERVCLASS_DST_CHG 0x1806         /* DST Time change */
#define BT_UUID_SERVCLASS_REF_TIME_UPD 0x1807    /* reference time update */
#define BT_UUID_SERVCLASS_THERMOMETER 0x1809     /* Thermometer UUID */
#define BT_UUID_SERVCLASS_DEVICE_INFO 0x180A     /* device info service */
#define BT_UUID_SERVCLASS_NWA 0x180B             /* Network availability */
#define BT_UUID_SERVCLASS_HEART_RATE 0x180D      /* Heart Rate service */
#define BT_UUID_SERVCLASS_PHALERT 0x180E         /* phone alert service */
#define BT_UUID_SERVCLASS_BATTERY 0x180F         /* battery service */
#define BT_UUID_SERVCLASS_BPM 0x1810             /*  blood pressure service */
#define BT_UUID_SERVCLASS_ALERT_NOTIFICATION 0x1811
#define BT_UUID_SERVCLASS_LE_HID 0x1812     /*  HID over LE */
#define BT_UUID_SERVCLASS_SCAN_PARAM 0x1813 /* Scan Parameter service */
#define BT_UUID_SERVCLASS_GLUCOSE 0x1808    /* Glucose Meter Service */
#define BT_UUID_SERVCLASS_RSC 0x1814 /* RUNNERS SPEED AND CADENCE SERVICE      */
#define BT_UUID_SERVCLASS_CSC 0x1816 /* Cycling SPEED AND CADENCE SERVICE      */


/* GATT attribute types
*/
#define GATT_UUID_PRI_SERVICE 0x2800
#define GATT_UUID_SEC_SERVICE 0x2801
#define GATT_UUID_INCLUDE_SERVICE 0x2802
/*  Characteristic Declaration*/
#define GATT_UUID_CHAR_DECLARE 0x2803

/*	Characteristic Extended Properties */
#define GATT_UUID_CHAR_EXT_PROP 0x2900
/*  Characteristic User Description*/
#define GATT_UUID_CHAR_DESCRIPTION 0x2901
/*  Client Characteristic Configuration */
#define GATT_UUID_CHAR_CLIENT_CONFIG 0x2902
/*  Server Characteristic Configuration */
#define GATT_UUID_CHAR_SRVR_CONFIG 0x2903
/*  Characteristic Presentation Format*/
#define GATT_UUID_CHAR_PRESENT_FORMAT 0x2904
/*  Characteristic Aggregate Format*/
#define GATT_UUID_CHAR_AGG_FORMAT 0x2905
/*  Characteristic Valid Range */
#define GATT_UUID_CHAR_VALID_RANGE 0x2906
#define GATT_UUID_EXT_RPT_REF_DESCR 0x2907
#define GATT_UUID_RPT_REF_DESCR 0x2908

/* GAP Profile Attributes
*/
#define GATT_UUID_GAP_DEVICE_NAME 0x2A00
#define GATT_UUID_GAP_ICON 0x2A01
#define GATT_UUID_GAP_PREF_CONN_PARAM 0x2A04
#define GATT_UUID_GAP_CENTRAL_ADDR_RESOL 0x2AA6

/* Attribute Profile Attribute UUID */
#define GATT_UUID_GATT_SRV_CHGD 0x2A05
/* Attribute Protocol Test */

/* Link Loss Service */
#define GATT_UUID_ALERT_LEVEL 0x2A06    /* Alert Level */
#define GATT_UUID_TX_POWER_LEVEL 0x2A07 /* TX power level */

/* Time Profile */
/* Current Time Service */
#define GATT_UUID_CURRENT_TIME 0x2A2B    /* Current Time */
#define GATT_UUID_LOCAL_TIME_INFO 0x2A0F /* Local time info */
#define GATT_UUID_REF_TIME_INFO 0x2A14   /* reference time information */

/* NwA Profile */
#define GATT_UUID_NW_STATUS 0x2A18  /* network availability status */
#define GATT_UUID_NW_TRIGGER 0x2A1A /* Network availability trigger */

/* phone alert */
#define GATT_UUID_ALERT_STATUS 0x2A3F   /* alert status */
#define GATT_UUID_RINGER_CP 0x2A40      /* ringer control point */
#define GATT_UUID_RINGER_SETTING 0x2A41 /* ringer setting */

/* Glucose Service */
#define GATT_UUID_GM_MEASUREMENT 0x2A18
#define GATT_UUID_GM_CONTEXT 0x2A34
#define GATT_UUID_GM_CONTROL_POINT 0x2A52
#define GATT_UUID_GM_FEATURE 0x2A51

/* device infor characteristic */
#define GATT_UUID_SYSTEM_ID 0x2A23
#define GATT_UUID_MODEL_NUMBER_STR 0x2A24
#define GATT_UUID_SERIAL_NUMBER_STR 0x2A25
#define GATT_UUID_FW_VERSION_STR 0x2A26
#define GATT_UUID_HW_VERSION_STR 0x2A27
#define GATT_UUID_SW_VERSION_STR 0x2A28
#define GATT_UUID_MANU_NAME 0x2A29
#define GATT_UUID_IEEE_DATA 0x2A2A
#define GATT_UUID_PNP_ID 0x2A50

/* HID characteristics */
#define GATT_UUID_HID_INFORMATION 0x2A4A
#define GATT_UUID_HID_REPORT_MAP 0x2A4B
#define GATT_UUID_HID_CONTROL_POINT 0x2A4C
#define GATT_UUID_HID_REPORT 0x2A4D
#define GATT_UUID_HID_PROTO_MODE 0x2A4E
#define GATT_UUID_HID_BT_KB_INPUT 0x2A22
#define GATT_UUID_HID_BT_KB_OUTPUT 0x2A32
#define GATT_UUID_HID_BT_MOUSE_INPUT 0x2A33

/* Battery Service char */
#define GATT_UUID_BATTERY_LEVEL 0x2A19

#define GATT_UUID_SC_CONTROL_POINT 0x2A55
#define GATT_UUID_SENSOR_LOCATION 0x2A5D

/* RUNNERS SPEED AND CADENCE SERVICE      */
#define GATT_UUID_RSC_MEASUREMENT 0x2A53
#define GATT_UUID_RSC_FEATURE 0x2A54

/* CYCLING SPEED AND CADENCE SERVICE      */
#define GATT_UUID_CSC_MEASUREMENT 0x2A5B
#define GATT_UUID_CSC_FEATURE 0x2A5C

/* Scan Parameter charatceristics */
#define GATT_UUID_SCAN_INT_WINDOW 0x2A4F
#define GATT_UUID_SCAN_REFRESH 0x2A31

/* Characteristic properties
*/
#define GATT_CHAR_PROP_BIT_BROADCAST (1 << 0)
#define GATT_CHAR_PROP_BIT_READ (1 << 1)
#define GATT_CHAR_PROP_BIT_WRITE_NR (1 << 2)
#define GATT_CHAR_PROP_BIT_WRITE (1 << 3)
#define GATT_CHAR_PROP_BIT_NOTIFY (1 << 4)
#define GATT_CHAR_PROP_BIT_INDICATE (1 << 5)
#define GATT_CHAR_PROP_BIT_AUTH (1 << 6)
#define GATT_CHAR_PROP_BIT_EXT_PROP (1 << 7)


#define GATT_CLIENT_OP_NONE 0
#define GATT_CLIENT_OP_EX_MTU 1
#define GATT_CLIENT_OP_PRIMARY_DISCOVERY 2
#define GATT_CLIENT_OP_RELATIONSHIP_DISCOVERY 3
#define GATT_CLIENT_OP_CHAR_DISCOVERY 4
#define GATT_CLIENT_OP_READ_CHAR_VALUE 5



typedef struct _gatt_pcb_t
{
    struct _gatt_pcb_t *next; /* For the linked list */
	struct bd_addr_t remote_addr;
    att_pcb_t *att_pcb;
}gatt_pcb_t;

typedef struct
{
	uint16_t start_handle;
	uint16_t end_handle;
	bt_uuid_t uuid;
}gatt_client_pri_service_t;


typedef struct
{
    void (*gatt_mtu_value)(struct bd_addr_t *remote_addr,uint16_t mtu);
	void (*gattc_discovery_primary_service)(struct bd_addr_t *remote_addr,gatt_client_pri_service_t *pri_service,uint16_t count);
	void (*gattc_discovery_uuid_primary_service)(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
	void (*gattc_discovery_char)(struct bd_addr_t *remote_addr,uint16_t attribute_handle,uint16_t char_value_handle,uint8_t properties,uint16_t uuid16,uint8_t *uuid128);
} gatt_client_cbs_t;


typedef struct
{
    void (*gatt_mtu_value)(struct bd_addr_t *remote_addr,uint16_t mtu);
} gatt_server_cbs_t;

typedef struct
{
	void (*gatt_connect_set_up)(struct bd_addr_t *remote_addr,uint8_t status);
    void (*gatt_connect_realease)(struct bd_addr_t *remote_addr,uint8_t status);
	gatt_client_cbs_t *gatt_client_cbs;
	gatt_server_cbs_t *gatt_server_cbs;
}gatt_cbs_t;


typedef struct
{
    void (*gatt_db_read)(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t *value_len,uint8_t *err_code);
    void (*gatt_db_write)(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t value_len,uint8_t *err_code);
} gatt_pri_service_cbs_t;


typedef struct
{
	uint16_t handle;
	uint16_t uuid16;
	uint8_t *uuid128;
	uint8_t *value;
	uint8_t value_length;
	uint16_t permission;
}gatt_server_service_t;

typedef struct
{
	uint8_t serivce_count;
	uint16_t start_handle;
	uint16_t end_handle;
	uint16_t pri_uuid;
	uint8_t pri_uuid128[16];
	gatt_server_service_t *gatt_server_service;
	gatt_pri_service_cbs_t *cb;
}gatt_server_pri_service_t;

typedef struct
{
	uint16_t client_mtu;
	uint16_t server_mtu;
	uint8_t gatt_server_pri_service_count;
}gatt_server_manager_t;

typedef struct
{
	uint16_t client_mtu;
	uint16_t server_mtu;
	uint8_t last_opcode;
}gatt_client_manager_t;



/* Gatt commmon API */
err_t gatt_init(gatt_cbs_t *cbs);

/* Gatt server API */
err_t gatt_server_init(void);
err_t gatt_server_add_pri_service(gatt_server_service_t *service,uint16_t start_handle,uint16_t end_handle,
	uint8_t service_count,uint16_t pri_uuid,uint8_t *pri_uuid128,gatt_pri_service_cbs_t *cb);
err_t gatt_server_notification(uint16_t handle,uint8_t *value,uint8_t value_length);
err_t gatt_server_indication(uint16_t handle,uint8_t *value,uint8_t value_length);

/* Gatt client API */
err_t gatt_client_init(void);
err_t gatt_client_exchange_mtu(struct bd_addr_t *remote_addr,uint16_t mtu);
err_t gatt_client_discovery_pri_service(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
err_t gatt_client_discovery_pri_service_uuid(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle,uint16_t uuid16,uint8_t *uuid128);
err_t gatt_client_find_include(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
err_t gatt_client_discovery_characteristics(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
err_t gatt_client_find_characteristics_uuid(uint16_t start_handle,uint16_t end_handle,uint16_t uuid);
err_t gatt_client_discovery_char_des(struct bd_addr_t *remote_addr,uint16_t start_handle,uint16_t end_handle);
err_t gatt_client_read_char_value(struct bd_addr_t *remote_addr,uint16_t handle);



#endif

