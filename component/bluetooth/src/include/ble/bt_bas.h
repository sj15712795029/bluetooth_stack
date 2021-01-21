/******************************************************************************
  * @file           bt_bas.h
  * @author         Yu-ZhongJun(124756828@qq.com)
  * @Taobao link    https://shop220811498.taobao.com/
  * @version        V0.0.1
  * @date           2021-1-13
  * @brief          bt ble bas header file
******************************************************************************/

#ifndef BT_BAS_H_H_H
#define BT_BAS_H_H_H

#include "bt_common.h"
#include "bt_gatt.h"

typedef struct
{
	uint8_t bat_level;
	uint8_t client_conf; /*  */
}bas_manager_t;
err_t bas_init(uint8_t bat_level);
err_t bas_batt_level_notification(uint8_t level);


#endif
