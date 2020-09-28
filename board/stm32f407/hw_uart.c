/******************************************************************************
  * @file    hw_usart.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-25
  * @brief   usart source file
******************************************************************************/

#include "hw_uart.h"


#define UART6_MAX_REV	256
uint8_t uart6_rev_buffer[UART6_MAX_REV];
uint8_t uart6_rev_len;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart2_rx;

extern uint8_t shell_parse(uint8_t *shell_string);


/******************************************************************************
 * func name   : fputc
 * para        : ch(IN)  --> output char
                 fp(IN)  --> file point
 * return      : return output char
 * description : The fputc () function redirects the fputc called by printf
                 For subsequent calls to printf,
                 you can use the PC terminal output from USART1 directly
******************************************************************************/
int fputc(int ch, FILE *fp)
{
    if (fp == stdout)
    {
        /*If you want to output a line feed, you can automatically add a carriage
        return to prevent the horizontal position of some terminal terminals
        from changing after line feed.*/
        if (ch == '\n')
        {
            while ((USART6->SR & USART_SR_TXE) == 0);
            USART6->DR = '\r';
        }
        /* Output characters to USART1 */
        while ((USART6->SR & USART_SR_TXE) == 0);
        USART6->DR = ch;
    }
    return ch;
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA0-WKUP     ------> USART2_CTS
    PA1     ------> USART2_RTS
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      return;
    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
    else if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();
  
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**USART6 GPIO Configuration    
    PG9     ------> USART6_RX
    PG14     ------> USART6_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* USART6 DMA Init */
    /* USART6_RX Init */
    hdma_usart6_rx.Instance = DMA2_Stream1;
    hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
    hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart6_rx.Init.Mode = DMA_NORMAL;
    hdma_usart6_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart6_rx) != HAL_OK)
    {
      return;
    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart6_rx);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
  }

}


/******************************************************************************
 * func name   : hw_uart_debug_init
 * para        : baud_rate(IN)  --> Baud rate of uart1
 * return      : hw_uart_debug_init result
 * description : Initialization of USART1.PA9->TX PA10->RX
******************************************************************************/
uint8_t hw_uart_debug_init(uint32_t baud_rate)
{
    /* DMA controller clock enable */
    /* WHAT THE FUCK???? If you put this statement after HAL_UART_Init, UART will not accept data!! */
    __HAL_RCC_DMA2_CLK_ENABLE();

    huart6.Instance = USART6;
  huart6.Init.BaudRate = baud_rate;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    return HW_HAL_EXCU_ERR;
  }

    HAL_UART_Receive_DMA(&huart6, uart6_rev_buffer, UART6_MAX_REV);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

    return HW_ERR_OK;

}

void USART6_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    if((__HAL_UART_GET_FLAG(&huart6,UART_FLAG_IDLE) != RESET))
    {
        /* Clear the interrupt and reset DMA */
        __HAL_UART_CLEAR_IDLEFLAG(&huart6);
        HAL_UART_DMAStop(&huart6);
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        uart6_rev_len =UART6_MAX_REV-huart6.hdmarx->Instance->NDTR;

        if(uart6_rev_len != 0)
        {
            /* Call the parse function */
            shell_parse(uart6_rev_buffer);
            hw_memset(uart6_rev_buffer,0,sizeof(uart6_rev_buffer));
        }

        HAL_UART_Receive_DMA(&huart6,uart6_rev_buffer,UART6_MAX_REV);
    }
    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart6);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}



