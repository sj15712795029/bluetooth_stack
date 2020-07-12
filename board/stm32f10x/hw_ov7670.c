/******************************************************************************
  * @file    hw_led.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2019-5-28
  * @brief   ov7670 operate source file
******************************************************************************/
#include "hw_ov7670.h"


/* FUNC DECLARATION */
static void protocol_sccb_init(void);
static void protocol_sccb_start(void);
static void protocol_sccb_stop(void);
static void protocol_sccb_noack(void);
static uint8_t protocol_sccb_read_byte(void);
static uint8_t protocol_sccb_write_byte(uint8_t dat);
static uint8_t protocol_sccb_read_reg(uint8_t reg_index);
static uint8_t protocol_sccb_write_reg(uint8_t reg,uint8_t data);

uint8_t ov7670_start = 0;
uint8_t ov7670_frame_interrupt = 0;
const uint8_t ov7670_init_reg_tbl_new[][2]=
{
    {ov7670_reg_tslb, 0x04},
    {ov7670_reg_com15, 0xd0},
    {ov7670_reg_com7, 0x14},/* QVGA 320*240 RGB格式 */

    {ov7670_reg_href, 0x80},
    {ov7670_reg_hstart, 0x16},
    {ov7670_reg_hstop, 0x04},
    {ov7670_reg_vstart, 0x02},
    {ov7670_reg_vstop, 0x7b},
    {ov7670_reg_vref, 0x06},

    {ov7670_reg_com3, 0x00},
    {ov7670_reg_com10, 0x00},
    {ov7670_reg_com14, 0x00},
    {ov7670_reg_scaling_xsc, 0x3a},
    {ov7670_reg_scaling_ysc, 0x35},
    {ov7670_reg_scaling_dcwctr, 0x11},
    {ov7670_reg_scaling_pclkdiv, 0x00},

    {ov7670_reg_scaling_pclk_dly, 0x02},
    {ov7670_reg_clkrc, 0x81},
    {ov7670_reg_slop, 0x20},
    {ov7670_reg_cam1, 0x1c},
    {ov7670_reg_cam2, 0x28},
    {ov7670_reg_cam3, 0x3c},
    {ov7670_reg_cam4, 0x55},
    {ov7670_reg_cam5, 0x68},
    {ov7670_reg_cam6, 0x76},
    {ov7670_reg_cam7, 0x80},
    {ov7670_reg_cam8, 0x88},
    {ov7670_reg_cam9, 0x8f},
    {ov7670_reg_cam10, 0x96},
    {ov7670_reg_cam11, 0xa3},
    {ov7670_reg_cam12, 0xaf},
    {ov7670_reg_cam13, 0xc4},
    {ov7670_reg_cam14, 0xd7},
    {ov7670_reg_cam15, 0xe8},
    {ov7670_reg_com8, 0xe0},
    {ov7670_reg_gain, 0x00},

    {ov7670_reg_aech, 0x00},
    {ov7670_reg_com4, 0x00},
    {ov7670_reg_com9, 0x28},
    {ov7670_reg_bd50max, 0x05},
    {ov7670_reg_bd60max, 0x07},

    {ov7670_reg_aew, 0x75},
    {ov7670_reg_aeb, 0x63},
    {ov7670_reg_vpt, 0xA5},
    {ov7670_reg_haecc1, 0x78},
    {ov7670_reg_haecc2, 0x68},

    {0xa1, 0x03},
    {ov7670_reg_haecc3, 0xdf},
    {ov7670_reg_haecc4, 0xdf},
    {ov7670_reg_haecc5, 0xf0},
    {ov7670_reg_haecc6, 0x90},

    {ov7670_reg_haecc7, 0x94},
    {ov7670_reg_com8, 0xe5},
    {ov7670_reg_com5, 0x61},
    {ov7670_reg_com6, 0x4b},
    {ov7670_reg_rsvd1, 0x02},

    {ov7670_reg_mvfp, 0x27},
    {ov7670_reg_adctl1, 0x02},
    {ov7670_reg_adctl2, 0x91},
    {0x29, 0x07},
    {ov7670_reg_chlf, 0x0b},

    {0x35, 0x0b},
    {ov7670_reg_adc, 0x1d},
    {ov7670_reg_acom, 0x71},
    {ov7670_reg_ofon, 0x2a},
    {ov7670_reg_com12, 0x78},

    {0x4d, 0x40},
    {0x4e, 0x20},
    {ov7670_reg_gfix, 0x00},
    {ov7670_reg_dblv, 0x40},
    {ov7670_reg_74, 0x19},
    {0x8d, 0x4f},

    {0x8e, 0x00},
    {0x8f, 0x00},
    {0x90, 0x00},
    {0x91, 0x00},
    {ov7670_reg_dmlnl, 0x00},

    {0x96, 0x00},
    {0x9a, 0x80},
    {0xb0, 0x84},
    {0xb1, 0x0c},
    {0xb2, 0x0e},

    {0xb3, 0x82},
    {0xb8, 0x0a},
    {ov7670_reg_awbc1, 0x14},
    {ov7670_reg_awbc2, 0xf0},
    {ov7670_reg_awbc3, 0x34},

    {ov7670_reg_awbc4, 0x58},
    {ov7670_reg_awbc5, 0x28},
    {ov7670_reg_awbc6, 0x3a},
    {0x59, 0x88},
    {0x5a, 0x88},

    {0x5b, 0x44},
    {0x5c, 0x67},
    {0x5d, 0x49},
    {0x5e, 0x0e},
    {ov7670_reg_lcc3, 0x04},
    {ov7670_reg_lcc4, 0x20},

    {ov7670_reg_lcc5, 0x05},
    {ov7670_reg_lcc6, 0x04},
    {ov7670_reg_lcc7, 0x08},
    {ov7670_reg_awbctr3, 0x0a},
    {ov7670_reg_awbctr2, 0x55},


    {ov7670_reg_mtx1, 0x80},
    {ov7670_reg_mtx2, 0x80},
    {ov7670_reg_mtx3, 0x00},
    {ov7670_reg_mtx4, 0x22},
    {ov7670_reg_mtx5, 0x5e},
    {ov7670_reg_mtx6, 0x80},

    {ov7670_reg_com2, 0x03},

    {ov7670_reg_awbctr1, 0x11},
    {ov7670_reg_awbctr0, 0x9f},
    {ov7670_reg_bright, 0x00},
    {ov7670_reg_contras, 0x40},
    {ov7670_reg_contras_center,0x40},

    {ov7670_reg_ggain, 0x40},
    {ov7670_reg_blue, 0x40},
    {ov7670_reg_red, 0x40},
    {ov7670_reg_com8, 0xe7},
    {ov7670_reg_com10, 0x00},

    {ov7670_reg_mtxs, 0x9e},

    {ov7670_reg_com16, 0x08},
    {ov7670_reg_edge, 0x00},
    {ov7670_reg_75, 0x05},
    {ov7670_reg_76, 0xe1},
    {ov7670_reg_dnsth, 0x00},
    {ov7670_reg_77, 0x01},
    {ov7670_reg_com13, 0xc2},
    {ov7670_reg_rgb4b, 0x09},
    {ov7670_reg_satctl, 0x60},
    {ov7670_reg_com16, 0x38},

    {ov7670_reg_arblm, 0x11},
    {ov7670_reg_com11, 0x02},

    {ov7670_reg_nt_ctl, 0x89},
    {0x96, 0x00},
    {0x97, 0x30},
    {0x98, 0x20},
    {0x99, 0x30},
    {0x9a, 0x84},
    {0x9b, 0x29},
    {0x9c, 0x03},
    {ov7670_reg_bd50st, 0x4c},
    {ov7670_reg_bd60st, 0x3f},
    {0x78, 0x04},

    {0x79, 0x01},
    {0xc8, 0xf0},
    {0x79, 0x0f},
    {0xc8, 0x00},
    {0x79, 0x10},
    {0xc8, 0x7e},
    {0x79, 0x0a},
    {0xc8, 0x80},
    {0x79, 0x0b},
    {0xc8, 0x01},
    {0x79, 0x0c},
    {0xc8, 0x0f},
    {0x79, 0x0d},
    {0xc8, 0x20},
    {0x79, 0x09},
    {0xc8, 0x80},
    {0x79, 0x02},
    {0xc8, 0xc0},
    {0x79, 0x03},
    {0xc8, 0x40},
    {0x79, 0x05},
    {0xc8, 0x30},
    {0x79, 0x26},
    {ov7670_reg_com2, 0x00},
};


