/******************************************************************************
  * @file    hw_wm8960.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-05-28
  * @brief   wm8960 header file
******************************************************************************/
#ifndef HW_WM8960_H_H_H
#define HW_WM8960_H_H_H

#include "hw_misc.h"


#define WM8960_IIC_PERIPH_CLK RCC_APB2Periph_GPIOB
#define WM8960_SCL_GPIO GPIOB
#define WM8960_SDA_GPIO GPIOB
#define WM8960_SCL_PIN GPIO_Pin_8
#define WM8960_SDA_PIN GPIO_Pin_9

#define WM8960_IIS_PERIPH_CLK_1 RCC_APB2Periph_GPIOB
#define WM8960_IIS_PERIPH_CLK_2 RCC_APB2Periph_GPIOC
#define WM8960_I2S2_WS_GPIO GPIOB
#define WM8960_I2S2_WS_PIN GPIO_Pin_12
#define WM8960_I2S2_CK_GPIO GPIOB
#define WM8960_I2S2_CK_PIN GPIO_Pin_13
#define WM8960_I2S2_SD_GPIO GPIOB
#define WM8960_I2S2_SD_PIN GPIO_Pin_15
#define WM8960_I2S2_MCLK_GPIO GPIOC
#define WM8960_I2S2_MCLK_PIN GPIO_Pin_6


#define WM8960_IIC_SCLK_LOW GPIO_ResetBits(WM8960_SCL_GPIO,WM8960_SCL_PIN)
#define WM8960_IIC_SCLK_HIGH GPIO_SetBits(WM8960_SCL_GPIO,WM8960_SCL_PIN)
#define WM8960_IIC_SDIN_LOW GPIO_ResetBits(WM8960_SDA_GPIO,WM8960_SDA_PIN)
#define WM8960_IIC_SDIN_HIGH GPIO_SetBits(WM8960_SDA_GPIO,WM8960_SDA_PIN)
#define WM8960_IIC_READ_SDA (((WM8960_SDA_GPIO->IDR&WM8960_SDA_PIN)!=0)?1:0)

#define WM8960_HIGH_BIT_MASK 0x80
#define WM8960_IIC_ADDR 0x34
#define WM8960_IIC_READ 0x1
#define WM8960_IIC_WRITE 0x0

/* Left Input volume Register Value(wm8960_reg_r0 = 0x00) */
#define WM8960_LINVOL_MIN ((uint16_t)(0x00 << 0))     /* Left Input PGA(Programmable Gain Amplifier) Volume Control */
#define WM8960_LINVOL_MAX ((uint16_t)(0x3f << 0)) 	/* 0.75dB progressive increase(range -17.25dB~+30dB) */
#define WM8960_LIZC_CG_IM ((uint16_t)(0 << 6)) /* 0 = Change gain immediately */
#define WM8960_LIZC_CG_ZC  ((uint16_t)(1<< 6))/* 1 = Change gain on zero cross only */
#define WM8960_LINMUTE_DM	((uint16_t)(0 << 7)) /* 0 = Disable Mute */
#define WM8960_LINMUTE_EM ((uint16_t)(1<< 7))/* 1 = Enable Mute */
#define WM8960_LIPVU_UPDATE ((uint16_t)(1<< 8)) /* Writing a 1 to this bit will cause left and right input PGA volumes to be updated (LINVOL and RINVOL)*/

/* Right Input volume Register Value(wm8960_reg_r1 = 0x01) */
#define WM8960_RINVOL_MIN ((uint16_t)(0x00 << 0))     /* Right Input PGA(Programmable Gain Amplifier) Volume Control */
#define WM8960_RINVOL_MAX ((uint16_t)(0x3f << 0)) 	/* 0.75dB progressive increase(range -17.25dB~+30dB) */
#define WM8960_RIZC_CG_IM ((uint16_t)(0 << 6)) /* 0 = Change gain immediately */
#define WM8960_RIZC_CG_ZC  ((uint16_t)(1<< 6))/* 1 = Change gain on zero cross only */
#define WM8960_RINMUTE_DM	((uint16_t)(0 << 7)) /* 0 = Disable Mute */
#define WM8960_RINMUTE_EM ((uint16_t)(1<< 7))/* 1 = Enable Mute */
#define WM8960_RIPVU_UPDATE ((uint16_t)(1<< 8)) /* Writing a 1 to this bit will cause left and right input PGA volumes to be updated (LINVOL and RINVOL)*/

/* LOUT1 volume Register Value(wm8960_reg_r2 = 0x02) */
#define WM8960_LOUT1VOL_MIN ((uint16_t)(0x30 << 0))  /* LOUT1 Volume */
#define WM8960_LOUT1VOL_MAX ((uint16_t)(0x7f << 0)) /* 1dB progressive increase(range -73dB~+6dB) 0101111b to 0000000b = Analogue MUTE */
#define WM8960_LO1ZC_CG_IM ((uint16_t)(0 << 7)) /* 0 = Change gain immediately */
#define WM8960_LO1ZC_CG_ZC ((uint16_t)(1 << 7)) /* 1 = Change gain on zero cross only */
#define WM8960_LOUT1VU_UPDATE ((uint16_t)(1<< 8)) /* Headphone Output PGA Volume Update Writing a 1 to this bit will cause left and right headphone output volumes to be updated (LOUT1VOL and ROUT1VOL) */
/* ROUT1 volume Register Value(wm8960_reg_r3 = 0x03) */
#define WM8960_ROUT1VOL_MIN ((uint16_t)(0x30 << 0))  /* ROUT1 Volume */
#define WM8960_ROUT1VOL_MAX ((uint16_t)(0x7f << 0)) /* 1dB progressive increase(range -73dB~+6dB) 0101111b to 0000000b = Analogue MUTE */
#define WM8960_RO1ZC_CG_IM ((uint16_t)(0 << 7)) /* 0 = Change gain immediately */
#define WM8960_RO1ZC_CG_ZC ((uint16_t)(1 << 7)) /* 1 = Change gain on zero cross only */
#define WM8960_ROUT1VU_UPDATE ((uint16_t)(1<< 8)) /* Headphone Output PGA Volume Update Writing a 1 to this bit will cause left and right headphone output volumes to be updated (LOUT1VOL and ROUT1VOL) */

/* Clocking (1) Register Value(wm8960_reg_r4 = 0x04) */
#define WM8960_CLKSEL_MCLK ((uint16_t)(0 << 0)) /* 0 = SYSCLK derived from MCLK  */
#define WM8960_CLKSEL_PLL ((uint16_t)(1 << 0)) /* 1 = SYSCLK derived from PLL output */
#define WM8960_SYSCLKDIV1 ((uint16_t)(0 << 1)) /* 00 = Divide SYSCLK by 1 */
#define WM8960_SYSCLKDIV2 ((uint16_t)(2 << 1)) /* 10 = Divide SYSCLK by 2 */
#define WM8960_DACDIV_1_0 ((uint16_t)(0 << 3)) /* 000 = SYSCLK / (1.0 * 256) */
#define WM8960_DACDIV_1_5 ((uint16_t)(1 << 3)) /* 001 = SYSCLK / (1.5 * 256) */
#define WM8960_DACDIV_2_0 ((uint16_t)(2 << 3))/* 010 = SYSCLK / (2 * 256) */
#define WM8960_DACDIV_3_0 ((uint16_t)(3 << 3))/* 011 = SYSCLK / (3 * 256) */
#define WM8960_DACDIV_4_0 ((uint16_t)(4 << 3))/* 100 = SYSCLK / (4 * 256)  */
#define WM8960_DACDIV_5_5 ((uint16_t)(5 << 3))/* 101 = SYSCLK / (5.5 * 256) */
#define WM8960_DACDIV_6_0 ((uint16_t)(6 << 3))/* 110 = SYSCLK / (6 * 256)  */
#define WM8960_ADCDIV_1_0 ((uint16_t)(0 << 6)) /* 000 = SYSCLK / (1.0 * 256) */
#define WM8960_ADCDIV_1_5 ((uint16_t)(1 << 6)) /* 001 = SYSCLK / (1.5 * 256) */
#define WM8960_ADCDIV_2_0 ((uint16_t)(2 << 6))/* 010 = SYSCLK / (2 * 256) */
#define WM8960_ADCDIV_3_0 ((uint16_t)(3 << 6))/* 011 = SYSCLK / (3 * 256) */
#define WM8960_ADCDIV_4_0 ((uint16_t)(4 << 6))/* 100 = SYSCLK / (4 * 256)  */
#define WM8960_ADCDIV_5_5 ((uint16_t)(5 << 6))/* 101 = SYSCLK / (5.5 * 256) */
#define WM8960_ADCDIV_6_0 ((uint16_t)(6 << 6))/* 110 = SYSCLK / (6 * 256)  */

