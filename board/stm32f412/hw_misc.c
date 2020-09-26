/******************************************************************************
  * @file    hw_misc.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2018-7-31
  * @brief   hw misc source file
******************************************************************************/

#include "hw_misc.h"

/* º¯ÊýÉùÃ÷Çø */


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

void system_clock_config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 7;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLR = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        return;
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        return;
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        return;
    }
}



