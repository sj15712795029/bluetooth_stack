/******************************************************************************
  * @file    hw_wm8960.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-05-28
  * @brief   wm8960 source file
******************************************************************************/

#include "hw_wm8960.h"
#include "fs_wrapper.h"

static GPIO_InitTypeDef  IIC_GPIO_InitStructure;

static uint8_t wm8960_write_reg(uint8_t reg,uint16_t val);
static void wm8960_protocol_iic_write_byte(uint8_t byte);
static void wm8960_protocol_iic_sda_output(void);
static void wm8960_protocol_iic_sda_input(void);
static void wm8960_protocol_iic_start(void);
static uint8_t wm8960_protocol_iic_wait_ack(void);
static void wm8960_protocol_iic_stop(void);
static uint8_t sw_wav_decoder(uint8_t *data,wave_info_t *wave_info);
static uint8_t wm8960_protocol_iis_init(uint32_t freq,uint16_t format,uint16_t mode,uint16_t standard);
static uint8_t wm8960_protocol_iis_play_dma_init(uint8_t *data_buffer,uint32_t buffer_len);
static uint8_t wm8960_start_play(void);

wave_info_t wave_play_info;

#define MUSIC_8_16K
#define MUSIC_44_1K

uint8_t hw_wm8960_init(void)
{
    GPIO_InitTypeDef  IIS_GPIO_InitStructure;
    /* I2C init */
    RCC_APB2PeriphClockCmd(WM8960_IIC_PERIPH_CLK, ENABLE);
    IIC_GPIO_InitStructure.GPIO_Pin = WM8960_SCL_PIN|WM8960_SDA_PIN;
    IIC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    IIC_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WM8960_SCL_GPIO, &IIC_GPIO_InitStructure);

    /* I2S init */
    RCC_APB2PeriphClockCmd(WM8960_IIS_PERIPH_CLK_1 | WM8960_IIS_PERIPH_CLK_2, ENABLE);
    IIS_GPIO_InitStructure.GPIO_Pin = WM8960_I2S2_WS_PIN|WM8960_I2S2_CK_PIN | WM8960_I2S2_SD_PIN;
    IIS_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    IIS_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WM8960_I2S2_WS_GPIO, &IIS_GPIO_InitStructure);

    IIS_GPIO_InitStructure.GPIO_Pin = WM8960_I2S2_MCLK_PIN;
    IIS_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    IIS_GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WM8960_I2S2_MCLK_GPIO, &IIS_GPIO_InitStructure);

    /* Reset */
    wm8960_write_reg(wm8960_reg_r15,0x0000);
    HW_DEBUG("WM8960 Reset complete\n");
    hw_delay_ms(250);

    /* Power */
    wm8960_write_reg(wm8960_reg_r25,WM8960_VREF_POWER_UP |WM8960_VMIDSEL_3); /* TODO:VREF?VMIDSEL?*/
    wm8960_write_reg(wm8960_reg_r26,WM8960_SPKR_POWER_UP | WM8960_SPKL_POWER_UP |
                     WM8960_ROUT1_POWER_UP | WM8960_LOUT1_POWER_UP |
                     WM8960_OUT3_POWER_UP |
                     WM8960_DACR_POWER_UP |WM8960_DACL_POWER_UP); /* TODO:?WM8960_LOUT1_POWER_UP */
    wm8960_write_reg(wm8960_reg_r47,WM8960_LOMIX_ENABLE | WM8960_ROMIX_ENABLE);

    /* Clock */
    wm8960_write_reg(wm8960_reg_r4,0);
    //wm8960_write_reg(wm8960_reg_r4,WM8960_DACDIV_6_0 | WM8960_ADCDIV_6_0);


    /* ADC & DAC control */
    wm8960_write_reg(wm8960_reg_r5, 0x0000);

    /* Audio interface */
    wm8960_write_reg(wm8960_reg_r7, WM8960_FORMAT_I2S);

    /* Set LOUT1 & ROUT1 valume */
    wm8960_write_reg(wm8960_reg_r2, 0x006F | WM8960_LIPVU_UPDATE);
    wm8960_write_reg(wm8960_reg_r3, 0x006F | WM8960_RIPVU_UPDATE);

    /* Set LOUT2 & ROUT2 valume */
    wm8960_write_reg(wm8960_reg_r40, 0x006F | WM8960_SPKLVU_UPDATE);
    wm8960_write_reg(wm8960_reg_r41, 0x006F | WM8960_SPKRVU_UPDATE);

    /* Class D control */
    wm8960_write_reg(wm8960_reg_r49, WM8960_SPK_OP_EN_LR);

    /* Configure Left/Right DAC volume */
    wm8960_write_reg(wm8960_reg_r10, 0x001F | WM8960_LDACVU_UPDATE);
    wm8960_write_reg(wm8960_reg_r11, 0x001F | WM8960_RDACVU_UPDATE);


    /* Config Left/Right Mixer */
    wm8960_write_reg(wm8960_reg_r34, WM8960_LI2LO_ENABLE |WM8960_LD2LO_ENABLE);
    wm8960_write_reg(wm8960_reg_r37, WM8960_RI2RO_ENABLE | WM8960_RD2RO_ENABLE);

    /* Config Additional Control */
    wm8960_write_reg(wm8960_reg_r24, WM8960_HPSWEN_ENABLE | WM8960_HPSWPOL_HP);
    wm8960_write_reg(wm8960_reg_r23, 0x01C3);
    wm8960_write_reg(wm8960_reg_r48, 0x0009);


    return HW_ERR_OK;
}

