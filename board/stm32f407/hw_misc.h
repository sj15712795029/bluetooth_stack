/******************************************************************************
  * @file    hw_misc.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   hw misc header file
******************************************************************************/

#ifndef HW_MISC_H_H_H
#define HW_MISC_H_H_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

#define BANK_SRAM3_ADDR    ((uint32_t)(0x68000000))		

#define HW_IOT_LWOS_TEST 0
#define HW_DEBUG_ENABLE 1
#define HW_FUNC_DEBUG	1
#if HW_DEBUG_ENABLE > 0
#define HW_DEBUG	printf
#else
#define HW_DEBUG
#endif

#define hw_memset 	memset
#define hw_memcpy	memcpy
#define hw_memcmp	memcmp
#define hw_strlen	strlen
#define hw_strcmp strcmp
#define hw_strncmp strncmp
#define hw_sprintf sprintf

#define WIFI_PREE_PRIO 0
#define WIFI_SUB_PRIO 0
#define BT_PREE_PRIO 0
#define BT_SUB_PRIO 0
#define USB_PREE_PRIO 0
#define USB_SUB_PRIO 0
#define DEBUG_PREE_PRIO 3
#define DEBUG_SUB_PRIO 0
#define KEY1_PREE_PRIO 4
#define KEY1_SUB_PRIO 0
#define KEY2_PREE_PRIO 4
#define KEY2_SUB_PRIO 0
#define KEY3_PREE_PRIO 4
#define KEY3_SUB_PRIO 0
#define KEY4_PREE_PRIO 4
#define KEY4_SUB_PRIO 0


#if HW_FUNC_DEBUG > 0
#define	HW_ENTER()			HW_DEBUG("Enter: %s\n", __func__)
#define	HW_LEAVE()			HW_DEBUG("Leave: %s\n", __func__)
#else
#define	HW_ENTER()
#define	HW_LEAVE()
#endif

/* hw error */
typedef enum
{
	HW_ERR_OK,

	HW_HAL_EXCU_ERR,

	HW_ERR_FS_INIT,
	
	HW_ERR_SHELL_NO_CMD,
	HW_ERR_SHELL_INVALID_PARA,
	
	/* OLED err */
	HW_ERR_OLED_INVALID_DIRECT,
	HW_ERR_OLED_INVALID_INTERVAL,
	HW_ERR_OLED_INVALID_OFFSET,
	HW_ERR_OLED_INVALID_PAGE,
	HW_ERR_OLED_INVALID_COL,

	/* SPI flash err */
	HW_ERR_FLASH_INVALID_PARA,	
	HW_ERR_FLASH_INVALID_ID,
	
	/* SDIO err */
	HW_ERR_SDIO_INVALID_PARA,	
	HW_ERR_SDIO_CMD3_FAIL,
	HW_ERR_SDIO_CMD5_FAIL,
	HW_ERR_SDIO_CMD7_FAIL,
	HW_ERR_SDIO_CMD52_FAIL,
	HW_ERR_SDIO_GET_VER_FAIL,
	HW_ERR_SDIO_ENABLE_FUNC_FAIL,
	HW_ERR_SDIO_DISABLE_FUNC_FAIL,
	HW_ERR_SDIO_ENABLE_FUNC_INT_FAIL,
	HW_ERR_SDIO_DISABLE_FUNC_INT_FAIL,
	HW_ERR_SDIO_ENABLE_MGR_INT_FAIL,
	HW_ERR_SDIO_DISABLE_MGR_INT_FAIL,
	HW_ERR_SDIO_GET_INT_PEND_FAIL,
	HW_ERR_SDIO_SET_ABORT_FAIL,
	HW_ERR_SDIO_RESET_FAIL,
	HW_ERR_SDIO_SET_BUS_WIDTH_FAIL,
	HW_ERR_SDIO_GET_BUS_WIDTH_FAIL,
	HW_ERR_SDIO_INVALID_FUNC_NUM,
	HW_ERR_SDIO_BLK_SIZE_ZERO,
	HW_ERR_SDIO_CMD53_FAIL,
	
}hw_err_e;

void system_clock_config(void);
void hw_fsmc_init(void);
void hw_hex_dump(uint8_t *data,int len);
void hw_delay_ms(uint32_t cms);
void hw_delay_us(uint32_t cus);
#endif

