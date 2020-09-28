/******************************************************************************
  * @file    hw_spi_flash.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-09-25
  * @brief   spi flash header file
******************************************************************************/
#ifndef HW_SPI_FLASH_H_H_H
#define HW_SPI_FLASH_H_H_H

#include "hw_misc.h"

/* SPI FLASH MANUFACTURER ID/DEVICE ID */
#define WINBOND_MANUFACTURER_ID	0xef
#define W25Q80_DEVICE_ID				0x13
#define W25Q16_DEVICE_ID				0x14
#define W25Q32_DEVICE_ID				0x15
#define W25Q64_DEVICE_ID				0x16
#define W25Q128_DEVICE_ID				0x17
#define W25Q256_DEVICE_ID				0x18

/* W25QXX total flash size & total block count */
#define FLASH_W25Q80_SIZE	(10*1024*1024)
#define FLASH_W25Q80_BLOCK 160
#define FLASH_W25Q16_SIZE	(2*1024*1024)
#define FLASH_W25Q16_BLOCK 32
#define FLASH_W25Q32_SIZE	(4*1024*1024)
#define FLASH_W25Q32_BLOCK 64
#define FLASH_W25Q64_SIZE	(8*1024*1024)
#define FLASH_W25Q64_BLOCK 128
#define FLASH_W25Q128_SIZE (16*1024*1024)
#define FLASH_W25Q128_BLOCK 256
#define FLASH_W25Q256_SIZE (32*1024*1024)
#define FLASH_W25Q256_BLOCK 512

#define PAGE_MAX_SIZE 256
#define SECOTR_SIZE		4096
#define SECTOR_COUNT_PER_BLOCK	16
#define SPI_MAX_RETRY 200


/* SPI flash select/deselect */
#define SPI_FLASH_SELECT	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_RESET)
#define SPI_FLASH_DESELECT	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, GPIO_PIN_SET)


/* SPI flash register1~register3 bitmap */
#define FLASH_REG1_INDEX 1
#define FLASH_REG2_INDEX 2
#define FLASH_REG3_INDEX 3
#define FLASH_REG1_BUSY	(1 << 0)
#define FLASH_REG1_WEL	(1 << 1)
#define FLASH_REG1_BP0	(1 << 2)
#define FLASH_REG1_BP1	(1 << 3)
#define FLASH_REG1_BP2	(1 << 4)
#define FLASH_REG1_BP3	(1 << 5)
#define FLASH_REG1_TB		(1 << 6)
#define FLASH_REG1_SRP	(1 << 7)
#define FLASH_REG2_SRL	(1 << 0)
#define FLASH_REG2_QE		(1 << 1)
#define FLASH_REG2_R1		(1 << 2)
#define FLASH_REG2_LB1	(1 << 3)
#define FLASH_REG2_LB2	(1 << 4)
#define FLASH_REG2_LB3	(1 << 5)
#define FLASH_REG2_CMP	(1 << 6)
#define FLASH_REG2_SUS	(1 << 7)
#define FLASH_REG3_ADS	(1 << 0)
#define FLASH_REG3_ADP	(1 << 1)
#define FLASH_REG3_WPS	(1 << 2)
#define FLASH_REG3_R1		(1 << 3)
#define FLASH_REG3_R2		(1 << 4)
#define FLASH_REG3_DRV0	(1 << 5)
#define FLASH_REG3_DRV1	(1 << 6)
#define FLASH_REG3_R3		(1 << 7)


typedef enum
{
    w25qxx_write_enable = 0x06,
    w25qxx_volatile_sr_write_enable = 0x50,
    w25qxx_write_disable = 0x04,
    w25qxx_rel_pdn_id = 0xab,
    w25qxx_manu_device_id = 0x90,
    w25qxx_jedec_id = 0x9f,
    w25qxx_read_unique_id = 0x4b,
    w25qxx_read_data = 0x03,
    w25qxx_read_data_4B_addr = 0x13,
    w25qxx_fast_read = 0x0b,
    w25qxx_fast_read_4B_addr = 0x0c,
    w25qxx_page_program = 0x02,
    w25qxx_page_program_4B_addr = 0x12,
    w25qxx_sector_erase = 0x20,
    w25qxx_sector_erase_4B_addr = 0x21,
    w25qxx_block_erase_32k = 0x52,
    w25qxx_block_erase_64k = 0xd8,
    w25qxx_block_erase_64k_4B_addr = 0xdc,
    w25qxx_chip_erase1 = 0xc7,
    w25qxx_chip_erase2 = 0x60,
    w25qxx_read_status_reg1 = 0x05,
    w25qxx_write_status_reg1 = 0x01,
    w25qxx_read_status_reg2 = 0x35,
    w25qxx_write_status_reg2 = 0x31,
    w25qxx_read_status_reg3 = 0x15,
    w25qxx_write_status_reg3 = 0x11,
    w25qxx_read_ext_add_reg = 0xc8,
    w25qxx_write_ext_addr_reg = 0xc5,
    w25qxx_read_sfdp_reg = 0x5a,
    w25qxx_erase_sec_reg = 0x44,
    w25qxx_program_sec_reg = 0x42,
    w25qxx_read_sec_reg = 0x48,
    w25qxx_global_block_lock = 0x7e,
    w25qxx_global_block_unlock = 0x98,
    w25qxx_read_block_lock = 0x3d,
    w25qxx_individual_block_lock = 0x36,
    w25qxx_individual_block_unlock = 0x39,
    w25qxx_erase_program_suspend = 0x75,
    w25qxx_erase_program_resume = 0x7a,
    w25qxx_power_down = 0xb9,
    w25qxx_enter_4B_addr_mode = 0xb7,
    w25qxx_exit_4B_addr_mode = 0xe9,
    w25qxx_enable_reset = 0x66,
    w25qxx_reset_device = 0x99,
} spi_flash_instuction_e;

uint8_t hw_spi_flash_init(void);
uint32_t hw_spi_flash_get_size(void);
uint16_t hw_spi_flash_get_sector_size(void);
uint32_t hw_spi_flash_get_block_size(void);
uint32_t hw_spi_flash_get_sector_count(void);
uint8_t hw_spi_flash_read_manu_dev_id(uint8_t *manuf_id,uint8_t *dev_id);
uint8_t hw_spi_flash_erase_chip(void);
uint8_t hw_spi_flash_erase_sector(uint32_t sector_index);
uint8_t hw_spi_flash_power_down(void);
uint8_t hw_spi_flash_wakeup(void);
uint8_t hw_spi_flash_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_len);
uint8_t hw_spi_flash_write(uint8_t* buffer,uint32_t write_addr,uint16_t write_len);

#endif

