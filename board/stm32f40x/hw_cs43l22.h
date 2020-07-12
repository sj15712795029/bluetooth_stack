/******************************************************************************
  * @file    hw_cs43l22.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2019-3-13
  * @brief   cs43l22 header file
******************************************************************************/
#ifndef HW_CS43L22_H_H_H
#define HW_CS43L22_H_H_H
#include "hw_misc.h"

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

/** @addtogroup Utilities
  * @{
  */


/** @addtogroup STM32F4_DISCOVERY
  * @{
  */
    
/** @defgroup STM32F4_DISCOVERY_AUDIO_CODEC 
  * @{
  */    


/** @defgroup STM32F4_DISCOVERY_AUDIO_CODEC_Exported_Types
  * @{
  */

/** @defgroup STM32F4_DISCOVERY_AUDIO_CODEC_Exported_Constants
  * @{
  */ 

/*------------------------------------
             CONFIGURATION: Audio Codec Driver Configuration parameters
                                      ----------------------------------------*/
/* Audio Transfer mode (I2S Interrupt) */
//#define I2S_INTERRUPT                 /* Uncomment this line to enable audio transfert with I2S interrupt*/ 

/* Audio Transfer mode (DMA, Interrupt or Polling) */
#define AUDIO_MAL_MODE_NORMAL         /* Uncomment this line to enable the audio 
                                         Transfer using DMA */
/* #define AUDIO_MAL_MODE_CIRCULAR */ /* Uncomment this line to enable the audio 
                                         Transfer using DMA */

/* For the DMA modes select the interrupt that will be used */
#define AUDIO_MAL_DMA_IT_TC_EN        /* Uncomment this line to enable DMA Transfer Complete interrupt */
/* #define AUDIO_MAL_DMA_IT_HT_EN */  /* Uncomment this line to enable DMA Half Transfer Complete interrupt */
/* #define AUDIO_MAL_DMA_IT_TE_EN */  /* Uncomment this line to enable DMA Transfer Error interrupt */

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define EVAL_AUDIO_IRQ_PREPRIO           0   /* Select the preemption priority level(0 is the highest) */
#define EVAL_AUDIO_IRQ_SUBRIO            0   /* Select the sub-priority level (0 is the highest) */

/* Uncomment the following line to use the default Codec_TIMEOUT_UserCallback() 
   function implemented in stm32f4_discovery_audio_codec.c file.
   Codec_TIMEOUT_UserCallback() function is called whenever a timeout condition 
   occurs during communication (waiting on an event that doesn't occur, bus 
   errors, busy devices ...). */   
/* #define USE_DEFAULT_TIMEOUT_CALLBACK */

/* Enable this define to use the I2S DMA for writing into DAC register */
//#define DAC_USE_I2S_DMA
/*----------------------------------------------------------------------------*/

/*------------------------------------
                    OPTIONAL Configuration defines parameters
                                      ----------------------------------------*/
/* I2C clock speed configuration (in Hz) 
  WARNING: 
   Make sure that this define is not already declared in other files (ie. 
  stm322xg_eval.h file). It can be used in parallel by other modules. */
#ifndef I2C_SPEED
 #define I2C_SPEED                        100000
#endif /* I2C_SPEED */

/* Uncomment defines below to select standard for audio communication between 
  Codec and I2S peripheral */
#define I2S_STANDARD_PHILLIPS
/* #define I2S_STANDARD_MSB */
/* #define I2S_STANDARD_LSB */

/* Uncomment the defines below to select if the Master clock mode should be 
  enabled or not */
#define CODEC_MCLK_ENABLED
/* #deine CODEC_MCLK_DISABLED */

/* Uncomment this line to enable verifying data sent to codec after each write 
  operation */
#define VERIFY_WRITTENDATA 
/*----------------------------------------------------------------------------*/

/*-----------------------------------
                    Hardware Configuration defines parameters
                                     -----------------------------------------*/
/* Audio Reset Pin definition */
#define AUDIO_RESET_GPIO_CLK           RCC_AHB1Periph_GPIOD  
#define AUDIO_RESET_PIN                GPIO_Pin_4    
#define AUDIO_RESET_GPIO               GPIOD 
                 
