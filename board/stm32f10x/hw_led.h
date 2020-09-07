/******************************************************************************
  * @file    hw_led.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   led header file
******************************************************************************/
#ifndef HW_LED_H_H_H
#define HW_LED_H_H_H

#include "hw_misc.h"

#define LED1_PERIPH_CLK RCC_APB2Periph_GPIOB
#define LED1_GPIO GPIOB
#define LED1_PIN GPIO_Pin_0

#define LED2_PERIPH_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO GPIOB
#define LED2_PIN GPIO_Pin_1


/* Operate LED on/off */
#define LED1_OFF	GPIO_WriteBit(LED1_GPIO, LED1_PIN, Bit_SET)
#define LED1_ON	GPIO_WriteBit(LED1_GPIO, LED1_PIN, Bit_RESET)

#define LED2_OFF	GPIO_WriteBit(LED2_GPIO, LED2_PIN, Bit_SET)
#define LED2_ON	GPIO_WriteBit(LED2_GPIO, LED2_PIN, Bit_RESET)

uint8_t hw_led_init(void);

#endif

