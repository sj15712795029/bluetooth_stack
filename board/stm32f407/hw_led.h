/******************************************************************************
  * @file    hw_led.h
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-25
  * @brief   led header file
******************************************************************************/
#ifndef HW_LED_H_H_H
#define HW_LED_H_H_H

#include "hw_misc.h"

#define LED1_PERIPH_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE
#define LED1_GPIO GPIOG
#define LED1_PIN GPIO_PIN_15

#define LED2_PERIPH_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE
#define LED2_GPIO GPIOG
#define LED2_PIN GPIO_PIN_11


/* Operate LED on/off */
#define LED1_OFF	HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_SET)
#define LED1_ON		HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, GPIO_PIN_RESET)

#define LED2_OFF	HAL_GPIO_WritePin(LED2_GPIO, LED2_PIN, GPIO_PIN_SET)
#define LED2_ON		HAL_GPIO_WritePin(LED2_GPIO, LED2_PIN, GPIO_PIN_RESET)

uint8_t hw_led_init(void);

#endif
