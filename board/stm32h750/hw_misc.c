/******************************************************************************
  * @file    hw_misc.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   hw misc source file
******************************************************************************/

#include "hw_misc.h"


#define MAX_COL 16
#define SHOW_LINE_SIZE 16
void hw_hex_dump(uint8_t *data,int len)
{
#if HW_DEBUG_ENABLE > 0
    int line;
    int curline = 0;
    int curcol = 0;
    char showline[SHOW_LINE_SIZE];
    int data_pos = 0;

    if(len % MAX_COL)
    {
        line = len/MAX_COL+1;
    }
    else
    {
        line = len/MAX_COL;
    }

    for(curline = 0; curline < line; curline++)
    {
        hw_sprintf(showline,"%08xh:",curline*MAX_COL);
        HW_DEBUG("%s",showline);
        for(curcol = 0; curcol < MAX_COL; curcol++)
        {
            if(data_pos < len)
            {
                HW_DEBUG("%02x ",data[data_pos]);
                data_pos++;
                continue;
            }
            else
            {
                break;
            }
        }
        HW_DEBUG("\n");
    }
#endif
}


void hw_delay_ms(uint32_t cms)
{
    HAL_Delay(cms);

}


void hw_delay_us(uint32_t cus)
{
    while(cus--);
}

void hw_loop_wait(uint32_t loop_count)
{
	uint32_t loop = loop_count*50;
	while(loop--);
}


void system_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 64;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
  }
}


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}


