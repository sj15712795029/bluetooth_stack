/******************************************************************************
  * @file    bsp_24c020.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-12-22
  * @brief   eeprom 24c02 header file
******************************************************************************/

#ifndef HW_24C02_H_H_H
#define HW_24C02_H_H_H
#include "bsp_iic.h"


uint8_t hw_24c02_init(void);
uint8_t hw_24c02_write(uint16_t addr,uint8_t *data,uint16_t data_len);
uint8_t hw_24c02_read(uint16_t addr,uint8_t *data,uint16_t data_len);

#endif

