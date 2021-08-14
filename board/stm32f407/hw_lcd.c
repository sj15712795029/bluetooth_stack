/******************************************************************************
  * @file    hw_lcd.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-10-02
  * @brief   lcd source file
******************************************************************************/

#include "hw_lcd.h"


static void hw_lcd_bk_init(void);
static void hw_lcd_rst_init(void);
static void hw_lcd_fsmc_init(void);
static void hw_r61505v_init(void);
static void hw_lcd_rst(void);
static void hw_lcd_write_reg(uint32_t reg,uint32_t val);
static uint16_t hw_lcd_read_reg(uint32_t reg);
static void hw_lcd_write_cmd16(uint32_t index);
static void hw_lcd_write_data16(uint32_t val);
static  uint16_t  hw_lcd_read_data16(void);
static void hw_lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
static void hw_lcd_set_cursor(uint16_t x, uint16_t y);
static void hw_lcd_set_display_windows(uint16_t x,uint16_t y,uint16_t width,uint16_t height);
static void hw_lcd_quit_windows_mode(void);

SRAM_HandleTypeDef hsram4;

lcd_info_t lcddev;


void hw_lcd_init(void)
{
    hw_lcd_bk_init();
    hw_lcd_rst_init();
    
    hw_lcd_fsmc_init();
	hw_lcd_rst();
    LCD_BK_HIGH;


    lcddev.dir = DIR_VERTICAL;
    //lcddev.dir = DIR_HORIZONTAL;
    lcddev.id = hw_lcd_get_id();
    if(lcddev.id == LCD_ID_R61509V)
    {
        if(lcddev.dir == DIR_VERTICAL)
        {
            lcddev.width =LCD_V_WIDTH;
            lcddev.height = LCD_V_HEIGHT;

        }
        else if(lcddev.dir == DIR_HORIZONTAL)
        {
            lcddev.width =  LCD_H_WIDTH;
            lcddev.height = LCD_H_HEIGHT;

        }
        hw_r61505v_init();
    }
}

uint16_t hw_lcd_get_id(void)
{
    return hw_lcd_read_reg(REG_DEVICE_CODE);
}

void hw_lcd_display_on(void)
{
	hw_lcd_write_reg(REG_DISP_CTRL1,DISP_CTRL1_BASEE);
}

void hw_lcd_display_off(void)
{
    hw_lcd_write_reg(REG_DISP_CTRL1,0);
}



void hw_lcd_clear(uint16_t clear_color)
{
    uint32_t index;

    hw_lcd_set_display_windows(0, 0, lcddev.width, lcddev.height);

    for (index = 0; index < lcddev.height * lcddev.width; index++)
    {
            /* 写显存 */
            hw_lcd_write_data16(clear_color);
    }

}


void hw_lcd_fill(uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,uint16_t color)
{
    uint32_t i;
    uint16_t width = end_x- start_x+1;
    uint16_t height = end_y - start_y+1;

    hw_lcd_set_display_windows(start_x, start_y, width, height);

    for (i = 0; i < height*width; i++)
    {
            /* 写显存 */
            hw_lcd_write_data16(color);
    }

    hw_lcd_quit_windows_mode();
	
}

void hw_lcd_draw_line(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, uint16_t color)
{
    int32_t dx, dy ;
    int32_t tx, ty ;
    int32_t inc1, inc2 ;
    int32_t d, iTag ;
    int32_t x, y ;

    /* 采用 Bresenham 算法，在2点间画一条直线 */

    hw_lcd_draw_pixel(start_x, start_y, color);

    /* 如果两点重合，结束后面的动作。*/
    if ( start_x == end_x && start_y == end_y )
    {
        return;
    }

    iTag = 0 ;
    /* dx = abs ( _usX2 - _usX1 ); */
    if (end_x >= start_x)
    {
        dx = end_x - start_x;
    }
    else
    {
        dx = start_x - end_x;
    }

    /* dy = abs ( _usY2 - _usY1 ); */
    if (end_y >= start_y)
    {
        dy = end_y - start_y;
    }
    else
    {
        dy = start_y - end_y;
    }

    if ( dx < dy )   /*如果dy为计长方向，则交换纵横坐标。*/
    {
        uint16_t temp;

        iTag = 1 ;
        temp = start_x;
        start_x = start_y;
        start_y = temp;
        temp = end_x;
        end_x = end_y;
        end_y = temp;
        temp = dx;
        dx = dy;
        dy = temp;
    }
    tx = end_x > start_x ? 1 : -1 ;    /* 确定是增1还是减1 */
    ty = end_y > start_y ? 1 : -1 ;
    x = start_x ;
    y = start_y ;
    inc1 = 2 * dy ;
    inc2 = 2 * ( dy - dx );
    d = inc1 - dx ;
    while ( x != end_x )     /* 循环画点 */
    {
        if ( d < 0 )
        {
            d += inc1 ;
        }
        else
        {
            y += ty ;
            d += inc2 ;
        }
        if ( iTag )
        {
            hw_lcd_draw_pixel ( y, x, color) ;
        }
        else
        {
            hw_lcd_draw_pixel ( x, y, color) ;
        }
        x += tx ;
    }
}

