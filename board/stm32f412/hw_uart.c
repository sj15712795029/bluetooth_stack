/******************************************************************************
  * @file    hw_usart.c
  * @author  Yu-ZhongJun
  * @version V0.0.1
  * @date    2020-9-25
  * @brief   usart source file
******************************************************************************/

#include "hw_uart.h"


#define UART1_MAX_REV	256
uint8_t uart1_rev_buffer[UART1_MAX_REV];
uint8_t uart1_rev_len;
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
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
            while ((USART1->SR & USART_SR_TXE) == 0);
            USART1->DR = '\r';
        }
        /* Output characters to USART1 */
        while ((USART1->SR & USART_SR_TXE) == 0);
        USART1->DR = ch;
    }
    return ch;
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance==USART1)
    {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 DMA Init */
        /* USART1_RX Init */
        hdma_usart1_rx.Instance = DMA2_Stream2;
        hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_NORMAL;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
        {
            return;
        }

        __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
	else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA0     ------> USART2_CTS
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
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
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

    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        return HW_HAL_EXCU_ERR;
    }

    HAL_UART_Receive_DMA(&huart1, uart1_rev_buffer, UART1_MAX_REV);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    return HW_ERR_OK;

}

void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET))
    {
        /* Clear the interrupt and reset DMA */
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_UART_DMAStop(&huart1);
        /* Without this, the interrupt cannot be cleared and continues into the interrupt */
        uart1_rev_len =UART1_MAX_REV-huart1.hdmarx->Instance->NDTR;

        if(uart1_rev_len != 0)
        {
            /* Call the parse function */
            shell_parse(uart1_rev_buffer);
            hw_memset(uart1_rev_buffer,0,sizeof(uart1_rev_buffer));
        }

        HAL_UART_Receive_DMA(&huart1,uart1_rev_buffer,UART1_MAX_REV);
    }
    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}



