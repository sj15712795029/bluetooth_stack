#include "bt_bas.h"

uint8_t gatt_bas_level[] = {100};
uint8_t gatt_bas_uuid[] = {BT_LE_U16_TO_ARRAY(BT_UUID_SERVCLASS_BATTERY)};
uint8_t gatt_bas_characteristic[] = {GATT_CHAR_PROP_BIT_WRITE|GATT_CHAR_PROP_BIT_READ|GATT_CHAR_PROP_BIT_NOTIFY,BT_LE_U16_TO_ARRAY(GATT_BAS_LEVEL_HANDLE),BT_LE_U16_TO_ARRAY(GATT_UUID_BATTERY_LEVEL)};
uint8_t gatt_bas_chara_client_conf[] = {0,0};
bas_manager_t bas_manager = {0};


gatt_server_service_t bas_service[] =
{
    {
        GATT_BAS_SERVICE_HANDLE,GATT_UUID_PRI_SERVICE,NULL,
        gatt_bas_uuid,sizeof(gatt_bas_uuid),GATT_PERM_READ_ENCRYPTED
    },
    {
        GATT_BAS_CHARACTERISTIC_HANDLE,GATT_UUID_CHAR_DECLARE,NULL,
        gatt_bas_characteristic,sizeof(gatt_bas_characteristic),GATT_PERM_READ
    },
    {
        GATT_BAS_LEVEL_HANDLE,GATT_UUID_BATTERY_LEVEL,NULL,
        gatt_bas_level,sizeof(gatt_bas_level),GATT_PERM_READ|GATT_PERM_WRITE
    },
    {
        GATT_BAS_CLIENT_CHARA_CONF_HANDLE,GATT_UUID_CHAR_CLIENT_CONFIG,NULL,
        gatt_bas_chara_client_conf,sizeof(gatt_bas_chara_client_conf),GATT_PERM_READ|GATT_PERM_WRITE
    },
};



/* Test */
uint8_t uuid_test[16] = {1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2};
uint8_t gatt_test_characteristic[] = {GATT_CHAR_PROP_BIT_AUTH,
                                      BT_LE_U16_TO_ARRAY(0x0b),BT_LE_U16_TO_ARRAY(GATT_UUID_BATTERY_LEVEL)
                                     };
uint8_t uuid_type[16] = {3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4};
uint8_t test_value[10];
gatt_server_service_t test_service[] =
{
    {
        0x09,GATT_UUID_PRI_SERVICE,NULL,
        uuid_test,sizeof(uuid_test),GATT_PERM_READ
    },
    {
        0x0a,GATT_UUID_CHAR_DECLARE,NULL,
        gatt_test_characteristic,sizeof(gatt_test_characteristic),GATT_PERM_READ
    },
    {
        0x0b,GATT_UUID_BATTERY_LEVEL,NULL,
        gatt_bas_level,sizeof(gatt_bas_level),GATT_PERM_READ|GATT_PERM_WRITE
    },
    {
        0x0c,GATT_UUID_CHAR_CLIENT_CONFIG,NULL,
        gatt_bas_chara_client_conf,sizeof(gatt_bas_chara_client_conf),GATT_PERM_READ|GATT_PERM_WRITE
    },
    {
        0x0d,0,uuid_type,
        test_value,sizeof(test_value),GATT_PERM_READ_ENC_MITM|GATT_PERM_WRITE
    },
};

void gatt_bas_read(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t *value_len,uint8_t *err_code)
{
    BT_BAS_TRACE_DEBUG("gatt_bas_write handle(%d)\n",handle);

    if(handle == GATT_BAS_LEVEL_HANDLE)
    {
        value[0] = bas_manager.bat_level;
        *value_len = 1;
    }

    *err_code = ATT_SUCCESS;
}

void gatt_bas_write(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t value_len,uint8_t *err_code)
{
    BT_BAS_TRACE_DEBUG("gatt_bas_write handle(%d) value_len(%d)\n",handle,value_len);
    bt_hex_dump(value,value_len);

    if(handle == GATT_BAS_LEVEL_HANDLE)
        bas_manager.bat_level = value[0];

    *err_code = ATT_SUCCESS;
}

static gatt_pri_service_cbs_t gatt_bas_service_cbs =
{
    gatt_bas_read,
    gatt_bas_write,
};



void gatt_test_read(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t *value_len,uint8_t *err_code)
{

    BT_BAS_TRACE_DEBUG("gatt_test_read handle(%d)\n",handle);

    {
        // TODO: test smp,Delete it later
        static uint8_t auth = 0;
        if(auth == 0)
        {
            *err_code = ATT_INSUF_AUTHENTICATION;
            auth = 1;
        }
        else
        {
            value[0] = 10;
            *value_len = 1;

            *err_code = ATT_SUCCESS;
        }
    }
}

void gatt_test_write(struct bd_addr_t *remote_addr,uint16_t handle,uint8_t *value,uint8_t value_len,uint8_t *err_code)
{
    BT_BAS_TRACE_DEBUG("gatt_test_write handle(%d) value_len(%d)\n",handle,value_len);
    bt_hex_dump(value,value_len);


    *err_code = ATT_SUCCESS;
}

static gatt_pri_service_cbs_t gatt_test_service_cbs =
{
    gatt_test_read,
    gatt_test_write,
};



err_t bas_init(uint8_t bat_level)
{
    bas_manager.bat_level = bat_level;
    BT_BAS_TRACE_DEBUG("bas_init\n");
    gatt_server_add_pri_service((gatt_server_service_t *)&bas_service,GATT_BAS_SERVICE_HANDLE,GATT_BAS_CLIENT_CHARA_CONF_HANDLE,
                                sizeof(bas_service)/sizeof(gatt_server_service_t),BT_UUID_SERVCLASS_BATTERY,NULL,&gatt_bas_service_cbs);

    /* Test */
    gatt_server_add_pri_service((gatt_server_service_t *)&test_service,0x09,0x0d,
                                sizeof(test_service)/sizeof(gatt_server_service_t),0,uuid_test,&gatt_test_service_cbs);
    return BT_ERR_OK;
}

err_t bas_batt_level_notification(uint8_t level)
{
    gatt_server_notification(GATT_BAS_LEVEL_HANDLE,&level,1);

    return BT_ERR_OK;
}