/* I2S peripheral configuration defines */
#define CODEC_I2S                      SPI3
#define CODEC_I2S_CLK                  RCC_APB1Periph_SPI3
#define CODEC_I2S_ADDRESS              0x40003C0C
#define CODEC_I2S_GPIO_AF              GPIO_AF_SPI3
#define CODEC_I2S_IRQ                  SPI3_IRQn
#define CODEC_I2S_GPIO_CLOCK           (RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA)
#define CODEC_I2S_WS_PIN               GPIO_Pin_4
#define CODEC_I2S_SCK_PIN              GPIO_Pin_10
#define CODEC_I2S_SD_PIN               GPIO_Pin_12
#define CODEC_I2S_MCK_PIN              GPIO_Pin_7
#define CODEC_I2S_WS_PINSRC            GPIO_PinSource4
#define CODEC_I2S_SCK_PINSRC           GPIO_PinSource10
#define CODEC_I2S_SD_PINSRC            GPIO_PinSource12
#define CODEC_I2S_MCK_PINSRC           GPIO_PinSource7
#define CODEC_I2S_GPIO                 GPIOC
#define CODEC_I2S_WS_GPIO              GPIOA
#define CODEC_I2S_MCK_GPIO             GPIOC
#define Audio_I2S_IRQHandler           SPI3_IRQHandler


 #define AUDIO_MAL_DMA_PERIPH_DATA_SIZE DMA_PeripheralDataSize_HalfWord
 #define AUDIO_MAL_DMA_MEM_DATA_SIZE    DMA_MemoryDataSize_HalfWord
 #define DMA_MAX_SZE                    0xFFFF


 #define DAC_DHR12L1_ADDRESS            0x4000740C
 #define DAC_DHR12R1_ADDRESS            0x40007408
 #define DAC_DHR8R1_ADDRESS             0x40007410
 #define AUDIO_DAC_CHANNEL              DAC_Channel_1

 /* I2S DMA Stream definitions */
 #define AUDIO_I2S_DMA_CLOCK            RCC_AHB1Periph_DMA1
 #define AUDIO_I2S_DMA_STREAM           DMA1_Stream7
 #define AUDIO_I2S_DMA_DREG             CODEC_I2S_ADDRESS
 #define AUDIO_I2S_DMA_CHANNEL          DMA_Channel_0
 #define AUDIO_I2S_DMA_IRQ              DMA1_Stream7_IRQn
 #define AUDIO_I2S_DMA_FLAG_TC          DMA_FLAG_TCIF7
 #define AUDIO_I2S_DMA_FLAG_HT          DMA_FLAG_HTIF7
 #define AUDIO_I2S_DMA_FLAG_FE          DMA_FLAG_FEIF7
 #define AUDIO_I2S_DMA_FLAG_TE          DMA_FLAG_TEIF7
 #define AUDIO_I2S_DMA_FLAG_DME         DMA_FLAG_DMEIF7

 #define Audio_MAL_I2S_IRQHandler       DMA1_Stream7_IRQHandler


 /* DAC DMA Stream definitions */
 #define AUDIO_DAC_DMA_CLOCK            RCC_AHB1Periph_DMA1
 #define AUDIO_DAC_DMA_STREAM           DMA1_Stream0
 #define AUDIO_DAC_DMA_DREG             DAC_DHR12L1_ADDRESS
 #define AUDIO_DAC_DMA_CHANNEL          DMA_Channel_0
 #define AUDIO_DAC_DMA_IRQ              DMA1_Stream0_IRQn
 #define AUDIO_DAC_DMA_FLAG_TC          DMA_FLAG_TCIF0
 #define AUDIO_DAC_DMA_FLAG_HT          DMA_FLAG_HTIF0
 #define AUDIO_DAC_DMA_FLAG_FE          DMA_FLAG_FEIF0
 #define AUDIO_DAC_DMA_FLAG_TE          DMA_FLAG_TEIF0
 #define AUDIO_DAC_DMA_FLAG_DME         DMA_FLAG_DMEIF0

 #define Audio_MAL_DAC_IRQHandler       DMA1_Stream0_IRQHandler


/* I2C peripheral configuration defines (control interface of the audio codec) */
#define CODEC_I2C                      I2C1
#define CODEC_I2C_CLK                  RCC_APB1Periph_I2C1
#define CODEC_I2C_GPIO_CLOCK           RCC_AHB1Periph_GPIOB
#define CODEC_I2C_GPIO_AF              GPIO_AF_I2C1
#define CODEC_I2C_GPIO                 GPIOB
#define CODEC_I2C_SCL_PIN              GPIO_Pin_6
#define CODEC_I2C_SDA_PIN              GPIO_Pin_9
#define CODEC_I2S_SCL_PINSRC           GPIO_PinSource6
#define CODEC_I2S_SDA_PINSRC           GPIO_PinSource9

/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define CODEC_FLAG_TIMEOUT             ((uint32_t)0x1000)
#define CODEC_LONG_TIMEOUT             ((uint32_t)(300 * CODEC_FLAG_TIMEOUT))
/*----------------------------------------------------------------------------*/

/*-----------------------------------
                        Audio Codec User defines
                                     -----------------------------------------*/
/* Audio interface : I2S or DAC */
#define AUDIO_INTERFACE_I2S           1
#define AUDIO_INTERFACE_DAC           2

/* Codec output DEVICE */
#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

#endif