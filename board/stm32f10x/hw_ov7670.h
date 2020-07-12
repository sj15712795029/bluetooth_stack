#ifndef HW_OV7670_H_H_H
#define HW_OV7670_H_H_H

#include "hw_misc.h"

/**************************** 以下需要换PIN脚的时候重新配置 ***************************/
/* OV7670 SCCB interface GPIO config */
#define OV7670_SCCB_C_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_SCCB_C_GPIO GPIOB
#define OV7670_SCCB_C GPIO_Pin_10
#define OV7670_SCCB_D_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_SCCB_D_GPIO GPIOB
#define OV7670_SCCB_D GPIO_Pin_11
/* OV7670 control interface GPIO config */
#define OV7670_VSYNC_PERIPH_CLK RCC_APB2Periph_GPIOA
#define OV7670_VSYNC_GPIO	GPIOA
#define OV7670_VSYNC 	GPIO_Pin_8
#define OV7670_EXIT_GPIO GPIO_PortSourceGPIOA
#define OV7670_EXIT_PIN GPIO_PinSource8
#define OV7670_EXIT_FUNC EXTI9_5_IRQHandler
#define OV7670_EXIT_IRQ EXTI9_5_IRQn
#define OV7670_EXIT_LINE EXTI_Line8
#define OV7670_FIFO_WEN_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_FIFO_WEN_GPIO	GPIOB
#define OV7670_FIFO_WEN	GPIO_Pin_3
#define OV7670_FIFO_RCLK_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_FIFO_RCLK_GPIO	GPIOB
#define OV7670_FIFO_RCLK 	GPIO_Pin_4 
#define OV7670_FIFO_RRST_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_FIFO_RRST_GPIO GPIOB
#define OV7670_FIFO_RRST	GPIO_Pin_15
#define OV7670_FIFO_OE_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_FIFO_OE_GPIO GPIOB
#define OV7670_FIFO_OE		GPIO_Pin_14
#define OV7670_FIFO_WRST_PERIPH_CLK RCC_APB2Periph_GPIOB
#define OV7670_FIFO_WRST_GPIO GPIOB
#define OV7670_FIFO_WRST	GPIO_Pin_13
/* OV7670 data interface GPIO config */
#define OV7670_DATA_PERIPH_CLK RCC_APB2Periph_GPIOC
#define OV7670_D0		GPIO_Pin_0
#define OV7670_D1		GPIO_Pin_1
#define OV7670_D2		GPIO_Pin_2
#define OV7670_D3		GPIO_Pin_3
#define OV7670_D4		GPIO_Pin_4
#define OV7670_D5		GPIO_Pin_5	
#define OV7670_D6		GPIO_Pin_6
#define OV7670_D7		GPIO_Pin_7 	
#define OV7670_DATA_GPIO GPIOC
#define OV7670_SCCB_D_IN OV7670_SCCB_D_GPIO->CRH&=~(GPIO_CRH_CNF11|GPIO_CRH_MODE11);\
											OV7670_SCCB_D_GPIO->CRH|=GPIO_CRH_CNF11_1
#define OV7670_SCCB_D_OUT OV7670_SCCB_D_GPIO->CRH&=~(GPIO_CRH_CNF11|GPIO_CRH_MODE11);\
											OV7670_SCCB_D_GPIO->CRH|=GPIO_CRH_MODE11
/**************************************************************************************/


/* OV7670 SCCB interface operate */
#define OV7670_SCCB_C_H OV7670_SCCB_C_GPIO->BSRR = OV7670_SCCB_C
#define OV7670_SCCB_C_L OV7670_SCCB_C_GPIO->BRR = OV7670_SCCB_C
#define OV7670_SCCB_D_H OV7670_SCCB_D_GPIO->BSRR = OV7670_SCCB_D
#define OV7670_SCCB_D_L OV7670_SCCB_D_GPIO->BRR = OV7670_SCCB_D
#define OV7670_SCCB_D_STATE	 (OV7670_SCCB_D_GPIO->IDR&OV7670_SCCB_D)
/* OV7670 control interface operate */
#define OV7670_WRST_H OV7670_FIFO_WRST_GPIO->BSRR = OV7670_FIFO_WRST
#define OV7670_WRST_L OV7670_FIFO_WRST_GPIO->BRR = OV7670_FIFO_WRST
#define OV7670_RRST_H OV7670_FIFO_RRST_GPIO->BSRR = OV7670_FIFO_RRST
#define OV7670_RRST_L OV7670_FIFO_RRST_GPIO->BRR = OV7670_FIFO_RRST
#define OV7670_WEN_H OV7670_FIFO_WEN_GPIO->BSRR = OV7670_FIFO_WEN
#define OV7670_WEN_L OV7670_FIFO_WEN_GPIO->BRR = OV7670_FIFO_WEN
#define OV7670_OE_H OV7670_FIFO_OE_GPIO->BSRR = OV7670_FIFO_OE
#define OV7670_OE_L OV7670_FIFO_OE_GPIO->BRR = OV7670_FIFO_OE
#define OV7670_RCLK_H OV7670_FIFO_RCLK_GPIO->BSRR = OV7670_FIFO_RCLK
#define OV7670_RCLK_L OV7670_FIFO_RCLK_GPIO->BRR = OV7670_FIFO_RCLK