void OV7670_EXIT_FUNC(void)
{
    if(EXTI_GetITStatus(OV7670_EXIT_LINE)==SET)
    {
        OV7670_WRST_L;
        OV7670_WRST_H;
        OV7670_WEN_H;
        ov7670_frame_interrupt++;
    }
    EXTI_ClearITPendingBit(OV7670_EXIT_LINE);
}

/******************************************************************************
 * func name   : hw_ov7670_init
 * para        : NULL
 * return      : ov7670 init result
 * description : ov7670 init
******************************************************************************/
uint8_t hw_ov7670_init()
{
    uint16_t i=0;
    uint8_t pid_msb,pid_lsb;
    uint8_t vid_h,vid_l;
	
    GPIO_InitTypeDef  GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable ov7670 clock */
    RCC_APB2PeriphClockCmd(OV7670_FIFO_WEN_PERIPH_CLK |OV7670_FIFO_RCLK_PERIPH_CLK |OV7670_FIFO_RRST_PERIPH_CLK |
                           OV7670_FIFO_OE_PERIPH_CLK |OV7670_FIFO_WRST_PERIPH_CLK |OV7670_VSYNC_PERIPH_CLK |
                           OV7670_DATA_PERIPH_CLK  | RCC_APB2Periph_AFIO,ENABLE);

    /* GPIO init */
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=OV7670_VSYNC;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(OV7670_VSYNC_GPIO,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=OV7670_FIFO_WEN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(OV7670_FIFO_WEN_GPIO,&GPIO_InitStructure);
    GPIO_SetBits(OV7670_FIFO_WEN_GPIO,OV7670_FIFO_WEN);

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=OV7670_FIFO_RCLK;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(OV7670_FIFO_RCLK_GPIO,&GPIO_InitStructure);
    GPIO_SetBits(OV7670_FIFO_RCLK_GPIO,OV7670_FIFO_RCLK);

    GPIO_InitStructure.GPIO_Pin  = OV7670_FIFO_RRST;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(OV7670_FIFO_RRST_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OV7670_FIFO_RRST_GPIO,OV7670_FIFO_RRST);

    GPIO_InitStructure.GPIO_Pin  = OV7670_FIFO_OE;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(OV7670_FIFO_OE_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OV7670_FIFO_OE_GPIO,OV7670_FIFO_OE);


    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=OV7670_FIFO_WRST;
    GPIO_Init(OV7670_FIFO_WRST_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OV7670_FIFO_WRST_GPIO,OV7670_FIFO_WRST);

    GPIO_InitStructure.GPIO_Pin  = OV7670_D0 | OV7670_D1 | OV7670_D2 | OV7670_D3 |
                                   OV7670_D4 | OV7670_D5 | OV7670_D6 | OV7670_D7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(OV7670_DATA_GPIO, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

    protocol_sccb_init();

    if(protocol_sccb_write_reg(ov7670_reg_com7,0x80))
		return HW_ERR_OV7670_SCCB_ERR;
	
    hw_delay_us(500);

    vid_h = protocol_sccb_read_reg(ov7670_reg_midh);
    vid_l = protocol_sccb_read_reg(ov7670_reg_midl);
    pid_msb = protocol_sccb_read_reg(ov7670_reg_pid);
    pid_lsb = protocol_sccb_read_reg(ov7670_reg_ver);
    HW_DEBUG("PIDM 0x%02x PIDL 0x%02x,VIDH 0x%02x VIDL 0x%02x\n",pid_msb,pid_lsb,vid_h,vid_l);

    for(i=0; i<sizeof(ov7670_init_reg_tbl_new)/sizeof(ov7670_init_reg_tbl_new[0]); i++)
    {
        protocol_sccb_write_reg(ov7670_init_reg_tbl_new[i][0],ov7670_init_reg_tbl_new[i][1]);
    }

    GPIO_EXTILineConfig(OV7670_EXIT_GPIO,OV7670_EXIT_PIN);

    EXTI_InitStructure.EXTI_Line=OV7670_EXIT_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = OV7670_EXIT_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =OV7670_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = OV7670_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    hw_ov7670_set_window(0,0,320,240);
    hw_ov7670_control(ov7670_ctl_start);

    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_ov7670_control
 * para        : control
 						ov7670_ctl_start --> start capture camera data
 						ov7670_ctl_stop  --> stop capture camera data
 * return      : ov7670 hw_ov7670_control result
 * description : control ov7670 start/stop
******************************************************************************/
uint8_t hw_ov7670_control(uint8_t control)
{
    if(control == ov7670_ctl_start)
    {
        ov7670_start = 1;
        OV7670_OE_L;
    }
    else if(control == ov7670_ctl_stop)
    {
        ov7670_start = 0;
        OV7670_OE_H;
    }

    return HW_ERR_OK;
}


/******************************************************************************
 * func name   : hw_ov7670_set_light_mode
 * para        : mode
 						ov7670_light_auto		--> set auto mode
 						ov7670_light_sunny		--> set sunny mode
 						ov7670_light_cloudy		--> set cloudy mode
 						ov7670_light_office		--> set office mode
 						ov7670_light_home		--> set home mode
 * return      : ov7670 hw_ov7670_set_light_mode result
 * description : set ov7670 light mode
******************************************************************************/
uint8_t hw_ov7670_set_light_mode(uint8_t mode)
{
    uint8_t reg13val=0xE7;
    uint8_t reg01val=0;
    uint8_t reg02val=0;
    switch(mode)
    {
    case ov7670_light_sunny:
        reg13val=0xE5;
        reg01val=0x5A;
        reg02val=0x5C;
        break;
    case ov7670_light_cloudy:
        reg13val=0xE5;
        reg01val=0x58;
        reg02val=0x60;
        break;
    case ov7670_light_office:
        reg13val=0xE5;
        reg01val=0x84;
        reg02val=0x4c;
        break;
    case ov7670_light_home:
        reg13val=0xE5;
        reg01val=0x96;
        reg02val=0x40;
        break;
    }
    protocol_sccb_write_reg(ov7670_reg_com8,reg13val);
    protocol_sccb_write_reg(ov7670_reg_blue,reg01val);
    protocol_sccb_write_reg(ov7670_reg_red,reg02val);
    return HW_ERR_OK;
}


/******************************************************************************
 * func name   : hw_ov7670_set_color_saturation
 * para        : sat
 						0		--> -2
 						1		--> -1
 						2		--> 0
 						3		--> 1
 						4		--> 2
 * return      : ov7670 hw_ov7670_set_color_saturation result
 * description : set ov7670 color saturation
******************************************************************************/
uint8_t hw_ov7670_set_color_saturation(uint8_t sat)
{
    uint8_t reg4f5054val=0x80;
    uint8_t reg52val=0x22;
    uint8_t reg53val=0x5E;
    switch(sat)
    {
    case 0:
        reg4f5054val=0x40;
        reg52val=0x11;
        reg53val=0x2F;
        break;
    case 1:
        reg4f5054val=0x66;
        reg52val=0x1B;
        reg53val=0x4B;
        break;
    case 3:
        reg4f5054val=0x99;
        reg52val=0x28;
        reg53val=0x71;
        break;
    case 4:
        reg4f5054val=0xC0;
        reg52val=0x33;
        reg53val=0x8D;
        break;
    }
    protocol_sccb_write_reg(ov7670_reg_mtx1,reg4f5054val);	//色彩矩阵系数1
    protocol_sccb_write_reg(ov7670_reg_mtx2,reg4f5054val);	//色彩矩阵系数2
    protocol_sccb_write_reg(ov7670_reg_mtx3,0x00);			//色彩矩阵系数3
    protocol_sccb_write_reg(ov7670_reg_mtx4,reg52val);		//色彩矩阵系数4
    protocol_sccb_write_reg(ov7670_reg_mtx5,reg53val);		//色彩矩阵系数5
    protocol_sccb_write_reg(ov7670_reg_mtx6,reg4f5054val);	//色彩矩阵系数6
    protocol_sccb_write_reg(ov7670_reg_mtxs,0x9E);			//MTXS

    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_ov7670_set_brightness
 * para        : bright
 						0		--> -2
 						1		--> -1
 						2		--> 0
 						3		--> 1
 						4		--> 2
 * return      : ov7670 hw_ov7670_set_brightness result
 * description : set ov7670 brightness
******************************************************************************/
uint8_t hw_ov7670_set_brightness(uint8_t bright)
{
    uint8_t reg55val=0x00;
    switch(bright)
    {
    case 0:
        reg55val=0xB0;
        break;
    case 1:
        reg55val=0x98;
        break;
    case 3:
        reg55val=0x18;
        break;
    case 4:
        reg55val=0x30;
        break;
    }
    protocol_sccb_write_reg(ov7670_reg_bright,reg55val);	//亮度调节
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_ov7670_set_contrast
 * para        : bright
 						0		--> -2
 						1		--> -1
 						2		--> 0
 						3		--> 1
 						4		--> 2
 * return      : ov7670 hw_ov7670_set_contrast result
 * description : set ov7670 contrast
******************************************************************************/
uint8_t hw_ov7670_set_contrast(uint8_t contrast)
{
    uint8_t reg56val=0x40;
    switch(contrast)
    {
    case 0:
        reg56val=0x30;
        break;
    case 1:
        reg56val=0x38;
        break;
    case 3:
        reg56val=0x50;
        break;
    case 4:
        reg56val=0x60;
        break;
    }
    protocol_sccb_write_reg(ov7670_reg_contras,reg56val);	//对比度调节
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_ov7670_set_special_effect
 * para        : bright
 						0		--> 普通模式
 						1		--> 负片
 						2		--> 黑白
 						3		--> 偏红色
 						4		--> 偏绿色
 						5		--> 偏蓝色
 						6		--> 复古
 * return      : ov7670 hw_ov7670_set_special_effect result
 * description : set ov7670 special effect
******************************************************************************/
uint8_t hw_ov7670_set_special_effect(uint8_t eft)
{
    uint8_t reg3aval=0X04;
    uint8_t reg67val=0XC0;
    uint8_t reg68val=0X80;
    switch(eft)
    {
    case 1:
        reg3aval=0X24;
        reg67val=0X80;
        reg68val=0X80;
        break;
    case 2:
        reg3aval=0X14;
        reg67val=0X80;
        reg68val=0X80;
        break;
    case 3:
        reg3aval=0X14;
        reg67val=0Xc0;
        reg68val=0X80;
        break;
    case 4:
        reg3aval=0X14;
        reg67val=0X40;
        reg68val=0X40;
        break;
    case 5:
        reg3aval=0X14;
        reg67val=0X80;
        reg68val=0XC0;
        break;
    case 6:
        reg3aval=0X14;
        reg67val=0XA0;
        reg68val=0X40;
        break;
    }
    protocol_sccb_write_reg(ov7670_reg_tslb,reg3aval);
    protocol_sccb_write_reg(ov7670_reg_manv,reg67val);
    protocol_sccb_write_reg(ov7670_reg_manu,reg68val);
    return HW_ERR_OK;
}

/******************************************************************************
 * func name   : hw_ov7670_set_window
 * para        : bright
 						sx		--> 普通模式
 						sy		--> 负片
 						width		--> 黑白
 						height		--> 偏红色
 * return      : ov7670 hw_ov7670_set_window result
 * description : set ov7670 windows
******************************************************************************/
uint8_t hw_ov7670_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
    uint16_t endx;
    uint16_t endy;
    uint8_t temp;

    if ((sx+width) > 320)
    {
        width = 320 - sx;
    }

    if ((sy+height) > 240)
    {
        height = 240 - sy;
    }

    sx += 176;
    sy += 12;

    endx=sx+width*2;	
    endy=sy+height*2;
    if(endx>784)
    {
        endx-=784;
    }

    temp=protocol_sccb_read_reg(0X32);
    temp&=0XC0;
    temp|=((endx&0X07)<<3)|(sx&0X07);
    protocol_sccb_write_reg(ov7670_reg_href,temp);
    protocol_sccb_write_reg(ov7670_reg_hstart,sx>>3);
    protocol_sccb_write_reg(ov7670_reg_hstop,endx>>3);

    temp=protocol_sccb_read_reg(0X03);
    temp&=0XF0;
    temp|=((endy&0X03)<<2)|(sy&0X03);
    protocol_sccb_write_reg(ov7670_reg_vref,temp);
    protocol_sccb_write_reg(ov7670_reg_vstart,sy>>2);
    protocol_sccb_write_reg(ov7670_reg_vstop,endy>>2);

    return HW_ERR_OK;
}


void hw_ov7670_get_data_send()
{
    uint32_t i, j;
    uint8_t color1,color2;
    if(ov7670_frame_interrupt)
    {
        OV7670_RRST_L;
        OV7670_RCLK_L;
        OV7670_RCLK_H;
        OV7670_RCLK_L;
        OV7670_RRST_H;
        OV7670_RCLK_H;
        if(ov7670_start)
            HW_DEBUG("camera_start\n");
        for(i=0; i<240; i++)
        {
            if(ov7670_start == 1)
            {
                HW_DEBUG("L");
                for(j=0; j < 320; j++)
                {
                    OV7670_RCLK_L;
                    color1=GPIOC->IDR&0XFF;
                    OV7670_RCLK_H;
                    OV7670_RCLK_L;
                    color2=GPIOC->IDR&0XFF;
                    OV7670_RCLK_H;
                    HW_DEBUG("%02x",color2);
                    HW_DEBUG("%02x",color1);
                }
                HW_DEBUG("\n");
            }
            else
                break;
        }
        ov7670_frame_interrupt=0;
    }
}

static void protocol_sccb_init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(OV7670_SCCB_C_PERIPH_CLK|OV7670_SCCB_D_PERIPH_CLK,ENABLE);

    GPIO_InitStructure.GPIO_Pin = OV7670_SCCB_D;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OV7670_SCCB_D_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OV7670_SCCB_D_GPIO,OV7670_SCCB_D);

    GPIO_InitStructure.GPIO_Pin = OV7670_SCCB_C;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(OV7670_SCCB_C_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(OV7670_SCCB_C_GPIO,OV7670_SCCB_C);

    OV7670_SCCB_D_OUT;
}



static void protocol_sccb_start()
{
    /* SCCB data high */
    OV7670_SCCB_D_H;
    hw_delay_us(50);
    /* SCCB clock high */
    OV7670_SCCB_C_H;
    hw_delay_us(50);
    /* SCCB data low */
    OV7670_SCCB_D_L;
    hw_delay_us(50);
    /* SCCB clock low */
    OV7670_SCCB_C_L;
    hw_delay_us(50);
}


static void protocol_sccb_stop()
{
    OV7670_SCCB_D_L;
    hw_delay_us(50);
    OV7670_SCCB_C_H;
    hw_delay_us(50);
    OV7670_SCCB_D_H;
    hw_delay_us(50);
}

static void protocol_sccb_noack()
{
    OV7670_SCCB_D_H;
    hw_delay_us(50);
    OV7670_SCCB_C_H;
    hw_delay_us(50);
    OV7670_SCCB_C_L;
    hw_delay_us(50);
    OV7670_SCCB_D_L;
    hw_delay_us(50);
}

static uint8_t protocol_sccb_write_byte(uint8_t dat)
{
    uint8_t j,res;
    for(j=0; j<8; j++)
    {
        if(dat&0x80)OV7670_SCCB_D_H;
        else OV7670_SCCB_D_L;
        dat<<=1;
        hw_delay_us(50);
        OV7670_SCCB_C_H;
        hw_delay_us(50);
        OV7670_SCCB_C_L;
    }
    OV7670_SCCB_D_IN;
    hw_delay_us(50);
    OV7670_SCCB_C_H;
    hw_delay_us(50);
    if(OV7670_SCCB_D_STATE)res=1;
    else res=0;
    OV7670_SCCB_C_L;
    OV7670_SCCB_D_OUT;
    return res;

}

static uint8_t protocol_sccb_read_byte()
{
    uint8_t temp=0,j;
    OV7670_SCCB_D_IN;
    for(j=8; j>0; j--)
    {
        hw_delay_us(50);
        OV7670_SCCB_C_H;
        temp=temp<<1;
        if(OV7670_SCCB_D_STATE)temp++;
        hw_delay_us(50);
        OV7670_SCCB_C_L;
    }
    OV7670_SCCB_D_OUT;
    return temp;
}

static uint8_t protocol_sccb_read_reg(uint8_t reg_index)
{
    uint8_t val=0;
    protocol_sccb_start();
    protocol_sccb_write_byte(SCCB_ID);
    hw_delay_us(50);
    protocol_sccb_write_byte(reg_index);
    hw_delay_us(50);
    protocol_sccb_stop();
    hw_delay_us(50);
    protocol_sccb_start();
    protocol_sccb_write_byte(SCCB_ID|0X01);
    hw_delay_us(50);
    val=protocol_sccb_read_byte();
    protocol_sccb_noack();
    protocol_sccb_stop();
    return val;
}

static uint8_t protocol_sccb_write_reg(uint8_t reg,uint8_t data)
{
    uint8_t res=0;
    protocol_sccb_start();
    if(protocol_sccb_write_byte(SCCB_ID))res=1;
    hw_delay_us(50);
    if(protocol_sccb_write_byte(reg))res=1;
    hw_delay_us(50);
    if(protocol_sccb_write_byte(data))res=1;
    protocol_sccb_stop();
    return	res;
}

