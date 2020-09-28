/******************************************************************************
  * @file    hw_spi_flash.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-09-25
  * @brief   spi flash source file, support:
  				W25Q80,W25Q16,W25Q32,W25Q64,W25Q128,W25Q256
******************************************************************************/
#include "hw_spi_flash.h"

/* FUNC DECLARATION */
static void protocol_spi1_init(void);
static uint8_t protocol_spi1_read_write_byte(uint8_t tx_data);
static void hw_spi_flash_wait_idle(void);
static void hw_spi_flash_write_page(uint8_t* buffer,uint32_t write_addr,uint16_t write_len);
static void hw_spi_flash_write_nocheck(uint8_t* buffer,uint32_t write_addr,uint16_t write_len);
void hw_spi_flash_write_enable(void);
void hw_spi_flash_write_disable(void);
uint8_t hw_spi_flash_read_reg(uint8_t reg_index);
void hw_spi_flash_write_reg(uint8_t reg_index,uint8_t reg_value);

uint8_t manuf_id,device_id;
uint32_t spi_flash_size = 0;
uint32_t block_count = 0;
uint32_t sector_total_count = 0;
uint8_t spi_flash_buffer[SECOTR_SIZE];
SPI_HandleTypeDef hspi1;

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hspi->Instance==SPI1)
    {
        /* USER CODE BEGIN SPI1_MspInit 0 */

        /* USER CODE END SPI1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI1 GPIO Configuration
        PA5     ------> SPI1_SCK
        PA7     ------> SPI1_MOSI
        PB4     ------> SPI1_MISO
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI1_MspInit 1 */

        /* USER CODE END SPI1_MspInit 1 */
    }

}

/******************************************************************************
 * func name   : hw_spi_flash_init
 * para        : NULL
 * return      : spi flash init result
 * description : spi flash init,use spi1 and PA4 for CS
******************************************************************************/
uint8_t hw_spi_flash_init()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOF_CLK_ENABLE();

    /*Configure GPIO pin : FLASH_SEL_Pin */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    SPI_FLASH_DESELECT;
    /* SPI1 init */
    protocol_spi1_init();
    /* Read manufacturer id and device id */
    hw_spi_flash_read_manu_dev_id(&manuf_id,&device_id);
    HW_DEBUG("manu manuf id 0x%02x, device id 0x%02x\n",manuf_id,device_id);

    /* SPI flash size update */
    if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q80_DEVICE_ID)
    {
        HW_DEBUG("SPI FLASH:W25Q80\n");
        spi_flash_size = FLASH_W25Q80_SIZE;
        block_count = FLASH_W25Q80_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;
    }
    else if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q16_DEVICE_ID)
    {
        HW_DEBUG("SPI FLASH:W25Q16\n");
        spi_flash_size = FLASH_W25Q16_SIZE;
        block_count = FLASH_W25Q16_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;
    }
    else if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q32_DEVICE_ID)
    {
        HW_DEBUG("SPI FLASH:W25Q32\n");
        spi_flash_size = FLASH_W25Q32_SIZE;
        block_count = FLASH_W25Q32_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;
    }
    else if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q64_DEVICE_ID)
    {
        HW_DEBUG("SPI FLASH:W25Q64\n");
        spi_flash_size = FLASH_W25Q64_SIZE;
        block_count = FLASH_W25Q64_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;
    }
    else if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q128_DEVICE_ID)
    {
        HW_DEBUG("SPI FLASH:W25Q128\n");
        spi_flash_size = FLASH_W25Q128_SIZE;
        block_count = FLASH_W25Q128_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;
    }
    else if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q256_DEVICE_ID)
    {
        /* Read spi flash register 3 ,get address mode */
        uint8_t reg3_ret = hw_spi_flash_read_reg(w25qxx_read_status_reg3);
        HW_DEBUG("SPI FLASH:W25Q256\n");
        spi_flash_size = FLASH_W25Q256_SIZE;
        block_count = FLASH_W25Q256_BLOCK;
        sector_total_count = block_count * SECTOR_COUNT_PER_BLOCK;

        /* If it is not 4 byte address mode,change 4 byte address mode */
        if((reg3_ret & FLASH_REG3_ADS) == 0)
        {
            SPI_FLASH_SELECT;
            protocol_spi1_read_write_byte(w25qxx_enter_4B_addr_mode);
            SPI_FLASH_DESELECT;
        }
    }
    else
    {
        HW_DEBUG("HW ERROR:invalid flash manuf id or device id\n");
        return HW_ERR_FLASH_INVALID_ID;
    }

    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_spi_flash_get_size
 * para        : NULL
 * return      : spi flash size
 * description : get spi flash size