/* ADC & DAC Control (CTR1) Register Value(wm8960_reg_r5 = 0x05) */
#define WM8960_ADCHPD_ENABLE ((uint16_t)(0 << 0)) /* 0 = Enable high pass filter on left and right channels */
#define WM8960_ADCHPD_DISABLE ((uint16_t)(1 << 0)) /* 1 = Disable high pass filter on left and right channels */
#define WM8960_DEEMPH_48K ((uint16_t)(3 << 1)) /* 11 = 48kHz sample rate */
#define WM8960_DEEMPH_44_1K ((uint16_t)(2 << 1)) /* 10 = 44.1kHz sample rate */
#define WM8960_DEEMPH_32K ((uint16_t)(1 << 1))/* 01 = 32kHz sample rate */
#define WM8960_DEEMPH_NO ((uint16_t)(0 << 1))/* 00 = No de-emphasis  */
#define WM8960_DACMU_MUTE ((uint16_t)(1 << 3)) /* 1 = Mute */
#define WM8960_DACMU_NOMUTE ((uint16_t)(0 << 3)) /* 0 = No mute (signal active) */
#define WM8960_ADCPOL_NO ((uint16_t)(0 << 5)) /* 00 = Polarity not inverted */
#define WM8960_ADCPOL_LI ((uint16_t)(1 << 5))  /* 01 = ADC L inverted  */
#define WM8960_ADCPOL_RI ((uint16_t)(2 << 5)) /* 10 = ADC R inverted */
#define WM8960_ADCPOL_LRI ((uint16_t)(3 << 5)) /* 11 = ADC L and R inverted  */
#define WM8960_DACDIV2_DISABLE ((uint16_t)(0 << 7)) /* 0 = Disabled (0dB) */
#define WM8960_DACDIV2_ENABLE ((uint16_t)(1 << 7)) /* 1 = -6dB Enabled */
/* ADC & DAC Control (CTR2) Register Value(wm8960_reg_r6 = 0x06) */
#define WM8960_DACSLOPE_NM ((uint16_t)(0 << 1)) /* 0 = Normal mode  */
#define WM8960_DACSLOPE_SS ((uint16_t)(1 << 1))/* 1 = Sloping stopband  */
#define WM8960_DACMR_FR ((uint16_t)(0 << 2)) /* 0 = Fast ramp (24kHz at fs=48k, providing maximum delay of 10.7ms) */
#define WM8960_DACMR_SR ((uint16_t)(1 << 2)) /* 1 = Slow ramp (1.5kHz at fs=48k, providing maximum delay of 171ms) */
#define WM8960_DACSMM_0 ((uint16_t)(0 << 3))/* 0 = Disabling soft-mute (DACMU=0) will cause the volume to change immediately to the LDACVOL / RDACVOL settings */
#define WM8960_DACSMM_1 ((uint16_t)(1 << 3))/* 1 = Disabling soft-mute (DACMU=0) will cause the volume to ramp up gradually to the LDACVOL / RDACVOL settings */
#define WM8960_DACPOL_NO ((uint16_t)(0 << 5))/* 00 = Polarity not inverted */
#define WM8960_DACPOL_LI ((uint16_t)(1 << 5))/* 01 = DAC L inverted  */
#define WM8960_DACPOL_RI ((uint16_t)(2 << 5))/* 10 = DAC R inverted */
#define WM8960_DACPOL_LRI  ((uint16_t)(3 << 5))/* 11 = DAC L and R inverted */
/* Audio Interface Register Value(wm8960_reg_r7 = 0x07) */
#define WM8960_FORMAT_RJ ((uint16_t)(0 << 0)) /* 00 = Right justified  */
#define WM8960_FORMAT_LJ ((uint16_t)(1 << 0)) /* 01 = Left justified */
#define WM8960_FORMAT_I2S ((uint16_t)(2 << 0))/* 10 = I2S Format */
#define WM8960_FORMAT_DSP ((uint16_t)(3 << 0))/* 11 = DSP Mode */
#define WM8960_WL_16BIT ((uint16_t)(0 << 2))/* 00 = 16 bits */
#define WM8960_WL_20BIT ((uint16_t)(1 << 2))/* 01 = 20 bits  */
#define WM8960_WL_24BIT ((uint16_t)(2 << 2))/* 10 = 24 bits  */
#define WM8960_WL_32BIT ((uint16_t)(3 << 2))/* 11 = 32 bits (see Note) */
#define WM8960_LRP_0 ((uint16_t)(0 << 4)) /* i2s mode:0 = normal LRCLK polarity DSP mode:0 = MSB is available on 2nd BCLK rising edge after LRC rising edge (mode A) */
#define WM8960_LRP_1 ((uint16_t)(1 << 4)) /* i2s mode:1 = invert LRCLK polarit DSP mode:1 = MSB is available on 1st BCLK rising edge after LRC rising edge (mode B) */
#define WM8960_DLRSWAP_NORMAL ((uint16_t)(0 << 5)) /* 0 = Output left and right data as normal */
#define WM8960_DLRSWAP_SWAP ((uint16_t)(1 << 5)) /* 1 = Swap left and right DAC data in audio interface */
#define WM8960_MS_SLAVE ((uint16_t)(0 << 6)) /* 0 = Enable slave mode */
#define WM8960_MS_MASTER ((uint16_t)(1 << 6)) /* 1 = Enable master mode */
#define WM8960_BCLKINV_N ((uint16_t)(0 << 7)) /* 0 = BCLK not inverted */
#define WM8960_BCLKINV_I ((uint16_t)(1 << 7)) /* 1 = BCLK inverted */
#define WM8960_ALRSWAP_NORMAL ((uint16_t)(0 << 8)) /* 0 = Output left and right data as normal */
#define WM8960_ALRSWAP_SWAP ((uint16_t)(1 << 8)) /* 1 = Swap left and right ADC data in audio interface */
/* Clocking (2) Register Value(wm8960_reg_r8 = 0x08) */
#define WM8960_BCLKDIV_1_0 ((uint16_t)(0 << 0)) /* 0000 = SYSCLK BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_1_5 ((uint16_t)(1 << 0)) /* 0001 = SYSCLK / 1.5 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_2_0 ((uint16_t)(2 << 0))/* 0010 = SYSCLK / 2 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_3_0 ((uint16_t)(3 << 0))/* 0011 = SYSCLK / 3 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_4_0 ((uint16_t)(4 << 0))/* 0100 = SYSCLK / 4 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_5_5 ((uint16_t)(5 << 0))/* 0101 = SYSCLK / 5.5 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_6_0 ((uint16_t)(6 << 0))/* 0110 = SYSCLK / 6  BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_8_0 ((uint16_t)(7 << 0))/* 0111 = SYSCLK / 8 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_11_0 ((uint16_t)(8 << 0))/* 1000 = SYSCLK / 11 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_12_0 ((uint16_t)(9 << 0))/* 1001 = SYSCLK / 12 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_16_0 ((uint16_t)(10 << 0))/* 1010 = SYSCLK / 16  BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_22_0 ((uint16_t)(11 << 0))/* 1011 = SYSCLK / 22 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_24_0 ((uint16_t)(12 << 0))/* 1100 = SYSCLK / 24 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_32_0_MIN ((uint16_t)(13 << 0))/* 1101 to 1111 = SYSCLK / 32 BCLK Frequency (Master Mode) */
#define WM8960_BCLKDIV_32_0_MAX ((uint16_t)(15 << 0))/* 1101 to 1111 = SYSCLK / 32 BCLK Frequency (Master Mode) */
#define WM8960_DCLKDIV_1_5 ((uint16_t)(0 << 6)) /* 000 = SYSCLK / 1.5 (Not recommended) */
#define WM8960_DCLKDIV_2_0 ((uint16_t)(1 << 6)) /* 001 = SYSCLK / 2 */
#define WM8960_DCLKDIV_3_0 ((uint16_t)(2 << 6)) /* 010 = SYSCLK / 3 */
#define WM8960_DCLKDIV_4_0 ((uint16_t)(3 << 6)) /* 011 = SYSCLK / 4  */
#define WM8960_DCLKDIV_6_0 ((uint16_t)(4 << 6)) /* 100 = SYSCLK / 6 */
#define WM8960_DCLKDIV_8_0 ((uint16_t)(5 << 6)) /* 101 = SYSCLK / 8 */
#define WM8960_DCLKDIV_12_0 ((uint16_t)(6 << 6)) /* 110 = SYSCLK / 12 */
#define WM8960_DCLKDIV_16_0 ((uint16_t)(7 << 6)) /* 111 = SYSCLK / 16  */
/* Audio Interface Register Value(wm8960_reg_r9 = 0x09) */
#define WM8960_LOOPBACK_DISABLE ((uint16_t)(0 << 0)) /* 0 = No loopback.  */
#define WM8960_LOOPBACK_ENABLE ((uint16_t)(1 << 0)) /* 1 = Loopback enabled, ADC data output is fed directly into DAC data input. */
#define WM8960_ADCCOMP_OFF ((uint16_t)(0 << 1)) /* 00 = off */
#define WM8960_ADCCOMP_ULAW ((uint16_t)(2 << 1)) /* 10 = μ-law */
#define WM8960_ADCCOMP_ALAW ((uint16_t)(3 << 1)) /* 11 = A-law */
#define WM8960_DACCOMP_OFF ((uint16_t)(0 << 3)) /* 00 = off */
#define WM8960_DACCOMP_ULAW ((uint16_t)(2 << 3)) /* 10 = μ-law */
#define WM8960_DACCOMP_ALAW ((uint16_t)(3 << 3)) /* 11 = A-law */
#define WM8960_WL8_OFF ((uint16_t)(0 << 5)) /* 0 = Off 8-Bit Word Length Selec */
#define WM8960_WL8_ON ((uint16_t)(1 << 5)) /* 1 = Device operates in 8-bit mode.  8-Bit Word Length Selec */
#define WM8960_ALRCGPIO_0 ((uint16_t)(0 << 6))/* 0 = ADCLRC frame clock for ADC */
#define WM8960_ALRCGPIO_1 ((uint16_t)(1 << 6))/* 1 = GPIO pin */
/* Left DAC volume Register Value(wm8960_reg_r10 = 0x0a) */
#define WM8960_LDACVOL_MIN ((uint16_t)(0 << 0)) /* Left DAC Digital Volume Contro */
#define WM8960_LDACVOL_MAX ((uint16_t)(0xff << 0)) /* 0.5dB progressive increase(range -127dB~0dB) */
#define WM8960_LDACVU_UPDATE ((uint16_t)(1 << 8)) /* DAC Volume Update Writing a 1 to this bit will cause left and right DAC volumes to be updated (LDACVOL and RDACVOL) */
/* Right DAC volume Register Value(wm8960_reg_r11 = 0x0b) */
#define WM8960_RDACVOL_MIN ((uint16_t)(0 << 0)) /* Right DAC Digital Volume Contro */
#define WM8960_RDACVOL_MAX ((uint16_t)(0xff << 0)) /* 0.5dB progressive increase(range -127dB~0dB) */
#define WM8960_RDACVU_UPDATE ((uint16_t)(1 << 8)) /* DAC Volume Update Writing a 1 to this bit will cause left and right DAC volumes to be updated (LDACVOL and RDACVOL) */
/* 3D control Register Value(wm8960_reg_r16 = 0x10) */
#define WM8960_3DEN_DISABLE ((uint16_t)(0 << 0)) /* 0 = Disabled */
#define WM8960_3DEN_ENABLE ((uint16_t)(1 << 0)) /* 1 = Enabled */
#define WM8960_3DDEPTH_MIN ((uint16_t)(0 << 1)) /* 3D Stereo Depth */
#define WM8960_3DDEPTH_MAX ((uint16_t)(0xf << 1)) /* 6.67% progressive increase(range 0~100%)*/
#define WM8960_3DLC_LOW ((uint16_t)(0 << 5)) /* 0 = Low (Recommended for fs>=32kHz)  */
#define WM8960_3DLC_HIGH ((uint16_t)(1 << 5)) /* 1 = High (Recommended for fs<32kHz)  */
#define WM8960_3DUC_HIGH ((uint16_t)(0 << 6)) /* 0 = High (Recommended for fs>=32kHz)  */
#define WM8960_3DUC_LOW ((uint16_t)(1 << 6)) /* 1 = Low (Recommended for fs<32kHz)  */
/* ALC1 Register Value(wm8960_reg_r17 = 0x11) */
#define WM8960_ALC1_TARGET_MIN ((uint16_t)(0 << 0)) /* ALC Target (Sets signal level at ADC input) */
#define WM8960_ALC1_TARGET_MAX ((uint16_t)(0xf << 0))/* 1.5dB progressive increase(range -22.5dB~-1.5dB) */
#define WM8960_ALC1_MAXGAIN_MIN ((uint16_t)(0 << 4)) /* Set Maximum Gain of PGA (During ALC operation) */
#define WM8960_ALC1_MAXGAIN_MAX ((uint16_t)(7 << 4))/* 6dB progressive increase(range -12dB~+30dB) */
#define WM8960_ALC1_ALCSEL_OFF ((uint16_t)(0 << 7)) /* 00 = ALC off (PGA gain set by register)  */
#define WM8960_ALC1_ALCSEL_RIGHT ((uint16_t)(1 << 7))/* 01 = Right channel only */
#define WM8960_ALC1_ALCSEL_LEFR ((uint16_t)(2 << 7))/* 10 = Left channel only */
#define WM8960_ALC1_ALCSEL_STEREO ((uint16_t)(3 << 7))/* 11 = Stereo (PGA registers unused) Note: ensure that LINVOL and RINVOL settings (reg. 0 and 1) are the same before entering this mode.  */
/* ALC2 Register Value(wm8960_reg_r18 = 0x12) */
#define WM8960_ALC2_HLD_MIN ((uint16_t)(0 << 0)) /* ALC hold time before gain is increased. */
#define WM8960_ALC2_HLD_MAX ((uint16_t)(0xf << 0))/* range 0ms-2.67ms-5.33ms......-43.691s(time doubles with every step)  */
#define WM8960_ALC2_MINGAIN_N17_25 ((uint16_t)(0 << 4)) /* 000 = -17.25dB */
#define WM8960_ALC2_MINGAIN_N11_25 ((uint16_t)(1 << 4))/* 001 = -11.25dB */
#define WM8960_ALC2_MINGAIN_N5_25 ((uint16_t)(2 << 4))/* 010 = -5.25dB  */
#define WM8960_ALC2_MINGAIN_P0_75 ((uint16_t)(3 << 4))/* 011 = +0.75dB  */
#define WM8960_ALC2_MINGAIN_P6_75 ((uint16_t)(4 << 4))/* 100 = +6.75dB  */
#define WM8960_ALC2_MINGAIN_P12_75 ((uint16_t)(5 << 4))/* 101 = +12.75dB */
#define WM8960_ALC2_MINGAIN_P18_75 ((uint16_t)(6 << 4))/* 110 = +18.75dB */
#define WM8960_ALC2_MINGAIN_P24_75 ((uint16_t)(7 << 4))/* 111 = +24.75dB */
/* ALC3 Register Value(wm8960_reg_r19 = 0x13) */
#define WM8960_ALC3_ATK_MIN ((uint16_t)(0 << 0)) /* ALC attack (gain ramp-down) time   */
#define WM8960_ALC3_ATK_MAX ((uint16_t)(0xa << 0))/* range 6ms-12ms-24ms......6.14s(time doubles with every step)  */
#define WM8960_ALC3_DCY_MIN ((uint16_t)(0 << 4)) /* ALC decay (gain ramp-up) time  */
#define WM8960_ALC3_DCY_MAX ((uint16_t)(0xa << 4))/* range 24ms-48ms-96ms......24.58s(time doubles with every step)  */
#define WM8960_ALC3_ALCMODE_ALC ((uint16_t)(0 << 8)) /* 0 = ALC mode */
#define WM8960_ALC3_ALCMODE_LIMITER ((uint16_t)(2 << 8)) /* 1 = Limiter mode */
/* Noise Gate Register Value(wm8960_reg_r20 = 0x14) */
#define WM8960_NGAT_DISABLE ((uint16_t)(0 << 0)) /* 0 = disable */
#define WM8960_NGAT_ENABLE ((uint16_t)(1 << 0)) /* 1 = enable */
#define WM8960_NGTH_MIN ((uint16_t)(0 << 3))
#define WM8960_NGTH_MAX ((uint16_t)(0x1f << 3))/* 1.5dB progressive increase(range -76.5dB~-30dB) */
/* Left ADC volume Register Value(wm8960_reg_r21 = 0x15) */
#define WM8960_LADCVOL_DMUTE ((uint16_t)(0 << 0)) /* 0000 0000 = Digital Mute */
#define WM8960_LADCVOL_MIN ((uint16_t)(1 << 0)) /* Left ADC Digital Volume Control  */
#define WM8960_LADCVOL_MAX ((uint16_t)(0xff << 0))/* 0.5dB progressive increase(range -97dB~+30dB) */
#define WM8960_LADCVU ((uint16_t)(1 << 8)) /* ADC Volume Update Writing a 1 to this bit will cause left and right ADC volumes to be updated (LADCVOL and RADCVOL) */
/* Right ADC volume Register Value(wm8960_reg_r22 = 0x16) */
#define WM8960_RADCVOL_DMUTE ((uint16_t)(0 << 0)) /* 0000 0000 = Digital Mute */
#define WM8960_RADCVOL_MIN ((uint16_t)(1 << 0)) /* Right ADC Digital Volume Control  */
#define WM8960_RADCVOL_MAX ((uint16_t)(0xff << 0))/* 0.5dB progressive increase(range -97dB~+30dB) */
#define WM8960_RADCVU ((uint16_t)(1 << 8)) /* ADC Volume Update Writing a 1 to this bit will cause left and right ADC volumes to be updated (LADCVOL and RADCVOL) */
/* Additional control(1) Register Value(wm8960_reg_r23 = 0x17) */
#define WM8960_TOEN_DISABLE ((uint16_t)(0 << 0)) /* 0 = Slow clock disabled */
#define WM8960_TOEN_ENABLE ((uint16_t)(1 << 0)) /* 1 = Slow clock enabled */
#define WM8960_TOCLKSEL_SP ((uint16_t)(0 << 1)) /* 0 = SYSCLK / 2^21(Slower Response) */
#define WM8960_TOCLKSEL_FP  ((uint16_t)(1 << 1)) /* 1 = SYSCLK / 2^19(Faster Response) */
#define WM8960_DATSEL_0 ((uint16_t)(0 << 2)) /* 00: left data = left ADC; right data =right ADC  */
#define WM8960_DATSEL_1 ((uint16_t)(1 << 2))/* 01: left data = left ADC; right data = left ADC  */
#define WM8960_DATSEL_2 ((uint16_t)(2 << 2))/* 10: left data = right ADC; right data =right ADC  */
#define WM8960_DATSEL_3 ((uint16_t)(3 << 2))/* 11: left data = right ADC; right data = left ADC  */
#define WM8960_DMONOMIX_STEREO ((uint16_t)(0 << 4))/* 0 = Stereo */
#define WM8960_DMONOMIX_MONO ((uint16_t)(1 << 4)) /* 1 = Mono (Mono MIX output on enabled DACs) */
#define WM8960_VSEL_01 ((uint16_t)(1 << 6)) /* 01 = Increased bias current optimized for AVDD=2.7V */
#define WM8960_VSEL_1X ((uint16_t)(2 << 6)) /* 1X = Lowest bias current, optimized for AVDD=3.3V */
#define WM8960_TSDEN_DISABLE ((uint16_t)(0 << 8)) /* 0 = Thermal shutdown disabled */
#define WM8960_TSDEN_ENABLE ((uint16_t)(1 << 8)) /* 1 = Thermal shutdown enabled */
/* Additional control(2) Register Value(wm8960_reg_r24 = 0x18) */
#define WM8960_LRCM_0 ((uint16_t)(0 << 2)) /* 0 = ADCLRC disabled when ADC (Left and Right) disabled; DACLRC disabled when DAC (Left and Right) disabled. */
#define WM8960_LRCM_1  ((uint16_t)(1 << 2)) /* 1 = ADCLRC and DACLRC disabled only when ADC (Left and Right) and DAC (Left and Right) are disabled.  */
#define WM8960_TRIS_0 ((uint16_t)(0 << 3)) /* 0 = ADCDAT is an output; ADCLRC, DACLRC and BCLK are inputs (slave mode) or outputs (master mode) */
#define WM8960_TRIS_1 ((uint16_t)(1 << 3)) /* 1 = ADCDAT is tristated; DACLRC and BCLK are inputs; ADCLRC is an input (when not configured as a GPIO)  */
#define WM8960_HPSWPOL_HP  ((uint16_t)(0 << 5)) /* 0 = HPDETECT high = headphone */
#define WM8960_HPSWPOL_SPK ((uint16_t)(1 << 5))/* 1 = HPDETECT high = speaker */
#define WM8960_HPSWEN_DISABLE ((uint16_t)(0 << 6)) /* 0 = Headphone switch disabled  */
#define WM8960_HPSWEN_ENABLE ((uint16_t)(1 << 6))/* 1 = Headphone switch enabled */
/* Pwr Mgmt (1)Register Value(wm8960_reg_r25 = 0x19) */
#define WM8960_DIGENB_ML_ENABLE ((uint16_t)(0 << 0)) /* 0 = Master clock enabled */
#define WM8960_DIGENB_ML_DISABLE ((uint16_t)(1 << 0)) /* 1 = Master clock disabled */
#define WM8960_MICB_POWER_DOWN ((uint16_t)(0 << 1))/* 0 = Power down */
#define WM8960_MICB_POWER_UP ((uint16_t)(1 << 1)) /* 1 = Power up */
#define WM8960_ADCR_POWER_DOWN ((uint16_t)(0 << 2)) /* 0 = Power down */
#define WM8960_ADCR_POWER_UP ((uint16_t)(1 << 2)) /* 1 = Power up */
#define WM8960_ADCL_POWER_DOWN ((uint16_t)(0 << 3)) /* 0 = Power down */
#define WM8960_ADCL_POWER_UP ((uint16_t)(1 << 3)) /* 1 = Power up */
#define WM8960_AINR_POWER_DOWN ((uint16_t)(0 << 4)) /* 0 = Power down */
#define WM8960_AINR_POWER_UP ((uint16_t)(1 << 4)) /* 1 = Power up */
#define WM8960_AINL_POWER_DOWN ((uint16_t)(0 << 5)) /* 0 = Power down */
#define WM8960_AINL_POWER_UP ((uint16_t)(1 << 5)) /* 1 = Power up */
#define WM8960_VREF_POWER_DOWN ((uint16_t)(0 << 6)) /* 0 = Power down */
#define WM8960_VREF_POWER_UP ((uint16_t)(1 << 6)) /* 1 = Power up */
#define WM8960_VMIDSEL_0 ((uint16_t)(0 << 7)) /* 00 = Vmid disabled (for OFF mode) */
#define WM8960_VMIDSEL_1 ((uint16_t)(1 << 7)) /* 01 = 2 x 50kΩdivider enabled (for playback / record) */
#define WM8960_VMIDSEL_2 ((uint16_t)(2 << 7)) /* 10 = 2 x 250kΩdivider enabled (for low-power standby)  */
#define WM8960_VMIDSEL_3 ((uint16_t)(3 << 7)) /* 11 = 2 x 5kΩdivider enabled (for fast start-up) */
/* Pwr Mgmt (2)Register Value(wm8960_reg_r26 = 0x1a) */
#define WM8960_PLL_EN_POWER_DOWN ((uint16_t)(0 << 0)) /* 0 = Power down */
#define WM8960_PLL_EN_POWER_UP ((uint16_t)(1 << 0)) /* 1 = Power up */
#define WM8960_OUT3_POWER_DOWN ((uint16_t)(0 << 1)) /* 0 = Power down */
#define WM8960_OUT3_POWER_UP ((uint16_t)(1 << 1)) /* 1 = Power up */
#define WM8960_SPKR_POWER_DOWN ((uint16_t)(0 << 3)) /* 0 = Power down */
#define WM8960_SPKR_POWER_UP ((uint16_t)(1 << 3)) /* 1 = Power up */
#define WM8960_SPKL_POWER_DOWN ((uint16_t)(0 << 4)) /* 0 = Power down */
#define WM8960_SPKL_POWER_UP ((uint16_t)(1 << 4)) /* 1 = Power up */
#define WM8960_ROUT1_POWER_DOWN ((uint16_t)(0 << 5)) /* 0 = Power down */
#define WM8960_ROUT1_POWER_UP ((uint16_t)(1 << 5)) /* 1 = Power up */
#define WM8960_LOUT1_POWER_DOWN ((uint16_t)(0 << 6)) /* 0 = Power down */
#define WM8960_LOUT1_POWER_UP ((uint16_t)(1 << 6)) /* 1 = Power up */
#define WM8960_DACR_POWER_DOWN ((uint16_t)(0 << 7)) /* 0 = Power down */
#define WM8960_DACR_POWER_UP ((uint16_t)(1 << 7)) /* 1 = Power up */
#define WM8960_DACL_POWER_DOWN ((uint16_t)(0 << 8)) /* 0 = Power down */
#define WM8960_DACL_POWER_UP ((uint16_t)(1 << 8)) /* 1 = Power up */
/* Additional Control (3) Register Value(wm8960_reg_r27 = 0x1b) */
#define WM8960_ADC_ALC_SR_44_1_48K ((uint16_t)(0 << 0)) /* 000 = 44.1k / 48k */
#define WM8960_ADC_ALC_SR_32K ((uint16_t)(1 << 0)) /* 001 = 32k */
#define WM8960_ADC_ALC_SR_22_05_24K ((uint16_t)(2 << 0))/* 010 = 22.05k / 24k */
#define WM8960_ADC_ALC_SR_16K ((uint16_t)(3 << 0))/* 011 = 16k */
#define WM8960_ADC_ALC_SR_11_25_12K ((uint16_t)(4 << 0))/* 100 = 11.25k / 12k  */
#define WM8960_ADC_ALC_SR_8K ((uint16_t)(5 << 0))/* 101 = 8k */
#define WM8960_OUT3CAP_0 ((uint16_t)(0 << 3)) /* 0 = OUT3 unaffected by jack detect events */
#define WM8960_OUT3CAP_1 ((uint16_t)(1 << 3))/* 1 = OUT3 enabled and disabled together with HP_L and HP_R in response to jack detect events */
#define WM8960_VROI_500 ((uint16_t)(0 << 6)) /* 0 = 500ΩVMID to output */
#define WM8960_VROI_20K ((uint16_t)(1 << 6))/* 1 = 20kΩVMID to output */
/* Anti-pop 1 Register Value(wm8960_reg_r28 = 0x1c) */
#define WM8960_HPSTBY_DISABLE ((uint16_t)(0 << 0)) /* 0 = Standby mode disabled (Normal operation) */
#define WM8960_HPSTBY_ENABLE ((uint16_t)(1 << 0))/* 1 = Standby mode enabled */
#define WM8960_SOFT_ST_DISABLE ((uint16_t)(0 << 2)) /* 0 = Disabled */
#define WM8960_SOFT_ST_ENABLE  ((uint16_t)(1 << 2)) /* 1 = Enabled */
#define WM8960_BUFIOEN_DISABLE ((uint16_t)(0 << 3)) /* 0 = Disabled */
#define WM8960_BUFIOEN_ENABLE  ((uint16_t)(1 << 3)) /* 1 = Enabled */
#define WM8960_BUFDCOPEN_DISABLE ((uint16_t)(0 << 4)) /* 0 = Disabled */
#define WM8960_BUFDCOPEN_ENABLE  ((uint16_t)(1 << 4)) /* 1 = Enabled */
#define WM8960_POBCTRL_0 ((uint16_t)(0 << 7)) /* 0 = VMID / R bias */
#define WM8960_POBCTRL_1  ((uint16_t)(1 << 7)) /* 1 = VGS / R bias */
/* Anti-pop 2 Register Value(wm8960_reg_r29 = 0x1d) */
#define WM8960_DRES_400 ((uint16_t)(0 << 4)) /* 400ms Resistance (Ohms) */
#define WM8960_DRES_200 ((uint16_t)(1 << 4))/* 200ms Resistance (Ohms) */
#define WM8960_DRES_600 ((uint16_t)(2 << 4))/* 600ms Resistance (Ohms) */
#define WM8960_DRES_150 ((uint16_t)(3 << 4))/* 150ms Resistance (Ohms) */
#define WM8960_DISOP_DISABLE ((uint16_t)(0 << 6)) /* 0 = Disabled */
#define WM8960_DISOP_ENABLE  ((uint16_t)(1 << 6)) /* 1 = Enabled */
/* ADCL signal path Register Value(wm8960_reg_r32 = 0x20) */
#define WM8960_LMIC2B_NC ((uint16_t)(0 << 3)) /* 0 = Not connected */
#define WM8960_LMIC2B_C ((uint16_t)(1 << 3))/* 1 = Connected  */
#define WM8960_LMICBOOST_0DB ((uint16_t)(0 << 4))/* 00 = +0dB */
#define WM8960_LMICBOOST_13DB ((uint16_t)(1 << 4))/* 01 = +13dB */
#define WM8960_LMICBOOST_20DB ((uint16_t)(2 << 4))/* 10 = +20dB  */
#define WM8960_LMICBOOST_29DB ((uint16_t)(3 << 4))/* 11 = +29dB */
#define WM8960_LMP2_NC ((uint16_t)(0 << 6))/* 0 = LINPUT2 not connected to PGA */
#define WM8960_LMP2_C ((uint16_t)(1 << 6))/* 1 = LINPUT2 connected to PGA (Constant input impedance)  */
#define WM8960_LMP3_NC ((uint16_t)(0 << 7))/* 0 = LINPUT3 not connected to PGA */
#define WM8960_LMP3_C ((uint16_t)(1 << 7))/* 1 = LINPUT3 connected to PGA (Constant input impedance)  */
#define WM8960_LMN1_NC ((uint16_t)(0 << 8))/* 0 = LINPUT1 not connected to PGA */
#define WM8960_LMN1_C ((uint16_t)(1 << 8))/* 1 = LINPUT1 connected to PGA */
/* ADCR signal path Register Value(wm8960_reg_r33 = 0x21) */
#define WM8960_RMIC2B_NC ((uint16_t)(0 << 3)) /* 0 = Not connected */
#define WM8960_RMIC2B_C ((uint16_t)(1 << 3))/* 1 = Connected  */
#define WM8960_RMICBOOST_0DB ((uint16_t)(0 << 4))/* 00 = +0dB */
#define WM8960_RMICBOOST_13DB ((uint16_t)(1 << 4))/* 01 = +13dB */
#define WM8960_RMICBOOST_20DB ((uint16_t)(2 << 4))/* 10 = +20dB  */
#define WM8960_RMICBOOST_29DB ((uint16_t)(3 << 4))/* 11 = +29dB */
#define WM8960_RMP2_NC ((uint16_t)(0 << 6))/* 0 = RINPUT2 not connected to PGA */
#define WM8960_RMP2_C ((uint16_t)(1 << 6))/* 1 = RINPUT2 connected to PGA (Constant input impedance)  */
#define WM8960_RMP3_NC ((uint16_t)(0 << 7))/* 0 = RINPUT3 not connected to PGA */
#define WM8960_RMP3_C ((uint16_t)(1 << 7))/* 1 = RINPUT3 connected to PGA (Constant input impedance)  */
#define WM8960_RMN1_NC ((uint16_t)(0 << 8))/* 0 = RINPUT1 not connected to PGA */
#define WM8960_RMN1_C ((uint16_t)(1 << 8))/* 1 = RINPUT1 connected to PGA */
/* Left out Mix (1) Register Value(wm8960_reg_r34 = 0x22) */
#define WM8960_LI2LOVOL_MIN ((uint16_t)(7 << 4)) /* LINPUT3 to Left Output Mixer Volume  */
#define WM8960_LI2LOVOL_MAX ((uint16_t)(0 << 4))/* 3dB progressive increase(range -21dB~0dB) */
#define WM8960_LI2LO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Disable (Mute) */
#define WM8960_LI2LO_ENABLE ((uint16_t)(1 << 7)) /* 1 = Enable Path  */
#define WM8960_LD2LO_DISABLE ((uint16_t)(0 << 8)) /* 0 = Disable (Mute) */
#define WM8960_LD2LO_ENABLE ((uint16_t)(1 << 8)) /* 1 = Enable Path  */
/* Right out Mix (2) Register Value(wm8960_reg_r37 = 0x25) */
#define WM8960_RI2ROVO_MIN ((uint16_t)(7 << 4)) /* RINPUT3 to Right Output Mixer Volume  */
#define WM8960_RI2ROVO_MAX ((uint16_t)(0 << 4))/* 3dB progressive increase(range -21dB~0dB) */
#define WM8960_RI2RO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Disable (Mute) */
#define WM8960_RI2RO_ENABLE ((uint16_t)(1 << 7)) /* 1 = Enable Path  */
#define WM8960_RD2RO_DISABLE ((uint16_t)(0 << 8)) /* 0 = Disable (Mute) */
#define WM8960_RD2RO_ENABLE ((uint16_t)(1 << 8)) /* 1 = Enable Path  */
/* Mono out Mix (1) Register Value(wm8960_reg_r38 = 0x26) */
#define WM8960_L2MO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Left channel mix disabled */
#define WM8960_L2MO_ENABLE  ((uint16_t)(1 << 7)) /* 1 = Left channel mix enabled  */
/* Mono out Mix (2) Register Value(wm8960_reg_r39 = 0x27) */
#define WM8960_R2MO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Right channel mix disabled */
#define WM8960_R2MO_ENABLE  ((uint16_t)(1 << 7)) /* 1 = Right channel mix enabled  */
/* LOUT2 volume Register Value(wm8960_reg_r40 = 0x28) */
#define WM8960_SPKLVOL_MIN ((uint16_t)(0x60 << 0)) /* SPK_LP/SPK_LN Volume */
#define WM8960_SPKLVOL_MAX ((uint16_t)(0x7f << 0)) /* 1dB progressive increase(range -73dB~+6dB) */
#define WM8960_SPKLZC_CZ ((uint16_t)(1 << 7)) /* 1 = Change gain on zero cross only */
#define WM8960_SPKLZC_CM ((uint16_t)(0 << 7)) /* 0 = Change gain immediately */
#define WM8960_SPKLVU_UPDATE  ((uint16_t)(1 << 8))/* Speaker Volume Update Writing a 1 to this bit will cause left and right speaker volumes to be updated (SPKLVOL and SPKRVOL)  */
/* ROUT2 volume Register Value(wm8960_reg_r41 = 0x29) */
#define WM8960_SPKRVOL_MIN ((uint16_t)(0x60 << 0)) /* SPK_LP/SPK_LN Volume */
#define WM8960_SPKRVOL_MAX ((uint16_t)(0x7f << 0)) /* 1dB progressive increase(range -73dB~+6dB) */
#define WM8960_SPKRZC_CZ ((uint16_t)(1 << 7)) /* 1 = Change gain on zero cross only */
#define WM8960_SPKRZC_CM ((uint16_t)(0 << 7)) /* 0 = Change gain immediately */
#define WM8960_SPKRVU_UPDATE  ((uint16_t)(1 << 8))/* Speaker Volume Update Writing a 1 to this bit will cause left and right speaker volumes to be updated (SPKLVOL and SPKRVOL)  */

