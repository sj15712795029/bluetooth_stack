#include "hw_lcd.h"


SPI_HandleTypeDef hspi4;

static uint8_t _bsp_lcd_gpio_init(void);
static uint8_t _bsp_lcd_spi_init(void);
static uint8_t _bsp_lcd_write_reg(uint8_t reg_index);
static uint8_t _bsp_lcd_write_byte(uint8_t byte);
static uint8_t _bsp_lcd_write_data32(uint32_t data);
static uint8_t _bsp_lcd_set_address(uint32_t col_start,uint32_t page_start,uint32_t col_end,uint32_t page_end);
static uint8_t _protocol_spi_read_write_byte(uint8_t tx_data);


uint8_t bsp_lcd_init(void)
{
    _bsp_lcd_gpio_init();
    _bsp_lcd_spi_init();

    LCD_RST_L;
    LCD_RST_H;
    LCD_BLK_L;

    _bsp_lcd_write_reg(lcd_cmd_soft_reset);
    /* It will be necessary to wait 5msec before sending new command following software reset */
    hw_delay_ms(5);
    _bsp_lcd_write_reg(lcd_cmd_sleep_out);
    /* It will be necessary to wait 5msec before sending next command */
    hw_delay_ms(5);

    _bsp_lcd_write_reg(lcd_cmd_if_pixel_format);
    _bsp_lcd_write_byte(0x66);

    _bsp_lcd_write_reg(lcd_cmd_normal_display_on);

    _bsp_lcd_write_reg(lcd_cmd_mem_access_ctl);
    if(USE_HORIZONTAL==0)_bsp_lcd_write_byte(0x08);
    else if(USE_HORIZONTAL==1)_bsp_lcd_write_byte(0xC8);
    else if(USE_HORIZONTAL==2)_bsp_lcd_write_byte(0x78);
    else _bsp_lcd_write_byte(0xA8);

    _bsp_lcd_write_reg(lcd_cmd_inversion_display);
    _bsp_lcd_write_byte(0x02);

    _bsp_lcd_write_reg(lcd_cmd_display_func_ctl);
    _bsp_lcd_write_byte(0x02);
    _bsp_lcd_write_byte(0x22);

    _bsp_lcd_write_reg(lcd_cmd_pwr_ctl_2);
    _bsp_lcd_write_byte(0x41);

    _bsp_lcd_write_reg(lcd_cmd_vcom_ctl);
    _bsp_lcd_write_byte(0x00);
    _bsp_lcd_write_byte(0x18);

    _bsp_lcd_write_reg(lcd_cmd_positive_gamma_ctl);
    _bsp_lcd_write_byte(0x0F);
    _bsp_lcd_write_byte(0x1F);
    _bsp_lcd_write_byte(0x1C);
    _bsp_lcd_write_byte(0x0C);
    _bsp_lcd_write_byte(0x0F);
    _bsp_lcd_write_byte(0x08);
    _bsp_lcd_write_byte(0x48);
    _bsp_lcd_write_byte(0x98);
    _bsp_lcd_write_byte(0x37);
    _bsp_lcd_write_byte(0x0A);
    _bsp_lcd_write_byte(0x13);
    _bsp_lcd_write_byte(0x04);
    _bsp_lcd_write_byte(0x11);
    _bsp_lcd_write_byte(0x0D);
    _bsp_lcd_write_byte(0x00);

    _bsp_lcd_write_reg(lcd_cmd_negative_gamma_ctl);
    _bsp_lcd_write_byte(0x0F);
    _bsp_lcd_write_byte(0x32);
    _bsp_lcd_write_byte(0x2E);
    _bsp_lcd_write_byte(0x0B);
    _bsp_lcd_write_byte(0x0D);
    _bsp_lcd_write_byte(0x05);
    _bsp_lcd_write_byte(0x47);
    _bsp_lcd_write_byte(0x75);
    _bsp_lcd_write_byte(0x37);
    _bsp_lcd_write_byte(0x06);
    _bsp_lcd_write_byte(0x10);
    _bsp_lcd_write_byte(0x03);
    _bsp_lcd_write_byte(0x24);
    _bsp_lcd_write_byte(0x20);
    _bsp_lcd_write_byte(0x00);

    _bsp_lcd_write_reg(lcd_cmd_sleep_out);
    hw_delay_ms(50);

    _bsp_lcd_write_reg(lcd_cmd_display_on);

    LCD_BLK_H;
    return HW_ERR_OK;

}

uint8_t _bsp_lcd_draw_point(uint32_t x,uint32_t y,uint32_t color)
{
    _bsp_lcd_set_address(x,y,x,y);
    _bsp_lcd_write_data32(color);

    return HW_ERR_OK;
}