******************************************************************************/
uint32_t hw_spi_flash_get_size()
{
    return spi_flash_size;
}

/******************************************************************************
 * func name   : hw_spi_flash_get_sector_size
 * para        : NULL
 * return      : spi flash sector size
 * description : get spi flash sector size
******************************************************************************/
uint16_t hw_spi_flash_get_sector_size()
{
    return SECOTR_SIZE;
}

/******************************************************************************
 * func name   : hw_spi_flash_get_sector_count
 * para        : NULL
 * return      : spi flash sector count
 * description : get spi flash sector count
******************************************************************************/
uint32_t hw_spi_flash_get_sector_count()
{
    return sector_total_count;
}

/******************************************************************************
 * func name   : hw_spi_flash_get_block_size
 * para        : NULL
 * return      : spi flash block size
 * description : get spi flash block size
******************************************************************************/
uint32_t hw_spi_flash_get_block_size()
{
    return SECTOR_COUNT_PER_BLOCK;
}

/******************************************************************************
 * func name   : hw_spi_flash_read_manu_dev_id
 * para        : manuf_id(OUT)		--> manufacturer id
                 dev_id(OUT)			--> device id
 * return      : hw_spi_flash_read_manu_dev_id result
 * description : get spi flash manufacturer & device id
******************************************************************************/
uint8_t hw_spi_flash_read_manu_dev_id(uint8_t *manuf_id,uint8_t *dev_id)
{
    if(manuf_id && manuf_id)
    {
        SPI_FLASH_SELECT;
        protocol_spi1_read_write_byte(w25qxx_manu_device_id);
        protocol_spi1_read_write_byte(0x00);
        protocol_spi1_read_write_byte(0x00);
        protocol_spi1_read_write_byte(0x00);
        *manuf_id=protocol_spi1_read_write_byte(0xff);
        *dev_id =protocol_spi1_read_write_byte(0xff);
        SPI_FLASH_DESELECT;
        return HW_ERR_OK;
    }
    else
    {
        return HW_ERR_FLASH_INVALID_PARA;
    }

}

/******************************************************************************
 * func name   : hw_spi_flash_erase_chip
 * para        : NULL
 * return      : hw_spi_flash_erase_chip result
 * description : erase spi flash,It need total time 40s(+-5s)
******************************************************************************/
uint8_t hw_spi_flash_erase_chip()
{
    hw_spi_flash_write_enable();
    hw_spi_flash_wait_idle();
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(w25qxx_chip_erase1);
    SPI_FLASH_DESELECT;
    hw_spi_flash_wait_idle();
    return HW_ERR_OK;

}

/******************************************************************************
 * func name   : hw_spi_flash_erase_sector
 * para        : dest_addr(IN)			--> sector index
 * return      : hw_spi_flash_erase_sector result
 * description : erase spi flash sector
******************************************************************************/
uint8_t hw_spi_flash_erase_sector(uint32_t sector_index)
{
    uint32_t dest_addr = sector_index*SECOTR_SIZE;
    hw_spi_flash_write_enable();
    hw_spi_flash_wait_idle();
    SPI_FLASH_SELECT;

    protocol_spi1_read_write_byte(w25qxx_sector_erase);
    if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q256_DEVICE_ID)
    {
        protocol_spi1_read_write_byte((uint8_t)((dest_addr)>>24));
    }
    protocol_spi1_read_write_byte((uint8_t)((dest_addr)>>16));
    protocol_spi1_read_write_byte((uint8_t)((dest_addr)>>8));
    protocol_spi1_read_write_byte((uint8_t)dest_addr);
    SPI_FLASH_DESELECT;
    hw_spi_flash_wait_idle();
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_spi_flash_power_down
 * para        : NULL
 * return      : hw_spi_flash_power_down result
 * description : power down spi flash