/* MONOOUT volume Register Value(wm8960_reg_r42 = 0x2a) */
#define WM8960_MOUTVOL_0DB ((uint16_t)(0 << 6))/* 0 = 0dB */
#define WM8960_MOUTVOL_N1DB ((uint16_t)(1 << 6))/* 1 = -6dB  */
/* Input boost mixer (1) Register Value(wm8960_reg_r43 = 0x2b) */
#define WM8960_LIN2BOOST_MUTE ((uint16_t)(0 << 1))/* 000 = Mute */
#define WM8960_LIN2BOOST_MIN ((uint16_t)(1 << 1))/* LINPUT2 to Boost Mixer Gain */
#define WM8960_LIN2BOOST_MAX ((uint16_t)(7 << 1))/* 3dB progressive increase(range -12dB~+3dB) */
#define WM8960_LIN3BOOST_MUTE ((uint16_t)(0 << 4))/* 000 = Mute */
#define WM8960_LIN3BOOST_MIN ((uint16_t)(1 << 4))/* LINPUT3 to Boost Mixer Gain */
#define WM8960_LIN3BOOST_MAX ((uint16_t)(7 << 4))/* 3dB progressive increase(range -12dB~+3dB) */
/* Input boost mixer (2) Register Value(wm8960_reg_r44 = 0x2c) */
#define WM8960_RIN2BOOST_MUTE ((uint16_t)(0 << 1))/* 000 = Mute */
#define WM8960_RIN2BOOST_MIN ((uint16_t)(1 << 1))/* RINPUT2 to Boost Mixer Gain */
#define WM8960_RIN2BOOST_MAX ((uint16_t)(7 << 1))/* 3dB progressive increase(range -12dB~+3dB) */
#define WM8960_RIN3BOOST_MUTE ((uint16_t)(0 << 4))/* 000 = Mute */
#define WM8960_RIN3BOOST_MIN ((uint16_t)(1 << 4))/* RINPUT3 to Boost Mixer Gain */
#define WM8960_RIN3BOOST_MAX ((uint16_t)(7 << 4))/* 3dB progressive increase(range -12dB~+3dB) */
/* Bypass (1) Register Value(wm8960_reg_r45 = 0x2d) */
#define WM8960_LB2LOVOL_MIN ((uint16_t)(7 << 4)) /* Left Input Boost Mixer to Left Output Mixer Volume */
#define WM8960_LB2LOVOL_MAX ((uint16_t)(0 << 4))/* 3dB progressive increase(range -21dB~0dB) */
#define WM8960_LB2LO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Disable (Mute)  */
#define WM8960_LB2LO_ENABLE  ((uint16_t)(1 << 7)) /* 1 = Enable Path */
/* Bypass (2) Register Value(wm8960_reg_r46 = 0x2e) */
#define WM8960_RB2ROVOL_MIN ((uint16_t)(7 << 4)) /* Right Input Boost Mixer to Left Output Mixer Volume */
#define WM8960_RB2ROVOL_MAX ((uint16_t)(0 << 4))/* 3dB progressive increase(range -21dB~0dB) */
#define WM8960_RB2RO_DISABLE ((uint16_t)(0 << 7)) /* 0 = Disable (Mute)  */
#define WM8960_RB2RO_ENABLE  ((uint16_t)(1 << 7)) /* 1 = Enable Path */
/* Pwr Mgmt (3) Register Value(wm8960_reg_r47 = 0x2f) */
#define WM8960_ROMIX_DISABLE ((uint16_t)(0 << 2))/* 0 = Disabled */
#define WM8960_ROMIX_ENABLE ((uint16_t)(1 << 2))/* 1 = Enabled */
#define WM8960_LOMIX_DISABLE ((uint16_t)(0 << 3))/* 0 = Disabled */
#define WM8960_LOMIX_ENABLE ((uint16_t)(1 << 3))/* 1 = Enabled */
#define WM8960_RMIC_DISABLE ((uint16_t)(0 << 4))/* 0 = PGA disabled  */
#define WM8960_RMIC_ENABLE ((uint16_t)(1 << 4))/* 1 = PGA enabled (if AINR = 1)  */
#define WM8960_LMIC_DISABLE ((uint16_t)(0 << 5))/* 0 = PGA disabled  */
#define WM8960_LMIC_ENABLE ((uint16_t)(1 << 5))/* 1 = PGA enabled (if AINR = 1)  */
/* Additional Control (4) Register Value(wm8960_reg_r48 = 0x30) */
#define WM8960_MBSE_0_9AVDD ((uint16_t)(0 << 2))/* 0 = 0.9 * AVDD */
#define WM8960_MBSE_0_65AVDD ((uint16_t)(1 << 2))/* 1 = 0.65 * AVDD */
#define WM8960_TSENSEN_DISABLE ((uint16_t)(0 << 1)) /* 0 = Temperature sensor disabled */
#define WM8960_TSENSEN_ENABLE ((uint16_t)(1 << 1)) /* 1 = Temperature sensor enabled  */
#define WM8960_HPSEL_GPIO1 ((uint16_t)(0 << 2))/* 0X = GPIO1 used for jack detect input (Requires ADCLRC pin to be configured as a GPIO)  */
#define WM8960_HPSEL_JD2 ((uint16_t)(2 << 2))/* 10 = JD2 used for jack detect input */
#define WM8960_HPSEL_JD3 ((uint16_t)(3 << 2))/* 11 = JD3 used for jack detect input  */
#define WM8960_GPIOSEL_0 ((uint16_t)(0 << 4))/* 000 = Jack detect input  */
#define WM8960_GPIOSEL_2 ((uint16_t)(2 << 4))/* 010 = Temperature ok */
#define WM8960_GPIOSEL_3 ((uint16_t)(3 << 4))/* 011 = Debounced jack detect output */
#define WM8960_GPIOSEL_4 ((uint16_t)(4 << 4))/* 100 = SYSCLK output */
#define WM8960_GPIOSEL_5 ((uint16_t)(5 << 4))/* 101 = PLL lock */
#define WM8960_GPIOSEL_6 ((uint16_t)(6 << 4))/* 110 = Logic 0 */
#define WM8960_GPIOSEL_7 ((uint16_t)(7 << 4))/* 111 = Logic 1 */
#define WM8960_GPIOPOL_NI ((uint16_t)(0 << 7)) /* 0 = Non inverted */
#define WM8960_GPIOPOL_I ((uint16_t)(1 << 7)) /* 1 = Inverted  */
/* Class D Control (1) Register Value(wm8960_reg_r49 = 0x31) */
#define WM8960_SPK_OP_EN_OFF ((uint16_t)(0 << 6)) /* 00 = Off */
#define WM8960_SPK_OP_EN_R ((uint16_t)(1 << 6))/* 01 = Left speaker only */
#define WM8960_SPK_OP_EN_L ((uint16_t)(2 << 6))/* 10 = Right speaker only */
#define WM8960_SPK_OP_EN_LR ((uint16_t)(3 << 6))/* 11 = Left and right speakers enabled  */
/* Class D Control (3) Register Value(wm8960_reg_r51 = 0x33) */
#define WM8960_ACGAIN_1_00 ((uint16_t)(0 << 0))/* 000 = 1.00x boost (+0dB) */
#define WM8960_ACGAIN_1_27 ((uint16_t)(1 << 0)) /* 001 = 1.27x boost (+2.1dB) */
#define WM8960_ACGAIN_1_40  ((uint16_t)2 << 0))/* 010 = 1.40x boost (+2.9dB)  */
#define WM8960_ACGAIN_1_52 ((uint16_t)(3 << 0)) /* 011 = 1.52x boost (+3.6dB) */
#define WM8960_ACGAIN_1_67  ((uint16_t)(4 << 0))/* 100 = 1.67x boost (+4.5dB) */
#define WM8960_ACGAIN_1_80  ((uint16_t)(5 << 0))/* 101 = 1.8x boost (+5.1dB) */
#define WM8960_DCGAIN_1_00 ((uint16_t)(0 << 3))/* 000 = 1.00x boost (+0dB) */
#define WM8960_DCGAIN_1_27 ((uint16_t)(1 << 3)) /* 001 = 1.27x boost (+2.1dB) */
#define WM8960_DCGAIN_1_40  ((uint16_t)2 << 3))/* 010 = 1.40x boost (+2.9dB)  */
#define WM8960_DCGAIN_1_52 ((uint16_t)(3 << 3)) /* 011 = 1.52x boost (+3.6dB) */
#define WM8960_DCGAIN_1_67  ((uint16_t)(4 << 3))/* 100 = 1.67x boost (+4.5dB) */
#define WM8960_DCGAIN_1_80  ((uint16_t)(5 << 3))/* 101 = 1.8x boost (+5.1dB) */
/* PLL N Register Value(wm8960_reg_r52 = 0x34) */
#define WM8960_PLLN_MIN ((uint16_t)(5 << 0))/* Integer (N) part of PLL input/output frequency ratio. Use values greater than 5 and less than 13. */
#define WM8960_PLLN_MAX ((uint16_t)(13 << 0))/* Integer (N) part of PLL input/output frequency ratio. Use values greater than 5 and less than 13. */
#define WM8960_PLLPRESCALE_1 ((uint16_t)(0 << 4)) /* 0 = Divide by 1 */
#define WM8960_PLLPRESCALE_2 ((uint16_t)(1 << 4)) /* 1 = Divide by 2  */
#define WM8960_SDM_IM ((uint16_t)(0 << 5)) /* 0 = Integer mode */
#define WM8960_SDM_FM ((uint16_t)(0 << 5)) /* 1 = Fractional mode */
#define WM8960_SYSCLK_1_0 ((uint16_t)(0 << 6)) /* 000 = SYSCLK  */
#define WM8960_SYSCLK_2_0 ((uint16_t)(1 << 6)) /* 001 = SYSCLK / 2  */
#define WM8960_SYSCLK_3_0 ((uint16_t)(2 << 6)) /* 010 = SYSCLK / 3 */
#define WM8960_SYSCLK_4_0 ((uint16_t)(3 << 6)) /* 011 = SYSCLK / 4 */
#define WM8960_SYSCLK_5_5 ((uint16_t)(4 << 6)) /* 100 = SYSCLK / 5.5 */
#define WM8960_SYSCLK_6_0 ((uint16_t)(5 << 6)) /* 101 = SYSCLK / 6  */
/* PLL K 1 Register Value(wm8960_reg_r53 = 0x35) */
/* PLL K 2 Register Value(wm8960_reg_r54 = 0x36) */
/* PLL K 3 Register Value(wm8960_reg_r55 = 0x37) */