static void hw_lcd_bk_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pins : LCD BK */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void hw_lcd_rst_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pins : LCD RST */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static void hw_lcd_fsmc_init(void)
{
    FSMC_NORSRAM_TimingTypeDef Timing = {0};
    hsram4.Instance = FSMC_NORSRAM_DEVICE;
    hsram4.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
    /* hsram4.Init */
    hsram4.Init.NSBank = FSMC_NORSRAM_BANK4;
    hsram4.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
    hsram4.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
    hsram4.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;
    hsram4.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
    hsram4.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram4.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
    hsram4.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
    hsram4.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
    hsram4.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
    hsram4.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
    hsram4.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram4.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
    hsram4.Init.PageSize = FSMC_PAGE_SIZE_NONE;
    /* Timing */
    Timing.AddressSetupTime = 15;
    Timing.AddressHoldTime = 15;
    Timing.DataSetupTime = 255;
    Timing.BusTurnAroundDuration = 15;
    Timing.CLKDivision = 16;
    Timing.DataLatency = 17;
    Timing.AccessMode = FSMC_ACCESS_MODE_A;
    /* ExtTiming */

    if (HAL_SRAM_Init(&hsram4, &Timing, NULL) != HAL_OK)
    {
        return ;
    }
}

static void hw_r61505v_init(void)
{
   /* Test Register (Software Reset) (R600h) */
    hw_lcd_write_reg(REG_SOFT_RESET,SOFT_RESET(1));
    hw_delay_ms(1);
    hw_lcd_write_reg(REG_SOFT_RESET,SOFT_RESET(0));
    hw_delay_ms(1);


    hw_lcd_write_reg(REG_DRIVER_OUTPUT,DRIVER_OUTPUT_SS_BIT);
    hw_lcd_write_reg(REG_LCD_DR_WAVE_CTRL,DRIVER_WAVE_CTL_BC_BIT);

    if(lcddev.dir == DIR_VERTICAL)
    {
        hw_lcd_write_reg(REG_ENTRY_MODE,/* ENTRY_MODE_HID | ENTRY_MODE_VID|*/ENTRY_MODE_AM | ENTRY_MODE_BGR);
    }
    else if(lcddev.dir == DIR_HORIZONTAL)
    {
    	hw_lcd_write_reg(REG_ENTRY_MODE,0);
        
    }

    hw_lcd_write_reg(REG_DISP_CTRL2,0x0808);

    /* gamma set */
    hw_lcd_write_reg(REG_GAMMA_CTRL1,0x000C);
    hw_lcd_write_reg(REG_GAMMA_CTRL2,0x5A0B);
    hw_lcd_write_reg(REG_GAMMA_CTRL3,0x0906);
    hw_lcd_write_reg(REG_GAMMA_CTRL4,0x1017);
    hw_lcd_write_reg(REG_GAMMA_CTRL5,0x2300);
    hw_lcd_write_reg(REG_GAMMA_CTRL6,0x1700);
    hw_lcd_write_reg(REG_GAMMA_CTRL7,0x6309);
    hw_lcd_write_reg(REG_GAMMA_CTRL8,0x0C09);
    hw_lcd_write_reg(REG_GAMMA_CTRL9,0x100C);
    hw_lcd_write_reg(REG_GAMMA_CTRL10,0x2232);

    /* Power Setting */
    hw_lcd_write_reg(REG_PAN_INTF_CTRL1,0x0014);
    hw_lcd_write_reg(REG_PAN_INTF_CTRL2,0x0101);
    hw_lcd_write_reg(REG_PWR_CTRL1,0x0230);
    hw_lcd_write_reg(REG_PWR_CTRL2,0x0247);
    hw_lcd_write_reg(REG_PWR_CTRL3,0xB1B0);
    hw_lcd_write_reg(REG_PWR_CTRL4,0x0A00);
    hw_lcd_write_reg(REG_RW_NVM,0xF0FF);

    /* window set */
    hw_lcd_write_reg(REG_RAM_HADDR_START,0);//Window Horizontal RAM Address Start
    hw_lcd_write_reg(REG_RAM_HADDR_END,lcddev.width);//Window Horizontal RAM Address End
    hw_lcd_write_reg(REG_RAM_VADDR_START,0);//Window Vertical RAM Address Start
    hw_lcd_write_reg(REG_RAM_HADDR_END,lcddev.height);//Window Horizontal RAM Address End
    hw_lcd_write_reg(REG_RAM_HADDR_SET,0);//RAM Address Set (Horizontal Address)
    hw_lcd_write_reg(REG_RAM_VADDR_SET,0);//RAM Address Set (Vertical Address)
    hw_lcd_write_reg(REG_BIMG_DISP_CTRL,0);//Base Image Display
    hw_lcd_write_reg(REG_DISP_CTRL1,0x0100);

}

