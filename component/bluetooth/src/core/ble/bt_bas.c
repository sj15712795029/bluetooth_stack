#include "bt_bas.h"

uint8_t gatt_bas_level[] = {100};
uint8_t gatt_bas_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_BATTERY)};
uint8_t gatt_bas_characteristic[] = {GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY,BT_LE_U16_TO_ARRAY(GATT_BAS_LEVEL_HANDLE),BT_LE_U16_TO_ARRAY(GATT_UUID_BATTERY_LEVEL)};
uint8_t gatt_bas_chara_client_conf[] = {0,0};

gatt_server_service_t bas_service[] = 
{
	{GATT_BAS_SERVICE_HANDLE,GATT_UUID_PRI_SERVICE,
		gatt_bas_uuid,sizeof(gatt_bas_uuid),GATT_PERM_READ,NULL},
	{GATT_BAS_CHARACTERISTIC_HANDLE,GATT_UUID_CHAR_DECLARE,
		gatt_bas_characteristic,sizeof(gatt_bas_characteristic),GATT_PERM_READ,NULL},
	{GATT_BAS_LEVEL_HANDLE,GATT_UUID_BATTERY_LEVEL,
		gatt_bas_level,sizeof(gatt_bas_level),GATT_PERM_READ|GATT_PERM_WRITE,NULL},
	{GATT_BAS_CLIENT_CHARA_CONF_HANDLE,GATT_UUID_CHAR_CLIENT_CONFIG,
		gatt_bas_chara_client_conf,sizeof(gatt_bas_chara_client_conf),GATT_PERM_READ|GATT_PERM_WRITE,NULL},
};


err_t bas_init(void)
{
	BT_BAS_TRACE_DEBUG("bas_init\n");
	gatt_server_add_pri_service(&bas_service,GATT_BAS_SERVICE_HANDLE,GATT_BAS_CLIENT_CHARA_CONF_HANDLE,sizeof(bas_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_BATTERY);

	return BT_ERR_OK;
}

err_t bas_batt_level_notification(uint8_t level)
{
	gatt_server_notification(GATT_BAS_LEVEL_HANDLE,&level,1);

	return BT_ERR_OK;
}