typedef enum
{
    wm8960_reg_r0 = 0x00,   /* Left Input volume */
    wm8960_reg_r1 = 0x01,   /* Right Input volume */
    wm8960_reg_r2 = 0x02,   /* LOUT1 volume */
    wm8960_reg_r3 = 0x03,   /* ROUT1 volume */
    wm8960_reg_r4 = 0x04,   /* Clocking (1) */
    wm8960_reg_r5 = 0x05,   /* ADC & DAC Control (CTR1) */
    wm8960_reg_r6 = 0x06,   /* ADC & DAC Control (CTR2) */
    wm8960_reg_r7 = 0x07,   /* Audio Interface */
    wm8960_reg_r8 = 0x08,   /* Clocking (2) */
    wm8960_reg_r9 = 0x09,   /* Audio Interface */
    wm8960_reg_r10 = 0x0a,  /* Left DAC volume */
    wm8960_reg_r11 = 0x0b,  /* Right DAC volume */
    wm8960_reg_r12 = 0x0c,  /* Reserved */
    wm8960_reg_r13 = 0x0d,  /* Reserved */
    wm8960_reg_r14 = 0x0e,  /* Reserved */
    wm8960_reg_r15 = 0x0f,  /* Reset */
    wm8960_reg_r16 = 0x10,  /* 3D control */
    wm8960_reg_r17 = 0x11,  /* ALC1 */
    wm8960_reg_r18 = 0x12,  /* ALC2 */
    wm8960_reg_r19 = 0x13,  /* ALC3 */
    wm8960_reg_r20 = 0x14,  /* Noise Gate */
    wm8960_reg_r21 = 0x15,  /* Left ADC volume */
    wm8960_reg_r22 = 0x16,  /* Right ADC volume */
    wm8960_reg_r23 = 0x17,  /* Additional control(1) */
    wm8960_reg_r24 = 0x18,  /* Additional control(2) */
    wm8960_reg_r25 = 0x19,  /* Pwr Mgmt (1) */
    wm8960_reg_r26 = 0x1a,  /* Pwr Mgmt (2) */
    wm8960_reg_r27 = 0x1b,  /* Additional Control (3) */
    wm8960_reg_r28 = 0x1c,  /* Anti-pop 1 */
    wm8960_reg_r29 = 0x1d,  /* Anti-pop 2 */
    wm8960_reg_r30 = 0x1e,  /* Reserved */
    wm8960_reg_r31 = 0x1f,  /* Reserved */
    wm8960_reg_r32 = 0x20,  /* ADCL signal path  */
    wm8960_reg_r33 = 0x21,  /* ADCR signal path */
    wm8960_reg_r34 = 0x22,  /* Left out Mix (1) */
    wm8960_reg_r35 = 0x23,  /* Reserved */
    wm8960_reg_r36 = 0x24,  /* Reserved */
    wm8960_reg_r37 = 0x25,  /* Right out Mix (2) */
    wm8960_reg_r38 = 0x26,  /* Mono out Mix (1) */
    wm8960_reg_r39 = 0x27,  /* Mono out Mix (2) */
    wm8960_reg_r40 = 0x28,  /* LOUT2 volume */
    wm8960_reg_r41 = 0x29,  /* ROUT2 volume */
    wm8960_reg_r42 = 0x2a,  /* MONOOUT volume */
    wm8960_reg_r43 = 0x2b,  /* Input boost mixer (1) */
    wm8960_reg_r44 = 0x2c,  /* Input boost mixer (2) */
    wm8960_reg_r45 = 0x2d,  /* Bypass (1) */
    wm8960_reg_r46 = 0x2e,  /* Bypass (2) */
    wm8960_reg_r47 = 0x2f,  /* Pwr Mgmt (3) */
    wm8960_reg_r48 = 0x30,  /* Additional Control (4) */
    wm8960_reg_r49 = 0x31,  /* Class D Control (1) */
    wm8960_reg_r50 = 0x32,  /* Reserved */
    wm8960_reg_r51 = 0x33,  /* Class D Control (3) */
    wm8960_reg_r52 = 0x34,  /* PLL N */
    wm8960_reg_r53 = 0x35,  /* PLL K 1 */
    wm8960_reg_r54 = 0x36,  /* PLL K 2 */
    wm8960_reg_r55 = 0x37,  /* PLL K 3 */
} wm8960_reg_index_e;

