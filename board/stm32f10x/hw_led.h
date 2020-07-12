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

#define LED_PERIPH_CLK RCC_APB2Periph_GPIOA
#define LED_GPIO GPIOA
#define LED_PIN GPIO_Pin_2


/* Operate LED on/off */
#define LED_ON	GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_SET)
#define LED_OFF	GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_RESET)

uint8_t hw_led_init(void);

#endif