uint8_t music_buffer1[10*1024] = {0};
uint8_t music_buffer2[10*1024] = {0};
FIL play_file;
uint8_t hw_wm8960_play_wav(uint8_t *path)
{

    FRESULT fr;
    wave_header_t wave_header;
    UINT read_len = 0;
    HW_DEBUG("hw_wm8960_play_wav\n");

    fr = f_open(&play_file, (const TCHAR*)path, FA_READ);
    if (fr == FR_OK)
    {
        HW_DEBUG("open success\n");
        f_read(&play_file,(uint8_t *)&wave_header,sizeof(wave_header_t),&read_len);
        HW_DEBUG("read len %d\n",read_len);
        f_read(&play_file,music_buffer1,sizeof(music_buffer1),&read_len);
        wave_play_info.music_buffer1 = music_buffer1;
        wave_play_info.music_buffer1_len = read_len;
        HW_DEBUG("read len %d\n",read_len);
        f_read(&play_file,music_buffer2,sizeof(music_buffer2),&read_len);
        wave_play_info.music_buffer2 = music_buffer2;
        wave_play_info.music_buffer2_len = read_len;
        HW_DEBUG("read len %d\n",read_len);


        sw_wav_decoder((uint8_t *)&wave_header,&wave_play_info);

        wm8960_protocol_iis_init(wave_play_info.sample_rate,I2S_DataFormat_16b,I2S_Mode_MasterTx,I2S_Standard_Phillips);

        wm8960_protocol_iis_play_dma_init(wave_play_info.music_buffer1,wave_play_info.music_buffer1_len/wave_play_info.channel_number);

        wave_play_info.current_play_buffer = 1;

        wm8960_start_play();


    }
    else
        HW_DEBUG("open %s fail\n",path);
		
		return HW_ERR_OK;
}

static uint8_t wm8960_protocol_iis_init(uint32_t freq,uint16_t format,uint16_t mode,uint16_t standard)
{
    I2S_InitTypeDef I2S_InitStructure;

    SPI_I2S_DeInit(SPI2);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
    I2S_InitStructure.I2S_AudioFreq = freq;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_InitStructure.I2S_DataFormat = format;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;;
    I2S_InitStructure.I2S_Mode = mode;
    I2S_InitStructure.I2S_Standard = standard;
    I2S_Init(SPI2,&I2S_InitStructure);
    I2S_Cmd(SPI2, ENABLE);

    return HW_ERR_OK;
}

DMA_InitTypeDef  DMA_InitStructure;
static uint8_t wm8960_protocol_iis_play_dma_init(uint8_t *data_buffer,uint32_t buffer_len)
{

    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)data_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = buffer_len;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    if(wave_play_info.channel_number == 1)
    {
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    }
    else if(wave_play_info.channel_number == 2)
    {
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    }

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);



    DMA_ClearFlag(DMA1_FLAG_TC5);
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=MUSIC_PREE_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = MUSIC_SUB_PRIO;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    return HW_ERR_OK;
}