******************************************************************************/
uint8_t hw_spi_flash_power_down()
{
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(w25qxx_power_down);
    SPI_FLASH_DESELECT;
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_spi_flash_wakeup
 * para        : NULL
 * return      : hw_spi_flash_wakeup result
 * description : wakeup spi flash
******************************************************************************/
uint8_t hw_spi_flash_wakeup()
{
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(w25qxx_rel_pdn_id);
    SPI_FLASH_DESELECT;
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_spi_flash_read
 * para        : buffer(OUT)			--> buffer out return data
 				   	 read_addr(IN)		--> read addr
 				    read_len(IN)		--> read len,max 65535 byte
 * return      : hw_spi_flash_read result
 * description : read spec len data base on read address
******************************************************************************/
uint8_t hw_spi_flash_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_len)
{
    uint16_t index;
    SPI_FLASH_SELECT;
    if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q256_DEVICE_ID)
    {
        protocol_spi1_read_write_byte(w25qxx_read_data_4B_addr);
        protocol_spi1_read_write_byte((uint8_t)((read_addr)>>24));
    }
    else
    {
        protocol_spi1_read_write_byte(w25qxx_read_data);
    }
    protocol_spi1_read_write_byte((uint8_t)((read_addr)>>16));
    protocol_spi1_read_write_byte((uint8_t)((read_addr)>>8));
    protocol_spi1_read_write_byte((uint8_t)read_addr);

    for(index = 0; index < read_len; index++)
    {
        buffer[index] = protocol_spi1_read_write_byte(0xff);
    }

    SPI_FLASH_DESELECT;
    return HW_ERR_OK;
}