#define SCCB_ID   			0X42 
/* OV7670 register index */
typedef enum
{
	ov7670_reg_gain	= 0x00,
	ov7670_reg_blue	= 0x01,
	ov7670_reg_red	= 0x02,
	ov7670_reg_vref	= 0x03,
	ov7670_reg_com1	= 0x04,
	ov7670_reg_bave	= 0x05,
	ov7670_reg_gbave=0x06,
	ov7670_reg_aechh=0x07,
	ov7670_reg_rave	= 0x08,
	ov7670_reg_com2	= 0x09,
	ov7670_reg_pid	= 0x0a,
	ov7670_reg_ver	= 0x0b,
	ov7670_reg_com3	= 0x0c,
	ov7670_reg_com4	= 0x0d,
	ov7670_reg_com5	= 0x0e,
	ov7670_reg_com6	= 0x0f,
	ov7670_reg_aech	= 0x10,
	ov7670_reg_clkrc	= 0x11,
	ov7670_reg_com7	= 0x12,
	ov7670_reg_com8	= 0x13,
	ov7670_reg_com9	= 0x14,
	ov7670_reg_com10=0x15,
	ov7670_reg_rsvd1	= 0x16,
	ov7670_reg_hstart= 0x17,
	ov7670_reg_hstop	= 0x18,
	ov7670_reg_vstart= 0x19,
	ov7670_reg_vstop	= 0x1a,
	ov7670_reg_pshft	= 0x1b,
	ov7670_reg_midh	= 0x1c,
	ov7670_reg_midl	= 0x1d,
	ov7670_reg_mvfp	= 0x1e,
	ov7670_reg_laec	= 0x1f,
	ov7670_reg_adctl0= 0x20,
	ov7670_reg_adctl1= 0x21,
	ov7670_reg_adctl2= 0x22,
	ov7670_reg_adctl3= 0x23,
	ov7670_reg_aew	= 0x24,
	ov7670_reg_aeb	= 0x25,
	ov7670_reg_vpt	= 0x26,
	ov7670_reg_bbias	= 0x27,
	ov7670_reg_gbbias=0x28,
	ov7670_reg_rsvd2	= 0x29,
	ov7670_reg_exhch= 0x2a,
	ov7670_reg_exhcl	= 0x2b,
	ov7670_reg_rbias	= 0x2c,
	ov7670_reg_advfl	= 0x2d,
	ov7670_reg_advfh	= 0x2e,
	ov7670_reg_yave	= 0x2f,
	ov7670_reg_hsyst	= 0x30,
	ov7670_reg_hsyen= 0x31,
	ov7670_reg_href	= 0x32,
	ov7670_reg_chlf	= 0x33,
	ov7670_reg_arblm	= 0x34,
	ov7670_reg_rsvd3	= 0x35,
	ov7670_reg_rsvd4	= 0x36,
	ov7670_reg_adc	= 0x37,
	ov7670_reg_acom	= 0x38,
	ov7670_reg_ofon	= 0x39,
	ov7670_reg_tslb	= 0x3a,
	ov7670_reg_com11=0x3b,
	ov7670_reg_com12=0x3c,
	ov7670_reg_com13=0x3d,
	ov7670_reg_com14=0x3e,
	ov7670_reg_edge	= 0x3f,
	ov7670_reg_com15=0x40,
	ov7670_reg_com16=0x41,
	ov7670_reg_com17=0x42,
	ov7670_reg_awbc1=0x43,
	ov7670_reg_awbc2=0x44,
	ov7670_reg_awbc3=0x45,
	ov7670_reg_awbc4=0x46,
	ov7670_reg_awbc5=0x47,
	ov7670_reg_awbc6=0x48,
	ov7670_reg_rsvd5	= 0x49,
	ov7670_reg_rsvd6	= 0x4a,
	ov7670_reg_rgb4b= 0x4b,
	ov7670_reg_dnsth	= 0x4c,
	ov7670_reg_rsvd7	= 0x4d,
	ov7670_reg_rsvd8	= 0x4e,
	ov7670_reg_mtx1	= 0x4f,
	ov7670_reg_mtx2	= 0x50,
	ov7670_reg_mtx3	= 0x51,
	ov7670_reg_mtx4	= 0x52,
	ov7670_reg_mtx5	= 0x53,
	ov7670_reg_mtx6	= 0x54,
	ov7670_reg_bright= 0x55,
	ov7670_reg_contras=0x56,
	ov7670_reg_contras_center=0x57,
	ov7670_reg_mtxs	= 0x58,
	ov7670_reg_rsvd9	= 0x59,
	ov7670_reg_rsd10= 0x5a,
	ov7670_reg_rsd11= 0x5b,
	ov7670_reg_rsd12	= 0x5c,
	ov7670_reg_rsd13	= 0x5d,
	ov7670_reg_rsd14	= 0x5e,
	ov7670_reg_rsd15	= 0x5f,
	ov7670_reg_rsd16	= 0x60,
	ov7670_reg_rsd17	= 0x61,
	ov7670_reg_lcc1	= 0x62,
	ov7670_reg_lcc2	= 0x63,
	ov7670_reg_lcc3	= 0x64,
	ov7670_reg_lcc4	= 0x65,
	ov7670_reg_lcc5	= 0x66,
	ov7670_reg_manu	= 0x67,
	ov7670_reg_manv	= 0x68,
	ov7670_reg_gfix	= 0x69,
	ov7670_reg_ggain	= 0x6a,
	ov7670_reg_dblv	= 0x6b,
	ov7670_reg_awbctr3=0x6c,
	ov7670_reg_awbctr2=0x6d,
	ov7670_reg_awbctr1=0x6e,
	ov7670_reg_awbctr0=0x6f,
	ov7670_reg_scaling_xsc=0x70,
	ov7670_reg_scaling_ysc=0x71,
	ov7670_reg_scaling_dcwctr=0x72,
	ov7670_reg_scaling_pclkdiv=0x73,
	ov7670_reg_74		= 0x74,
	ov7670_reg_75		= 0x75,
	ov7670_reg_76		= 0x76,
	ov7670_reg_77		= 0x77,
	ov7670_reg_rsd18	= 0x78,
	ov7670_reg_rsd19	= 0x79,
	ov7670_reg_slop	= 0x7a,
	ov7670_reg_cam1 = 0x7b,
	ov7670_reg_cam2 = 0x7c,
	ov7670_reg_cam3 = 0x7d,
	ov7670_reg_cam4 = 0x7e,
	ov7670_reg_cam5 = 0x7f,
	ov7670_reg_cam6	= 0x80,
	ov7670_reg_cam7	= 0x81,
	ov7670_reg_cam8	= 0x82,
	ov7670_reg_cam9	= 0x83,
	ov7670_reg_cam10=0x84,
	ov7670_reg_cam11=0x85,
	ov7670_reg_cam12=0x86,
	ov7670_reg_cam13=0x87,
	ov7670_reg_cam14=0x88,
	ov7670_reg_cam15=0x89,
	ov7670_reg_rsd20	= 0x8a,
	ov7670_reg_rsd21	= 0x8b,
	ov7670_reg_rgb444=0x8c,
	ov7670_reg_rsd22= 0x8d,
	ov7670_reg_rsd23	= 0x8e,
	ov7670_reg_rsd24	= 0x8f,
	ov7670_reg_rsd25	= 0x90,
	ov7670_reg_rsd26	= 0x91,
	ov7670_reg_dmlnl	= 0x92,
	ov7670_reg_dmlnh= 0x93,
	ov7670_reg_lcc6	= 0x94,
	ov7670_reg_lcc7	= 0x95,
	ov7670_reg_bd50st=0x9d,
	ov7670_reg_bd60st=0x9e,
	ov7670_reg_haecc1=0x9f,
	ov7670_reg_haecc2=0xa0,
	ov7670_reg_scaling_pclk_dly=0xa2,
	ov7670_reg_nt_ctl	= 0xa4,
	ov7670_reg_bd50max=0xa5,
	ov7670_reg_haecc3=0xa6,
	ov7670_reg_haecc4=0xa7,
	ov7670_reg_haecc5=0xa8,
	ov7670_reg_haecc6=0xa9,
	ov7670_reg_haecc7=0xaa,
	ov7670_reg_bd60max=0xab,
	ov7670_reg_stropt=0xac,
	ov7670_reg_str_r	= 0xad,
	ov7670_reg_str_g	= 0xae,
	ov7670_reg_str_b	= 0xaf,
	ov7670_reg_ablc1= 0xb1,
	ov7670_reg_thl_st= 0xb3,
	ov7670_reg_thl_dlt=0xb5,
	ov7670_reg_adchb= 0xbe,
	ov7670_reg_adchr	= 0xbf,
	ov7670_reg_adchgb= 0xc0,
	ov7670_reg_adchgr=0xc1,
	ov7670_reg_satctl	= 0xc9,
}ov7670_reg_index_e;	

typedef enum
{
	ov7670_ctl_start = 0,
	ov7670_ctl_stop = 1,
}ov7670_control_e;

typedef enum 
{
	ov7670_light_auto = 0,
	ov7670_light_sunny = 1,
	ov7670_light_cloudy = 2,
	ov7670_light_office = 3,
	ov7670_light_home = 4,
}ov7670_light_mode_e;

uint8_t hw_ov7670_init(void);
uint8_t hw_ov7670_control(uint8_t control);
uint8_t hw_ov7670_set_light_mode(uint8_t mode);
uint8_t hw_ov7670_set_color_saturation(uint8_t sat);
uint8_t hw_ov7670_set_brightness(uint8_t bright);
uint8_t hw_ov7670_set_contrast(uint8_t contrast);
uint8_t hw_ov7670_set_special_effect(uint8_t eft);
uint8_t hw_ov7670_set_window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void hw_ov7670_get_data_send(void);

#endif