uint8_t wm8960_bypass_init()
{
    wm8960_write_reg(wm8960_reg_r15,0x0000);
    hw_delay_ms(250);

    wm8960_write_reg(wm8960_reg_r25,WM8960_AINR_POWER_UP |WM8960_AINL_POWER_UP | WM8960_VREF_POWER_UP |WM8960_VMIDSEL_1);
    wm8960_write_reg(0x1a,0x18);
    hw_delay_ms(250);
    wm8960_write_reg(0x20,0x0);
    wm8960_write_reg(0x21,0);
    wm8960_write_reg(0x2f,0xc);
    wm8960_write_reg(0x22,0x80);
    wm8960_write_reg(0x25,0x80);
    wm8960_write_reg(0x31,0xf7);
    wm8960_write_reg(0x33,0x9b);
    wm8960_write_reg(0x28,0x179);
    wm8960_write_reg(0x29,0x179);
	
	return HW_ERR_OK;

}

static uint8_t wm8960_start_play()
{
    //wm8960_protocol_iis_init();
    //wm8960_protocol_iis_play_dma_init();

    //SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
    return HW_ERR_OK;
}

uint8_t wm8960_stop_play()
{

		SPI_I2S_DeInit(SPI2);
	DMA_DeInit(DMA1_Channel5);
		hw_memset(&wave_play_info,0,sizeof(wave_info_t));

	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,DISABLE);
    SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    return HW_ERR_OK;
}


static uint8_t wm8960_write_reg(uint8_t reg,uint16_t val)
{
    uint8_t control_byte[2] = {0};
    control_byte[0] = (reg<<1)|((uint8_t)((val>>8)&0x0001));
    control_byte[1] = (uint8_t)(val&0x00ff);

    wm8960_protocol_iic_start();
    wm8960_protocol_iic_write_byte(WM8960_IIC_ADDR | WM8960_IIC_WRITE);
    if(wm8960_protocol_iic_wait_ack())
        return 1;
    wm8960_protocol_iic_write_byte(control_byte[0]);
    if(wm8960_protocol_iic_wait_ack())
        return 2;
    wm8960_protocol_iic_write_byte(control_byte[1]);
    if(wm8960_protocol_iic_wait_ack())
        return 3;

    wm8960_protocol_iic_stop();

    return HW_ERR_OK;
}


static void wm8960_protocol_iic_write_byte(uint8_t byte)
{
    uint8_t t;
    wm8960_protocol_iic_sda_output();
    WM8960_IIC_SCLK_LOW;
    for(t=0; t<8; t++)
    {
        if(((byte&WM8960_HIGH_BIT_MASK)>>7) == 1)
            WM8960_IIC_SDIN_HIGH;
        else
            WM8960_IIC_SDIN_LOW;

        byte<<=1;
        hw_delay_us(2);
        WM8960_IIC_SCLK_HIGH;
        hw_delay_us(2);
        WM8960_IIC_SCLK_LOW;
        hw_delay_us(2);
    }

}


static void wm8960_protocol_iic_sda_output()
{
    IIC_GPIO_InitStructure.GPIO_Pin = WM8960_SDA_PIN;
    IIC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(WM8960_SDA_GPIO, &IIC_GPIO_InitStructure);
}

static void wm8960_protocol_iic_sda_input()
{
    IIC_GPIO_InitStructure.GPIO_Pin = WM8960_SDA_PIN;
    IIC_GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(WM8960_SDA_GPIO, &IIC_GPIO_InitStructure);
}

static void wm8960_protocol_iic_start()
{
    wm8960_protocol_iic_sda_output();

    WM8960_IIC_SCLK_HIGH;
    WM8960_IIC_SDIN_HIGH;
    hw_delay_us(4);
    WM8960_IIC_SDIN_LOW;
    hw_delay_us(4);
    WM8960_IIC_SCLK_LOW;
}


static uint8_t wm8960_protocol_iic_wait_ack()
{
    uint8_t err_count = 0;
    wm8960_protocol_iic_sda_input();

    WM8960_IIC_SDIN_HIGH;
    hw_delay_us(1);
    WM8960_IIC_SCLK_HIGH;
    hw_delay_us(1);

    while(WM8960_IIC_READ_SDA)
    {
        err_count++;
        if(err_count > 250)
        {
            wm8960_protocol_iic_stop();
            return 1;
        }
    }
    WM8960_IIC_SCLK_LOW;

    return 0;
}