/* wav header */
#define PLAYER_PACK_END	__attribute__ ((packed))

#define RIFF_CHUNK_ID 0x46464952
#define RIFF_CHUNK_FORMAT_WAV 0x45564157
#define FORMAT_CHUNK_ID 0x20746D66
#define DATA_CHUNK_ID 0x61746164



typedef struct
{
    uint32_t riff_chunk_id;		   	/* riff chunk id;这里固定为"RIFF",即0X46464952 */
    uint32_t riff_chunk_size ;		/* 集合大小;文件总大小-8 */
    uint32_t riff_format;	   		/* 格式;WAVE,即0X45564157 */

    uint32_t fmt_chunk_id;		   	/* format chunk id;这里固定为"fmt ",即0X20746D66 */
    uint32_t fmt_chunk_size ;		   	/* 子集合大小(不包括ID和Size);这里为:20.*/
    uint16_t fmt_audio_format;	  	/* 音频格式;0X01,表示线性PCM;0X11表示IMA ADPCM */
    uint16_t fmt_channel_number;		/* 通道数量;1,表示单声道;2,表示双声道; */
    uint32_t fmt_sample_rate;			/* 采样率;0X1F40,表示8Khz */
    uint32_t fmt_byte_rate;			/* 字节速率;sample_rate * channel_number * bits_per_sample / 8  */
    uint16_t fmt_block_align;			/* 块对齐(字节) channel_number * bits_per_sample / 8 */
    uint16_t fmt_bits_per_sample;		/* 单个采样数据大小 8：8bit，16：16bit，32：32bit */
    /* uint16_t fmt_byte_extra;		附加的数据字节;2个; 线性PCM,没有这个参数 */

    uint32_t data_chunk_id;		   	/* data chunk id;这里固定为"data",即0x61746164 */
    uint32_t data_chunk_size ;		   	/* 子集合大小(不包括ID和Size) */

} PLAYER_PACK_END wave_header_t;

typedef struct
{
    uint32_t sample_rate; /* 采样率;0X1F40,表示8Khz */
    uint16_t channel_number; /* 通道数量;1,表示单声道;2,表示双声道; */
    uint16_t block_align;	/* 块对齐(字节) channel_number * bits_per_sample / 8 */
    uint32_t data_size; /* wav的音频长度 */
    uint32_t totol_sec;/* 整个wav的长度(s) */
    uint32_t current_sec;/* 目前数据的定位(s) */
    uint32_t byte_rate;			/* 字节速率;sample_rate * channel_number * bits_per_sample / 8  */
    uint16_t bits_per_sample;		/* 单个采样数据大小 8：8bit，16：16bit，32：32bit */
	uint8_t *music_buffer1;
	uint32_t music_buffer1_len;
	uint8_t *music_buffer2;
	uint32_t music_buffer2_len;
	uint8_t current_play_buffer; /* 1->music_buffer1  2->music_buffer2 */
} wave_info_t;


uint8_t hw_wm8960_init(void);
uint8_t hw_wm8960_play_wav(uint8_t *path);
uint8_t wm8960_stop_play(void);

#endif