uint8_t bsp_lcd_fill_color(uint32_t xsta,uint32_t ysta,uint32_t xend,uint32_t yend,uint32_t color)
{
    uint32_t i,j;

    _bsp_lcd_set_address(xsta,ysta,xend,yend);
    for(i=ysta; i<=yend; i++)
    {
        for(j=xsta; j<=xend; j++)_bsp_lcd_write_data32(color);
    }

    return HW_ERR_OK;
}


static uint8_t _protocol_spi_read_write_byte(uint8_t tx_data)
{
#if 1
    HAL_SPI_Transmit(&hspi4, &tx_data, 1, 1);
#else
    while (__HAL_SPI_GET_FLAG(bsp_lcd_manager.spi_port, SPI_FLAG_TXE ) == RESET);

    WRITE_REG(bsp_lcd_manager.spi_port->Instance->TXDR, tx_data);

    while (__HAL_SPI_GET_FLAG(bsp_lcd_manager.spi_port, SPI_FLAG_RXNE ) == RESET);

    return READ_REG(bsp_lcd_manager.spi_port->Instance->RXDR);
#endif

    return HW_ERR_OK;


}

static uint8_t _bsp_lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, LCD_RS_Pin|LCD_CS_Pin|LCD_RESET_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : LCD_RS_Pin LCD_CS_Pin LCD_RESET_Pin */
    GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_CS_Pin|LCD_RESET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pin : LCD_BL_Pin */
    GPIO_InitStruct.Pin = LCD_BL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_BL_GPIO_Port, &GPIO_InitStruct);

    return HW_ERR_OK;
}


static uint8_t _bsp_lcd_spi_init(void)
{
    hspi4.Instance = SPI4;
    hspi4.Init.Mode = SPI_MODE_MASTER;
    hspi4.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi4.Init.NSS = SPI_NSS_SOFT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 0x0;
    hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    if (HAL_SPI_Init(&hspi4) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI4_Init 2 */

    __HAL_SPI_ENABLE(&hspi4);

    return HW_ERR_OK;
}


void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(hspi->Instance==SPI4)
    {
        /* USER CODE BEGIN SPI4_MspInit 0 */

        /* USER CODE END SPI4_MspInit 0 */
        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
        PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* Peripheral clock enable */
        __HAL_RCC_SPI4_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**SPI4 GPIO Configuration
        PE5     ------> SPI4_MISO
        PE6     ------> SPI4_MOSI
        PE12     ------> SPI4_SCK
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_12;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI4_MspInit 1 */

        /* USER CODE END SPI4_MspInit 1 */
    }

}

/**
* @brief SPI MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hspi: SPI handle pointer
* @retval None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
    if(hspi->Instance==SPI4)
    {
        /* USER CODE BEGIN SPI4_MspDeInit 0 */

        /* USER CODE END SPI4_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI4_CLK_DISABLE();

        /**SPI4 GPIO Configuration
        PE5     ------> SPI4_MISO
        PE6     ------> SPI4_MOSI
        PE12     ------> SPI4_SCK
        */
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_12);

        /* USER CODE BEGIN SPI4_MspDeInit 1 */

        /* USER CODE END SPI4_MspDeInit 1 */
    }

}


static uint8_t _bsp_lcd_write_reg(uint8_t reg_index)
{
    LCD_CS_L;
    LCD_DC_L;
    _protocol_spi_read_write_byte(reg_index);
    LCD_CS_H;

    return HW_ERR_OK;
}

static uint8_t _bsp_lcd_write_byte(uint8_t byte)
{
    LCD_CS_L;
    LCD_DC_H;
    _protocol_spi_read_write_byte(byte);
    LCD_CS_H;

    return HW_ERR_OK;
}

static uint8_t _bsp_lcd_write_data32(uint32_t data)
{
    LCD_CS_L;
    LCD_DC_H;
    _protocol_spi_read_write_byte(data>>16);
    _protocol_spi_read_write_byte(data>>8);
    _protocol_spi_read_write_byte(data);
    LCD_CS_H;

    return HW_ERR_OK;
}

static uint8_t _bsp_lcd_set_address(uint32_t col_start,uint32_t page_start,uint32_t col_end,uint32_t page_end)
{
    _bsp_lcd_write_reg(lcd_cmd_col_addr_set);
    _bsp_lcd_write_byte(col_start>>8);
    _bsp_lcd_write_byte(col_start);
    _bsp_lcd_write_byte(col_end>>8);
    _bsp_lcd_write_byte(col_end);
    _bsp_lcd_write_reg(lcd_cmd_page_addr_set);
    _bsp_lcd_write_byte(page_start>>8);
    _bsp_lcd_write_byte(page_start);
    _bsp_lcd_write_byte(page_end>>8);
    _bsp_lcd_write_byte(page_end);
    _bsp_lcd_write_reg(lcd_cmd_mem_write);

    return HW_ERR_OK;
}