static void wm8960_protocol_iic_stop()
{
    wm8960_protocol_iic_sda_output();

    WM8960_IIC_SCLK_LOW;
    WM8960_IIC_SDIN_LOW;

    hw_delay_us(4);
    WM8960_IIC_SCLK_HIGH;
    WM8960_IIC_SDIN_HIGH;
    hw_delay_us(4);

}

static uint8_t sw_wav_decoder(uint8_t *data,wave_info_t *wave_info)
{
    uint8_t decoder_ret = HW_ERR_OK;
    wave_header_t *wave_header =  (wave_header_t *)data;
    if((wave_header->riff_chunk_id == RIFF_CHUNK_ID) \
            && (wave_header->riff_format == RIFF_CHUNK_FORMAT_WAV) \
            && (wave_header->fmt_chunk_id == FORMAT_CHUNK_ID))
    {
        HW_DEBUG("sw_wav_decoder success\n");
        wave_info->sample_rate = wave_header->fmt_sample_rate;
        wave_info->channel_number = wave_header->fmt_channel_number;
        wave_info->block_align= wave_header->fmt_block_align;
        wave_info->data_size = wave_header->data_chunk_size;
        wave_info->byte_rate = wave_header->fmt_byte_rate;
        wave_info->bits_per_sample = wave_header->fmt_bits_per_sample;

        HW_DEBUG("wave_info->sample_rate:%d\n",wave_info->sample_rate);
        HW_DEBUG("wave_info->fmt_channel_number:%d\n",wave_info->channel_number);
        HW_DEBUG("wave_info->fmt_block_align:%d\n",wave_info->block_align);
        HW_DEBUG("wave_info->data_size:%d\n",wave_info->data_size);
        HW_DEBUG("wave_info->byte_rate:%d\n",wave_info->byte_rate);
        HW_DEBUG("wave_info->bits_per_sample:%d\n",wave_info->bits_per_sample);

    }
    else
    {
        HW_DEBUG("sw_wav_decoder fail\n");
        HW_DEBUG("wave_header->riff_chunk_id 0x%x\n",wave_header->riff_chunk_id);
        HW_DEBUG("wave_header->riff_format 0x%x\n",wave_header->riff_format);
        HW_DEBUG("wave_header->fmt_chunk_id 0x%x\n",wave_header->fmt_chunk_id);
        HW_DEBUG("wave_header->data_chunk_id 0x%x\n",wave_header->data_chunk_id);
        decoder_ret = HW_ERR_WAV_FOMAT_FAIL;
    }

    return decoder_ret;
}

void wm8978_switch_buffer_play()
{
    UINT read_len = 0;
    if(wave_play_info.current_play_buffer  == 1)
    {
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wave_play_info.music_buffer2;
        DMA_InitStructure.DMA_BufferSize = wave_play_info.music_buffer2_len/wave_play_info.channel_number;
        wave_play_info.current_play_buffer = 2;
    }
    else
    {
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wave_play_info.music_buffer1;
        DMA_InitStructure.DMA_BufferSize = wave_play_info.music_buffer1_len/wave_play_info.channel_number;
        wave_play_info.current_play_buffer = 1;
    }

    if(DMA_InitStructure.DMA_BufferSize != 0)
    {
        DMA_DeInit(DMA1_Channel5);
        DMA_Init(DMA1_Channel5, &DMA_InitStructure);
        //SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);
        DMA_Cmd(DMA1_Channel5, ENABLE);
        DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);

        if(wave_play_info.current_play_buffer == 1)
        {
            f_read(&play_file,music_buffer2,sizeof(music_buffer2),&read_len);
            wave_play_info.music_buffer2_len = read_len;
        }
        else
        {
            f_read(&play_file,music_buffer1,sizeof(music_buffer1),&read_len);
            wave_play_info.music_buffer1_len = read_len;
        }
    }
    else
        f_close(&play_file);



}

void DMA1_Channel5_IRQHandler()
{
    if(DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
        DMA_ClearITPendingBit(DMA1_FLAG_TC5);
        wm8978_switch_buffer_play();
    }
}