static void hw_lcd_rst(void)
{
    LCD_RST_HIGH;
    hw_delay_ms(10);
    LCD_RST_LOW;
    hw_delay_ms(10);
    LCD_RST_HIGH;
    hw_delay_ms(10);
}


static void hw_lcd_write_reg(uint32_t reg,uint32_t val)
{
    hw_lcd_write_cmd16(reg);
    hw_lcd_write_data16(val);
}

static uint16_t hw_lcd_read_reg(uint32_t reg)
{
    hw_lcd_write_cmd16(reg);
    return hw_lcd_read_data16();
}

static void hw_lcd_write_cmd16(uint32_t index)
{
    *(__IO uint16_t *) (BANK_LCD_C)= index;
}

static void hw_lcd_write_data16(uint32_t val)
{
    *(__IO uint16_t *) (BANK_LCD_D)= val;
}

uint16_t  hw_lcd_read_data16(void)
{
    return (*(__IO uint16_t *) (BANK_LCD_D));
}

static void hw_lcd_set_cursor(uint16_t x, uint16_t y)
{
    if( LCD_ID_R61509V == lcddev.id)
    {
    	if(lcddev.dir == DIR_VERTICAL)
        {
        	hw_lcd_write_reg(REG_RAM_HADDR_SET,x);//RAM Address Set (Horizontal Address)
        	hw_lcd_write_reg(REG_RAM_VADDR_SET,y);//RAM Address Set (Vertical Address)
        }
        else if(lcddev.dir == DIR_HORIZONTAL)
        {
            x = lcddev.width - x  - 1;

            hw_lcd_write_cmd16(0x0201);
            hw_lcd_write_data16(x);//RAM Address Set (Horizontal Address)
            hw_lcd_write_cmd16(0x0200);
            hw_lcd_write_data16(y);//RAM Address Set (Vertical Address)

        }
    }
}


static void hw_lcd_set_display_windows(uint16_t x,uint16_t y,uint16_t width,uint16_t height)
{
    uint16_t Start_X, Start_Y, End_X, End_Y;

    if(LCD_ID_R61509V == lcddev.id)
    {
        if(lcddev.dir == DIR_VERTICAL)
        {
            Start_X = x;
            Start_Y = y;
            End_X =  x + width-1;
            End_Y =  y + height-1;
        }
        else if(lcddev.dir == DIR_HORIZONTAL)
        {
        #if 1
            Start_X = y;
            End_X =  y + height-1;

            End_Y = lcddev.width -  x - 1;
            Start_Y =  lcddev.width - (-x + width-1) - 1;
			#else
			Start_X = lcddev.width - _usX- _usWidth -1;
			End_X = lcddev.width -  _usX - 1;

			Start_Y = _usY;
			End_Y= _usY + _usHeight-1;
			#endif
        }

        /*window set*/
		hw_lcd_write_reg(REG_RAM_HADDR_START,Start_X);//Window Horizontal RAM Address Start
		hw_lcd_write_reg(REG_RAM_HADDR_END,End_X);//Window Horizontal RAM Address End
		hw_lcd_write_reg(REG_RAM_VADDR_START,Start_Y);//Window Vertical RAM Address Start
		hw_lcd_write_reg(REG_RAM_VADDR_END,End_Y);//Window Vertical RAM Address End

        hw_lcd_set_cursor(Start_X,Start_Y);
        hw_lcd_write_cmd16(REG_RW_GRAM);
    }
}

static void hw_lcd_quit_windows_mode(void)
{
    hw_lcd_set_display_windows(0, 0, lcddev.width, lcddev.height);
}

static void hw_lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    hw_lcd_set_cursor(x, y);	/* 设置光标位置 */
    if(LCD_ID_R61509V == lcddev.id)
    {
        hw_lcd_write_cmd16(0x202);
    }
    hw_lcd_write_data16(color);
}


void test_dir()
{
    uint32_t index = 0;
    /*window set*/
    hw_lcd_write_cmd16(0x0210);
    hw_lcd_write_data16(0);//Window Horizontal RAM Address Start
    hw_lcd_write_cmd16(0x0211);
    hw_lcd_write_data16(100);//Window Horizontal RAM Address End
    hw_lcd_write_cmd16(0x0212);
    hw_lcd_write_data16(0);//Window Vertical RAM Address Start
    hw_lcd_write_cmd16(0x0213);
    hw_lcd_write_data16(200);//Window Vertical RAM Address End

    hw_lcd_set_cursor(0,0);
    hw_lcd_write_cmd16(0x0202);

    for (index = 0; index < 100*200; index++)
    {
        hw_lcd_write_data16(COLOR_YELLOW);
    }

    hw_lcd_quit_windows_mode();
}



