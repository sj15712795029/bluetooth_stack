/******************************************************************************
  * @file    hw_cs43l22.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2019-3-13
  * @brief   cs43l22 source file
******************************************************************************/

#include "hw_cs43l22.h"

__IO uint32_t CurrAudioInterface = AUDIO_INTERFACE_I2S; 

 /* DMA Stream definitions */
 uint32_t AUDIO_MAL_DMA_CLOCK    = AUDIO_I2S_DMA_CLOCK;
 DMA_Stream_TypeDef * AUDIO_MAL_DMA_STREAM   = AUDIO_I2S_DMA_STREAM ;       
 uint32_t AUDIO_MAL_DMA_DREG     = AUDIO_I2S_DMA_DREG;
 uint32_t AUDIO_MAL_DMA_CHANNEL  = AUDIO_I2S_DMA_CHANNEL;
 uint32_t AUDIO_MAL_DMA_IRQ      = AUDIO_I2S_DMA_IRQ  ;
 uint32_t AUDIO_MAL_DMA_FLAG_TC  = AUDIO_I2S_DMA_FLAG_TC;
 uint32_t AUDIO_MAL_DMA_FLAG_HT  = AUDIO_I2S_DMA_FLAG_HT;
 uint32_t AUDIO_MAL_DMA_FLAG_FE  = AUDIO_I2S_DMA_FLAG_FE;
 uint32_t AUDIO_MAL_DMA_FLAG_TE  = AUDIO_I2S_DMA_FLAG_TE;
 uint32_t AUDIO_MAL_DMA_FLAG_DME = AUDIO_I2S_DMA_FLAG_DME;

/**
  * @brief  Set the current audio interface (I2S or DAC).
  * @param  Interface: AUDIO_INTERFACE_I2S or AUDIO_INTERFACE_DAC
  * @retval None
  */
void EVAL_AUDIO_SetAudioInterface(uint32_t Interface)
{    
  CurrAudioInterface = Interface;
  
  if (CurrAudioInterface == AUDIO_INTERFACE_I2S)
  {
    /* DMA Stream definitions */
    AUDIO_MAL_DMA_CLOCK    = AUDIO_I2S_DMA_CLOCK;
    AUDIO_MAL_DMA_STREAM   = AUDIO_I2S_DMA_STREAM;        
    AUDIO_MAL_DMA_DREG     = AUDIO_I2S_DMA_DREG;
    AUDIO_MAL_DMA_CHANNEL  = AUDIO_I2S_DMA_CHANNEL;
    AUDIO_MAL_DMA_IRQ      = AUDIO_I2S_DMA_IRQ  ;
    AUDIO_MAL_DMA_FLAG_TC  = AUDIO_I2S_DMA_FLAG_TC;
    AUDIO_MAL_DMA_FLAG_HT  = AUDIO_I2S_DMA_FLAG_HT;
    AUDIO_MAL_DMA_FLAG_FE  = AUDIO_I2S_DMA_FLAG_FE;
    AUDIO_MAL_DMA_FLAG_TE  = AUDIO_I2S_DMA_FLAG_TE;
    AUDIO_MAL_DMA_FLAG_DME = AUDIO_I2S_DMA_FLAG_DME;
  }
  else if (Interface == AUDIO_INTERFACE_DAC)
  {
    /* DMA Stream definitions */
    AUDIO_MAL_DMA_CLOCK    = AUDIO_DAC_DMA_CLOCK;
    AUDIO_MAL_DMA_STREAM   = AUDIO_DAC_DMA_STREAM;        
    AUDIO_MAL_DMA_DREG     = AUDIO_DAC_DMA_DREG;
    AUDIO_MAL_DMA_CHANNEL  = AUDIO_DAC_DMA_CHANNEL;
    AUDIO_MAL_DMA_IRQ      = AUDIO_DAC_DMA_IRQ  ;
    AUDIO_MAL_DMA_FLAG_TC  = AUDIO_DAC_DMA_FLAG_TC;
    AUDIO_MAL_DMA_FLAG_HT  = AUDIO_DAC_DMA_FLAG_HT;
    AUDIO_MAL_DMA_FLAG_FE  = AUDIO_DAC_DMA_FLAG_FE;
    AUDIO_MAL_DMA_FLAG_TE  = AUDIO_DAC_DMA_FLAG_TE;
    AUDIO_MAL_DMA_FLAG_DME = AUDIO_DAC_DMA_FLAG_DME;    
  }
}


uint8_t hw_cs43l22_init()
{
	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
}