/******************************************************************************
 * func name   : hw_spi_flash_write
 * para        : buffer(IN)			--> you want write data to spi flash
 				   write_addr(IN)		--> write addr
 				   write_len(IN)		--> write len
 * return      : hw_spi_flash_write result
 * description : write spec len data to write address
******************************************************************************/
uint8_t hw_spi_flash_write(uint8_t* buffer,uint32_t write_addr,uint16_t write_len)
{
    uint16_t index;
    uint32_t sector_index = write_addr/SECOTR_SIZE;	/* index of sector */
    uint16_t sector_offset = write_addr%SECOTR_SIZE;	/* offset of sector */
    uint16_t sector_remain = SECOTR_SIZE - sector_offset;		/* sector remain size */
    uint8_t * buffer_temp = spi_flash_buffer;

    /* If write len < sector remain size,It only needs to be written once */
    if(write_len<=sector_remain)
    {
        sector_remain=write_len;
    }
    while(1)
    {
        /* Reads data from the entire sector for backup purposes */
        hw_spi_flash_read(buffer_temp,sector_index*SECOTR_SIZE,SECOTR_SIZE);
        /* If the data inside is not equal to 0xff, the entire sector needs to be erased */
        for(index=0; index<sector_remain; index++)
        {
            if(buffer_temp[sector_offset+index]!=0xff)
            {
                break;
            }
        }
        if(index < sector_remain)
        {
            /* erase spi sector */
            hw_spi_flash_erase_sector(sector_index);
            for(index=0; index<sector_remain; index++)
            {
                buffer_temp[index+sector_offset]=buffer[index];
            }
            /* Write data without checking whether it needs to be erased  */
            hw_spi_flash_write_nocheck(buffer_temp,sector_index*SECOTR_SIZE,SECOTR_SIZE);

        }
        else
        {
            /* Write data without checking whether it needs to be erased  */
            hw_spi_flash_write_nocheck(buffer,write_addr,sector_remain);
        }
        if(write_len == sector_remain)
        {
            break;
        }
        else
        {
            sector_index++;
            sector_offset = 0;

            buffer += sector_remain;
            write_addr += sector_remain;
            write_len -= sector_remain;
            if(write_len>SECOTR_SIZE)
            {
                sector_remain = SECOTR_SIZE;
            }
            else
            {
                sector_remain = write_len;
            }
        }
    };
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_spi_flash_write_enable
 * para        : NULL
 * return      : NULL
 * description : spi flash write enable
******************************************************************************/
void hw_spi_flash_write_enable()
{
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(w25qxx_write_enable);
    SPI_FLASH_DESELECT;
}

/******************************************************************************
 * func name   : hw_spi_flash_write_disable
 * para        : NULL
 * return      : NULL
 * description : spi flash write disable
******************************************************************************/
void hw_spi_flash_write_disable()
{
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(w25qxx_write_disable);
    SPI_FLASH_DESELECT;
}

/******************************************************************************
 * func name   : hw_spi_flash_read_reg
 * para        : reg_index(IN)		--> index of register
 * return      : hw_spi_flash_read_reg result
 * description : Read registers by index
******************************************************************************/
uint8_t hw_spi_flash_read_reg(uint8_t reg_index)
{
    uint8_t byte=0,command=0;
    switch(reg_index)
    {
    case FLASH_REG1_INDEX:
        command=w25qxx_read_status_reg1;
        break;
    case FLASH_REG2_INDEX:
        command=w25qxx_read_status_reg2;
        break;
    case FLASH_REG3_INDEX:
        command=w25qxx_read_status_reg3;
        break;
    default:
        command=w25qxx_read_status_reg1;
        break;
    }
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(command);
    byte=protocol_spi1_read_write_byte(0xff);
    SPI_FLASH_DESELECT;
    return byte;
}

/******************************************************************************
 * func name   : hw_spi_flash_write_reg
 * para        : reg_index(IN)		--> index of register
 				   reg_value(IN)		--> The value to write
 * return      : hw_spi_flash_write_reg result
 * description : Writes a value to a register with a specific number
******************************************************************************/
void hw_spi_flash_write_reg(uint8_t reg_index,uint8_t reg_value)
{
    uint8_t command=0;
    switch(reg_index)
    {
    case 1:
        command=w25qxx_read_status_reg1;
        break;
    case 2:
        command=w25qxx_read_status_reg2;
        break;
    case 3:
        command=w25qxx_read_status_reg3;
        break;
    default:
        command=w25qxx_read_status_reg1;
        break;
    }
    SPI_FLASH_SELECT;
    protocol_spi1_read_write_byte(command);
    protocol_spi1_read_write_byte(reg_value);
    SPI_FLASH_DESELECT;
}

/******************************************************************************
 * func name   : hw_spi_flash_wait_idle
 * para        : NULL
 * return      : NULL
 * description : Wait spi flash idle
******************************************************************************/
static void hw_spi_flash_wait_idle()
{
    while((hw_spi_flash_read_reg(FLASH_REG1_INDEX)&FLASH_REG1_BUSY) == FLASH_REG1_BUSY);
}

/******************************************************************************
 * func name   : hw_spi_flash_write_page
 * para        : buffer(IN)			--> Data to write
 				   write_addr(IN)		--> The address to write to
 				   write_len(IN)		--> The length to write
 * return      : NULL
 * description : The maximum amount of data written to the page address is
 					no more than 256
******************************************************************************/
static void hw_spi_flash_write_page(uint8_t* buffer,uint32_t write_addr,uint16_t write_len)
{
    uint16_t index;
    hw_spi_flash_write_enable();
    SPI_FLASH_SELECT;

    protocol_spi1_read_write_byte(w25qxx_page_program);
    if(manuf_id == WINBOND_MANUFACTURER_ID && device_id == W25Q256_DEVICE_ID)
    {
        protocol_spi1_read_write_byte((uint8_t)((write_addr)>>24));
    }

    protocol_spi1_read_write_byte((uint8_t)((write_addr)>>16));
    protocol_spi1_read_write_byte((uint8_t)((write_addr)>>8));
    protocol_spi1_read_write_byte((uint8_t)write_addr);

    for(index = 0; index < write_len; index++)
    {
        protocol_spi1_read_write_byte(buffer[index]);
    }

    SPI_FLASH_DESELECT;
    hw_spi_flash_wait_idle();
}


/******************************************************************************
 * func name   : hw_spi_flash_write_nocheck
 * para        : buffer(IN)			--> Data to write
 				   write_addr(IN)		--> The address to write to
 				   write_len(IN)		--> The length to write
 * return      : NULL
 * description : Write data without checking whether it needs to be erased
******************************************************************************/
static void hw_spi_flash_write_nocheck(uint8_t* buffer,uint32_t write_addr,uint16_t write_len)
{
    uint16_t page_remain = PAGE_MAX_SIZE - write_addr%PAGE_MAX_SIZE;
    if(write_len <= page_remain)
    {
        page_remain = write_len;
    }
    while(1)
    {
        hw_spi_flash_write_page(buffer,write_addr,page_remain);
        if(write_len==page_remain)
        {
            /* write complete,exit while loop */
            break;
        }
        else
        {
            buffer += page_remain;
            write_addr += page_remain;
            write_len -= page_remain;

            if(write_len > PAGE_MAX_SIZE)
            {
                /* One more page wait write */
                page_remain=PAGE_MAX_SIZE;
            }
            else
            {
                /* Less than one page wait write */
                page_remain=write_len;
            }
        }
    };
}

/******************************************************************************
 * func name   : protocol_spi1_init
 * para        : NULL
 * return      : NULL
 * description : spi1 init PA5->SCK PA6->MISO PA7->MOSI
******************************************************************************/
static void protocol_spi1_init()
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        return;
    }

    __HAL_SPI_ENABLE(&hspi1);

    protocol_spi1_read_write_byte(0xff);

}

/******************************************************************************
 * func name   : protocol_spi1_read_write_byte
 * para        : tx_data(IN)		--> Data to be sent
 * return      : NULL
 * description : spi1 read write data
******************************************************************************/
static uint8_t protocol_spi1_read_write_byte(uint8_t tx_data)
{
    while((SPI1->SR&1<<1)==0);
    SPI1->DR=tx_data;
    while((SPI1->SR&1<<0)==0) {};
    return SPI1->DR;
}
